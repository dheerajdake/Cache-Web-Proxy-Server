/*=== Assignment 2 ===

Author: Srinivasa Maringanti
SJSU id: 009874969
Tags: Multithreaded HTTP web server
Course: CMPE 207
Submitted to: Prof. Wei Xu

*/
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <sys/wait.h> 
#include <sys/stat.h>
#include <signal.h> 
#include <pthread.h>
#include <fcntl.h>
#include<dirent.h>

//Global variables used only for setiing the command line root from the user.
char demoRpath[100];
char *rootPath;
DIR *dir;
char CREATE_DIR[100];


/*=========================== Error handling-Start ===========================*/
void errormsg(const char *msg)// Method used to call errors
{
	    perror(msg);
	    exit(0);
}
/*=========================== Error handling-End ===========================*/


/*=========================== Readline- Start ===========================*/
 #if 1
int http_readline(int sock, char *buf, int maxlen)// Function to read each line of the GET reuqest
{
	int n = 0;
	char *p = buf;
	while (n < maxlen - 1) 
	{
		char c;
		int rc = read(sock, &c, 1);// Reading from a socket and copying it to a integer
		
		if (rc == 1) 
		{
			/* Stop at \n and strip away \r and \n. */
			if (c == '\n') 
			{
				*p = 0; /* null-terminated */
				return n;
			} 
			else if (c != '\r') 
			{
				*p++ = c;
				n++;
			}
		} 
		else if (rc == 0) 
		{
			return -1;
			/* EOF */
		} 
		else if (errno == EINTR) 
		{
			continue;
			/* retry */
		} 
		else 
		{
			return -1;
			/* error */
		}
	}
		return -1; /* buffer too small */
}
#endif	
/*=========================== Readline- End ===========================*/

/*=========================== Calculating Filesize- Start ===========================*/	
int http_get_filesize(FILE *fp)// // Method given in assignment 2 to calculate filesize.
{
	int fsize;
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	rewind(fp);
	return fsize;
}       
        
/*=========================== Calculating Filesize- End ===========================*/	        
        
