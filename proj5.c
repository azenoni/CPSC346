/*
Class: CPSC 346-02
Team Member 1: Andrew Zenoni
Team Member 2: N/A 
GU Username of project lead: azenoni
Pgm Name: proj5.c 
Pgm Desc: Simulates the Peterson algorithm of shared memory with processes
Usage: 1) ./a.out
       2) ./a.out <Num> <Num> <Num> <Num> 
*/

#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

void parent(int, int);
void child(int, int);
void cs(char, int);
void non_cs(int);

int shmid, value;
int* values;

int turn;
int pr_0 = 0;
int pr_1 = 0;
int time_child = 4;
int time_child_non_cs = 7;
int time_parent = 4;
int time_parent_non_cs = 5;

int main(int argc, char* argv[]) {
  //create a shared memory segment
  shmid = shmget(0,5,0777 | IPC_CREAT);

  //attach it to the process, cast its address, and 
  values = (int*)shmat(shmid,0,0); 

  values[0] = pr_0;
  values[1] = pr_1;
  values[2] = turn;
  values[3] = 1; // parent running
  values[4] = 1; // child running

  //check for proper arguments
  if(argc == 1) {
    // init default values
    time_child = 1;
    time_child_non_cs = 5;
    time_parent = 1;
    time_parent_non_cs = 3;
  } else if(argc != 5) {
    printf("Error: Incorrect number of parameters\n");
    exit(1);
  } else {
    time_child = atoi(argv[1]);
    time_child_non_cs = atoi(argv[3]);
    time_parent = atoi(argv[0]);
    time_parent_non_cs = atoi(argv[2]);
  }
  //fork here

  if (fork() == 0)
    child(time_child, time_child_non_cs);
  else 
    parent(time_parent, time_parent_non_cs);

  //remove it 
  while(values[3] || values[4]); //wait till both processes finish
  shmdt(values);
  shmctl(shmid,IPC_RMID,0);

  return 0;
}

void parent(int time_crit_sect, int time_non_crit_sect) {
 for (int i = 0; i < 10; i++)
  {
   //protect this
    values[0] = 1;
    values[2] = 1;
    while(values[1] && values[2]);
    cs('p', time_crit_sect);
    values[0] = 0;
    non_cs(time_non_crit_sect); 
  }
  //detach shared memory from the process
  values[3] = 0;
   
}

void child(int time_crit_sect, int time_non_crit_sect) {
  for (int i = 0; i < 10; i++)
  {
    //protect this
    values[1] = 1;
    values[2] = 0;
    while(values[0] && !values[2]);
    cs('c', time_crit_sect);
    values[1] = 0;
   
    non_cs(time_non_crit_sect); 
  }
  //detach shared memory from the process
  values[4] = 0;
}

void cs(char process, int time_crit_sect) {
 if (process == 'p')
  {
   printf("parent in critical sction\n");
   sleep(time_crit_sect);
   printf("parent leaving critical section\n");
   
  }
 else
  {
   printf("child in critical sction\n");
   sleep(time_crit_sect);
   printf("child leaving critical section\n");
   
  }
}

void non_cs(int time_non_crit_sect) {
 sleep(time_non_crit_sect);
}
