/**
*   https://curl.se/libcurl/c/example.html
*   https://github.com/MaaSTaaR/SSFS
*   https://stackoverflow.com/questions/4224722/curl-writefunction-and-classes
*   https://github.com/MaaSTaaR/LSYSFS
*   https://www.geeksforgeeks.org/implement-itoa/
*   https://www.cs.hmc.edu/~geoff/classes/hmc.cs135.201001/homework/fuse/fuse_doc.html
*  It is a part of a tutorial in MQH Blog with the title "Writing Less Simple, Yet Stupid Filesystem Using FUSE in C":
http://maastaar.net/fuse/linux/filesystem/c/2019/09/28/writing-less-simple-yet-stupid-filesystem-using-FUSE-in-C/
**/
 
// importing libraries
#define FUSE_USE_VERSION 30

#include <curl/curl.h>
#include <limits.h>
#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

// For storing the information 
char USERNAME[255];
char PASSWORD[255];
char HOST[255];
int PORT;

// Storing the folder related information
int arr[100];

// Sample Send Message taken from smtp-libcurl examples.
#define FROM_ADDR    "verma.preetesh10mail@gmail.com"
#define TO_ADDR      "verma.preetesh21@gmail.com"
 
#define FROM_MAIL "Sender Person " FROM_ADDR
#define TO_MAIL   "A Receiver " TO_ADDR
 
static const char *payload_text =
  "Date: Mon, 29 Nov 2010 21:54:29 +1100\r\n"
  "To: " TO_MAIL "\r\n"
  "From: " FROM_MAIL "\r\n"
  "Message-ID: <dcd7cb36-11db-487a-9f3a-e652a94f58efd@"
  "rfcpedant.example.org>\r\n"
  "Subject: smtp Message code\r\n"
  "\r\n" /* empty line to divide headers from body, see RFC5322 */
  "The body of the message starts here.\r\n"
  "\r\n"
  "It could be a lot of lines, could be MIME encoded, whatever.\r\n"
  "Check RFC5322.\r\n";
 
// Structure declaration for sending mail
struct upload_status {
  size_t bytes_read;
};
 
static size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp)
{
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;
  size_t room = size * nmemb;
 
  if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
    return 0;
  }
 
  data = &payload_text[upload_ctx->bytes_read];
 
  if(data) {
    size_t len = strlen(data);
    if(room < len)
      len = room;
    memcpy(ptr, data, len);
    upload_ctx->bytes_read += len;
 
    return len;
  }
 
  return 0;
}

// String structure
struct string {
  char *ptr;
  size_t len;
}string;
//  https://www.geeksforgeeks.org/implement-itoa/
void reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        char a=*(str+start);
        *(str+start)= *(str+end);
         *(str+end)=a;
        start++;
        end--;
    }
}
//  https://www.geeksforgeeks.org/implement-itoa/
char* itoa(int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; 
    reverse(str, i);
    return str;
}

// Function to initialize my string structure
void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

// Following function extracts characters pretrash in `src`
// between `m` and `n` (excluding `n`)
char* substr(const char *src, int m, int n)
{
    // get the length of the destination string
    int len = n - m;
 
    // allocate (len + 1) chars for destination (+1 for extra null character)
    char *dest = (char*)malloc(sizeof(char) * (len + 1));
 
    // extracts characters between m'th and n'th index from source string
    // and copy them into the destination string
    for (int i = m; i < n && (*(src + i) != '\0'); i++)
    {
        *dest = *(src + i);
        dest++;
    }
 
    // null-terminate the destination string
    *dest = '\0';
 
    // return the destination string
    return dest - len;
}

// This function helps me write the output from the Search Call into my local variable which later on 
// I use for making other calls;    https://stackoverflow.com/questions/4224722/curl-writefunction-and-classes
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
    size_t new_len = s->len + size*nmemb;
    s->ptr = realloc(s->ptr, new_len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;
    //printf("%ld\n",s->len);
    // Removing the extra unwanted results from the string
    //printf("sfd%s %ld\n", temp,s->len)
    //printf("%s\n",mail_id);
    return size*nmemb;
}

// Here declaring all the necessary folder matrices(It is hardcoded since I was unable to parse the string properly)
char dir_list[ 256 ][ 256 ];
int curr_dir_idx = -1;

char files_list[ 256 ][ 256 ];
int curr_file_idx = -1;

char files_content[ 256 ][ 256 ];
int curr_file_content_idx = -1;

char inbox_files_list[ 256 ][ 256 ];
int curr_inbox_file_idx = -1;

char inbox_files_content[ 256 ][ 256 ];
int curr_inbox_file_content_idx = -1;

char draft_files_list[ 256 ][ 256 ];
int curr_draft_file_idx = -1;

char draft_files_content[ 256 ][ 256 ];
int curr_draft_file_content_idx = -1;

char trash_files_list[ 256 ][ 256 ];
int curr_trash_file_idx = -1;

char trash_files_content[ 256 ][ 256 ];
int curr_trash_file_content_idx = -1;

