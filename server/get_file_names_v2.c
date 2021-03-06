/*
** Author: Dheeraj Dake
** Date:   5/31/16
** Description: This piece of code implements the file tree walk structure. It scans the files in a given directory and stores the file names,
** 		last modified time(epoch time) and latest modified time. If any file changes are detected, those file names are taken into account.
** 		If those files are accessed by the cache server, the files are automatically transferred to the cache proxy server. 
*/


#include<stdio.h>
#include<dirent.h>
#include<stdlib.h>
#include<sys/stat.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/wait.h> 
#include<string.h>
#include<pthread.h>
#include <stdbool.h>
#include <ftw.h>
#include <fcntl.h>
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <signal.h> 
#include <pthread.h>
#include <fcntl.h>

#include "get_file_names_v2.h"  //structs
#include "server.h" //using MIME

#define FILE_LOCATION getenv("PWD")


int indx = 0;
DIR *dir, *getfiles;
struct dirent *ent, *scanfiles;
struct stat st;

int FILE_COUNT = 0;         //Do not use this
int STORE_FILE_COUNT = 0;   //Use this for storing the current file count


/*Tracking file count*/
int track_files(const char *name, const struct stat *status, int type)
{
	if(type == FTW_NS)
	{
		  return 0;
	}
	if(type == FTW_F)
	{
		if(name[strlen(name)-1] != '~')
		{
			FILE_COUNT++;
		}
	}
	if(type == FTW_D && strcmp(".", name) != 0)
	{
		if(name[strlen(name)-1] != '~') 
		{
			FILE_COUNT++;
		}
	}
	return 0;
}

/*Using FTW for getfile count*/
int get_file_count()
{
	FILE_COUNT = 0;
	ftw(".", track_files, 1); 	
	return FILE_COUNT;
}

/*File tree walk*/
int search_initial(const char *name, const struct stat *status, int type) 
{
	//FILE_COUNT = get_file_count();
 	if(type == FTW_NS)
	  return 0;
	stat(name, &st);	

	/*For files*/
	if(type == FTW_F)
	{
	   if(name[strlen(name)-1] != '~') {
		strcpy(file_details[indx].filename, name);
		file_details[indx].epoch_time = (long long)st.st_mtim.tv_sec;
		file_details[indx].epoch_time_l = (long long)st.st_mtim.tv_sec;
		file_details[indx].time_difference = file_details[indx].epoch_time_l - file_details[indx].epoch_time;

		printf("\n%d. %s", indx, file_details[indx].filename);
		/*Format spacing*/
		int getfilename_size = strlen(name);
		int b;
		for(b=0; b<(40 - getfilename_size); b++)
		{
			printf(" ");
		}
		printf("| %lld ", file_details[indx].epoch_time);
		printf("| %lld ", file_details[indx].epoch_time_l);
		printf("| %lld ", file_details[indx].time_difference);
		indx++;
	  }
 	}

	/*For directories*/
	if(type == FTW_D && strcmp(".", name) != 0)
	{
	  if(name[strlen(name)-1] != '~') {
		strcpy(file_details[indx].filename, name);
		file_details[indx].epoch_time = (long long)st.st_mtim.tv_sec;
		file_details[indx].epoch_time_l = (long long)st.st_mtim.tv_sec;
		file_details[indx].time_difference = file_details[indx].epoch_time_l - file_details[indx].epoch_time;

		printf("\n%d. %s", indx, file_details[indx].filename);
		/*Format spacing*/
		int getfilename_size = strlen(name);
		int b;
		for(b=0; b<(40 - getfilename_size); b++)
		{
			printf(" ");
		}
		printf("| %lld ", file_details[indx].epoch_time);
		printf("| %lld ", file_details[indx].epoch_time_l);
		printf("| %lld ", file_details[indx].time_difference);
		indx++;
	  }
	}
 return 0;
}

