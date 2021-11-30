// Preetesh Verma
// 2018eeb1171
// including the libraries for the code
#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <time.h>
#include <stdbool.h>
#include<math.h>
#include <unistd.h>


// Few global variables
int p;
float q;
clock_t t;
int nums=1;

/*--------------------------------------------
         QUEUE IMPLEMENTATION
--------------------------------------------*/

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


// Declaring Linked List for memory block management
struct node
{
    int addr;           // Starting address of the block
    int length;         // The length of the block
    int num;            // Count of the block;
    int state;          // Whether the memory block is free or held
    int duration;    // Time for which the memory is blocked by the thread
    clock_t time_value;     // To store the time part of the question
    struct node *prev;  // For linking the previous node in the memory chain
    struct node *next;  // For linking the next node in the memory chain
};
struct node* head = NULL;

// Here showing the current situation of the memory block and printing the size and address of every partition and its state
void show_memory_location()
{
    struct node* temp=head;
    printf("------------------------------------------------------\n");
    printf("The memory allocation so far is as follows:\n");
    printf("The OS holds address %d and of length %d\n",temp->addr,temp->length);
    temp=temp->next;
    int count=1;
    while(temp)
    {
        char* ans=temp->state==0?"Free":"Occupied";
        printf("The memory block %d holds address %d is of length %d and is currently %s\n",count,temp->addr,temp->length,ans);
        count++;
        temp=temp->next;
    }
    printf("------------------------------------------------------\n");
}

// A simple allocation function which allocates the head and the rest of the memory to blocks.
void initialize_memory_allocation(){
    // Rest of the memory been allocated as a continuous block linked to the OS part
    struct node* memory_block=(struct node*)malloc(sizeof(struct node));
    memory_block->prev=head;
    memory_block->state=0;
    memory_block->next=NULL;
    memory_block->length=p-q;
    memory_block->addr=q;
    memory_block->num=nums++;
    memory_block->time_value=clock();
    // Linking the OS part of the memory to the continuous block
    head->next=memory_block;
}

// To create random request for memory allocation
int generate_random_request(int lower, int upper)
{
    int num=(rand()%(upper - lower + 1)) + lower;
    return num;
}

// Function to release memory i.e. whenever the duration has been spent the memory held by that process is freed.
void release_memory_function()
{
    struct node* temp=head->next;
    while(temp)
    {
        // Not free
        //printf("%duu\n",temp->state);
        if(temp->state==1)
        {
            clock_t te=clock()-temp->time_value;
            double time_taken = ((double)te)/CLOCKS_PER_SEC;
            if(time_taken*20000>temp->duration)// change 20 to duration and make changes accordingly
            {
                //printf("aa\n");
                struct node* temp1;
                struct node* temp2;
                temp1=temp->prev;
                //printf("%d %d %d\n",temp1->length,temp1->state,temp->length);
                temp2=temp->next;
                // If both the prev and next nodes are free
                if(temp2!=NULL && temp1->state==0 && temp2->state==0)
                {
                    //printf("WE ARE HERE1");
                    temp->prev=temp1->prev;
                    temp->next=temp2->next;
                    temp->addr=temp1->addr;
                    temp->state=0;
                    temp->length+=temp1->length+temp2->length;
                    if(temp2->next)
                    {
                        temp2->next->prev=temp;
                    }
                    if(temp1->prev)
                    {
                        temp1->prev->next=temp;
                    }
                    free(temp1);
                    free(temp2);
                    show_memory_location();
                }
                // If only prev is free
                else if(temp2!=NULL && temp1->state==0 && temp2->state==1)
                {
                    //printf("WE ARE HERE2");
                    temp1->next=temp->next;
                    temp->next->prev=temp1;
                    temp1->state=0;
                    temp1->length+=temp->length;
                    free(temp);
                    show_memory_location();
                }
                // If only next is free
                else if(temp2!=NULL && temp1->state==1 && temp2->state==0)
                {
                    //printf("WE ARE HERE3");
                    temp2->prev=temp->prev;
                    temp->prev->next=temp2;
                    temp2->addr=temp->addr;
                    temp2->length+=temp->length;
                    temp2->state=0;
                    free(temp);
                    show_memory_location();
                }
                // If next is NULL
                else if(temp2==NULL && temp1->state==0)
                {
                    //printf("WE ARE HERE4");
                    temp1->next=NULL;
                    temp1->state=0;
                    temp1->length+=temp->length;
                    free(temp);
                    show_memory_location();
                }
                // If next is null and prev not free
                else if(temp2==NULL && temp1->state==1)
                {
                    temp->state=0;
                    show_memory_location();
                }
            }
            else
            {
                printf("The time spent till now is %f and the total time needed is  %d  \n",time_taken*20000,temp->duration);
            }
        }
        // It is free
        else
        {
            // Since free so only merging with the next memory block
            struct node* temp1=temp->next;
            if(temp1!=NULL && temp1->state==0)
            {
                temp->next=temp1->next;
                temp->state=0;
                temp->length+=temp1->length;
                free(temp1);   
            }
        }
        temp=temp->next; 
    }
    return ;
}