char important_files_list[ 256 ][ 256 ];
int curr_important_file_idx = -1;

char important_files_content[ 256 ][ 256 ];
int curr_important_file_content_idx = -1;

char starred_files_list[ 256 ][ 256 ];
int curr_starred_file_idx = -1;

char starred_files_content[ 256 ][ 256 ];
int curr_starred_file_content_idx = -1;

char sent_files_list[ 256 ][ 256 ];
int curr_sent_file_idx = -1;

char sent_files_content[ 256 ][ 256 ];
int curr_sent_file_content_idx = -1;

char all_files_list[ 256 ][ 256 ];
int curr_all_file_idx = -1;

char all_files_content[ 256 ][ 256 ];
int curr_all_file_content_idx = -1;

// Adding directory to FUSE
void add_dir( const char *dir_name )
{
	curr_dir_idx++;
	strcpy( dir_list[ curr_dir_idx ], dir_name );
}

// Removing Directory in FUSE
void remove_dir(const char *dir_name)
{
    for(int i=0;i<=curr_dir_idx;i++)
    {
        if(strcmp(dir_list[i],dir_name)==0)
        {
            strcpy(dir_list[i],"\0");
            return;
        }
    }
}

// Removing a Draft File
void remove_draft_dir(const char *file_name)
{
    for(int i=0;i<=curr_dir_idx;i++)
    {
        if(strcmp(draft_files_list[i],file_name)==0)
        {
            strcpy(draft_files_list[i],"\0");
            strcpy(draft_files_content[i],"\0");
            return;
        }
    }
}

// Renaming a Directory in FUSE
void rename_dir(const char *dir_name1,const char *dir_name2)
{
    for(int i=0;i<=curr_dir_idx;i++)
    {
        if(strcmp(dir_list[i],dir_name1)==0)
        {
            strcpy(dir_list[i],"\0");
            strcpy(dir_list[i],dir_name2);
            return;
        }
    }
}

// Checking if it is a Directory
int is_dir( const char *path )
{
	path++; // Eliminating "/" in the path
	
	for ( int curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++ )
		if ( strcmp( path, dir_list[ curr_idx ] ) == 0 )
			return 1;
	
	return 0;
}

/* 
````````````````````````````````````````````
Adding mails in corresponding folders
```````````````````````````````````````````
*/

void inbox_add_file( const char *filename )
{
    //printf("%s",filename);
	curr_inbox_file_idx++;
    char temp[255];
    strcpy(temp,filename);
    char temp2[]=".txt";
    strcat(temp,temp2);
	strcpy( inbox_files_list[ curr_inbox_file_idx ], temp );
	
	curr_inbox_file_content_idx++;
	strcpy( inbox_files_content[ curr_inbox_file_content_idx ], "" );
}

void trash_add_file( const char *filename )
{
    //printf("%s",filename);
	curr_trash_file_idx++;
    char temp[255];
    strcpy(temp,filename);
    char temp2[]=".txt";
    strcat(temp,temp2);
	strcpy( trash_files_list[ curr_trash_file_idx ], temp );
	
	curr_trash_file_content_idx++;
	strcpy( trash_files_content[ curr_trash_file_content_idx ], "" );
}

void starred_add_file( const char *filename )
{
    //printf("%s",filename);
	curr_starred_file_idx++;
    char temp[255];
    strcpy(temp,filename);
    char temp2[]=".txt";
    strcat(temp,temp2);
	strcpy( starred_files_list[ curr_starred_file_idx ], temp );
	
	curr_starred_file_content_idx++;
	strcpy( starred_files_content[ curr_starred_file_content_idx ], "" );
}

void draft_add_file( const char *filename )
{
   
	curr_draft_file_idx++;
    char temp[255];
    strcpy(temp,filename);
    char temp2[]=".txt";
    strcat(temp,temp2);
	strcpy( draft_files_list[ curr_draft_file_idx ], temp );
	
	curr_draft_file_content_idx++;
	strcpy( draft_files_content[ curr_draft_file_content_idx ], "" );
        
}

void important_add_file( const char *filename )
{
    //printf("%s",filename);
	curr_important_file_idx++;
    char temp[255];
    strcpy(temp,filename);
    char temp2[]=".txt";
    strcat(temp,temp2);
	strcpy( important_files_list[ curr_important_file_idx ], temp );
	
	curr_important_file_content_idx++;
	strcpy( important_files_content[ curr_important_file_content_idx ], "" );
}

void sent_add_file( const char *filename )
{ 
    
	curr_sent_file_idx++;
    char temp[255];
    strcpy(temp,filename);
    char temp2[]=".txt";
    strcat(temp,temp2);
	strcpy( sent_files_list[ curr_sent_file_idx ], temp );
	
	curr_sent_file_content_idx++;
	strcpy( sent_files_content[ curr_sent_file_content_idx ], "" );
}

