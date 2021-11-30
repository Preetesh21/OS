#include <netinet/in.h> //internet address family
#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>
#include <string.h> 
#include <math.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h> 
#include <sys/resource.h>
#include <stdio.h>
#include <ctype.h>
#include<pthread.h> // for multithreading 
#include <sys/socket.h> //for socket programming, Internet Protocol family
#include <unistd.h> //unistd.h is the name of the header file that provides access to the POSIX operating system API. 
// Portable Operating System Interface POSIX
// definig the PORT to work for the server
#include <dlfcn.h> 
#include "server.c"

int main(int argc, char* argv[])
{
    if(argc<4)
    {
        printf("Kindly provide the following arguments:\n [1]PORT NUMBER ARGUMENT\n [2] MAX THREAD COUNT\n [3] MAX CLIENT COUNT\n [4] FILE COUNT\n [5] MEMORY LIMIT\n");
        exit(1);
    }
    run(argc,argv);
    return 0;
}