// First Fit algorithm to allocate memory
void first_fit_allocate()
{
    int req_memory=front->memory_requirement;
    int dura=front->duration;
    //printf("dfs2");    
    struct node* temp=head;
    temp=temp->next;
    while(temp)
    {
        // Memory block is occupied right now
        if(temp->state==1)
            temp=temp->next;
        // Memory block is free but size if not sufficient
        else if(temp->state==0 && temp->length<req_memory)
            temp=temp->next;
        // First fit for the required memory
        else if(temp->state==0 && temp->length==req_memory)
        {
            temp->state=1;
            temp->time_value=clock();
            temp->duration=dura;
            temp->num=nums++;
            printf("Successfully Allocated the memory block of the requested size\n");
            Dequeue();
            return ;
        }
        else if(temp->state==0 && temp->length>req_memory)
        {
            int extra=temp->length-req_memory;
            struct node* temp2=(struct node*)malloc(sizeof(struct node));
            // Assigning the length and other parameters to the newly constructed memory block and linking it to the temp
            temp2->prev=temp;
            temp2->next=temp->next;
            temp2->state=1;
            temp2->duration=dura;
            temp2->length=req_memory;
            temp2->addr=temp->addr+extra;
            temp2->num=nums++;
            temp2->time_value=clock();
            // Modifying the temp block as well.
            temp->next=temp2;
            temp->length=extra;
            temp2=temp2->next;
            temp=temp->next;
            while(temp2)
            {
                temp2->prev=temp;
                temp=temp->next;
                temp2=temp2->next;
            }
            printf("Successfully Allocated the memory block of the requested size\n");
            Dequeue();
            return ;
        }
    }
    printf("Sorry there is just not enough memory space for the request.\n");
    return ;
}