void all_add_file( const char *filename )
{
    //printf("%s",filename);
	curr_all_file_idx++;
    char temp[255];
    strcpy(temp,filename);
    char temp2[]=".txt";
    strcat(temp,temp2);
	strcpy( all_files_list[ curr_all_file_idx ], temp );
	
	curr_all_file_content_idx++;
	strcpy( all_files_content[ curr_all_file_content_idx ], "" );
}

void add_file( const char *filename )
{
	curr_file_idx++;
	strcpy( files_list[ curr_file_idx ], filename );
	
	curr_file_content_idx++;
	strcpy( files_content[ curr_file_content_idx ], "" );
}

/* 
````````````````````````````````````````````
Checking mails in corresponding folders
```````````````````````````````````````````
*/

int is_file( const char *path )
{
	path++; // Eliminating "/" in the path
	
	for ( int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++ )
		if ( strcmp( path, files_list[ curr_idx ] ) == 0 )
			return 1;
	
	return 0;
}

int is_inbox_file( const char *path )
{
	for(int i=1;i<=7;i++)
    path++; // Eliminating "/" in the path
	
	for ( int curr_idx = 0; curr_idx <= curr_inbox_file_idx; curr_idx++ )
		if ( strcmp( path, inbox_files_list[ curr_idx ] ) == 0 )
			return 1;
	
	return 0;
}

int is_important_file( const char *path )
{
    //printf("\nwwwwwwwwwwwwwwwwwwwww%s\n",path);
	for(int i=1;i<=11;i++)
    path++; // Eliminating "/" in the path
	
	for ( int curr_idx = 0; curr_idx <= curr_important_file_idx; curr_idx++ )
		if ( strcmp( path, important_files_list[ curr_idx ] ) == 0 )
			return 1;
	
	return 0;
}

int is_trash_file( const char *path )
{
	for(int i=1;i<=7;i++)
    path++; // Eliminating "/" in the path
	//printf("sew%s",path);
	for ( int curr_idx = 0; curr_idx <= curr_trash_file_idx; curr_idx++ )
		if ( strcmp( path, trash_files_list[ curr_idx ] ) == 0 )
			return 1;
	
	return 0;
}

int is_starred_file( const char *path )
{
	for(int i=1;i<=9;i++)
    path++; // Eliminating "/" in the path
	//printf("sew%s",path);
	for ( int curr_idx = 0; curr_idx <= curr_starred_file_idx; curr_idx++ )
		if ( strcmp( path, starred_files_list[ curr_idx ] ) == 0 )
			return 1;
	
	return 0;
}

int is_sent_file( const char *path )
{
	for(int i=1;i<=10;i++)
    path++; // Eliminating "/" in the path
	//printf("sew%s",path);
	for ( int curr_idx = 0; curr_idx <= curr_sent_file_idx; curr_idx++ )
		if ( strcmp( path, sent_files_list[ curr_idx ] ) == 0 )
			return 1;
	
	return 0;
}

int is_all_file( const char *path )
{
	for(int i=1;i<=9;i++)
    path++; // Eliminating "/" in the path
	//printf("sew%s",path);
	for ( int curr_idx = 0; curr_idx <= curr_all_file_idx; curr_idx++ )
		if ( strcmp( path, all_files_list[ curr_idx ] ) == 0 )
			return 1;
	
	return 0;
}

int is_draft_file( const char *path )
{
    
	 for(int i=1;i<=8;i++)
        path++; // Eliminating "/" in the path
	for ( int curr_idx = 0; curr_idx <= curr_draft_file_idx; curr_idx++ )
    {
		if ( strcmp( path, draft_files_list[ curr_idx ] ) == 0 )
    		return 1;
    }
	return 0;
}

// Getting the mail indices of mails in the folders
int get_file_index( const char *path )
{
	path++; // Eliminating "/" in the path
	
	for ( int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++ )
		if ( strcmp( path, files_list[ curr_idx ] ) == 0 )
			return curr_idx;
	
	return -1;
}

int get_sent_file_index( const char *path )
{
	path++; // Eliminating "/" in the path
    char ttt[255];
    strcpy(ttt,path);
    char* tt=".txt";
    strcat(ttt,tt);
	//printf("%s",path);
	for ( int curr_idx = 0; curr_idx <= curr_sent_file_idx; curr_idx++ )
		if ( strcmp( ttt, sent_files_list[ curr_idx ] ) == 0 )
			return curr_idx;
	//printf("wewewewew");
	return -1;
}

// Write Function implementation    https://github.com/MaaSTaaR/LSYSFS
void write_to_file( const char *path, const char *new_content )
{
	int file_idx = get_file_index( path );
	int z=get_sent_file_index(path);
    //printf("\newseweeweeeewewewew%d  %s\n",curr_sent_file_content_idx,path);
	if ( file_idx == -1 ) // No such file
		return;
	strcpy(sent_files_content[curr_sent_file_content_idx],new_content);
	strcpy( files_content[ file_idx ], new_content ); 
}

