// Preetesh Verma
// 2018eeb1171
// including the libraries for the code
#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>
#include <semaphore.h>
#include <string.h> 
#include <math.h>
#include <limits.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h> 
#include <sys/resource.h>
#include <stdio.h>
#include <ctype.h>
#include<pthread.h> // for multithreading 
#include <unistd.h> //unistd.h is the name of the header file that provides access to the POSIX operating system API. 
// Portable Operating System Interface POSIX
#include <dlfcn.h> 

// Simple Functions to compute min and max of two values.
int maximum(int a,int b)            
{                           
    return a > b ? a : b;       
}

int minimum(int a,int b)            
{                           
    return a < b ? a : b;       
}

// Declaring a structure class
// The purpose of this node is to help in applying different types of heuristics to the threads 
// in deciding which thread to terminate.
struct Node {
    int thread;
    int resources;
};


//mutual exclusion
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t th[100];

//the shared variable across different functions
// To hold the maximum requirement for a thread
int **max;
// To hold the amount of resources available
int *available_resources;
// To hold the total amount of resources in the system
int *total_resources;
// The request vector randomly generated for every thread
int *req;
// The id's for each threads
int **threadIDs;
// The amount of resources required by the threads
int **required;
// The amount of resources held by a thread
int **hold;
// Time variable
int d;
clock_t t;
//number of threads
int number_of_Threads;

//number of resources
int number_of_Resources;
// heurisitic method
int heuristic;
int completed_thread=0;

/*
Generates resource requests by threads by passing a random value less than the need for the thread as a request
*/
void request_generator_function(int pr_id, int* ask_array){
    //initializing request vector to its required with random probability of item being selected
    //critical section
    for(int i=0; i < number_of_Resources;i++){
        ask_array[i]=0;
    }
    // Here making the random decision to request for a resource or not
    for(int i=0; i < number_of_Resources;i++){
        if(required[pr_id][i]){
            int prob = rand() % 2;
            if(prob){
                //int num = (rand() % (required[pr_id][i]+1));
                ask_array[i] =required[pr_id][i];
                break;
            }
        }
        else
            ask_array[i]=0;
    }    
    //printing the resource vector as computed by the above function
    // printf("The ask array is: ");
    // for(int i=0; i < number_of_Resources;i++){
    //     printf("%d ", ask_array[i]);
    // }
}

/*
Implementation of Allocation of resources to the thread which will only happen if there are enough resources
available to be allocated to that process else no resources are allocated for that resource to that thread.
*/
int allocate(int pr_id, int* ask_array){
    for(int j=0;j<number_of_Resources;j++){
        //Check if asked amount is available or not and is needed by that resource or not or if it has already
        // acquired that much amount of resource through prior request.
        if(required[pr_id][j]==0||ask_array[j] > available_resources[j] || ask_array[j]>required[pr_id][j]) 
            continue;
        //If  no problem then proceed
        available_resources[j] = available_resources[j] - ask_array[j];
        hold[pr_id][j] = hold[pr_id][j] + ask_array[j];
        required[pr_id][j] = required[pr_id][j] - ask_array[j];
    }
    //printing the allocation vector as computed by the above function
    // printf("The allocation vector is\n");
    // for(int j=0;j<number_of_Resources;j++)
    // {
    //     printf("%d ",hold[pr_id][j]);
    // }
    // printf("\n");
    return 0;
}

/*
Threads running parallely function.
*/
void* thread_function_simulation(void* pr_id){
    //ID of the process
    int id = *(int *) pr_id;
    
    // running forever
    int done = 1;
    while(1){
        // First checking if the thread has got everything it required.
        done = 0;
        for(int i=0;i<number_of_Resources;i++){
            if(required[id][i] != 0) {
                done = 1;
                break;
            }
        }
        // If true then it must wait for some time and then release its resources.
        if(done == 0) 
        {
            sleep(d*0.5);
            //free allocation pf the thread
            pthread_mutex_lock(&mutex);
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
            printf("Thread %d has finished. Releasing resources of the same\n", id);
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
            // Random pause
            sleep(0.7*d);
            for(int i=0;i<number_of_Resources;i++){
                available_resources[i] += hold[id][i];
                //required[id][i]=hold[id][i];
                hold[id][i] -= hold[id][i];
                //required[id][i] = -1;
            }
            // Printing the new required vector
            //printf("The required vector is\n");
            for(int j=0;j<number_of_Resources;j++)
            {
                int num = (rand() % (total_resources[j]+1));
                required[id][j]=num;
                //printf("%d ",required[id][j]);
            }
            printf("\n");
            // Printing the currently held resources which would be 0.
            //printf("The hold vector is\n");
            for(int j=0;j<number_of_Resources;j++)
            {
                int num = (rand() % (total_resources[j]+1));
                num=num%2;
                //printf("%d ",hold[id][j]);
            }
            printf("\n");
            done=1;
            // Here releasing the lock
            pthread_mutex_unlock(&mutex);   
        }
        // If not having everything required.
        else
        {
            // Acquiring the lock for the arrays
            pthread_mutex_lock(&mutex);
            //call request_generator_function to create a random request vector
            printf("Requesting resources for process %d\n", id);
            request_generator_function(id,req);

            printf("\nAllocating resources if possible\n");
            //calling allocation to allocate resources
            allocate(id,req);
            printf("Done Allocation\n");

            //Printing the new required vector
            // printf("The required vector is\n");
            // for(int j=0;j<number_of_Resources;j++)
            // {
            //     printf("%d ",required[id][j]);
            // }
            // printf("\n");
            // Here Releasing the locks
            pthread_mutex_unlock(&mutex);
            // Random Pause
            sleep(d*0.5);
        }
    }
    //return 0;
}

