// Preetesh Verma
// 2018eeb1171
// including the libraries for the code
#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>
#include <string.h> 
#include <math.h>
#include <limits.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h> 
#include <sys/resource.h>
#include <stdio.h>
#include <ctype.h>
#include<pthread.h> // for multithreading 
#include <unistd.h> //unistd.h is the name of the header file that provides access to the POSIX operating system API. 
// Portable Operating System Interface POSIX

double revolution_speed;
double seek_time;
double sector_size;
double sum=0;
clock_t t;
int number_of_requests=1000;
double rotation_delay;
double transfer_time;
int matrix[4][25][20];

// Declaring a structure class
// The purpose of this node is to help in applying different types of heuristics to the threads 
// in deciding which thread to terminate.
struct request {
    int platter_number;
    int track_number;
    int sector_begin;
    int sector_count;
    clock_t start;
};

// Details given in the question itself
int number_of_surfaces=4;
int read_write_heads=4;
int cylinder_count=25;
int sector_per_cylinder=20;

// struct array for the requests
struct request* array[1000];

// Simple Functions to compute min and max of two values.
double maximum(double a,double b)            
{                           
    return a > b ? a : b;       
}

double minimum(double a,double b)            
{                           
    return a < b ? a : b;       
}

int min(int a,int b)            
{                           
    return a < b ? a : b;       
}


/*--------------------------------------------
         QUEUE IMPLEMENTATION BEGINS
--------------------------------------------*/