// Main functions of FUSE   https://github.com/MaaSTaaR/LSYSFS
static int do_getattr( const char *path, struct stat *st )
{
	st->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
	st->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
	st->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
	st->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now
	
	if ( strcmp( path, "/" ) == 0 || is_dir( path ) == 1 )
	{
		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
	}
	else if ( is_file( path ) == 1 )
	{
		st->st_mode = S_IFREG | 0644;
		st->st_nlink = 1;
		st->st_size = 1024;
	}
	else if ( strcmp( path, "/INBOX" ) == 0 || is_dir( path ) == 1 )
	{
		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
	}
	
	else if ( is_inbox_file( path ) == 1 )
	{
		st->st_mode = S_IFREG | 0644;
		st->st_nlink = 1;
		st->st_size = 1024;
	}

    else if ( strcmp( path, "/Trash" ) == 0 || is_dir( path ) == 1 )
	{
		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
	}
	
	else if ( is_trash_file( path ) == 1 )
	{
		st->st_mode = S_IFREG | 0644;
		st->st_nlink = 1;
		st->st_size = 1024;
	}

    else if ( strcmp( path, "/Drafts" ) == 0 || is_dir( path ) == 1 )
	{
		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
	}

	else if ( is_draft_file( path ) == 1 )
	{
		st->st_mode = S_IFREG | 0644;
		st->st_nlink = 1;
		st->st_size = 1024;
	}

	else if ( strcmp( path, "/Important" ) == 0 || is_dir( path ) == 1 )
	{
		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
	}

	else if ( is_important_file( path ) == 1 )
	{
		st->st_mode = S_IFREG | 0644;
		st->st_nlink = 1;
		st->st_size = 1024;
	}

	else if ( strcmp( path, "/Starred" ) == 0 || is_dir( path ) == 1 )
	{
		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
	}

	else if ( is_starred_file( path ) == 1 )
	{
		st->st_mode = S_IFREG | 0644;
		st->st_nlink = 1;
		st->st_size = 1024;
	}

    else if ( strcmp( path, "/SentMail" ) == 0 || is_dir( path ) == 1 )
	{
		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
	}

	else if ( is_sent_file( path ) == 1 )
	{
		st->st_mode = S_IFREG | 0644;
		st->st_nlink = 1;
		st->st_size = 1024;
	}

     else if ( strcmp( path, "/AllMail" ) == 0 || is_dir( path ) == 1 )
	{
		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
	}

	else if ( is_all_file( path ) == 1 )
	{
		st->st_mode = S_IFREG | 0644;
		st->st_nlink = 1;
		st->st_size = 1024;
	}

	else
	{
		return -ENOENT;
	}
	
	return 0;
}

// Main Fuctions of FUSE    https://github.com/MaaSTaaR/LSYSFS
static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
{
	if ( strcmp( path, "/" ) == 0 ) // If the user is trying to show the files/directories of the root directory show the following
	{
		filler( buffer, ".", NULL, 0 ); // Current Directory
		filler( buffer, "..", NULL, 0 ); // Parent Directory
		for ( int curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++ )
			filler( buffer, dir_list[ curr_idx ], NULL, 0 );
	
		for ( int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++ )
			filler( buffer, files_list[ curr_idx ], NULL, 0 );
	}

	if ( strcmp( path, "/INBOX" ) == 0 )
	{
		for ( int curr_idx = 0; curr_idx <= curr_inbox_file_idx; curr_idx++ )
			filler( buffer, inbox_files_list[ curr_idx ], NULL, 0 );
	}
	
    if ( strcmp( path, "/Trash" ) == 0 )
	{
		for ( int curr_idx = 0; curr_idx <= curr_trash_file_idx; curr_idx++ )
			filler( buffer, trash_files_list[ curr_idx ], NULL, 0 );
	}

    if ( strcmp( path, "/Starred" ) == 0 )
	{
		for ( int curr_idx = 0; curr_idx <= curr_starred_file_idx; curr_idx++ )
			filler( buffer, starred_files_list[ curr_idx ], NULL, 0 );
	}

    if ( strcmp( path, "/SentMail" ) == 0 )
	{
		for ( int curr_idx = 0; curr_idx <= curr_sent_file_idx; curr_idx++ )
			filler( buffer, sent_files_list[ curr_idx ], NULL, 0 );
	}

    if ( strcmp( path, "/AllMail" ) == 0 )
	{
		for ( int curr_idx = 0; curr_idx <= curr_all_file_idx; curr_idx++ )
			filler( buffer, all_files_list[ curr_idx ], NULL, 0 );
	}

    if ( strcmp( path, "/Drafts" ) == 0 )
	{
		for ( int curr_idx = 0; curr_idx <= curr_draft_file_idx; curr_idx++ )
			filler( buffer, draft_files_list[ curr_idx ], NULL, 0 );
	}

    if ( strcmp( path, "/Important" ) == 0 )
	{
		for ( int curr_idx = 0; curr_idx <= curr_important_file_idx; curr_idx++ )
			filler( buffer, important_files_list[ curr_idx ], NULL, 0 );
	}

	return 0;
}

