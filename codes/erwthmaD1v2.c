#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>


typedef struct my_process
{
	int ppid;
	int pid;
	int number;	//= srand(time(NULL));
} my_process;

typedef sem_t Semaphore;
void display(char *str);

Semaphore *s13;
Semaphore *s23;
Semaphore *s34;
Semaphore *s35;
  
int main() 
{ 

	s13 = sem_open ("s13", O_CREAT | O_EXCL, 0644, 0); 
	if (s13 == SEM_FAILED) {
        perror("Failed to open semphore for s13");
        return -1;
    }

	s23 = sem_open ("s23", O_CREAT | O_EXCL, 0644, 0);  
	if (s23 == SEM_FAILED) {
        perror("Failed to open semphore for s23");
        return -1;
    }
	s34 = sem_open ("s34", O_CREAT | O_EXCL, 0644, 0);  
	if (s34 == SEM_FAILED) {
        perror("Failed to open semphore for s34");
        return -1;
    }
	s35 = sem_open ("s35", O_CREAT | O_EXCL, 0644, 0);  
	if (s35 == SEM_FAILED) {
        perror("Failed to open semphore for s35");
        return -1;
    }

	pid_t pid;
	key_t shmkey; /*shared memory key */
	int shmid;
	shmkey = ftok("/dev/null", 56);
	shmid = shmget(shmkey, 5 * sizeof(my_process), 0644 | IPC_CREAT);

	my_process * processes; // array
	processes = (my_process*) shmat(shmid, NULL, 0);
	
	
	int i;

    for(i=0;i<5;i++) // loop will run n times (n=5) 
    { 
		int pid = fork();
        if(pid == 0) 
        {
            my_process *process = (my_process*) malloc(sizeof(my_process));
			process->number = i + 1;
			process->ppid = getppid();
			process->pid = getpid();
			processes[i] = *process;
			//printf("%d\n", i);
			// every child process runs from here
			//printf("Child #%d, PID: %d, PPID: %d\n", i, getpid(), getppid());
			//printf("--->%d\n", p[i].number);
			
			if(process->number == 1){
				printf("\n---Process 1---\n");
				system("ls -l");
				sem_post(s13);
				
			}
			
			if(process->number == 2){
				printf("\n---Process 2---\n");
				system("ls -l");
				//sem_post(s23);
				
			}
			
			if(process->number == 3){
				sem_wait(s13);
				sem_wait(s23);
				printf("\n---Process 3---\n");
				system("ls -l");
				sem_post(s34);
				sem_post(s35);
				
			}
			
			if(process->number == 4){
				sem_wait(s34);
				printf("\n---Process 4---\n");
				system("ls -l");
			}
			
			if(process->number == 5){
				sem_wait(s35);
				printf("\n---Process 5---\n");
				system("ls -l");

			}
			printf("here");
			exit(0);
			// every child process stops here
        }
		wait(NULL);

    }

	sem_unlink("s13");
    sem_close(s13);
	sem_unlink("s23");
    sem_close(s23);
	sem_unlink("s34");
    sem_close(s34);
    sem_unlink("s35");
    sem_close(s35);

    return 0;
} 