// Best Fit algorithm to allocate memory
void best_fit_allocate()
{
    int req_memory=front->memory_requirement;
    int dura=front->duration;
    struct node* temp=head;
    int ans_num=-1;
    int best_so_far=-1;
    temp=temp->next;
    while(temp)
    {
        // Memory block is occupied right now
        if(temp->state==1)
            temp=temp->next;
        // Memory block is free but size if not sufficient
        else if(temp->state==0 && temp->length<req_memory)
            temp=temp->next;
        // Best Fit for the required memory
        else if(temp->state==0 && temp->length>=req_memory && (best_so_far==-1 || best_so_far>temp->length-req_memory))
        {
            best_so_far=temp->length-req_memory;
            ans_num=temp->num;
            temp=temp->next;
        }
        else
        temp=temp->next;
    }
    // No memory block found
    if(ans_num==-1)
    {
        printf("Sorry there is just not enough memory space for the request.\n");
        return ;
    }
    // Assigning the memory to best fit
    else
    {
        temp=head->next;
        while(temp->num!=ans_num)
            temp=temp->next;
        if(best_so_far==0)
        { 
            temp->state=1;
            temp->time_value=clock();
            temp->num=nums++;
            temp->duration=dura;
            printf("Successfully Allocated the memory block of the requested size\n");
            Dequeue();
            return ;
        }
        else
        {
            int extra=temp->length-req_memory;
            struct node* temp2=(struct node*)malloc(sizeof(struct node));
            // Assigning the length and other parameters to the newly constructed memory block and linking it to the temp
            temp2->prev=temp;
            temp2->next=temp->next;
            temp2->state=1;
            temp2->duration=dura;
            temp2->length=req_memory;
            temp2->addr=temp->addr+extra;
            temp2->num=nums++;
            temp2->time_value=clock();
            // Modifying the temp block as well.
            temp->next=temp2;
            temp->length=extra;
            temp2=temp2->next;
            temp=temp->next;
            while(temp2)
            {
                temp2->prev=temp;
                temp=temp->next;
                temp2=temp2->next;
            }
            printf("Successfully Allocated the memory block of the requested size\n");
            Dequeue();
            return ;
        }
    }
}

// Next Fit algorithm to allocate memory
struct node* next_fit_allocate(struct node* start)
{
    //printf("ababa %d\n",head->next->length);
    
    int req_memory=front->memory_requirement;
    int dura=front->duration;
    struct node* temp;
    if(start==NULL || start->next==NULL)
        temp=head->next;
    else
        temp=start->next;
    if(head->next->length==p-q)
    {
        temp=head->next;
        //printf("Hello\n");
    }
    if(!temp)
        temp=head->next;
    start=temp;
    //printf("%das",temp->addr);
    while(temp)
    {
        // Memory block is occupied right now
        if(temp->state==1)
            temp=temp->next;
        // Memory block is free but size if not sufficient
        else if(temp->state==0 && temp->length<req_memory)
            temp=temp->next;
        // First fit for the required memory
        else if(temp->state==0 && temp->length==req_memory)
        {
            temp->state=1;
            temp->time_value=clock();
            temp->num=nums++;
            temp->duration=dura;
            printf("Successfully Allocated the memory block of the requested size\n");
            Dequeue();
            return temp;
        }
        else if(temp->state==0 && temp->length>req_memory)
        {
            int extra=temp->length-req_memory;
            struct node* temp2=(struct node*)malloc(sizeof(struct node));
            // Assigning the length and other parameters to the newly constructed memory block and linking it to the temp
            temp2->prev=temp;
            temp2->next=temp->next;
            temp2->duration=dura;
            temp2->state=1;
            temp2->length=req_memory;
            temp2->addr=temp->addr+extra;
            temp2->num=nums++;
            temp2->time_value=clock();
            // Modifying the temp block as well.
            temp->next=temp2;
            temp->length=extra;
            struct node* te=temp2;
            temp2=temp2->next;
            temp=temp->next;
            while(temp2)
            {
                temp2->prev=temp;
                temp=temp->next;
                temp2=temp2->next;
            }
            printf("Successfully Allocated the memory block of the requested size\n");
            Dequeue();
            return te;
        }
        else
        temp=temp->next;
    }
    temp=head;
    while(temp!=start)
    {
        // Memory block is occupied right now
        if(temp->state==1)
            temp=temp->next;
        // Memory block is free but size if not sufficient
        else if(temp->state==0 && temp->length<req_memory)
            temp=temp->next;
        // First fit for the required memory
        else if(temp->state==0 && temp->length==req_memory)
        {
            temp->state=1;
            temp->time_value=clock();
            temp->num=nums++;
            temp->duration=dura;
            printf("Successfully Allocated the memory block of the requested size\n");
            Dequeue();
            return temp;
        }
        else if(temp->state==0 && temp->length>req_memory)
        {
            int extra=temp->length-req_memory;
            struct node* temp2=(struct node*)malloc(sizeof(struct node));
            // Assigning the length and other parameters to the newly constructed memory block and linking it to the temp
            temp2->prev=temp;
            temp2->next=temp->next;
            temp2->state=1;
            temp2->duration=dura;
            temp2->length=req_memory;
            temp2->addr=temp->addr+extra;
            temp2->num=nums++;
            temp2->time_value=clock();
            // Modifying the temp block as well.
            temp->next=temp2;
            temp->length=extra;
            struct node* te=temp2;
            temp2=temp2->next;
            temp=temp->next;
            while(temp2)
            {
                temp2->prev=temp;
                temp=temp->next;
                temp2=temp2->next;
            }
            printf("Successfully Allocated the memory block of the requested size\n");
            Dequeue();
            return te;
        }
        else
        {
            temp=temp->next;
        }
    }
    printf("Sorry there is just not enough memory space for the request.\n");
    return head->next;
}

