#include<stdio.h>
#include<string.h>
#include <curl/curl.h>
#include<stdlib.h>
#include <errno.h>
#include <limits.h>
#include <time.h>

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;
  
    // Storing start time
    clock_t start_time = clock();
  
    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}

int main(void)
{
        
    CURL *curl;
    CURLcode res = CURLE_OK;
    for(int LOOP=1;LOOP<12;LOOP++){
        curl = curl_easy_init();
        if(curl) {
            /* Set username and password */
            curl_easy_setopt(curl, CURLOPT_USERNAME, "");
            curl_easy_setopt(curl, CURLOPT_PASSWORD, "");
            char result[50];
            sprintf(result, "%d", LOOP);
            char temp[]="imaps://imap.gmail.com:993/%5BGmail%5D/Important;MAILINDEX=";
            char *temp2=";SECTION=TEXT";
            
            strcat(temp,result);
            strcat(temp,temp2);
            printf("%s\n",temp);
            delay(4);
            curl_easy_setopt(curl, CURLOPT_URL,temp);
            curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            
        }
    }
    return (int)res;
}