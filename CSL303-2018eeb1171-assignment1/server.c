// Preetesh Verma
// 2018eeb1171
// including the libraries for the server
#include <netinet/in.h> //internet address family
#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>
#include <string.h> 
#include <math.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
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


int jj = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int number_of_args=0; 
pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

struct request {
  char* dll_name; // Name of a dynamically loaded library
  char *func_name; // Name of a function to call from the DLL
  char** func_args; // Arguments to pass to the function
};

/*--------------------------------------------
         QUEUE IMPLEMENTATION
--------------------------------------------*/

struct Node {
    struct request* value;
    struct Node* next;
    int socket_id;
};

static struct Node *head, *tail;

// Initializing the queue
void InitQueue()
{
    head = tail = NULL;
}

// isEmpty returns true if the queue is empty
int isEmpty()
{
    return (head == NULL);
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
    while(head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp);
    }

    head = tail = NULL; // Reset indices to start over
}

// Enqueue an item into the queue.Returns TRUE if enqueue was successful
// or FALSE if the enqueue failed.
bool Enqueue(struct reuqest* r,int socket_id)
{
    struct Node *temp;

    // Check to see if the Queue is full
    if(isFull()) 
        return false;

    // Create new node for the queue
    temp = (struct Node *)malloc(sizeof(struct Node*));
    temp->value = r;
    temp->socket_id=socket_id;
    temp->next = NULL;

    // Check for inserting first in the queue
    if(head == NULL)
        head = tail = temp;
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
        //r = head->value;        // Get request to return
        temp = head;
        head = head->next;    // Advance head pointer
        //free(temp);            // Free old head
        // Check to see if queue is empty
        if(isEmpty())
        {
            head = tail = NULL;        // Reset everything to empty queue
        }
    }
    return temp;                // Return popped request
}

// Communicating with the client and sending acknowledgment as and when the request are added to the queue.
void * socketThread(void *arg)
{
    // client socket ID
    int client_socket = *((int *)arg);
    char str[1000];
    // receiving the string sent by client and then replying them 
    int client_request1 = recv(client_socket, str, sizeof(str),0); 
   // printf("String sent by client:%s %d \n", str,client_socket);
    char str1[1000];
    char str2[1000];
    char str3[10][1000];
    char *str4[10];
    // breaking the string to the dll_name,dll_func,func_args.
    int len=strlen(str);int i;
    for(i=0;i<len;i++)
    {
        if(str[i]==' ')
            break;
        str1[i]=str[i];
    }
    int j=0;i++;
    for(;i<len;i++)
    {
        if(str[i]==' ')
            break;
        str2[j++]=str[i];
    }
    int k=0;j=0;
    char* emptyString="";
    i++;int p=0;
    int flag=0;
    for(;i<len;i++)
    {
        //printf("sds");
        flag=1;
        if(str[i]==' ' &&j>0)
        {
            str4[k]=str3[p];
            k=k+1;
            j=0;p++;
        }
        else{
            str3[p][j++]=str[i];
            flag=1;
        }
    }
    str4[k]=str3[p];k++;
    if(flag==0)
    number_of_args=0;
    else
    number_of_args=k;
    
    // Putting together all the elements
    struct request* r=(struct request*)malloc(sizeof(struct request*));
    r->dll_name=str1;
    r->func_name=str2;
    r->func_args=str4;
    // Adding to the request the immediate request made by the client.
    pthread_mutex_lock(&mutexQueue);
    bool a=Enqueue(r,client_socket);
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
    if(a==true){
    // Send Enqueue Acknowledgement to the client
    char *hello = "Added to request queue";
    send(client_socket,  hello , strlen(hello)  , 0 );}
    // Thread operations
    pthread_exit(NULL);
}