/*=========================== Handling HTTP Requests- Start ===========================*/    
int nsfd;
void* http_req(void* data)
{
	
	int th,r,b=0,fileOK=0,f_size,f_size1,ifp,z='.',dot;
	char *req,tmp_var[50],doc_root[100],line[65000],header[6000],header1[6000],demoRpath[100],buf[6500],buf10[6500], *filePath,*ftype,*type,status1[100]="200 OK",status[100]="404 FILE NOT FOUND",*tmp_ptr1,*tmp_ptr2,tmp_ptr3[100],*tmp_ptr4,*tmp_ptr5,tmp_ptr6[100]="/",tmp_ptr7[100],tmp_ptr8[100],*tmp_ptr10,buf2[6500]; 
	char tmp_ptr9[100];
	char tmp_ptr11[100];
	bzero(tmp_var, sizeof(tmp_var));
		
	/*
	
	Int variables
	-------------
	int r			-----> Used to save the readline return value.
	int b			-----> Used in iteration in while loop.
	
	Char variables/arrays description:
	----------------------------------
	
	char *req 		-----> Used to save the returned buffer line.
	char tmp_var[] 		-----> Used to check empty space and empty space.
	char doc_root 		-----> Used to save the root location taken from the user.
	char line[] 		-----> Used to save the data from the file opened.
	char header[] 		-----> Used to save the header that has to be sent.
	char demoRpath[]	-----> Used to commandline arugument passed for root address 
	char buf[] 		-----> Used to save the GET request from client.
	char *filePath 		-----> Used to save the file path requested in root folder.
	char *ftype 		-----> Used to save the file file.
	char *type 		-----> Used to return MIME type.
	char *status1[] 	-----> Used to return status when file exist.
	char *status[]	 	-----> Used to return status when file does not exist.
	char *tmp_ptr1 		-----> Used to break the line and processes the string which has "/".
	char *tmp_ptr2 		-----> Used to removes the  " " in string.
	char *tmp_ptr3 		-----> Used to save file location and also used as tmp array to perform string operations.
	char *tmp_ptr4 		-----> Used to remove "/" from the and save the one before seond "/".
	char *tmp_ptr5 		-----> Used to save the filename and removes the string before when encounters "/" for the last time.
	char *tmp_ptr6 		-----> Used to contatinate img to initial existing "/" in its array.
	char *tmp_ptr7 		-----> Used to save the root path and add the requesting file after Image URL mapping.
	char *tmp_ptr8 		-----> Used to save the contents of tmp_ptr5 temporarily.
	
	*/

	
	th=*((int *)data); //Type casting void pointer to integer pointer
	int a=0;
	//int rc = read(th, buf, sizeof(buf));
	char buf1[6500];
	char buf3[6500];
	bzero(buf, sizeof(buf));
	bzero(buf3, sizeof(buf3));
	r=http_readline(th,buf,sizeof(buf));
	read(th,buf1,sizeof(buf1));
	printf("\nFirst buf: %s\n", buf);	
//	puts(buf);
//	puts(buf1);
	char *req2;
	
 	//r=http_readline(th,buf,sizeof(buf));// 
 	char get_request[200];
	bzero(get_request, sizeof(get_request));
 	if(r>0)
	{ 	
	 	snprintf(get_request, 4, "%s" ,buf);
		req = buf;	
		//puts(req);
//		puts(buf1);
	 	//puts(get_request);
	}
	//else
	//{
	//	errormsg("[ERROR:] Failed to read.");
	//}

	/*while(req[b]!=' ')// Condition runs till it encounters an empty space
	{
		tmp_var[b]=req[b++];
		printf("%c",tmp_var[b]);
	}
		printf("%s",tmp_var);
*/
//	puts(req2);
        memcpy(tmp_var, get_request, 3);
	//puts(req);

	printf("\n Before");

	//puts(buf);
	printf("\n AFter \n");

	strcpy(demoRpath,rootPath);//copying the command line entered root location to rootPath to make it rootlocation

	//puts(tmp_var);
	//char qwert[100];
	printf("%s",tmp_var);
	printf("\nIs this working?\n");
	//puts(qwert);
#if 1  //latest block - 1
 	if((strncmp(tmp_var,"GET",3))==0)// Comparing the temporary array to check the string
 	{
	 	 // filePath is the name of the file to map the URL tmp_ptr1 and 2 are used to perform string operation
	 	  
	        strcpy(doc_root,demoRpath);//copying the command line entered root location to rootPath to make it rootlocation
	        fprintf(stderr,"\nDocument Root\n\n\t");
	        puts(doc_root);
		  
		//puts(tmp_ptr8);
		  
		  
		fprintf(stderr,"\nreq\n\n\t");
	        puts(req);		  
	        tmp_ptr1=strpbrk(req,"/"); // Breaks the line and processes the string which has "/".
	        fprintf(stderr,"\ntmp_ptr1\n\n\t");
	        puts(tmp_ptr1);
	        tmp_ptr2=strtok(tmp_ptr1," "); // Removes blankspces if any
	  	fprintf(stderr,"\ntmp_ptr2\n\n\t");
	        puts(tmp_ptr2);
	        filePath=strpbrk(tmp_ptr2,"/");
	      	fprintf(stderr,"\ntmp File Path\n\n\t");
	        puts(filePath);
   		      // Copying the file loaction that the client want to access and break at / and copy after that
		strcpy(tmp_ptr3,tmp_ptr2);//coping tmp_ptr2 value to tmp_ptr3
     	   	fprintf(stderr,"\ntmp_ptr3\n\n\t");
		puts(tmp_ptr3);    

		if (strcmp(filePath,"/")==0)// When GET request sends "/" we map to "/index.html"
		{
			strcpy(filePath,strcat(doc_root,"/index.html"));
		 	
	 	}//strcmp if	 	
	 	else
	 	{
	 		 
		        tmp_ptr5=strrchr(tmp_ptr3,'/');//Copies the file name reuested.
	                fprintf(stderr,"\ntmp_ptr5\n\n\t");
		        puts(tmp_ptr5);
		        fprintf(stderr,"\n File name: %s\n",tmp_ptr5);
		        strcpy(tmp_ptr8,tmp_ptr5);//Copying the value of tmp_ptr5 to tmp_ptr8.
		        tmp_ptr4=strtok(tmp_ptr3,"/");//Removes any "/".
		        fprintf(stderr,"\ntmp_ptr4\n\n\t");
		        puts(tmp_ptr4);
		        fprintf(stderr,"\n File name: %s\n",tmp_ptr4);
		        strcpy(filePath,strcat(doc_root,filePath));//Adding the requested file.
		       
			printf("\nRequest File path\n\n\t");
	 		puts(filePath);
	 		
	 		
	 	}//else strcmp
	 	
		//strcpy(tmp_ptr8,strcat(doc_root,tmp_ptr4));
		//printf("\nNew Request File path\n\n\t");
	 	//	puts(tmp_ptr8);
		FILE *fp;
		fp=fopen(filePath,"r");
		ifp=open(filePath,O_RDONLY);
	 	printf("\nRequested File status\n\n\t");
	 	
	 	if(fp!=NULL)
	 	{
	 		fileOK=1;
	 		printf("Requested file exist in the server.\n");
	 	}//if
	 	else 
	 	{
	 		printf("Requested file does exist in the server.\n");
	 	}
	 		
		if(fileOK==1)
		{
		
			f_size= http_get_filesize(fp);// Calculating the Filesize of the requested file.
		 	fclose (fp);
		
	
			ftype=strrchr(filePath,z);
			printf("\nRequested file is of type:\t");
			puts(ftype);
		
			// MIME recognition 
			if (ftype == NULL) 
			{
				type= "application/octet-stream";
			} 
			else if((strcmp(ftype,".htm"))==0 || (strcmp(ftype,".html"))==0)
			{
				type="text/html";
			} 
			else if (strcmp(ftype, ".jpg") == 0)
			{
				type= "image/jpeg";
			} 
			else if (strcmp(ftype, ".gif") == 0) 
			{
				type= "image/gif";
			} 
			else 
			{
				type= "application/octet-stream";
			}
		
		
			sprintf(header,"HTTP/1.1 %s\r\nServer:207httpd/0.0.1\r\nConnention:Close\r\nContent-Type:%s\r\nContent-Length:%d\r\n\n",status1,type,f_size);
			printf("\nHeader sent to client as response 1:\n\n");		
			puts(header);
			send(th,header,strlen(header),0);
			printf("Body sent to client as response:\n\n");		
			while((dot = read(ifp,line,sizeof(line)))>0)
			{		
				send(th,line,dot,0);
			}
			read(ifp,line,sizeof(line));
			//puts(line);
		}//if file ok
		
		//close(ifp);	
		else if(fileOK==0)
		{
			int syn,portnum;// fd will be used as a socket descriptor
			struct sockaddr_in client;
			//socklen_t c_len;
			char buf4[6500];// Unbounded buffer. We transfer byte by byte 
			bzero(buf4, sizeof(buf4));
		
			//Typecast port number from char to int
			int nportnum=8000;
		
			// Creating socket and socket descriptor
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

			// Read data byte by byte and send it to the server making data unbounded
			//while(read(0,&buffer,1)>0)// First 0 indicates stdin
		//{	//whats this for?
	
	

			send(nsfd,buf,strlen(buf1),0);
			//puts(header);
	
			int rcvs;
			/*
			while((rcvs = recv(nsfd,buf3,1,0)) > 0)
			{
				
			}
			printf("\n\nI got -------> %s", buf3);
			*/
		
			FILE *fp1,*fp2;
			int ifp1,ifp2;	
			int ret;
			ret = chdir (demoRpath);
			printf("\n %d",ret);
			char cwd[1024];
			getcwd(cwd, sizeof(cwd));
		    	printf("\nCurrent working dir: %s\n", cwd);
		    	int ret1;
    	
    	
		    	//strcpy(tmp_ptr9,filePath);
    			tmp_ptr10=strrchr(tmp_ptr2,'/');
		    	int a2=tmp_ptr10-tmp_ptr2+1;
    	
    			memcpy(tmp_ptr8,tmp_ptr2,a2-1);	
			ret1=chdir(tmp_ptr8);	
			printf("ret1%d",ret1);
			puts(tmp_ptr8);
		    	//puts(tmp_ptr10);    	
		    	//int chk1=chdir(filePath);
    
    			if(ret1==-1)
		    	{
		    		bzero(CREATE_DIR, sizeof(CREATE_DIR));  
		  		strcpy(CREATE_DIR, "mkdir -p ");
		  		printf("ret 1:%d ",ret1);
		    		strcat(CREATE_DIR, tmp_ptr8);
		                int status = system(CREATE_DIR);
			        printf("\nMKDIR returns: %d\n" ,status);
		    	}
		
		    	strcpy(tmp_ptr11,doc_root);
		    	printf("\ntmp_ptr11 after copy: ");
		    	puts(tmp_ptr11);
    	
		    	//int ret3=chdir(demoRpath);
  		
		    	//printf("ret 3%d",ret3);
		    	
		    	//printf("\n %d",chk1);
		    	
		    	/*Create file path system here*/
    	
    	
    	
		    	//if 
    	
    	
		 	fp1=fopen(tmp_ptr11,"w");    // file creating in mkdir or chdir	  	
			int rcv;
			while((rcv = recv(nsfd,buf3,1,0)) > 0)
			{
				fwrite(buf3, sizeof(char), 1, fp1);
			}
			 	//
			 	//
			 //puts(buf3);
			fclose(fp1);
			fp2=fopen(tmp_ptr11,"r+");		
			//close(th);
			f_size1= http_get_filesize(fp2);
			sprintf(header1,"HTTP/1.1 %s\r\nServer: Cache-Proxy Server/0.0.1\r\nConnention:Close\r\nContent-Type:%s\r\nContent-Length:%d\r\n\n",status1,type,f_size1);
			printf("\nHeader sent to client as response 2:\n\n");			
			fread(buf4, sizeof(char), sizeof(buf4), fp2);
			puts(buf4);
			
			/*Sending it back to the client*/
			send (th,header1,strlen(header1),0);
			send (th,buf4,sizeof(buf4),0);
		
			#if 0
			int snd;
			while( (fread(buf4, sizeof(char), 1, fp2))>0)
			{
				send(th, buf4, 1, 0);
				//puts(buf4);	
			}
			#endif


			int rcvs2;
			/*while(1)
			{
						
				printf("\n Before");
				read(nsfd,buf10,100);
		
				puts(buf10);
				sleep(10);
				printf("\n After/");
				
				if (rcvs2>0)
				break;
			}*/
			printf("\n\nI got 2 -------> %s", buf4);
			//puts(buf3);
			//fclose(fp2);
			//close();		
			//		fread(buf4,sizeof(char), sizeof(buf4), fp2);
			//send (th,buf4,strlen(buf4),0);
			//	
			//		send (th,buf3,strlen(buf3),0);
			//write(fd,buffer,1);
	//}
			//close(th);
			//close(nsfd);	
			//close(fd);
			//return 0;
			//int nsfd1;
			//nsfd1=socket(AF_INET,SOCK_STREAM,0);

		//sprintf(header,"HTTP/1.1 %s\r\nServer:207httpd/0.0.1\r\nConnention:Close\r\n\n<html><head><title>404 Page Not Found</title></head><body><h1>404 Page Not Found</h1><p>Snap! We lost the page somewhere.We sent the batallion out to search for the page. Meanwhile go to homepage and look around other pages, we will find the page you wanted ASAP!</p>   </body></html>\r\n",status);
		//send(th,header,strlen(header),0);

	 		
		}//fileokayelse
		
		//printf("\n\nAll requests handled and responses sent and waiting for client Requests.\n");		

		//close(ifp);
	}//end of if

	else
	{
		printf("\nFoucking Shit!");
					
#if 1			
			printf("\n\nThe BUFFER is %s", buf);
			int rcvs;
			char file_buf[4000], file_path[200], new_buf[500];
			char *find_len, *find_len_newbuf;
			find_len = file_path;
			find_len_newbuf = new_buf;
			bzero(file_buf, sizeof(file_buf));
			bzero(file_path, sizeof(file_path));
			printf("\n\nCLEARED BUFFERS");
			sprintf(file_path, "%s", rootPath);
			//printf("\nThe file_path is %s", file_path);
			//printf("\n @@@The len1 is %d",strlen(find_len));
			//printf("\n @@@The len2 is %d",strlen(find_len_newbuf));
			
			memcpy(&new_buf[0], buf, sizeof(buf));
			memcpy(&file_path[strlen(find_len)], new_buf, strlen(find_len_newbuf)-1);
			printf("\n\nTHE FINAL FILE PATH is %s", file_path);
	


			/*stripping file name for the directory*/
			char *dir_path, store_dir_path[300], *dir_store;
			bzero(store_dir_path, sizeof(store_dir_path));
			dir_path = strrchr(file_path, '/');
			puts(dir_path);
			snprintf(store_dir_path, (strlen(file_path)-strlen(dir_path)+1), "%s", file_path);
			printf("\nThe DIR is: %s", store_dir_path);
			
			/*Check for that directory*/
			FILE *fp3,*fp4;
			int ifp3,ifp4;	
			int ret;
			char buf25[10];
			bzero(buf25, sizeof(buf25));
			ret = chdir(store_dir_path);
			printf("\nThe return value is %d",ret);
			if(ret == 0)
			{
				//receive the file
				puts("\nIn ret");
				if((fp3=fopen(file_path,"w+"))>0)    // file creating in mkdir or chdir
				{
					//printf("\nThe file path inside chdir is %s", file_path);	  	
					puts("\nFILE PATH: ");
					puts(file_path);		
					int rv;
					//while((rv = read(th,buf25,1)) > 0)
					{
						puts("GGG ");
						//printf("%d", rv);
						//read(th,buf25,10);
						sprintf(buf25, "%s", "HELLO HOW");						
//						fread(buf25, 1, 10, th);
						puts("\nPRINTED");
						fwrite(buf25, 1, 10, fp3);
						puts(buf25);
					}
				}
			}
			fclose(fp3);

#if 0
		    	//strcpy(tmp_ptr9,filePath);
    			tmp_ptr10=strrchr(tmp_ptr2,'/');
		    	int a2=tmp_ptr10-tmp_ptr2+1;
    	
    			memcpy(tmp_ptr8,tmp_ptr2,a2-1);	
			ret1=chdir(tmp_ptr8);	
			printf("ret1%d",ret1);
			puts(tmp_ptr8);
		    	//puts(tmp_ptr10);    	
		    	//int chk1=chdir(filePath);
    
    			if(ret1==-1)
		    	{
		    		bzero(CREATE_DIR, sizeof(CREATE_DIR));  
		  		strcpy(CREATE_DIR, "mkdir -p ");
		  		printf("ret 1:%d ",ret1);
		    		strcat(CREATE_DIR, tmp_ptr8);
		                int status = system(CREATE_DIR);
			        printf("\nMKDIR returns: %d\n" ,status);
		    	}
		
		    	strcpy(tmp_ptr11,doc_root);
		    	printf("\ntmp_ptr11 after copy: ");
		    	puts(tmp_ptr11);
    	
		    	//int ret3=chdir(demoRpath);
  		
		    	//printf("ret 3%d",ret3);
		    	
		    	//printf("\n %d",chk1);
		    	
		    	/*Create file path system here*/
    	
    	
    	
		    	//if 
    	
    	
		 	fp1=fopen(tmp_ptr11,"w");    // file creating in mkdir or chdir	  	
			int rcv;
			while((rcv = recv(nsfd,buf3,1,0)) > 0)
			{
				fwrite(buf3, sizeof(char), 1, fp1);
			}
			 	//
			 	//
			 //puts(buf3);
			fclose(fp1);
			#endif

	}//endofelse
}//http_req
#endif
#endif //latest block - 1

