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

struct Node {
    struct Node* next;
    int memory_requirement;
    int duration;
};

static struct Node *front, *tail;

// Initializing the queue
void InitQueue()
{
    front = tail = NULL;
}

// isEmpty returns true if the queue is empty
int isEmpty()
{
    return (front == NULL);
}

// Returns true if the queue is full.
int isFull()
{
    return false;
}

// Clear the queue function
void ClearQueue()
{
    struct Node *temp;
    while(front != NULL)
    {
        temp = front;
        front = front->next;
        free(temp);
    }

    front = tail = NULL; // Reset indices to start over
}

// Enqueue an item into the queue.Returns TRUE if enqueue was successful
// or FALSE if the enqueue failed.
bool Enqueue(int duration,int memory_requirement)
{
    struct Node *temp;

    // Check to see if the Queue is full
    if(isFull()) 
        return false;

    // Create new node for the queue
    temp = (struct Node *)malloc(sizeof(struct Node*));
    temp->duration = duration;
    temp->memory_requirement=memory_requirement;
    temp->next = NULL;

    // Check for inserting first in the queue
    if(front == NULL)
        front = tail = temp;
    else
    {
        tail->next = temp; // Insert into the queue
        tail = temp;       // Set tail to new last node
    }

    return true;
}

// Dequeue function which return the first request present in the qeueue
// or if the queue is empty return NULL.
struct Node* Dequeue()
{
    //struct request* r;
    struct Node *temp;

    // Check for empty Queue
    if(isEmpty()) return NULL;  // Return null character if queue is empty
    else
    {
        //r = front->value;        // Get request to return
        temp = front;
        front = front->next;    // Advance front pointer
        //free(temp);            // Free old front
        // Check to see if queue is empty
        if(isEmpty())
        {
            front = tail = NULL;        // Reset everything to empty queue
        }
    }
    return temp;                // Return popped request
}




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
    struct Node* r=(struct Node*)malloc(sizeof(struct Node*));
    InitQueue();

    // TEST CASE 4
    if(isEmpty()==true)
        printf("-----Passed Test Case 4-----\n");
    else
        printf("Failed\n");
    
    // TEST CASE 5
    if(Dequeue()==NULL)
        printf("-----Passed Test Case 5-----\n");
    else
        printf("Failed\n");
    
    // TEST CASE 6
    Enqueue(0,0);
    if(isEmpty()==false)
        printf("-----Passed Test Case 6-----\n");
    else
        printf("Failed\n");
    return 0;
}