int create_thread_pool()
{
    pthread_t ttt;
    if (pthread_create(&ttt,NULL, &isFull, NULL) != 0) {
            perror("Failed to create the thread");
            return 1;
        }
        return 0;
}
// DLL function invocator
int executeTask(struct Node* task)
{
    struct request* r=task->value;
    char func_names[10];
    int i;
    double final;
    for(i=0;i<strlen(task->value->func_name);i++)
    {
        func_names[i]=task->value->func_name[i];
    }
    func_names[i]='\0';
    int client_socket=task->socket_id;
    // Receive the output from the thread and send it to client.s
    if(number_of_args==0){
        void *handle;
        double (*some_fun)();
        char *error;

        char *lib_name=r->dll_name;
        handle = dlopen (lib_name, RTLD_NOW);
        if (!handle) {
            fputs (dlerror(), stderr);
            int ans=-1;
            send(client_socket, ans, sizeof(ans), 0);
            //exit(1);
            close(client_socket);jj--;
            return 0;
        }
        
        some_fun = dlsym(handle, func_names);
        if ((error = dlerror()) != NULL)  {
            fputs(error, stderr);
            int ans=-1;
            send(client_socket, ans, sizeof(ans), 0);
            //exit(1);
            close(client_socket);jj--;
            return 0;
        }

        final=(*some_fun)();
        printf ("%f\n", (*some_fun)());
        
        int ans=1;
        send(client_socket, &final, sizeof(final), 0);
        
        // Kill the connection with the client and free the thread.
        printf("Exit socketThread \n");
        close(client_socket);jj--;
        dlclose(handle);
    }
    if(number_of_args==1){
        void *handle;
        double (*some_fun)(double);
        char *error;
        char *lib_name=r->dll_name;
        handle = dlopen (lib_name, RTLD_NOW);
        if (!handle) {
            fputs (dlerror(), stderr);
            int ans=-1;
            send(client_socket, ans, sizeof(ans), 0);
            close(client_socket);jj--;
            return 0;
        }
        
        some_fun = dlsym(handle, func_names);
        if ((error = dlerror()) != NULL)  {
            fputs(error, stderr);
            int ans=-1;
            send(client_socket, ans, sizeof(ans), 0);
            close(client_socket);jj--;
            return 0;
        }

        char *b=r->func_args[0];
        double cc;
        sscanf(b,"%lf",&cc);
        final=(*some_fun)(cc);
        printf ("%f\n", (*some_fun)(cc));
        
        int ans=1;
        send(client_socket, &final, sizeof(final), 0);
        
        // Kill the connection with the client and free the thread.
        printf("Exit socketThread \n");
        close(client_socket);jj--;
        dlclose(handle);
    }
    if(number_of_args==2){
        void *handle;
        double (*some_fun)(double,double);
        char *error;
        //printf("AA");
        char *lib_name=r->dll_name;
        handle = dlopen (lib_name, RTLD_NOW);
        if (!handle) {
            fputs (dlerror(), stderr);
            int ans=-1;
            send(client_socket, ans, sizeof(ans), 0);
            close(client_socket);jj--;
            return 0;
        }
        
        some_fun = dlsym(handle, func_names);
        if ((error = dlerror()) != NULL)  {
            fputs(error, stderr);
            int ans=-1;
            send(client_socket, ans, sizeof(ans), 0);
            close(client_socket);jj--;
            return 0;
        }

        char *b=r->func_args[0];
        char *c=r->func_args[1];
        double cc;
        double dd;
        sscanf(b,"%lf",&cc);
        sscanf(c,"%lf",&dd);
        final=(*some_fun)(cc,dd);
        printf ("%f\n", (*some_fun)(cc,dd));
        
        int ans=1;
        send(client_socket, &final, sizeof(final), 0);
        
        // Kill the connection with the client and free the thread.
        printf("Exit socketThread \n");
        close(client_socket);jj--;
        dlclose(handle);
    }
    return 1;
}

// A simple function designed for basically testing the DLL invocator
double executeforTest(struct Node* task)
{
    struct request* r=task->value;
    char func_names[10];
    int i;
    double final;
    for(i=0;i<strlen(task->value->func_name);i++)
    {
        func_names[i]=task->value->func_name[i];
    }
    func_names[i]='\0';
    int client_socket=task->socket_id;
    void *handle;
    double (*some_fun)(double);
    char *error;
    //printf("dd");
    char *lib_name=r->dll_name;
    handle = dlopen (lib_name, RTLD_NOW);
    if (!handle) {
        //fputs (dlerror(), stderr);
        return 0;
    }
    
    some_fun = dlsym(handle, func_names);
    if ((error = dlerror()) != NULL)  {
        //fputs(error, stderr);
        return 0;
    }

    char *b=r->func_args[0];
    double cc;
    sscanf(b,"%lf",&cc);
    final=(*some_fun)(cc);
    //printf ("%f\n", (*some_fun)(cc));
    return final;
}

// the thread pool is running here and as soon as a request the threads try to lock the queue
void* startThread(void* args) {
    while (1) {
        struct Node* task;
        // Critical Section
        pthread_mutex_lock(&mutexQueue);
        while (isEmpty()) {
            pthread_cond_wait(&condQueue, &mutexQueue);
        }

        task =Dequeue();
        pthread_mutex_unlock(&mutexQueue);
        // Critical Section ends
        executeTask(task);
    }
}