// These comparots help in implementing different heuristics
int comparator(const void* p, const void* q)
{
    return ((struct Node*)q)->resources -((struct Node*)p)->resources;
}

int compa(int p, int q)
{
    return p-q;
}

// These comparots help in implementing different heuristics
int comparator2(const void* p, const void* q)
{
    return ((struct Node*)p)->resources -((struct Node*)q)->resources;
}

// A simple function which would tell if at the current situation after terminating few threads there is a deadlock or not.
int checker()
{
    int k=0;
    // Checking for all the threads
    for(int i=0;i<number_of_Threads;i++){
        int flag = 1;//it is deadlocke
        for(int j=0;j<number_of_Resources;j++){
            if(required[i][j] <= available_resources[j]) {
                flag = 0;//it is not in deadlock
            }
            else{
                flag=1;
                break;
            }
        }
        if(flag==0)
            return 0;
    }
    return 1;
}

/*
Removes thread.Then Spawn a new thread in it's replacement.
*/
void* thread_terminate_functions(int* threads_in_deadlock,int size){
    // here remove the resources held by these threads in threads_in_deadlock and then check if deadlock is removed
    // 1. if removed then resume the normal processing of the threads.
    // 2. continue releasing resources from the threads.
    // Here Create a struct array of thread number and the total number of resources it holds.
    // The struct is created depening on the heuristics arguments provided.
    struct Node array[size];
    if(heuristic!=3){
        for(int i=0;i<size;i++)
        {
            int sum=0;
            for(int j=0;j<number_of_Resources;j++)
            {
                sum+=hold[threads_in_deadlock[i]][j];
            }
            array[i].thread=threads_in_deadlock[i];
            array[i].resources=sum;
        }
    }
    else
    {
        for(int i=0;i<size;i++)
        {
            int maxi=0;
            for(int j=0;j<number_of_Resources;j++)
            {
                maxi=maximum(maxi,hold[threads_in_deadlock[i]][j]);
            }
            array[i].thread=threads_in_deadlock[i];
            array[i].resources=maxi;
        }
    }
    // Implementing the heuristics as requested by the user
    // Sort the structure in decreasing order of resources each thread in the deadlock holds the resources.
    if(heuristic==1 || heuristic ==3)
        qsort(array, size, sizeof(struct Node), comparator);
    // Sort the structure in increasing order of resources each thread in the deadlock holds the resources.
    else
        qsort(array, size, sizeof(struct Node), comparator2);
    // Printing the struct details necessary
    // printf("The details of structure after sorting\n");    
    // for(int i=0;i<size;i++)
    // {
    //     printf("%d %d \n",array[i].thread,array[i].resources);
    // }
    // Now terminate one by one and check if deadlock is removed.
    // If yes then our work for time being is done and we can exit
    // Else continue for next thread
    for(int i=0;i<size;i++)
    {
        // releasing resources held by threads and moving ahead. 
        int curr_thread_id=array[i].thread;
        printf("The thread terminated is %d \n",curr_thread_id);
        for(int j=0;j<number_of_Resources;j++)
        {
            available_resources[j]+=hold[curr_thread_id][j];
            required[curr_thread_id][j]+=hold[curr_thread_id][j];
            hold[curr_thread_id][j]=0;
        }
        // pthread_exit(&th[curr_thread_id]);
        // th[curr_thread_id]=NULL;
        // pthread_create(&th[curr_thread_id],NULL,thread_function_simulation,threadIDs[curr_thread_id]);
        if(!checker())
        {
            printf("Deadlock has been removed for now and normal processing can proceed \n");
            return 0;
        }
    }
    return 0;
}