/*=========================== Handling HTTP Requests- Start===========================*/


/*=========================== Main function- Start ===========================*/
int main(int argc, char *argv[])
{

	int fd,nfd,th_id,portnum;// fd will be used as a socket descriptor
	struct sockaddr_in client,server;
	socklen_t c_len;
	pthread_t p_thread[3000];// Number of slaves threads
	pthread_attr_t attr_t;// Attritube to thread
	pthread_attr_init(&attr_t);// Initilizing the attribute
	pthread_attr_setdetachstate(&attr_t, PTHREAD_CREATE_DETACHED); //Creating the tread in detach state
	int i=0;
	portnum = 8001;
	rootPath= argv[1];
	
	// Creating socket and socket descriptor
	fd=socket(AF_INET,SOCK_STREAM,0);// TCP uses stream so SOCK_STREAM
	//fprintf(stderr,"Socket server");
	// Try opening the socket
	if(fd<0)
	{
		
		errormsg("[ERROR:] Cannot open the socket: ");	
	}
	int enable=1;
	//Assigning the PORT IP and FAMILY
	if(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(int))==-1){
	errormsg("[ERROR:]");
	exit(1);
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;// IP
	server.sin_port=htons(portnum);//PORT
	//fprintf(stderr,"Family server");
	if (bind(fd, (struct sockaddr *) &server,sizeof(server)) < 0) 
	

              errormsg("[ERROR:] Binding");
              //fprintf(stderr,"Bind server");
	// Listen 
     	listen(fd,5);
	c_len = sizeof(client);
	char cpt[c_len];


	//Accept the connection from client     
	while(1)
	    {
	     nfd = accept(fd,(struct sockaddr *) &client,&c_len);
	inet_ntop(AF_INET, &(client.sin_addr.s_addr),cpt, c_len);// to display the client details we convert Network to Presntation 
		printf("\nConnected client details\n\n\tClient Address: %s\n",cpt);
	
		fprintf(stderr,"\tClient Port: %i \n\n",client.sin_port);
	     if (nfd < 0) 
		  {
		errormsg("[ERROR:] Accepting client");
		continue;
     		  }
	
     	th_id = pthread_create(&p_thread[i++],&attr_t,http_req,(void*)&nfd);
	//pthread_create(&p_thread[i+3], &attr_t, (void *)sync_server_files_cont, NULL);
     	pthread_attr_destroy(&attr_t);// Destroying the thread attribute
     	
     	
     	

             }
        }
 /*=========================== Main function- End ===========================*/      
 	/*=========================== END ===========================*/
       