// Read Function implementation     https://github.com/MaaSTaaR/LSYSFS
static int do_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
	int file_idx = get_file_index( path );
	
	if ( file_idx == -1 )
		return -1;
	
	char *content = files_content[ file_idx ];
	
	memcpy( buffer, content + offset, size );
		
	return strlen( content ) - offset;
}

// Creating Directory
static int do_mkdir( const char *path, mode_t mode )
{
	path++;
	add_dir( path );
	CURL *curl;
  	CURLcode res = CURLE_OK;
 
  	curl = curl_easy_init();
  	if(curl) {
    /* Set username and password */
		curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
	
		/* This is just the server URL */
        char url[255]="imaps://";
        strcat(url, HOST);
        strcat(url, ":");
        char part[100];
        strcat(url, itoa(PORT, part, 10));
        curl_easy_setopt(curl, CURLOPT_URL, url);
		//curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com:993/");
		char urls[255]="CREATE ";
		strcat(urls, path);
 		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, urls);
		/* Set the CREATE command specifying the new folder name */
		//curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "CREATE FOLDER");
	
		/* Perform the custom request */
		res = curl_easy_perform(curl);
	
		/* Check for errors */
		if(res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		
		/* Always cleanup */
		curl_easy_cleanup(curl);
	}
}

// Removing Directory
static int do_rmdir(const char *path)
{
	path++;
	//printf("The path is: %s\n",path);
	remove_dir(path);
	curr_dir_idx--;
	CURL *curl;
	CURLcode res = CURLE_OK;
	
	curl = curl_easy_init();
	if(curl) {
		/* Set username and password */
		curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
	
		/* This is just the server URL */
        char url[255]="imaps://";
        strcat(url, HOST);
        strcat(url, ":");
        char part[100];
        strcat(url, itoa(PORT, part, 10));
        curl_easy_setopt(curl, CURLOPT_URL, url);
		//curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com:993/");
		char urls[255]="DELETE ";
		strcat(urls, path);
 		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, urls);
		/* Set the DELETE command specifying the existing folder */
		//curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE FOLDER");
	
		/* Perform the custom request */
		res = curl_easy_perform(curl);
	
		/* Check for errors */
		if(res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
	
		/* Always cleanup */
		curl_easy_cleanup(curl);
	}
	return 0;
}

// Creating Files
static int do_mknod( const char *path, mode_t mode, dev_t rdev )
{
	path++;
	add_file( path );
	//sent_add_file(path);
    curr_sent_file_idx++;
    char temp[255];
    strcpy(temp,path);
	strcpy( sent_files_list[ curr_sent_file_idx ], temp );
	curr_sent_file_content_idx++;
	strcpy( sent_files_content[ curr_sent_file_content_idx ], "" );
	return 0;
}

// writing to files
static int do_write( const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *info )
{
	write_to_file( path, buffer );
	CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx = { 0 };
    
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM_ADDR);
    
        recipients = curl_slist_append(recipients, TO_ADDR);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        res = curl_easy_perform(curl);
    
        if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    
        curl_slist_free_all(recipients);

        curl_easy_cleanup(curl);
    }
	return size;
}

// Unlinking Files
static int do_unlink(const char *path)
{
	path++;
	//printf("The path is: %s\n",path);
	remove_draft_dir(path);
	curr_draft_file_idx--;
    curr_draft_file_content_idx--;
	CURL *curl;
    CURLcode res = CURLE_OK;
 
    curl = curl_easy_init();
    if(curl) {
        /* Set username and password */
        curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
    
        /* This is the mailbox folder to select */
        curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com:993/%5BGmail%5D/Drafts");
    
        /* Set the STORE command with the Deleted flag for message 1. Note that
        * you can use the STORE command to set other flags such as Seen, Answered,
        * Flagged, Draft and Recent. */
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "STORE 2 +Flags \\Deleted");
    
        /* Perform the custom request */
        res = curl_easy_perform(curl);
    
        /* Check for errors */
        if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        else {
        /* Set the EXPUNGE command, although you can use the CLOSE command if you
        * do not want to know the result of the STORE */
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "EXPUNGE");
    
        /* Perform the second custom request */
        res = curl_easy_perform(curl);
    
        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }
    
        /* Always cleanup */
        curl_easy_cleanup(curl);
    }
        return 0;
}

// Renaming Files
static int do_mv(const char *path1,const char *path2)
{
    // printf("HERERE\n");
    CURL *curl;
    CURLcode res = CURLE_OK;
    path1++;
    path2++;
    rename_dir(path1,path2);
    curl = curl_easy_init();
    if(curl) {
        /* Set username and password */
        curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
    
        /* This is just the server URL */
        char url[255]="imaps://";
        strcat(url, HOST);
        strcat(url, ":");
        char part[100];
        strcat(url, itoa(PORT, part, 10));
        curl_easy_setopt(curl, CURLOPT_URL, url);
        //curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com:993/");
        char urls[255]="RENAME ";
		strcat(urls, path1);
        strcat(urls," ");
        strcat(urls, path2);
        // printf("%s\n",url);
        /* Set the CREATE command specifying the new folder name */
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, urls);
    
        /* Perform the custom request */
        res = curl_easy_perform(curl);
    
        /* Check for errors */
        if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    
        /* Always cleanup */
        curl_easy_cleanup(curl);
    }
    return 0;
}