/*
This function is responsible for checking for any potential deadlock
among the threads by seeing the matrix and the requirements of the threads.
*/
void* deadlock_detector(void * pr_id){
    // Infinte loop begins
    while(1){
        printf("Detecting potential deadlock\n");
        // Acquiring the lock to the matrix and the arrays
        pthread_mutex_lock(&mutex);
        // First computing those threads which are in sleep state
        // i.e. they have got whatever their requirement was and now they are sleep
        int temp[number_of_Resources];
        for(int i=0;i<number_of_Resources;i++)
            temp[i]=0;
        for(int i=0;i<number_of_Threads;i++){
            int flag = 1;//it will soon release its resources
            for(int j=0;j<number_of_Resources;j++){
                if(required[i][j] !=0) {
                    flag = 0;//it has not completed its processing.
                    break;
                }
            }
            for(int j=0;j<number_of_Resources;j++){
                if(required[i][j]>available_resources[j]) {
                    flag = 2;//it will not release its resources.
                    break;
                }
            }
            // Marking the resources
            if(flag!=2)
            {
                for(int j=0;j<number_of_Resources;j++)
                    temp[j]+=hold[i][j];
            }
        }
        int k=0;
        int threads_in_deadlock[number_of_Threads];
        for(int i=0;i<number_of_Threads;i++){
            int flag = 1;//it is deadlocked
            for(int j=0;j<number_of_Resources;j++){
                if(required[i][j] <= available_resources[j]+temp[j]) {
                    flag = 0;//it is not in deadlock
                }
                else{
                    flag=1;
                    break;
                }
            }
            if(flag==1)
                threads_in_deadlock[k++]=i;
        }
        if(k != 0){
            // Printing a meesage
            printf("#########################################\n");
            printf("Deadlcok Found here\n");
            printf("#########################################\n");
            t = clock() - t; 
            double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
            printf("Deadlock occurred at %f seconds \n", time_taken);
            printf("Threads in Deadlock are:%d\n",k);
            for(int i=0;i<k;i++)
            {
                printf("Thread number %d \n",threads_in_deadlock[i]);
            }
            thread_terminate_functions(threads_in_deadlock,k);
            //exit(1);
        }
        pthread_mutex_unlock(&mutex);
        sleep(d);
    }
}

void print_the_details()
{
    printf("The number of resources %d \n",number_of_Resources);
    printf("The number of threads %d \n",number_of_Threads);

    printf("Resource Array\n: ");
    for(int i=0; i< number_of_Resources;i++) 
        printf("%d ",available_resources[i]);
    printf("\n");
}

int main()
{
    t = clock();
    printf("Enter the number of threads:\n");
    scanf("%d",&number_of_Threads);

    printf("Enter resource count:\n");
    scanf("%d",&number_of_Resources);

    //1D array of available_resourcesable resources
    available_resources = malloc(sizeof(int)*number_of_Resources);
    req = malloc(sizeof(int)*number_of_Resources);
    threadIDs = malloc(number_of_Threads*sizeof(int*));
    total_resources = malloc(sizeof(int)*number_of_Resources);
    
    pthread_t deadlock;

    printf("Enter total quantity of each resource:\n");
    for(int i=0;i<number_of_Resources;i++){
        scanf("%d",&available_resources[i]);
    }
    
    // Copying data
    for(int i=0;i<number_of_Resources;i++)
    {
        total_resources[i]=available_resources[i];
    }

    //2D array of maximum resources each process can claim 
    // The amount of resources required by the threads
    required = malloc(number_of_Resources*sizeof(int*));
    // The amount of resources held by a thread
    hold = malloc(number_of_Resources*sizeof(int*));
    // The max amount of resources required by the threads
    max = malloc(number_of_Resources*sizeof(int*));

    for(int i=0;i<number_of_Threads;i++){
        // The max amount of resources required by the threads
        max[i] = (int *)malloc(number_of_Resources*sizeof(int));
        // The amount of resources held by a thread
        hold[i] = (int *)malloc(number_of_Resources*sizeof(int));
        // The amount of resources required by the threads
        required[i] = (int *)malloc(number_of_Resources*sizeof(int));
    }

    //Random Generation of resource required
    for(int i=0;i<number_of_Threads;i++){
        for(int j=0;j<number_of_Resources;j++){
            int num = (rand() % (available_resources[j]+1));
            max[i][j]=num;
        }
    }

    // Time after which deadlock checkker must check
    printf("Enter the time delay \n");
    scanf("%d",&d);

    printf("Enter the heuristic approach you want to see");
    printf("There are three approahces as implemented by me\n");
    printf("[1]-thread with total most resources is released first\n");
    printf("[2]-thread with total least resources is released first\n");
    printf("[3]-thread with a most amount of resource is released first\n");
    scanf("%d",&heuristic);

    // filling the arrays as required
    for(int i=0;i<number_of_Threads;i++){
        for(int j=0;j<number_of_Resources;j++){
            required[i][j] = max[i][j];
            hold[i][j] = 0;
        }
    }
    // Here printing all the information uptil now
    print_the_details();
    
    printf("Simulating threads simulator\n");
    // Thread pool creation done
    for(int i=0;i<number_of_Threads;i++){
        threadIDs[i] = malloc(sizeof(int));
        *threadIDs[i] = i;
        pthread_create(&th[i],NULL,thread_function_simulation,threadIDs[i]);
    }
    // Deadlock Thread
    pthread_create(&deadlock,NULL,deadlock_detector,NULL);

    //joining threads
    for(int j=0;j<number_of_Threads;j++) 
        pthread_join(th[j],NULL);

    pthread_cancel(deadlock);
    pthread_join(deadlock,NULL);

    //destroying all the semaphores
    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);
    return 0;
}