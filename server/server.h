#include<stdio.h>


int sync_socket; //socket used in get_file_names_v2.c

char *MIME_type(char *ftype);
void errormsg(const char *msg);
int http_readline(int sock, char *buf, int maxlen);
int http_get_filesize(FILE *fp);
void* http_req(void* data);
void scan_periodically();


