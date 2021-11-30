#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>
#include <string.h>

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


int main(void)
{
    struct Node* r=(struct Node*)malloc(sizeof(struct Node*));
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
    Enqueue(0,0);
    if(isEmpty()==false)
        printf("-----Passed Test Case 3-----\n");
    else
        printf("Failed\n");
    
}
