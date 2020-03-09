#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>

#define PROCESS_NUM 5

typedef sem_t Semaphore;

Semaphore *sem1;
Semaphore *sem2;

void print_semaphore_values() {
    int value1, value2;
    sem_getvalue(sem1, &value1);
    sem_getvalue(sem2, &value2);
    printf("\nSemaphore 1: %d | Semaphore 2: %d\n", value1, value2);
}


int main(){

    sem1 = sem_open("sem1", O_CREAT, 0644, 0);
    if (sem1 == SEM_FAILED) {
        perror("Failed to open semphore for sem1");
        return -1;
    }

    sem2 = sem_open("sem2", O_CREAT, 0644, 0);
    if (sem2 == SEM_FAILED) {
        perror("Failed to open semphore for sem2");
        return -1;
    }

    print_semaphore_values();


    int i;
    for(i=0;i<PROCESS_NUM;i++){

        pid_t pid = fork();

        if(pid == 0) {
            if(i == 0) {
                printf("\n-Process 0 (ls -l)-\n");
                system("ls -l");
                sem_post(sem1);     /* UP */
                print_semaphore_values();
            }
            else if(i == 1) {
                printf("\n-Process 1 (pwd)-\n");
                system("pwd");
                sem_post(sem1);     /* UP */
                print_semaphore_values();
            }
            else if(i == 2) {
                sem_wait(sem1);     /* DOWN */
                print_semaphore_values();
                sem_wait(sem1);     /* DOWN */
                print_semaphore_values();
                printf("\n-Process 2 (ps -l)-\n");
                system("ps -l");
                sem_post(sem2);     /* UP */
                print_semaphore_values();
                sem_post(sem2);     /* UP */
                print_semaphore_values();
            }
            else if(i == 3) {
                sem_wait(sem2);     /* DOWN */
                print_semaphore_values();
                printf("\n-Process 3 (hostname)-\n");
                system("hostname");
            }
            else if(i == 4) {
                sem_wait(sem2);     /* DOWN */
                print_semaphore_values();
                printf("\n-Process 4 (echo $HOME)-\n");
                system("echo $HOME");
            }
            
            exit(0);
        }
        else if(pid<0) {
            printf("Fork error.\n");
        }

        wait(NULL);
    }

    print_semaphore_values();

    sem_unlink("sem1");
    sem_close(sem1);
    sem_unlink("sem2");
    sem_close(sem2);

            
    return 0;
}
