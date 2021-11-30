#include<stdio.h>
#include<string.h>
#include <curl/curl.h>
#include<stdlib.h>
#include <errno.h>
#include <limits.h>

char USERNAME[255];
char PASSWORD[255];
char HOST[255];
int PORT;

struct string {
    char *ptr;
    size_t len;
};


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

// Following function extracts characters present in `src`
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
// I use for making other calls;
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
    // Removing the extra unwanted results from the string
    //printf("sfd%s %ld\n", temp,s->len)
    //printf("%s\n",mail_id);
    return size*nmemb;
}
void drafts()
{
    FILE *outfile;
    outfile = fopen ("output.txt", "w");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit (1);
    }
    CURL *curl;
    CURLcode res = CURLE_OK;
  
    curl = curl_easy_init();
    if(curl) {
      // String initialization
      struct string s;
      init_string(&s);
      /* Set username and password */
      curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
      curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
  
      /* This will list the folders within the user's mailbox. If you want to
      * list the folders within a specific folder, for example the inbox, then
      * specify the folder as a path in the URL such as /INBOX */
      curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com:993/"); 
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "EXAMINE %5BGmail%5D/Drafts");
      curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4); 
      // 
      // // So writing the output into a local structure
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
      
      // /* Perform the list */
      res = curl_easy_perform(curl);
      printf("dasa%s %ld",s.ptr,s.len);
      // write struct to file
      fwrite (&s, sizeof(struct string), 1, outfile);
      /* Check for errors */
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
  
      /* Always cleanup */
      curl_easy_cleanup(curl);
      
    }
    // close file
    fclose (outfile);
}
void important()
{
    FILE *outfile;
    outfile = fopen ("output.txt", "w");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit (1);
    }
    CURL *curl;
    CURLcode res = CURLE_OK;
  
    curl = curl_easy_init();
    if(curl) {
      // String initialization
      struct string s;
      init_string(&s);
      /* Set username and password */
      curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
      curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
  
      /* This will list the folders within the user's mailbox. If you want to
      * list the folders within a specific folder, for example the inbox, then
      * specify the folder as a path in the URL such as /INBOX */
      curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com:993/"); 
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "EXAMINE %5BGmail%5D/Important");
      curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4); 
      // 
      // // So writing the output into a local structure
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
      
      // /* Perform the list */
      res = curl_easy_perform(curl);
      printf("dasa%s %ld",s.ptr,s.len);
      // write struct to file
      fwrite (&s, sizeof(struct string), 1, outfile);
      /* Check for errors */
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
  
      /* Always cleanup */
      curl_easy_cleanup(curl);
      
    }
    // close file
    fclose (outfile);
}
void starred()
{
    FILE *outfile;
    outfile = fopen ("output.txt", "w");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit (1);
    }
    CURL *curl;
    CURLcode res = CURLE_OK;
  
    curl = curl_easy_init();
    if(curl) {
      // String initialization
      struct string s;
      init_string(&s);
      /* Set username and password */
      curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
      curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
  
      /* This will list the folders within the user's mailbox. If you want to
      * list the folders within a specific folder, for example the inbox, then
      * specify the folder as a path in the URL such as /INBOX */
      curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com:993/"); 
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "EXAMINE %5BGmail%5D/Starred");
      curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4); 
      // 
      // // So writing the output into a local structure
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
      
      // /* Perform the list */
      res = curl_easy_perform(curl);
      printf("dasa%s %ld",s.ptr,s.len);
      // write struct to file
      fwrite (&s, sizeof(struct string), 1, outfile);
      /* Check for errors */
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
  
      /* Always cleanup */
      curl_easy_cleanup(curl);
      
    }
    // close file
    fclose (outfile);
}
void spam()
{
    FILE *outfile;
    outfile = fopen ("output.txt", "w");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit (1);
    }
    CURL *curl;
    CURLcode res = CURLE_OK;
  
    curl = curl_easy_init();
    if(curl) {
      // String initialization
      struct string s;
      init_string(&s);
      /* Set username and password */
      curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
      curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
  
      /* This will list the folders within the user's mailbox. If you want to
      * list the folders within a specific folder, for example the inbox, then
      * specify the folder as a path in the URL such as /INBOX */
      curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com:993/"); 
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "EXAMINE %5BGmail%5D/Spam");
      curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4); 
      // 
      // // So writing the output into a local structure
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
      
      // /* Perform the list */
      res = curl_easy_perform(curl);
      printf("dasa%s %ld",s.ptr,s.len);
      // write struct to file
      fwrite (&s, sizeof(struct string), 1, outfile);
      /* Check for errors */
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
  
      /* Always cleanup */
      curl_easy_cleanup(curl);
      
    }
    // close file
    fclose (outfile);
}
void trash()
{
    FILE *outfile;
    outfile = fopen ("output.txt", "w");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit (1);
    }
    CURL *curl;
    CURLcode res = CURLE_OK;
  
    curl = curl_easy_init();
    if(curl) {
      // String initialization
      struct string s;
      init_string(&s);
      /* Set username and password */
      curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
      curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
  
      /* This will list the folders within the user's mailbox. If you want to
      * list the folders within a specific folder, for example the inbox, then
      * specify the folder as a path in the URL such as /INBOX */
      curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com:993/"); 
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "EXAMINE %5BGmail%5D/Trash");
      curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4); 
      // 
      // // So writing the output into a local structure
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
      
      // /* Perform the list */
      res = curl_easy_perform(curl);
      printf("dasa%s %ld",s.ptr,s.len);
      // write struct to file
      fwrite (&s, sizeof(struct string), 1, outfile);
      /* Check for errors */
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
  
      /* Always cleanup */
      curl_easy_cleanup(curl);
      
    }
    // close file
    fclose (outfile);
}
void inbox()
{
    FILE *outfile;
    outfile = fopen ("output.txt", "w");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit (1);
    }
    CURL *curl;
    CURLcode res = CURLE_OK;
  
    curl = curl_easy_init();
    if(curl) {
      // String initialization
      struct string s;
      init_string(&s);
      /* Set username and password */
      curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
      curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
  
      /* This will list the folders within the user's mailbox. If you want to
      * list the folders within a specific folder, for example the inbox, then
      * specify the folder as a path in the URL such as /INBOX */
      curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com:993/"); 
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "EXAMINE INBOX");
      curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4); 
      // 
      // // So writing the output into a local structure
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
      
      // /* Perform the list */
      res = curl_easy_perform(curl);
      printf("dasa%s %ld",s.ptr,s.len);
      // write struct to file
      fwrite (&s, sizeof(struct string), 1, outfile);
      /* Check for errors */
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
  
      /* Always cleanup */
      curl_easy_cleanup(curl);
      
    }
    // close file
    fclose (outfile);
}
void all_mail()
{
    FILE *outfile;
    outfile = fopen ("output.txt", "w");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit (1);
    }
    CURL *curl;
    CURLcode res = CURLE_OK;
  
    curl = curl_easy_init();
    if(curl) {
      // String initialization
      struct string s;
      init_string(&s);
      /* Set username and password */
      curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
      curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
  
      /* This will list the folders within the user's mailbox. If you want to
      * list the folders within a specific folder, for example the inbox, then
      * specify the folder as a path in the URL such as /INBOX */
      curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com:993/"); 
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "EXAMINE %5BGmail%5D/All%20Mail");
      curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4); 
      // 
      // // So writing the output into a local structure
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
      
      // /* Perform the list */
      res = curl_easy_perform(curl);
      printf("dasa%s %ld",s.ptr,s.len);
      // write struct to file
      fwrite (&s, sizeof(struct string), 1, outfile);
      /* Check for errors */
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
  
      /* Always cleanup */
      curl_easy_cleanup(curl);
      
    }
    // close file
    fclose (outfile);
}
void sent_mail()
{
    FILE *outfile;
    outfile = fopen ("output.txt", "w");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit (1);
    }
    CURL *curl;
    CURLcode res = CURLE_OK;
  
    curl = curl_easy_init();
    if(curl) {
      // String initialization
      struct string s;
      init_string(&s);
      /* Set username and password */
      curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
      curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
  
      /* This will list the folders within the user's mailbox. If you want to
      * list the folders within a specific folder, for example the inbox, then
      * specify the folder as a path in the URL such as /INBOX */
      curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com:993/"); 
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "EXAMINE %5BGmail%5D/Sent%20Mail");
      curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4); 
      // 
      // // So writing the output into a local structure
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
      
      // /* Perform the list */
      res = curl_easy_perform(curl);
      printf("dasa%s %ld",s.ptr,s.len);
      // write struct to file
      fwrite (&s, sizeof(struct string), 1, outfile);
      /* Check for errors */
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
  
      /* Always cleanup */
      curl_easy_cleanup(curl);
      
    }
    // close file
    fclose (outfile);
}
int main(int argc, char *argv[])
{
    char *paths=argv[1];
    FILE *fp;
    fp = fopen(paths, "r");
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
    drafts();
    inbox();
    starred();
    spam();
    trash();
    important();
}