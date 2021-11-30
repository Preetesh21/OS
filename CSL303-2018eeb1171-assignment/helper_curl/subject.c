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
    //char* temp=substr(s->ptr,9,s->len-2);
    //printf("sfd%s %ld\n", temp,s->len);
    //strcat(mail_id,temp);
    //printf("%s\n",mail_id);
    return size*nmemb;
}


int main(void)
{
        
    CURL *curl;
    CURLcode res = CURLE_OK;
    for(int LOOP=1;LOOP<=13;LOOP++){
        curl = curl_easy_init();
        if(curl) {
            struct string st;
            init_string(&st);
            /* Set username and password */
            curl_easy_setopt(curl, CURLOPT_USERNAME, "");
            curl_easy_setopt(curl, CURLOPT_PASSWORD, "");
            char result[50];
            sprintf(result, "%d", LOOP);
//imap.gmail.com:993/%5BGmail%5D/Sent%20Mail


//imaps://imap.gmail.com:993/%5BGmail%5D/Important;UID=2;SECTION=HEADER.FIELDS%20(SUBJECT)
            char temp[]="imaps://imap.gmail.com:993/%5BGmail%5D/All%20Mail;MAILINDEX=";
            char *temp2=";SECTION=HEADER.FIELDS%20(SUBJECT)";
            
            strcat(temp,result);
            strcat(temp,temp2);
            printf("%s\n",temp);
            curl_easy_setopt(curl, CURLOPT_URL,temp);
            curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
            // So writing the output into a local structure
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &st);
            res = curl_easy_perform(curl);
            printf("%s %ld",st.ptr,st.len);
            /* Check for errors */   
            // char *ptr = st.ptr;
            // char word[255];
            // int count=0;
            // int start=0;
            // int leng=strlen(ptr);
            // for(int i=9;i<leng;i++)
            // {
            //     word[start++]=st.ptr[i];
            //     if(start==252)
            //     break;
            // }
            // word[start]='\0';
            // printf("%s",word);
            
            curl_easy_cleanup(curl);
        }
    }
    return (int)res;
}