// Fuse Opertations List   https://github.com/MaaSTaaR/LSYSFS
static struct fuse_operations operations = {
    .getattr	= do_getattr,
    .readdir	= do_readdir,
    .read		= do_read,
    .mkdir		= do_mkdir,
    .mknod		= do_mknod,
    .write		= do_write,
	.rmdir		= do_rmdir,
    .rename     = do_mv,
    .unlink     = do_unlink,
};

/* 
````````````````````````````````````````````
Getting mails in corresponding folders
```````````````````````````````````````````
*/
void get_inbox_subjects(int k)
{
    CURL *curl;
    CURLcode res = CURLE_OK;
    for(int LOOP=1;LOOP<=arr[k];LOOP++){
        curl = curl_easy_init();
        if(curl) {
            struct string st;
            init_string(&st);
            /* Set username and password */
            curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
            curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
            char result[50];
            sprintf(result, "%d", LOOP);
            char temp[]="imaps://imap.gmail.com:993/INBOX;MAILINDEX=";
            char *temp2=";SECTION=HEADER.FIELDS%20(SUBJECT)";
            strcat(temp,result);
            strcat(temp,temp2);
            //printf("%s\n",temp);
            curl_easy_setopt(curl, CURLOPT_URL,temp);
            curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
            // So writing the output into a local structure
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &st);
            res = curl_easy_perform(curl);
            /* Check for errors */   
            char *ptr = st.ptr;
            char word[255];
            int count=0;
            int start=0;
            int leng=strlen(ptr);
            for(int i=9;i<leng-3;i++)
            {
                if(st.ptr[i]=='\n' || st.ptr[i]==' ')
                    continue;
                word[start++]=st.ptr[i];
            }
            word[start]='\0';
            if(start>4)
            {
                inbox_add_file(word);
                all_add_file(word);
            }
            curl_easy_cleanup(curl);
            
        }
    }
}


void get_draft_subjects(int k)
{
CURL *curl;
    CURLcode res = CURLE_OK;
    for(int LOOP=1;LOOP<=arr[k];LOOP++){
        curl = curl_easy_init();
        if(curl) {
            struct string st;
            init_string(&st);
            /* Set username and password */
            curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
            curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
            char result[50];
            sprintf(result, "%d", LOOP);
            char temp[]="imaps://imap.gmail.com:993/%5BGmail%5D/Drafts;MAILINDEX=";
            char *temp2=";SECTION=HEADER.FIELDS%20(SUBJECT)";
            strcat(temp,result);
            strcat(temp,temp2);
            //printf("%s\n",temp);
            curl_easy_setopt(curl, CURLOPT_URL,temp);
            curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
            // So writing the output into a local structure
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &st);
            res = curl_easy_perform(curl);
            /* Check for errors */   
            char *ptr = st.ptr;
            char word[255];
            int count=0;
            int start=0;
            int leng=strlen(ptr);
            for(int i=9;i<leng-3;i++)
            {
                if(st.ptr[i]=='\n' || st.ptr[i]==' ')
                    continue;
                word[start++]=st.ptr[i];
            }
            word[start]='\0';
            if(start>4)
            {
                draft_add_file(word);
                all_add_file(word);
            }
            curl_easy_cleanup(curl);
            
        }
    }
}

void get_trash_subjects(int k)
{
CURL *curl;
    CURLcode res = CURLE_OK;
    for(int LOOP=1;LOOP<=arr[k];LOOP++){
        curl = curl_easy_init();
        if(curl) {
            struct string st;
            init_string(&st);
            /* Set username and password */
            curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
            curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
            char result[50];
            sprintf(result, "%d", LOOP);
            char temp[]="imaps://imap.gmail.com:993/%5BGmail%5D/Trash;MAILINDEX=";
            char *temp2=";SECTION=HEADER.FIELDS%20(SUBJECT)";
            strcat(temp,result);
            strcat(temp,temp2);
            //printf("%s\n",temp);
            curl_easy_setopt(curl, CURLOPT_URL,temp);
            curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
            // So writing the output into a local structure
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &st);
            res = curl_easy_perform(curl);
            /* Check for errors */   
            char *ptr = st.ptr;
            char word[255];
            int count=0;
            int start=0;
            int leng=strlen(ptr);
            for(int i=9;i<leng-3;i++)
            {
                if(st.ptr[i]=='\n' || st.ptr[i]==' ')
                    continue;
                word[start++]=st.ptr[i];
            }
            word[start]='\0';
            if(start>4)
            {
                trash_add_file(word);
                all_add_file(word);
            }
            curl_easy_cleanup(curl);
            
        }
    }
}