int search_periodic(const char *name, const struct stat *status, int type) 
{
 	if(type == FTW_NS)
	  return 0;
	stat(name, &st);	


	/*Handle insertions and deletions*/

	

	/*For files*/
	if(type == FTW_F)
	{
	   if(name[strlen(name)-1] != '~') {
		strcpy(file_details[indx].filename, name);
		//file_details[indx].epoch_time = (long long)st.st_mtim.tv_sec;
		file_details[indx].epoch_time_l = (long long)st.st_mtim.tv_sec;
		file_details[indx].time_difference = file_details[indx].epoch_time_l - file_details[indx].epoch_time;

		printf("\n%d. %s", indx, file_details[indx].filename);
		/*Format spacing*/
		int getfilename_size = strlen(name);
		int b;
		for(b=0; b<(40 - getfilename_size); b++)
		{
			printf(" ");
		}
		printf("| %lld ", file_details[indx].epoch_time);
		printf("| %lld ", file_details[indx].epoch_time_l);
		printf("| %lld ", file_details[indx].time_difference);
		indx++;
	  }
 	}

	/*For directories*/
	if(type == FTW_D && strcmp(".", name) != 0)
	{
	  if(name[strlen(name)-1] != '~') {
		strcpy(file_details[indx].filename, name);
		file_details[indx].epoch_time = (long long)st.st_mtim.tv_sec;
		file_details[indx].epoch_time_l = (long long)st.st_mtim.tv_sec;
		file_details[indx].time_difference = file_details[indx].epoch_time_l - file_details[indx].epoch_time;

		printf("\n%d. %s", indx, file_details[indx].filename);
		/*Format spacing*/
		int getfilename_size = strlen(name);
		int b;
		for(b=0; b<(40 - getfilename_size); b++)
		{
			printf(" ");
		}
		printf("| %lld ", file_details[indx].epoch_time);
		printf("| %lld ", file_details[indx].epoch_time_l);
		printf("| %lld ", file_details[indx].time_difference);
		indx++;
	  }
	}
 return 0;
}


void sync_files(char *filename)
{
	
			int syn,nsfd,portnum;// fd will be used as a socket descriptor
			struct sockaddr_in client;
			//socklen_t c_len;
			char buf_cli4[6500];// Unbounded buffer. We transfer byte by byte 
			bzero(buf_cli4, sizeof(buf_cli4));
		
			//Typecast port number from char to int
			int nportnum=8001;
			nsfd=socket(AF_INET,SOCK_STREAM,0);// TCP uses stream so SOCK_STREAM
			
	
			// Try opening the socket
			if(nsfd<0)
			{
				
				errormsg("[ERROR:] Cannot open the socket: ");	
			}
			if(syn<0)
			{			
				errormsg("[ERROR:] Cannot open the syn socket: ");	
			}
		
			//Assigning the PORT IP and FAMILY	
			client.sin_family = AF_INET;
			client.sin_addr.s_addr=INADDR_ANY;// IP
			client.sin_port=htons(nportnum);//PORT

			//Connet to the server CHECK
			if(connect(nsfd,(struct sockaddr*)&client,sizeof(client))<0)
			{
				errormsg("[ERROR:]Cannot connect to the server \n");
			}



	printf("\n----------IN SYNC FUNCTION");

	/*Remove ./ from filename*/
	char filename_update[20];
	bzero(filename_update, sizeof(filename_update));
	memcpy(&filename_update[0], &filename[1], strlen(filename)-1);

	printf("\n\nThe filename is %s\n\n", filename_update);
	char *rootpath = "/home/dheeraj/Desktop/207/server/";
	/*File code*/

	char store_filepath[400];

	/*Send file path*/
	printf("\nSending file name");
	bzero(store_filepath, sizeof(store_filepath));
//	send(sync_socket, filename_update, strlen(filename_update),0);
	send(nsfd, filename_update, strlen(filename_update),0);
	printf("\nFile name sent");
	//close(sync_socket);

	//sleep(2);
	/*Now send the file*/
	/*Create the path for fopen*/
	char file_path[200], latest_file_path[200];
	bzero(file_path, sizeof(file_path));
	bzero(latest_file_path, sizeof(latest_file_path));
	sprintf(file_path, "%s", rootpath);
	strcat(file_path, &filename_update[1]); //removing the initial /
	memcpy(&latest_file_path[0], &file_path[1], strlen(file_path)-1);

	//printf("\nThe FILE ACCESS PATH is %s", latest_file_path);
	printf("\nThe FILE ACCESS PATH is ");
	puts(latest_file_path);
	FILE *fp; 
	int ifp, f_size, dot=0;
	char *ftype, *type, *file_type;
	char header[1000], status1[100], line[100], send_filepath;
	bzero(header, sizeof(header));
	bzero(status1, sizeof(line));
	bzero(line, sizeof(status1));

	if(  !(fp=fopen(file_path,"r"))  )
	{
		perror("\n CANNOT OPEN");
	}

	ifp=open(file_path,O_RDONLY);
	
	dot = 0;
	printf("Body sent to client as response:\n\n");		
	while((dot = read(ifp,line,1))>0)
	{		
		send(nsfd,line,1,0); //change this to nsfd
		read(ifp,line,sizeof(line));
		puts(line);
	}		

	/*Update the last modified with latest modified, make time difference 0*/
	int file_cnt = get_file_count();
	int a=0;
	for(a; a<file_cnt; a++)
	{
		file_details[a].epoch_time = file_details[a].epoch_time_l;
	}
	printf("\nFile %s synced!", filename_update);

        close(nsfd);
}