// The main function
int main()
{
    // Asking the user for the inputs of p,q,algorithm,n,m,t
    printf("Enter the value of p\n");
    scanf("%d",&p);
    printf("Enter the value of q which needs to be less than equal to 0.2 times p\n");
    scanf("%f",&q);
    // Here Declaring the OS storage in the memory
    head = (struct node*)malloc(sizeof(struct node));
    head->addr=0;
    head->length=q;
    head->state=1;
    head->next=NULL;
    head->prev=NULL;
    head->num=nums++;
    head->time_value=clock();
    initialize_memory_allocation();
    int algo=-1;
    printf("Enter the Algorithm you want for allocation\n");
    printf("[1] FIRST FIT\n");
    printf("[2] BEST FIT\n");
    printf("[3] NEXT FIT\n");
    scanf("%d",&algo);
    struct node* tmp=head;
    printf("Number of processes per minute generated are i.e 'n' \n");
    int n;
    scanf("%d",&n);
    int r=generate_random_request(n*0.1,n*1.2);
    int m;
    printf("Enter the value of m i.e. the size multiplier for processes i.e. 'm' \n");
    scanf("%d",&m);
    int ti;
    printf("Enter the time multiplier for duration of processes i.e. 't' \n");
    scanf("%d",&ti);
    clock_t main_clock=clock();
    while(1)
    {   
        release_memory_function();
        //printf("sdss%d\n",n);
        // Random request being generated here
        int l=(0.5*m);
        int u=(3*m);
        int req_memory=generate_random_request(l,u);
        int l2=(0.5*ti);
        int u2=(6*ti);
        int duration=generate_random_request(l2,u2);
        //printf("%d %d\n",req_memory*10,duration);
        bool a=Enqueue(duration*5,req_memory*5);
        if(!a)
        {
            printf("Error in Enqueuing");
        }
        
        if(algo==1)
        {
            clock_t first_clock=clock();
            first_fit_allocate();
            clock_t tt=clock()-first_clock;
            double time_takens = ((double)tt)/CLOCKS_PER_SEC;
            printf("The time taken by the function is as follows: %f\n",time_takens);
            show_memory_location();
        }
        if(algo==2)
        {
            clock_t first_clock=clock();
            best_fit_allocate();
            clock_t tt=clock()-first_clock;
            double time_takens = ((double)tt)/CLOCKS_PER_SEC;
            printf("The time taken by the function is as follows: %f\n",time_takens);
            show_memory_location();
        }
        if(algo==3)
        {
            clock_t first_clock=clock();
            tmp=next_fit_allocate(tmp);
            clock_t tt=clock()-first_clock;
            double time_takens = ((double)tt)/CLOCKS_PER_SEC;
            printf("The time taken by the function is as follows: %f\n",time_takens);
            show_memory_location();
        }
        sleep(1);
        clock_t totalt=clock()-main_clock;
        double time_takenss = ((double)totalt)/CLOCKS_PER_SEC;
        //printf("TOTAL TIME: %f\n",time_takenss*10000);
        if(time_takenss*10000>100)
            break;
    }
    show_memory_location();
}