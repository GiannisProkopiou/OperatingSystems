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
Semaphore *s24;
Semaphore *s35;
Semaphore *s36;
Semaphore *s25;
Semaphore *s12;
Semaphore *s46;
  
int main() 
{ 
	s12 = sem_open ("ss12", O_CREAT | O_EXCL, 0644, 0);
	if (s12 == SEM_FAILED) {
        perror("Failed to open semphore for s12");
        return -1;
    }
	s24 = sem_open ("ss24", O_CREAT | O_EXCL, 0644, 0); 
	if (s24 == SEM_FAILED) {
        perror("Failed to open semphore for s24");
        return -1;
    }
	s25 = sem_open ("ss25", O_CREAT | O_EXCL, 0644, 0); 
	if (s25 == SEM_FAILED) {
        perror("Failed to open semphore for s25");
        return -1;
    }
	s13 = sem_open ("ss13", O_CREAT | O_EXCL, 0644, 0); 
	if (s13 == SEM_FAILED) {
        perror("Failed to open semphore for s13");
        return -1;
    }
	s46 = sem_open ("ss46", O_CREAT | O_EXCL, 0644, 0); 
	if (s46 == SEM_FAILED) {
        perror("Failed to open semphore for s46");
        return -1;
    }
	s36 = sem_open ("ss36", O_CREAT | O_EXCL, 0644, 0); 
	if (s36 == SEM_FAILED) {
        perror("Failed to open semphore for s36");
        return -1;
    }
	s35 = sem_open ("ss35", O_CREAT | O_EXCL, 0644, 0); 
	if (s35 == SEM_FAILED) {
        perror("Failed to open semphore for s35");
        return -1;
    }


	pid_t pid;
	key_t shmkey; /*shared memory key */
	int shmid;
	shmkey = ftok("/dev/null", 5);
	shmid = shmget(shmkey, 6 * sizeof(my_process), 0644 | IPC_CREAT);
	my_process * p;
	p = (my_process*) shmat(shmid, NULL, 0);
	int i;

    for(i=0;i<6;i++) // loop will run n times (n=5) 
    { 
		int pid = fork();
        if(pid == 0) 
        { 
            my_process *ppprocess = (my_process*) malloc(sizeof(my_process));
			ppprocess->number = i + 1;
			ppprocess->ppid = getppid();
			ppprocess->pid = getpid();
			p[i] = *ppprocess;
			printf("%d\n", i);
			// every child process runs from here
			printf("Child #%d, PID: %d, PPID: %d\n", i, getpid(), getppid());
			printf("--->%d\n", p[i].number);
			
			if(ppprocess->number == 1){
				
				system("ls -l");
				sem_post(s12);
				sem_post(s13);
				
			}
			
			if(ppprocess->number == 2){
				
				sem_wait(s12);
				system("ls -l");
				sem_post(s24);
				sem_post(s25);
				
			}
			
			if(ppprocess->number == 3){
				
				sem_wait(s13);
				system("ls -l");
				sem_post(s35);
				sem_post(s36);
				
			}
			
			if(ppprocess->number == 4){
				
				sem_wait(s24);
				system("ls -l");
				sem_post(s46);
			}
			
			if(ppprocess->number == 5){
				
				sem_wait(s25);
				sem_wait(s35);
				system("ls -l");
			}
			
			if(ppprocess->number == 6){
				
				sem_wait(s46);
				sem_wait(s36);
				system("ls -l");
			}

			exit(0);
			// every child process stops here
        } 
		
		else if(pid < 0){
			printf ("Fork error.\n");
		}

		wait(NULL); 

    }

    sem_unlink("ss12");
    sem_unlink("ss13");
    sem_unlink("ss24");
    sem_unlink("ss25");
    sem_unlink("ss36");
    sem_unlink("ss46");
    sem_unlink("ss35");
	sem_close(s12);
	sem_close(s13);
	sem_close(s24);
	sem_close(s25);
	sem_close(s36);
	sem_close(s46);
	sem_close(s35);

	return 0;
      
} 