void update_tables()
{
	/*First run make last_modified equals latest_modified*/	
	static int only_once = 1;
	if(only_once == 1)
	{
		ftw(".", search_initial, 1); //. is current directory
		STORE_FILE_COUNT = get_file_count();
		printf("\n---> File count is %d", STORE_FILE_COUNT); //Updates file_count
		indx = 0;   	             //clear the index always
		only_once = 0;
	}
	else
	{

		/*Checks for file deletions or insertions and re-walk the file tree/
		/*Do not delete and add in the same loop*/
		int get_file_num = get_file_count();

		if(get_file_num < STORE_FILE_COUNT)//deletions
		{
			printf("\n Deleted file!");
			ftw(".", search_initial, 1); 
			indx = 0;
			STORE_FILE_COUNT = get_file_num;
			return;	
		}
		else if(get_file_num > STORE_FILE_COUNT)
		{
			printf("\n Added new file!");
			ftw(".", search_initial, 1); 
			indx = 0;
			STORE_FILE_COUNT = get_file_num;
			return;
		}

		/*If no insertions or deletions were made, continue else return at this point*/
		ftw(".", search_periodic, 1); 
		indx = 0;

		//Scan all the time difference fields and keep a note of the indx whose diff is greater than zero
		int track_time_diff = get_file_count();
		int a = 0, b = 0;
		for(a; a < track_time_diff; a++)
		{
			if(file_details[a].time_difference > 0)
			{
				/*Keeping track of the modified file names*/
				strcpy(send_files[a].filename, file_details[a].filename);
				printf("\n\nThe file is %s\n", send_files[a].filename);
			}
		}
		printf("\n -->>The cache has the file %s", cache_files[0].filename);
		/*Compare these file names with the file names from cache access table*/		
		/*If it's a match, send those files*/
		/*If the cache structure is empty, move on......*/

		for(a=0; a<50; a++)  //limiting the max files to 50 in the directory
		{
			for(b=0; b<50; b++)
			{
				/*Check valid files only - WTF moment -_-*/
				if( (strlen(cache_files[a].filename)!= 0) && (strlen(send_files[b].filename)!=0) )
				{		
					/*If the files names match in both the tables - HIT*/
					if((strcmp(cache_files[a].filename, send_files[b].filename)) == 0)
					{
						printf("\nThe files are: %s %s", cache_files[a].filename, send_files[b].filename);
						/*Sends this file to the proxy*/
						sync_files(send_files[b].filename);
	
						/*Delete the file which is sent*/
						memset(&send_files[b].filename, 0, sizeof(send_files[b].filename));
					}
				}
			}
		}//end of for
	}//else	
}


#if 0
{
	/*Files in the cache table*/
	
	strcpy(cache_files[3].filename, "./ll");
	strcpy(cache_files[1].filename, "./hello");
	strcpy(cache_files[0].filename, "./Project/Proxy/hello");


	printf("\n            File Name                      |  Last mod  | Latest mod | Time difference");
	printf("\n--------------------------------------------------------------------------------------");
	int r;
	/*Modify r and sleep to change the loop count and sleep time*/
	for(r=0; r<10; r++)
	{
		update_tables();
		printf("\n---------------------------------------------------------------------------------------------------");
		fflush(stdout);
		sleep(10);
	}
	return 0;
}
#endif