struct Node {
    struct Node* next;
    double data;
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
bool Enqueue(double r)
{
    struct Node *temp;

    // Check to see if the Queue is full
    if(isFull()) 
        return false;

    // Create new node for the queue
    temp = (struct Node *)malloc(sizeof(struct Node*));
    temp->data = r;
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

/*--------------------------------------------
         QUEUE IMPLEMENTATION ENDS
--------------------------------------------*/

// To create random request for memory allocation
int generate_random_request(int lower, int upper)
{
    int num=(rand()%(upper - lower + 1)) + lower;
    return num;
}

/*
Generates disk schedulling requests for the disk
*/
void request_generator_function(){

    for(int i=0;i<number_of_requests;i++)
    {
        struct request* temp=(struct request*)malloc(sizeof(struct request));
        int platter=generate_random_request(1,4);
        int cylinder=generate_random_request(1,cylinder_count);
        int sector=generate_random_request(1,sector_per_cylinder);
        int number_of_sec=generate_random_request(1,450);
        temp->platter_number=platter;
        temp->sector_begin=sector;
        temp->sector_count=number_of_sec;
        temp->track_number=cylinder;
        temp->start=clock();
        array[i]=temp;
    }   
}

/*
 calculates seek time based on number of tracks traversed and average seek time
 */
double calcSeekTime(int trackCount) {
	return (trackCount *seek_time );
}
/*
 calculates total request time based on seek time, rotational latency, and transfer time
 */
double calcRequestTime(int trackCount, int sectorCount,int sec_len) {
	return (calcSeekTime(trackCount) +rotation_delay*sec_len+ 
    (rotation_delay*(sectorCount/sector_per_cylinder) + transfer_time* sectorCount));
}


void RANDOM(struct request* arr[],int arr_size,int platter) {
	int tempHead = 0;

	int trackDistance, sectorDistance, cur_track, cur_sector,sec_c;
	// loop through requests
	for (int i = 0; i < arr_size; i=i+2) {
		cur_track = arr[i]->track_number;
		cur_sector = arr[i]->sector_begin;
        sec_c=arr[i]->sector_count;
		// calculate absolute distance
		trackDistance = abs(cur_track - tempHead);
		sectorDistance = min(abs(cur_sector - matrix[platter-1][cur_track-1][cur_sector-1]),
        abs(matrix[platter-1][cur_track-1][cur_sector-1]-cur_sector));

        // Computing the time delays
        double tt=calcRequestTime(trackDistance,sec_c,sectorDistance);
        // Computing the response time
        clock_t tep = clock() - arr[i]->start; 
        double time_taken = ((double)tep)/CLOCKS_PER_SEC; // in seconds
        time_taken=time_taken+tt;
        //printf("%lf\n",time_taken);
        // Adding to the queue
        sum+=time_taken;
        Enqueue(time_taken);
		// current track is now head
        matrix[platter-1][cur_track-1][cur_sector-1]=cur_sector;
		tempHead = cur_track;
	}
    for (int i = 1; i < arr_size; i=i+2) {
		cur_track = arr[i]->track_number;
		cur_sector = arr[i]->sector_begin;
        sec_c=arr[i]->sector_count;
		// calculate absolute distance
		trackDistance = abs(cur_track - tempHead);
		sectorDistance = min(abs(cur_sector - matrix[platter-1][cur_track-1][cur_sector-1]),
        abs(matrix[platter-1][cur_track-1][cur_sector-1]-cur_sector));

        // Computing the time delays
        double tt=calcRequestTime(trackDistance,sec_c,sectorDistance);
        // Computing the response time
        clock_t tep = clock() - arr[i]->start; 
        double time_taken = ((double)tep)/CLOCKS_PER_SEC; // in seconds
        time_taken=time_taken+tt;
        //printf("%lf\n",time_taken);
        // Adding to the queue
        sum+=time_taken;
        Enqueue(time_taken);
		// current track is now head
        matrix[platter-1][cur_track-1][cur_sector-1]=cur_sector;
		tempHead = cur_track;
	}
}


void FIFO(struct request* arr[],int arr_size,int platter) {
	int tempHead = 0;

	int trackDistance, sectorDistance, cur_track, cur_sector,sec_c;
	// loop through requests
	for (int i = 0; i < arr_size; i++) {
		cur_track = arr[i]->track_number;
		cur_sector = arr[i]->sector_begin;
        sec_c=arr[i]->sector_count;
		// calculate absolute distance
		trackDistance = abs(cur_track - tempHead);
		sectorDistance = min(abs(cur_sector - matrix[platter-1][cur_track-1][cur_sector-1]),
        abs(matrix[platter-1][cur_track-1][cur_sector-1]-cur_sector));

        // Computing the time delays
        double tt=calcRequestTime(trackDistance,sec_c,sectorDistance);
        // Computing the response time
        clock_t tep = clock() - arr[i]->start; 
        double time_taken = ((double)tep)/CLOCKS_PER_SEC; // in seconds
        time_taken=time_taken+tt;
        //printf("%lf\n",time_taken);
        // Adding to the queue
        sum+=time_taken;
        Enqueue(time_taken);
		// current track is now head
        matrix[platter-1][cur_track-1][cur_sector-1]=cur_sector;
		tempHead = cur_track;
	}
}


void SSTF(struct request* arr[],int arr_size,int platter) {
	int tempHead = 12;
	int trackCount = 0;
	int sectorCount = 0;

	int trackDistance, sectorDistance, cur_track, cur_sector,sec_c;

	// loop through requests
	for (int i = 0; i < arr_size; i++) {

		int index = -1;
		int bestTime = 100000;
	// loop through requests within to find shortest service time
		for (int j = 0; j < arr_size; j++) {
            
			int check = abs(arr[j]->track_number - tempHead);
	        // find index of shortest service time next
			if (bestTime > check && arr[j]->track_number != -1) {
				index = j;
                bestTime=check;
			}
		}
        cur_track = arr[index]->track_number;
		cur_sector = arr[index]->sector_begin;
        sec_c=arr[index]->sector_count;

		// calculate absolute distance
		trackDistance = abs(cur_track - tempHead);
        
        //printf("%d %d %d %d ",index,cur_sector,cur_track,matrix[platter-1][cur_track-1][cur_sector-1]);
		
        sectorDistance = min(abs(cur_sector - matrix[platter-1][cur_track-1][cur_sector-1]),
        abs(matrix[platter-1][cur_track-1][cur_sector-1]-cur_sector));
       
        // Computing the time delays
        double tt=calcRequestTime(trackDistance,sec_c,sectorDistance);
        // Computing the response time
        clock_t tep = clock() - arr[index]->start; 
        double time_taken = ((double)tep)/CLOCKS_PER_SEC; // in seconds
        time_taken=time_taken+tt;
        //printf("%lf\n",time_taken);
        // Adding to the queue
        sum+=time_taken;
        Enqueue(time_taken);
		// current track is now head
		tempHead = arr[index]->track_number;
		matrix[platter-1][cur_track-1][cur_sector-1]=cur_sector;
        arr[index]->track_number=-1;
	}
}


int comparator(const void* p, const void* q)
{
    return ((struct request*)q)->track_number -((struct request*)p)->track_number;
}
void SCAN(struct request* arr[],int arr_size,int platter) {

	int tempHead = 12;
    
	int trackCount = 0;
	int sectorCount = 0;
	int trackDistance, sectorDistance, cur_track, cur_sector, sec_c;
	// create vectors for each side of SCAN
    struct request* left[1000];
    struct request* right[1000];
    int p=0;
    int q=0;
	// add requests to designated sides based on head location
	for (int i = 0; i < arr_size; i++) {
		if (arr[i]->track_number <= tempHead) {
			left[p++]=arr[i];
		} else {
			right[q++]=arr[i];
		}
	}
    
	// sort vectors
	qsort(left, p, sizeof(struct request*), comparator);
    qsort(right, q, sizeof(struct request*), comparator);
    //qsort(arr, sizeof(arr) / sizeof(s), sizeof(s), (int (*) (const void *, const void *)) &comp);
	// determine which direction to traverse first, false = right, true = left
	bool direction = false;

	for (int i = 0; i < 2; i++) {
		if (direction) {
			for (int j = p - 1; j >= 0; j--) {
				cur_track = left[j]->track_number;
				cur_sector = left[j]->sector_begin;
                sec_c=left[j]->sector_count;
                // calculate absolute distance
                trackDistance = abs(cur_track - tempHead);
                sectorDistance = min(abs(cur_sector - matrix[platter-1][cur_track-1][cur_sector-1]),
                abs(matrix[platter-1][cur_track-1][cur_sector-1]-cur_sector));

                // Computing the time delays
                double tt=calcRequestTime(trackDistance,sec_c,sectorDistance);
                // Computing the response time
                clock_t tep = clock() - left[i]->start; 
                double time_taken = ((double)tep)/CLOCKS_PER_SEC; // in seconds
                time_taken=time_taken+tt;
                //printf("%lf\n",time_taken);
                // Adding to the queue
                sum+=time_taken;
                Enqueue(time_taken);
                // current track is now head
                matrix[platter-1][cur_track-1][cur_sector-1]=cur_sector;
				tempHead = cur_track;
			}
			direction = false;
		} else if (!direction) {
			for (int j = 0; j < q; j++) {
				cur_track = right[j]->track_number;
				cur_sector = right[j]->sector_begin;
                sec_c=right[j]->sector_count;
                // calculate absolute distance
                trackDistance = abs(cur_track - tempHead);
                sectorDistance = min(abs(cur_sector - matrix[platter-1][cur_track-1][cur_sector-1]),
                abs(matrix[platter-1][cur_track-1][cur_sector-1]-cur_sector));
                // Computing the time delays
                double tt=calcRequestTime(trackDistance,sec_c,sectorDistance);
                // Computing the response time
                clock_t tep = clock() - right[i]->start; 
                double time_taken = ((double)tep)/CLOCKS_PER_SEC; // in seconds
                time_taken=time_taken+tt;
                //printf("%lf\n",time_taken);
                // Adding to the queue
                sum+=time_taken;
                Enqueue(time_taken);
                // current track is now head
                matrix[platter-1][cur_track-1][cur_sector-1]=cur_sector;

				tempHead = cur_track;
			}
			direction = true;
		}
	}
}


void CSCAN(struct request* arr[],int arr_size,int platter) {

	int tempHead = 12;
    
	int trackCount = 0;
	int sectorCount = 0;
	int trackDistance, sectorDistance, cur_track, cur_sector, sec_c;
	// create vectors for each side of SCAN
    struct request* left[1000];
    struct request* right[1000];
    int p=0;
    int q=0;
	// add requests to designated sides based on head location
	for (int i = 0; i < arr_size; i++) {
		if (arr[i]->track_number <= tempHead) {
			left[p++]=arr[i];
		} else {
			right[q++]=arr[i];
		}
	}
    
	// sort vectors
	qsort(left, p, sizeof(struct request*), comparator);
    qsort(right, q, sizeof(struct request*), comparator);
    //qsort(arr, sizeof(arr) / sizeof(s), sizeof(s), (int (*) (const void *, const void *)) &comp);
	// determine which direction to traverse first, false = right, true = left
	bool direction = false;

	for (int i = 0; i < 1; i++) {
		if (direction) {
			for (int j = p - 1; j >= 0; j--) {
				cur_track = left[j]->track_number;
				cur_sector = left[j]->sector_begin;
                sec_c=left[j]->sector_count;
                // calculate absolute distance
                trackDistance = abs(cur_track - tempHead);
                sectorDistance = min(abs(cur_sector - matrix[platter-1][cur_track-1][cur_sector-1]),
                abs(matrix[platter-1][cur_track-1][cur_sector-1]-cur_sector));

                // Computing the time delays
                double tt=calcRequestTime(trackDistance,sec_c,sectorDistance);
                // Computing the response time
                clock_t tep = clock() - left[i]->start; 
                double time_taken = ((double)tep)/CLOCKS_PER_SEC; // in seconds
                time_taken=time_taken+tt;
                //printf("%lf\n",time_taken);
                // Adding to the queue
                sum+=time_taken;
                Enqueue(time_taken);
                // current track is now head
                matrix[platter-1][cur_track-1][cur_sector-1]=cur_sector;
				tempHead = cur_track;
			}
			//direction = false;
		} else if (!direction) {
			for (int j = 0; j < q; j++) {
				cur_track = right[j]->track_number;
				cur_sector = right[j]->sector_begin;
                sec_c=right[j]->sector_count;
                // calculate absolute distance
                trackDistance = abs(cur_track - tempHead);
                sectorDistance = min(abs(cur_sector - matrix[platter-1][cur_track-1][cur_sector-1]),
                abs(matrix[platter-1][cur_track-1][cur_sector-1]-cur_sector));
                // Computing the time delays
                double tt=calcRequestTime(trackDistance,sec_c,sectorDistance);
                // Computing the response time
                clock_t tep = clock() - right[i]->start; 
                double time_taken = ((double)tep)/CLOCKS_PER_SEC; // in seconds
                time_taken=time_taken+tt;
                //printf("%lf\n",time_taken);
                // Adding to the queue
                sum+=time_taken;
                Enqueue(time_taken);
                // current track is now head
                matrix[platter-1][cur_track-1][cur_sector-1]=cur_sector;

				tempHead = cur_track;
			}
			//direction = true;
		}
	}
    direction=true;
    if(direction)
        tempHead=0;
    else
        tempHead=25;
    for (int i = 0; i < 1; i++) {
		if (direction) {
			for (int j = 0; j <p; j++) {
				cur_track = left[j]->track_number;
				cur_sector = left[j]->sector_begin;
                sec_c=left[j]->sector_count;
                // calculate absolute distance
                trackDistance = abs(cur_track - tempHead);
                sectorDistance = min(abs(cur_sector - matrix[platter-1][cur_track-1][cur_sector-1]),
                abs(matrix[platter-1][cur_track-1][cur_sector-1]-cur_sector));

                // Computing the time delays
                double tt=calcRequestTime(trackDistance,sec_c,sectorDistance);
                // Computing the response time
                clock_t tep = clock() - left[i]->start; 
                double time_taken = ((double)tep)/CLOCKS_PER_SEC; // in seconds
                time_taken=time_taken+tt;
                //printf("%lf\n",time_taken);
                // Adding to the queue
                sum+=time_taken;
                Enqueue(time_taken);
                // current track is now head
                matrix[platter-1][cur_track-1][cur_sector-1]=cur_sector;
				tempHead = cur_track;
			}
			//direction = false;
		} else if (!direction) {
			for (int j = q-1; j >=0; j--) {
				cur_track = right[j]->track_number;
				cur_sector = right[j]->sector_begin;
                sec_c=right[j]->sector_count;
                // calculate absolute distance
                trackDistance = abs(cur_track - tempHead);
                sectorDistance = min(abs(cur_sector - matrix[platter-1][cur_track-1][cur_sector-1]),
                abs(matrix[platter-1][cur_track-1][cur_sector-1]-cur_sector));
                // Computing the time delays
                double tt=calcRequestTime(trackDistance,sec_c,sectorDistance);
                // Computing the response time
                clock_t tep = clock() - right[i]->start; 
                double time_taken = ((double)tep)/CLOCKS_PER_SEC; // in seconds
                time_taken=time_taken+tt;
                //printf("%lf\n",time_taken);
                // Adding to the queue
                sum+=time_taken;
                Enqueue(time_taken);
                // current track is now head
                matrix[platter-1][cur_track-1][cur_sector-1]=cur_sector;

				tempHead = cur_track;
			}
			//direction = true;
		}
	}
}


// Dispatcher which is responsible for using different algorithms for schedulling
void dispatcher(int algorithm)
{
    struct request* arr1[1000];
    struct request* arr2[1000];
    struct request* arr3[1000];
    struct request* arr4[1000];
    int a1=0,a2=0,a3=0,a4=0;
    // Segregating the requests based on the platter number or the surfaces.
    for(int i=0;i<1000;i++)
    {
        if(array[i]->platter_number==1)
            arr1[a1++]=array[i];
        if(array[i]->platter_number==2)
            arr2[a2++]=array[i];
        if(array[i]->platter_number==3)
            arr3[a3++]=array[i];
        if(array[i]->platter_number==4)
            arr4[a4++]=array[i];
    }
    printf("%d,%d,%d,%d\n",a1,a2,a3,a4);
    if(algorithm==0)
    {
        RANDOM(arr1,a1,1);
        RANDOM(arr2,a2,2);
        RANDOM(arr3,a3,3);
        RANDOM(arr4,a4,4);
    }
    if(algorithm==1)
    {
        FIFO(arr1,a1,1);
        FIFO(arr2,a2,2);
        FIFO(arr3,a3,3);
        FIFO(arr4,a4,4);
    }
    else if(algorithm==2)
    {
        SSTF(arr1,a1,1);
        SSTF(arr2,a2,2);
        SSTF(arr3,a3,3);
        SSTF(arr4,a4,4);
    }
    else if(algorithm==3)
    {
        SCAN(arr1,a1,1);
        SCAN(arr2,a2,2);
        SCAN(arr3,a3,3);
        SCAN(arr4,a4,4);
    }
    else
    {
        CSCAN(arr1,a1,1);
        CSCAN(arr2,a2,2);
        CSCAN(arr3,a3,3);
        CSCAN(arr4,a4,4);
    }
}

 
// Printing the final details for the question
void print_the_details()
{
    printf("The final results from the Algorithm are \n");
    double mean=sum/number_of_requests;
    printf("the mean time is %lf\n",mean);
    double sum_temp=0;
    double maxi=0;
    double mini=50;
    while(head!=NULL)
    {
        maxi=maximum(maxi,head->data);
        mini=minimum(mini,head->data);
        double temp=(head->data-mean)*(head->data-mean);
        sum_temp=sum_temp+temp;
        head=head->next;
    }
    double ans=sum_temp/number_of_requests;
    ans=sqrt(ans);
    printf("The standard deviation is %lf\n",ans);
    printf("The maximum time a request had to wait %lf\n",maxi);
    printf("The minimum time a request had to wait %lf\n",mini);
    ClearQueue();
}

int main(int argc, char* argv[])
{
    char *aa;
    aa=argv[1];
    //printf("%s",aa);
    revolution_speed=atoi(aa);
    aa=argv[2];
    //printf("%s",aa);
    seek_time=atoi(aa);
    aa=argv[3];
    //printf("%s",aa);
    sector_size=atoi(aa);
    //printf("\n%lf",revolution_speed);
    // printf("Enter the revolution speed\n");
    // scanf("%lf",&revolution_speed);
    // printf("Enter the seek time\n");
    // scanf("%lf",&seek_time);
    // printf("Enter the sector size\n");
    // scanf("%lf",&sector_size);
    int algorithm;
    printf("Select the algorithm you want to test\n");
    printf("[0]->Random\n[1]->FIFO\n[2]->SSTF\n[3]->SCAN\n[4]->CSCAN\n");
    scanf("%d",&algorithm);
    seek_time=seek_time*0.001;
    rotation_delay=(1000*60)/revolution_speed;
    rotation_delay=rotation_delay/2;
    rotation_delay=rotation_delay*0.001;
    transfer_time=(rotation_delay*2)/sector_per_cylinder;
    printf("The seek time, rotational time and transfer time are %lf,%lf,%lf\n",seek_time,rotation_delay,transfer_time);

    for(int i=0;i<4;i++)
    {
        for(int j=0;j<25;j++)
        {
            for(int k=0;k<20;k++)
                matrix[i][j][k]=0;
        }
    }
    request_generator_function();
    dispatcher(algorithm);
    print_the_details();
    return 0;
}