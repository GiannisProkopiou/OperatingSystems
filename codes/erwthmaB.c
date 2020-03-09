#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>

#define PROCESS_NUM 10

typedef sem_t Semaphore;

typedef struct my_process
{
	int ppid;
	int pid;
	int priority;
} my_process;

typedef struct  my_heap
{
	my_process array[PROCESS_NUM];
	int size;

} my_heap;


void insertKey(my_heap * heap, my_process * key);
void heapify(my_heap * heap, int index);


Semaphore *mutex;


int main() {
	
	int i, j, k;
	int shmid;
	my_heap * heap;

	key_t shmkey;	/*shared memory key */
	shmkey = ftok("/dev/null", 6);
	shmid = shmget(shmkey, sizeof (my_heap ), 0644 | IPC_CREAT);
	heap = (my_heap *) shmat(shmid, NULL, 0);
	
	heap->size=0;
	mutex = sem_open("Sem", O_CREAT | O_EXCL, 0644, 1);
	
	
	for (i = 0; i < PROCESS_NUM; i++)
	{
		pid_t pid = fork();
		if (pid == 0)
		{
			// every child process runs from here
			
			srand(getpid());

			my_process *new_process = (my_process*) malloc(sizeof(my_process));  //create a process boject
			new_process -> priority = rand()%50;                                 //generate randdom number
			new_process -> ppid = getppid();									 //get parent's ID
			new_process -> pid = getpid();        								 //get PROCESS ID

			/*******CRITICAL RIGION STARTS HERE******/
			sem_wait (mutex);
			printf("\n-I AM INSIDE THE CRITICAL REGION-\n");
	        																
			insertKey(heap, new_process);

			heap->size++;
			for(k=0 ; k< heap->size ; k++){
				printf("%d\n",heap->array[k].priority);
			}
			
			printf("\n-I LEAVE THE CRITICAL REGION-\n");
			sem_post (mutex);
			
			/*******CRITICAL REGION ENDS HERE*******/
			
			exit(0);
			// every child process stops here
		}
		else if(pid < 0) {
			printf("Error fork!\n");
		}
	}

	wait(NULL);	// to avoid zombie process

	sem_unlink("Sem");   
	sem_close(mutex);                  

	return 0;
}

void insertKey(my_heap * heap, my_process * key) {

	heap->array[heap->size]=*key;
	heapify(heap, heap->size);
}

void heapify(my_heap * heap, int index){					//MIN HEAP

	if(index<0)
		return;

	int parent=(index-1) / 2;

	if(parent >= 0){
		
		if(heap->array[index].priority < heap->array[parent].priority ) {

			my_process temp=heap->array[parent];
			heap->array[parent]=heap->array[index];
			heap->array[index]=temp;

			heapify(heap,parent);
		}
	}
}
