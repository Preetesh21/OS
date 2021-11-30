#include<stdio.h>
#include<string.h>
#include <curl/curl.h>

int main(void)
{
    CURL *curl;
    CURLcode res = CURLE_OK;
for(int LOOP=1;LOOP<3856;LOOP++){
    curl = curl_easy_init();
    if(curl) {
        /* Set username and password */
        curl_easy_setopt(curl, CURLOPT_USERNAME, "");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "");
        char result[50];
        sprintf(result, "%d", LOOP);
        char temp[]="imaps://imap.gmail.com:993/%5BGmail%5D/Starred%20Mail;MAILINDEX=";
        char *temp2=";SECTION=TEXT";
        strcat(temp,result);
        strcat(temp,temp2);
        printf("%s\n",temp);
        curl_easy_setopt(curl, CURLOPT_URL,temp);
        curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
        // LIST THE FOLDERS
         //curl_easy_setopt(curl, CURLOPT_URL,"imaps://imap.gmail.com:993/");  

        // GET FROM INBOX MAIL WITH UID=29        
 //       char temp[]="imaps://imap.gmail.com:993/INBOX/;MAILINDEX=193";
     //   curl_easy_setopt(curl, CURLOPT_URL,"imaps://imap.gmail.com:993/%5BGmail%5D/Drafts;UID=3469");
        //curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "EXAMINE INBOX");
        

//        curl_easy_setopt(curl, CURLOPT_URL,temp); 
        //curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
        
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        /* Perform the fetch */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
        curl_easy_strerror(res));  
        curl_easy_cleanup(curl);
    }
}
    return (int)res;
}