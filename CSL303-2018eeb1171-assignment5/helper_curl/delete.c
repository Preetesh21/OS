 
#include <stdio.h>
#include <curl/curl.h>
 
/* This is a simple example showing how to delete an existing mail using
 * libcurl's POP3 capabilities.
 *
 * Note that this example requires libcurl 7.26.0 or above.
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
 
    /* You can specify the message either in the URL or DELE command */
    curl_easy_setopt(curl, CURLOPT_URL, "pop3s://pop.gmail.com:993/1");
 
    /* Set the DELE command */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELE");
 
    /* Do not perform a transfer as DELE returns no data */
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
 
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