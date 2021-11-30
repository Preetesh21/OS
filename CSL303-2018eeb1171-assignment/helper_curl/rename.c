#include <stdio.h>
#include <curl/curl.h>
 
/* This is a simple example showing how to create a new mailbox folder using
 * libcurl's IMAP capabilities.
 *
 * Note that this example requires libcurl 7.30.0 or above.
 */
 
int main(void)
{
  CURL *curl;
  CURLcode res = CURLE_OK;
 
  curl = curl_easy_init();
  if(curl) {
    /* Set username and password */
    curl_easy_setopt(curl, CURLOPT_USERNAME, "");
    curl_easy_setopt(curl, CURLOPT_PASSWORD, "");
 
    /* This is just the server URL */
    curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com:993/");
 
    /* Set the CREATE command specifying the new folder name */
    //curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "RENAME FOLDER FOLDER1");
 
    /* Set the COPY command specifying the message ID and destination folder */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "COPY 12 Important");
    
    /* Perform the custom request */
    res = curl_easy_perform(curl);
 
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* Always cleanup */
    curl_easy_cleanup(curl);
  }
 
  return (int)res;
}