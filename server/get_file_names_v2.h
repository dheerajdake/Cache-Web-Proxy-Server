#include<stdio.h>

#define FILE_COLUMN_SPACING 50

typedef struct FILE_DETAILS
{
    char filename[FILE_COLUMN_SPACING];  //changed from pointer to array
    long long int epoch_time;
    long long int epoch_time_l;
    long long int time_difference;
}FILE_DETAILS;

/*Stores the file names which are modified*/
typedef struct SEND_FILES
{
    char filename[FILE_COLUMN_SPACING];
}SEND_FILES;

/*Stores the file names requested by proxy*/
typedef struct CACHE_FILES
{
    char filename[FILE_COLUMN_SPACING];
}CACHE_FILES;



/*This should be global to be accessed*/
FILE_DETAILS file_details[100]; //supporting 100 files
SEND_FILES send_files[20];  //maximum of 20 file changes in a directory in a cycle
CACHE_FILES cache_files[20];

int track_files(const char *name, const struct stat *status, int type);
int get_file_count();
int search_initial(const char *name, const struct stat *status, int type);
int search_periodic(const char *name, const struct stat *status, int type);
void sync_files(char *filename);
void update_tables();