void get_important_subjects(int k)
{
CURL *curl;
    CURLcode res = CURLE_OK;
    for(int LOOP=1;LOOP<=arr[k];LOOP++){
        curl = curl_easy_init();
        if(curl) {
            struct string st;
            init_string(&st);
            /* Set username and password */
            curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
            curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
            char result[50];
            sprintf(result, "%d", LOOP);
            char temp[]="imaps://imap.gmail.com:993/%5BGmail%5D/Important;MAILINDEX=";
            char *temp2=";SECTION=HEADER.FIELDS%20(SUBJECT)";
            strcat(temp,result);
            strcat(temp,temp2);
            //printf("%s\n",temp);
            curl_easy_setopt(curl, CURLOPT_URL,temp);
            curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
            // So writing the output into a local structure
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &st);
            res = curl_easy_perform(curl);
            /* Check for errors */   
            char *ptr = st.ptr;
            char word[255];
            int count=0;
            int start=0;
            int leng=strlen(ptr);
            for(int i=9;i<leng-3;i++)
            {
                if(st.ptr[i]=='\n' || st.ptr[i]==' ')
                    continue;
                word[start++]=st.ptr[i];
            }
            word[start]='\0';
            if(start>4)
            {
                important_add_file(word);
                all_add_file(word);
            }
            curl_easy_cleanup(curl);
            
        }
    }
}

void get_starred_subjects(int k)
{
    CURL *curl;
    CURLcode res = CURLE_OK;
    for(int LOOP=1;LOOP<=arr[k];LOOP++){
        curl = curl_easy_init();
        if(curl) {
            struct string st;
            init_string(&st);
            /* Set username and password */
            curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
            curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
            char result[50];
            sprintf(result, "%d", LOOP);
            char temp[]="imaps://imap.gmail.com:993/%5BGmail%5D/Starred;MAILINDEX=";
            char *temp2=";SECTION=HEADER.FIELDS%20(SUBJECT)";
            strcat(temp,result);
            strcat(temp,temp2);
            //printf("%s\n",temp);
            curl_easy_setopt(curl, CURLOPT_URL,temp);
            curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
            // So writing the output into a local structure
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &st);
            res = curl_easy_perform(curl);
            /* Check for errors */   
            char *ptr = st.ptr;
            char word[255];
            int count=0;
            int start=0;
            int leng=strlen(ptr);
            for(int i=9;i<leng-3;i++)
            {
                if(st.ptr[i]=='\n' || st.ptr[i]==' ')
                    continue;
                word[start++]=st.ptr[i];
            }
            word[start]='\0';
            if(start>4)
            {
                starred_add_file(word);
                all_add_file(word);
            }
            curl_easy_cleanup(curl);
            
        }
    }
}

void get_sent_subjects(int k)
{
    CURL *curl;
    CURLcode res = CURLE_OK;
    for(int LOOP=1;LOOP<=arr[k];LOOP++){
        curl = curl_easy_init();
        if(curl) {
            struct string st;
            init_string(&st);
            /* Set username and password */
            curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
            curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
            char result[50];
            sprintf(result, "%d", LOOP);
            char temp[]="imaps://imap.gmail.com:993/%5BGmail%5D/Sent%20Mail;MAILINDEX=";
            char *temp2=";SECTION=HEADER.FIELDS%20(SUBJECT)";
            strcat(temp,result);
            strcat(temp,temp2);
            //printf("%s\n",temp);
            curl_easy_setopt(curl, CURLOPT_URL,temp);
            curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
            // So writing the output into a local structure
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &st);
            res = curl_easy_perform(curl);
            /* Check for errors */   
            char *ptr = st.ptr;
            char word[255];
            int count=0;
            int start=0;
            int leng=strlen(ptr);
            for(int i=9;i<leng-3;i++)
            {
                if(st.ptr[i]=='\n' || st.ptr[i]==' ')
                    continue;
                word[start++]=st.ptr[i];
            }
            word[start]='\0';
            if(start>4)
            {
                sent_add_file(word);
                all_add_file(word);
            }
            curl_easy_cleanup(curl);
            
        }
    }
}

