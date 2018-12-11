/*
Class: CPSC 346-02
Team Member 1: Andrew Zenoni
Team Member 2: N/A 
GU Username of project lead: azenoni
Pgm Name: proj6.c 
Pgm Desc: Simulates a producer consumer problem using semaphoress
Usage: 1) ./a.out 
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_MUTEX 1
#define INIT_EMPTY 100 
#define INIT_FULL  0
#define EMPTY_ARG  0 //used when argument is optional 

const int PROD = 0;
const int CONS = 1;


typedef struct sembuf sem_struct;  //give it a more reasonable name

//prototypes for functions used to declutter main 
void set_sem_values(int,int,int);
void get_sem_values(int,int,int);
void set_sem_struct(sem_struct[],int);
int create_semaphore(int);
void fatal_error(char[]);
void cleanup(int,int,int);
void prod_cons();
void producer(sem_struct*, sem_struct*, int, int, int);
void consumer(sem_struct*, sem_struct*, int, int, int);
void criticalSection();

int main(int argc, char* argv[]) {
    prod_cons();
    return 0;
}

void prod_cons() {
    sem_struct up[1], down[1]; //create semaphore structs
    int mutex, empty, full;   //will hold semaphore identifiers 
    int value, status, ch_stat, i;
    int processesRunning;
    //set wait/signal structs used in semop 
    set_sem_struct(down,-1);
    set_sem_struct(up,1);

    //create semaphore sets using arbitrary int unique to the semaphore set. 
    mutex = create_semaphore(0);
    empty = create_semaphore(1);
    full = create_semaphore(2);
    processesRunning = create_semaphore(3);
    semctl(processesRunning, 0, SETVAL, 2);

    //set semaphores to initial values
    set_sem_values(mutex, empty, full);

    printf("Initial semaphore values: \n");
    get_sem_values(mutex, empty, full);
    //prod();
    //cons();
    //fork off a process
    if ((value = fork()) < 0)
        fatal_error("fork");
    else if (value == 0) { //child
        consumer(up, down, mutex, empty, full);
        if (semop(processesRunning, down, 1) == -1)
            fatal_error("processesRunning");
    } else {   //parent 
        producer(up, down, mutex, empty, full);
        if (semop(processesRunning, down, 1) == -1)
            fatal_error("processesRunning");
        //status = wait(&ch_stat); //wait for child to exit
    }
    //int m = semctl(processesRunning, 0, GETVAL, 0);
    //printf("Value is: %d\n", m);
    while(semctl(processesRunning, 0, GETVAL, 0));
    if (value == 0) {
        printf("Final semaphore values: \n");
        get_sem_values(mutex, empty, full); 
        cleanup(mutex,empty,full); //remove semaphores
    }
}
 
void producer(sem_struct* up, sem_struct* down, int mutex, int empty, int full) {
    int i;
    for(i = 0; i < 5; i++) {
         //empty.down();
        while(semctl(empty, 0, GETVAL, 0) == 0);
        if (semop(empty, down, 1) == -1)
            fatal_error("mutex"); 
        // mutex.down();
        while(semctl(mutex, 0, GETVAL, 0) != INIT_MUTEX);
        if (semop(mutex, down, 1) == -1)
            fatal_error("empty"); 
        
        criticalSection(PROD);
        // mutex.up();
        if (semop(mutex, up, 1) == -1)
            fatal_error("down"); 

        
        // full.up(); //experiment with putting delays after this line
        if (semop(full, up, 1) == -1)
            fatal_error("mutex"); 
        //get_sem_values(mutex, empty, full);
        sleep(3);
        
    }
}
 
void consumer(sem_struct* up, sem_struct* down, int mutex, int empty, int full) {
    int i;
    for (i = 0; i <5; i++) {
        while(semctl(full, 0, GETVAL, 0) == 0);
        // full.down();
        if (semop(full, down, 1) == -1)
            fatal_error("mutex"); 
        // mutex.down();
        while(semctl(mutex, 0, GETVAL, 0) != INIT_MUTEX);
        if (semop(mutex, down, 1) == -1)
            fatal_error("mutex"); 
        criticalSection(CONS);
        // mutex.up();
        if (semop(mutex, up, 1) == -1)
            fatal_error("mutex"); 
        // empty.up(); //experiment with putting delays after this line
        if (semop(empty, up, 1) == -1)
            fatal_error("mutex"); 
        //get_sem_values(mutex, empty, full);
        sleep(7);
    }
}

void criticalSection(int who) {
    if (who == PROD)
        printf("Producer making an item\n");
    else
        printf("Consumer consuming an item\n");
}

//create a semaphore set of 1 semaphore specified by key 
int create_semaphore(int key) {
    int new_sem;
    if ((new_sem = semget(key, 1, 0777 | IPC_CREAT)) == -1) {
        perror("semget failed");
        exit(1);
    }
    return new_sem;
}

//set struct values for down/up ops to be used in semop
//By design, we will always refer to the 0th semaphore in the set
void set_sem_struct(sem_struct sem[], int op_val) {
    sem[0].sem_num = 0;
    sem[0].sem_op = op_val;
    sem[0].sem_flg = SEM_UNDO;
} 

//set mutex, empty, full to initial values
//these semaphore sets consist of a single semaphore, the 0th 
void set_sem_values(int mutex, int empty, int full) {
    semctl(mutex, 0, SETVAL, INIT_MUTEX);
    semctl(empty, 0, SETVAL, INIT_EMPTY);
    semctl(full, 0, SETVAL, INIT_FULL);
}

//retrieve value held by the 0th semaphore, which is the second arg.
//0 in the final argument indicates that it is not used 
void get_sem_values(int mutex, int empty, int full) {
int m, e, f; 
m = semctl(mutex, 0, GETVAL, 0);
e = semctl(empty, 0, GETVAL, 0);
f = semctl(full, 0, GETVAL, 0);
printf("mutex: %d empty: %d full: %d\n", m,e,f);
}

//Display error and exit if semop fails
void fatal_error(char sem[]) {
    perror(strcat("Exiting. Failed on ",sem)); 
    exit(1);
}

//remove semaphores
void cleanup(int mutex, int empty, int full) {
    semctl(mutex, 1, IPC_RMID, EMPTY_ARG);
    semctl(empty, 1, IPC_RMID, EMPTY_ARG);
    semctl(full, 1, IPC_RMID, EMPTY_ARG);
}
