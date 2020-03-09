#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>

#define PROCESS_NUM 6

typedef sem_t Semaphore;

Semaphore *sem1;
Semaphore *sem2;
Semaphore *sem3;
Semaphore *sem4;

void print_semaphore_values() {
    int value1, value2, value3, value4;
    sem_getvalue(sem1, &value1);
    sem_getvalue(sem2, &value2);
    sem_getvalue(sem3, &value3);
    sem_getvalue(sem3, &value4);
    printf("\nSemaphore 1: %d | Semaphore 2: %d | Semaphore 3: %d | Semaphore 4: %d\n", value1, value2, value3, value4);
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

    sem3 = sem_open("sem3", O_CREAT, 0644, 0);
    if (sem3 == SEM_FAILED) {
        perror("Failed to open semphore for sem3");
        return -1;
    }

    sem4 = sem_open("sem4", O_CREAT, 0644, 0);
    if (sem4 == SEM_FAILED) {
        perror("Failed to open semphore for sem4");
        return -1;
    }

    print_semaphore_values();


    int i;
    for(i=1;i<=PROCESS_NUM;i++){

        pid_t pid = fork();

        if(pid == 0) {
            if(i == 1) {
                printf("\n-Process 1 (ls -l)-\n");
                system("ls -l");
                sem_post(sem1);     /* UP */
                sem_post(sem1);     /* UP */
                print_semaphore_values();
            }
            else if(i == 2) {
                sem_wait(sem1);     /* DOWN */
                printf("\n-Process 2 (pwd)-\n");
                system("pwd");
                sem_post(sem2);     /* UP */
                sem_post(sem4);     /* UP */
                print_semaphore_values();
            }
            else if(i == 3) {
                sem_wait(sem1);     /* DOWN */
                printf("\n-Process 3 (ps -l)-\n");
                system("ps -l");
                sem_post(sem2);     /* UP */
                sem_post(sem3);     /* UP */
                print_semaphore_values();
            }
            else if(i == 4) {
                sem_wait(sem4);     /* DOWN */
                printf("\n-Process 4 (hostname)-\n");
                system("hostname");
                sem_post(sem3);     /* UP */
                print_semaphore_values();
            }
            else if(i == 5) {
                sem_wait(sem2);     /* DOWN */
                sem_wait(sem2);     /* DOWN */
                printf("\n-Process 5 (echo $HOME)-\n");
                system("echo $HOME");
                print_semaphore_values();
            }
            else if(i == 6) {
                sem_wait(sem3);     /* DOWN */
                sem_wait(sem3);     /* DOWN */
                printf("\n-Process 6 (cal 01 2020)-\n");
                system("cal 01 2020");
                print_semaphore_values();
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
    sem_unlink("sem3");
    sem_close(sem3);
    sem_unlink("sem4");
    sem_close(sem4);

            
    return 0;
}
