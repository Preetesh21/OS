#include<stdio.h>
#include<string.h>
#include <curl/curl.h>
#include<stdlib.h>
#include <errno.h>
#include <limits.h>
struct string {
  char *ptr;
  size_t len;
};

// A simple variable to store the index of the result mail 
char mail_id[]="imaps://imap.gmail.com:993/INBOX/;MAILINDEX=";

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
    char* temp=substr(s->ptr,9,s->len-2);
    //printf("sfd%s %ld\n", temp,s->len);
    strcat(mail_id,temp);
    //printf("%s\n",mail_id);
    return size*nmemb;
}

// The main function
int main(void)
{
    CURL *curl;
    CURLcode res = CURLE_OK;
    
    curl = curl_easy_init();
    if(curl) {
        // String initialization
        struct string s;
        init_string(&s);
        // First Connection
        curl_easy_setopt(curl, CURLOPT_USERNAME, "");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "");

        curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com:993/INBOX");
        // Searching for CS-GO in mail subjects
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "SEARCH SUBJECT CS-GO");
        // Output would be mail's UID
        // So writing the output into a local structure
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        // Closing the current curl request
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        // Starting a fresh curl request to access the mail
        curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_USERNAME, "");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "");
        curl_easy_setopt(curl, CURLOPT_URL,mail_id); 
        curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
        res = curl_easy_perform(curl);
        
        // Releasing the memory
        free(s.ptr);
        if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }
    return (int)res;
}