// Function where the server starts and listens to incoming requests
int run(int argc, char* argv[]) 
{
    // declaring the necessary variables for establishing the socket connection
    long int CLIENTSS,MAX_THREAD,PORT,FILE,MEMORY;
    char *aa;
    //The port on which the server would run.
    aa=argv[1];
    PORT=strtol(aa,NULL,10);
    //The maximum threads in thread pool. 
    aa=argv[2];
    MAX_THREAD=strtol(aa,NULL,10);
    //Maximum number of clients that server could connect to.
    aa=argv[3];
    CLIENTSS=strtol(aa,NULL,10);
    //Maximum number of files that a thread could open.
    aa=argv[4];
    FILE=strtol(aa,NULL,10);
    //Maximum amount of memory a thread could access.
    aa=argv[5];
    MEMORY=strtol(aa,NULL,10);

    if(MAX_THREAD==0 || CLIENTSS==0 || FILE==0 || MEMORY==0)
    {
        printf("Wrong Input Parameters");
        return 0;
    }
    char clients[CLIENTSS];
    InitQueue();
    pthread_t th[MAX_THREAD];
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);
    pthread_attr_t attr;

    // allocate a 8MB region for the stack.
    size_t stacksize;
    if(MEMORY!=0)
        stacksize = MEMORY*1024*1024;
    else
        stacksize=8*1024*1024;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, stacksize);

    struct rlimit old_lim, lim;
    // Set new value
    lim.rlim_cur = FILE;
    lim.rlim_max = 1024;
  
    // Set limits
    //RLIMIT_NOFILE--This specifies a value one greater than the maximum file descriptor  number  that  can be opened by this process.
    if(setrlimit(RLIMIT_NOFILE, &lim) == -1)
        fprintf(stderr, "%s\n", strerror(errno));
    
    int j;
    for (j = 0; j < MAX_THREAD; j++) {
        if (pthread_create(&th[j], &attr, &startThread, NULL) != 0) {
            perror("Failed to create the thread");
        }
    }
    pid_t c_pid;
    char* hello = "Hello from server"; 
    printf("%s\n",hello);
    int s_fd;
    struct sockaddr_in address,temp_address;  // struct to store the deatails of the newly created server socket.
    // Creating socket file descriptor 
    // AF_INET is is used for IPv4 protocol while AF_INET6 is used for IPv6 protocol
    // SOCK_STREAM is used for setting up TCP connection 
    // Protocol value for Internet Protocol(IP), which is the 0.
    if ((s_fd = socket(AF_INET,  SOCK_STREAM, 0)) == 0) { 
        // if file descriptor is 0, the socket is not created successfully
        printf("socket creation failed"); 
        return -1; 
    } 
    // telling the values of server PORT i.e. 8090 to the struct address and the family as well i.e IPv6
    address.sin_family = AF_INET; //IPv4
    address.sin_addr.s_addr = INADDR_ANY; //will listen to any IP
    // htons - host-to-network short
    address.sin_port = htons(PORT); //specific port 

    // binding the socket to  the port 8090 and address
    if (bind(s_fd, (struct sockaddr*)&address, sizeof(address)) < 0) { 
        // negative output indicates socket binding faiure
        printf("socket binding failed"); 
        return -1; 
    } 
    // Now the socket is successfully bind
    // converting the socket into listening socket using listen system call 
    if (listen(s_fd, CLIENTSS) < 0) { 
        // If listening failed
        printf("Could not listen"); 
        return -1; 
    } 
    // Number of clients that the connection can at once atmost listen to
    pthread_t tid[CLIENTSS];
    
    // Now the port is a listening port and ready to accept client query
    while(1){
        // So that no extra client connection is established in the socket. 
        if(jj>CLIENTSS)
            continue;
        else{
            int client_socket;
            int addrlen = sizeof(temp_address);
            // Connecting with the client
            if ((client_socket = accept(s_fd, (struct sockaddr*)&temp_address, (socklen_t*)&addrlen)) < 0) { 
                // If coudn't accept successfully, function would return negative value
                printf("Could not accept"); 
                return -1; 
            }
            if(jj>=CLIENTSS)
            {
                close(client_socket);
            }
            // If successfully accepted the connection 
            else if(jj<CLIENTSS){
                printf("Connection accepted\n");
                //inet_ntop(AF_INET, &(temp_address.sin_addr), clients, CLIENTSS);
                // Now server is accepting client's requests and multithreading
                if( pthread_create(&tid[jj++], NULL, socketThread, &client_socket) != 0 )
                    printf("Failed to create thread\n");
            }
        }
    }
    // Closing all the running threads.
    for (jj = 0; jj < MAX_THREAD; jj++) {
        if (pthread_join(th[jj], NULL) != 0) {
            perror("Failed to join the thread");
        }
    }
    // Destroying the locks and conditional variables
    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);
    return 0;
} 