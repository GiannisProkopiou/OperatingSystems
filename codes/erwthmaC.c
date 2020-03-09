#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>

#define FILENAME "my_text.txt"

typedef sem_t Semaphore;

Semaphore *db;
Semaphore *rcSem;


void print_semaphore_values() {
    int value1, value2;
    sem_getvalue(db, &value1);
    sem_getvalue(rcSem, &value2);
    printf("\n| Database Semaphore: %d | Readers Counter Semaphore: %d |\n", value1, value2);
}


int main(){

    db = sem_open("db111111", O_CREAT, 0644, 1);
    if (db == SEM_FAILED) {
        perror("Failed to open semphore for db");
        return -1;
    }

    rcSem = sem_open("rcSem111111", O_CREAT, 0644, 1);
    if (rcSem == SEM_FAILED) {
        perror("Failed to open semphore for rcSem");
        return -1;
    }
    
    sem_init(db, 0, 1);
    sem_init(rcSem, 0, 1);


    
    int *rc;
    key_t shmkey = ftok("/dev/null", 550);
    int shmid = shmget(shmkey, sizeof(int), 0644 | IPC_CREAT);
    if (shmid < 0) {
        perror("shmget\n");
        exit (1);
    }

    rc  = (int *) shmat(shmid, NULL, 0);
    *rc = 0;
    printf("\n%d\n", *rc);

    FILE *file;
    shmkey = ftok("/dev/random", 6);
    shmid = shmget(shmkey, sizeof(FILE), 0644 | IPC_CREAT);
    if (shmid < 0) {
        perror("shmget\n");
        exit (1);
    }

    file  = (FILE *) shmat(shmid, NULL, 0);


    print_semaphore_values();


    int *flag;
    shmkey = ftok("/dev/null", 55550);
    shmid = shmget(shmkey, sizeof(int), 0644 | IPC_CREAT);
    if (shmid < 0) {
        perror("shmget\n");
        exit (1);
    }

    flag  = (int *) shmat(shmid, NULL, 0);
    *flag = 0;




    pid_t pid = fork();

    if(pid == 0) {  // Child
        // Writer
        while (*flag == 0) {
            sem_wait(db);     /* DOWN */
            printf("\n-Process Writer-\n");

            char txt[256];
            printf("Enter text: ");
            fgets(txt, sizeof(txt), stdin);  // read string

            printf("-%s-", txt);
            if(strcmp(txt, "exit\n") != 0) {
                file = fopen(FILENAME, "w");
                if (file != NULL)
                {
                    fputs(txt, file);
                    fclose(file);
                }
            }
            else {
                printf("\nEXIT\n");
                *flag = 1;
            }

            print_semaphore_values();

            sem_post(db);     /* UP */

            print_semaphore_values();
        }

        exit(0);
    }
    else if (pid > 0){  // Parent
        // Reader
        while (*flag == 0)
        {
            sem_wait(rcSem);     /* DOWN */

            print_semaphore_values();
            (*rc)++;
            printf("\n%d\n", *rc);
            if(*rc == 1)
                sem_wait(db);     /* DOWN */
            print_semaphore_values();

            sem_post(rcSem);     /* UP */

            printf("\n-Process Reader-\n");
            
            int c;
            file = fopen(FILENAME, "r");
            if (file) {
                printf("-> File Contains:\n");
                while ((c = getc(file)) != EOF)
                    putchar(c);
                fclose(file);
            }
            else {
                printf("Error Opening File!\n");
            }
            

            print_semaphore_values();

            sem_wait(rcSem);     /* DOWN */

            (*rc)--;
            printf("\n%d\n", *rc);
            if(*rc == 0)
                sem_post(db);     /* UP */
            print_semaphore_values();

            sem_post(rcSem);     /* UP */
            print_semaphore_values();
        }
    }
    else {
        printf("Fork error.\n");
    }

    wait(NULL);


    /* Parent will execute the following */
    
    sem_unlink("db111111");
    sem_close(db);
    sem_unlink("rcSem111111");
    sem_close(rcSem);

    shmdt(rc);
    shmctl(shmid, IPC_RMID, 0);
            
    return 0;
}
