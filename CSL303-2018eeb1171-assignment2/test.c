#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>
#include <string.h>
#include<pthread.h>
#include <dlfcn.h> 
#include <limits.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h> 
#include <sys/resource.h>
#include <stdio.h>
#include <ctype.h>

void request_gen_func(int maxi, int* ask_array,int size_of_array){
    //initializing request vector to its required with random probability of item being selected
    //critical section
    for(int i=0; i < size_of_array;i++){
        ask_array[i]=0;
    }
    // Here making the random decision to request for a resource or not
    for(int i=0; i < size_of_array;i++){
        int prob = rand() % 2;
        if(prob){
            int num = (rand() % maxi);
            ask_array[i] =num;
        }
    }    
}

int compa(const void* p, const void* q)
{
    return (int*)q<(int*)p;
}
void allo(int size, int* ask_array,int* total_array){
    for(int j=0;j<size;j++){
        if(ask_array[j]<total_array[j])
            ask_array[j]=total_array[j];
    }
}

int main(void)
{
    
    int temp[5]={1,2,3,4,5};
    qsort(temp,5,sizeof(int),compa);    
    int flag=0;
    for(int i=0;i<5;i++)
    {
        if(temp[i]!=i+1)
        {
            flag=1;
            break;
        }
    }

    // TEST CASE 1 based on heuristic approach
    if(flag==0)
        printf("-----Passed Test Case 1-----\n");
    else
        printf("Failed\n");
    
    int request_array_for_testing[5];
    int size_of_array=5;
    request_gen_func(5,request_array_for_testing,size_of_array);

    for(int i=0;i<5;i++)
    {
        if(request_array_for_testing[i]>5)
        {
            flag=1;
            break;
        }
    }
    // TEST CASE 2 based on random generation
    if(flag==0)
        printf("-----Passed Test Case 2-----\n");
    else
        printf("Failed\n");
    
    // TEST CASE 3 allocation vector
    allo(5,request_array_for_testing,temp);
    for(int i=0;i<5;i++)
    {
        if(request_array_for_testing[i]==0)
        {
            flag=1;
            break;
        }
    }
    if(flag==0)
        printf("-----Passed Test Case 3-----\n");
    else
        printf("Failed\n");
    
    return 0;
}