// The main function
int main( int argc, char *argv[] )
{
    // Getting the USERNAME and PASSWORD
    FILE *fp;
    fp = fopen("/home/hp/Desktop/config.txt", "r");
    if (fp == NULL) {
      perror("Failed: ");
      return 1;
    }
    int line=0;
    char buffers[255];
    int bufferlength=255;
    // -1 to allow room for NULL terminator for really long string
    while (fgets(buffers, 255, fp))
    {
        line++;
        int len=strlen(buffers);
        int start=0;
        if(line==1)
        {
            for(int l=0;l<len-1;l++)
            HOST[l]=buffers[l];
        }
        else if(line ==2)
        {
            PORT=atoi(buffers);
        }
        else if(line ==3)
        {
            for(int l=0;l<len-1;l++)
            USERNAME[l]=buffers[l];
        }
        else if(line ==4)
        {
            for(int l=0;l<len;l++)
            PASSWORD[l]=buffers[l];
        }
        // Remove trailing newline
    //    buffers[strcspn(buffers, "\n")] = 0;

    }
    fclose(fp);
    //printf("%s %d %s %s",HOST,PORT,USERNAME,PASSWORD);
//     int ii=0;
//     char delim[] = " ";
//     char *ptr = strtok(buffers, delim);
//     while(ptr != NULL)
//     {
//         //printf("asa%s\n",ptr);
//         char word[255];
//         int start=0;
//         int leng=strlen(ptr);
//         for(int i=0;i<leng+1;i++)
//         {    
//             word[start++]=ptr[i];            
//         }
//         if(ii==0){
//             for(int j=0;j<strlen(word);j++)
//             USERNAME[j]=word[j];
// //                printf("vghb%s",USERNAME);
//             ii++;
//         }
//         else
//         {
//             for(int j=0;j<strlen(word);j++)
//             PASSWORD[j]=word[j];
//         }
//         ptr = strtok(NULL, delim);
//         ii++;
//     }
    printf("Got Username and Password\n");
    FILE *fpt;
    fpt = fopen("folder.txt", "r");
    if (fpt == NULL) {
      perror("Failed: ");
      return 1;
    }

    char buffer[255];
    // -1 to allow room for NULL terminator for really long string
    while (fgets(buffer, 255 - 1, fpt))
    {
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;
    }
    fclose(fpt);
    int iii=0;
    char delima[] = " ";
    char *ptra = strtok(buffer, delima);
    while(ptra != NULL)
    {
        //printf("asa%s\n",ptra);
        char word[255];
        int start=0;
        int leng=strlen(ptra);
        for(int i=0;i<leng+1;i++)
        {    
            word[start++]=ptra[i];            
        }
        int k;
        sscanf(word, "%d", &k);
        arr[iii]=k;
        ptra = strtok(NULL, delima);
        iii++;
    }
    printf("Got count information\n");
    int chars;
    struct string s;
    init_string(&s);
    CURL *curl;
    CURLcode res = CURLE_OK;
    //init_string(st);
    curl = curl_easy_init();
    if(curl) {
        // String initialization
        
        /* Set username and password */
        curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
        char url[255]="imaps://";
        strcat(url, HOST);
        strcat(url, ":");
        char part[100];
        strcat(url, itoa(PORT, part, 10));
        curl_easy_setopt(curl, CURLOPT_URL, url);
    
        /* This will list the folders within the user's mailbox. If you want to
        * list the folders within a specific folder, for example the inbox, then
        * specify the folder as a path in the URL such as /INBOX */
        //curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com:993");
        // So writing the output into a local structure
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        
        /* Perform the list */
        res = curl_easy_perform(curl);
        //printf("%s %ld",s.ptr,s.len);
        
        char delim[] = "\n";
	    char *ptr = strtok(s.ptr, delim);
    	while(ptr != NULL)
	    {
		    //printf("'%s'\n", ptr);
            char word[255];
            int count=0;
            int start=0;
            int leng=strlen(ptr);
            bool open=false;
            for(int i=0;i<leng+1;i++)
            {
                if(ptr[i]=='"')
                count++;
                if(count==3)
                {
                    for(int j=i+1;j<leng+1;j++)
                    {
                        if(ptr[j]==' ')
                        continue;
                        if(ptr[j]=='/')
							continue;
							
						if(ptr[j]=='"')
							break;
                        if(ptr[j]=='[')
                        {
                            open=true;
                            continue;
                        }
                        if(ptr[j]==']')
                        {
                            open=false;
                            continue;
                        }
                        if(!open)
                        word[start++]=ptr[j];
                        
                    }
                    word[start]='\0';
						break;
                }
            }
            if(start!=0)
    			add_dir(word);
		    ptr = strtok(NULL, delim);
	    }
        /* Check for errors */
        if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    
        /* Always cleanup */
        curl_easy_cleanup(curl);
    }
    
    printf("Got Folder information\n");
    printf("Getting Email Info\n");
    // Get the Subjects of the following kinds
    get_inbox_subjects(0);
    get_trash_subjects(2);
    printf("Getting Email Info\n");
    get_important_subjects(1);
    get_draft_subjects(3);
    get_starred_subjects(4);
    get_sent_subjects(5);
    printf("Getting Email Info\n");
    printf("LOADING FILES\n");
	return fuse_main( argc, argv, &operations, NULL );
}


/*

https://curl.se/libcurl/c/example.html
https://github.com/MaaSTaaR/SSFS
https://github.com/MaaSTaaR/LSYSFS
https://www.cs.hmc.edu/~geoff/classes/hmc.cs135.201001/homework/fuse/fuse_doc.html
*/

