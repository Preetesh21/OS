#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>
#include <string.h>
#include<pthread.h> // for multithreading 
#include <sys/socket.h> //for socket programming, Internet Protocol family
#include <dlfcn.h> 
#include "server.c"

int main(void)
{
    void *handle2;
    double (*some_fun2)(double);
    char *error;
    handle2 = dlopen ("libm.so.6", RTLD_NOW);
    if (!handle2) {
        fputs (dlerror(), stderr);
        exit(1);
    }
    some_fun2 = dlsym(handle2, "cos");
    if ((error = dlerror()) != NULL)  {
        fputs(error, stderr);
        exit(1);
    }
    double f=(*some_fun2)(1);
    char *a="1";
    char **b=&a;
    struct request* r=(struct request*)malloc(sizeof(struct request*));
    r->dll_name="libm.so.6";
    r->func_name="cos";
    r->func_args= b;
    InitQueue();

    // TEST CASE 1
    if(isEmpty()==true)
        printf("-----Passed Test Case 1-----\n");
    else
        printf("Failed\n");
    
    // TEST CASE 2
    if(Dequeue()==NULL)
        printf("-----Passed Test Case 2-----\n");
    else
        printf("Failed\n");
    
    // TEST CASE 3
    Enqueue(r,1);
    if(isEmpty()==false)
        printf("-----Passed Test Case 3-----\n");
    else
        printf("Failed\n");
    
    //TEST CASE 4
    struct Node *t=Dequeue();
    if(executeforTest(t)==f)
        printf("-----Passed Test Case 4-----\n");
    else
        printf("Failed\n");
    
    // TEST CASE 5
    int dropped=0;    
    for(int i=0;i<1000;i++)
    {
        if(create_thread_pool())
            dropped++;    
    }
    if(dropped==0)
    printf("-----Passed Test Case 5-----\n");
    else
        printf("Failed\n");
        
    // TEST CASE 6
    t->value->func_name="aa";
    if(executeforTest(t)==0)
        printf("-----Passed Test Case 6-----\n");
    else
        printf("Failed\n");
    return 0;
}