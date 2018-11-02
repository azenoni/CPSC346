/*
Class: CPSC 346-02
Team Member 1: Andrew Zenoni
Team Member 2: N/A 
GU Username of project lead: azenoni
Pgm Name: proj7.c 
Pgm Desc: Uses pthreads and pipes to find prime numbers
To compile: gcc proj7.c -lpthread
Usage: 1) ./a.out <Num>
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <time.h>

void* readThread(void*);
void* writeThread(void*);
int is_prime(int);

struct params
{
    int num;
    int* pipe;
};

typedef struct params param;
 
int main(int argc, char* argv[])
{
    srand(time(0));
    int p[2], i; 

    if (pipe(p) < 0) 
        exit(1); 

    pthread_t t1,t2,t3,t4;
    param p1,p2,p3,p4;
    int n, status;
    
    p1.num = atoi(argv[1]);
    p1.pipe = p;
    status = pthread_create(&t1, NULL, writeThread, (void*)&p1 );
    if (status != 0)
    {
        printf("Error in thread 1:  %d\n",status);
        exit(-1);
    }

    // printf("thread1 created\n");

    p2.pipe = p;
    p2.num = 1;
    status = pthread_create(&t2, NULL, readThread, (void*)&p2 );
    if (status != 0)
    {
        printf("Error in thread 2:  %d\n",status);
        exit(-1);
    }

    // printf("thread2 created\n");

    p3.pipe = p;
    p3.num = 2;
    status = pthread_create(&t3, NULL, readThread, (void*)&p3 );
    if (status != 0)
    {
        printf("Error in thread 3:  %d\n",status);
        exit(-1);
    }

    // printf("thread3 created\n");

    p4.pipe = p;
    p4.num = 3;
    status = pthread_create(&t4, NULL, readThread, (void*)&p4 );
    if (status != 0)
    {
        printf("Error in thread 4:  %d\n",status);
        exit(-1);
    }

    // printf("thread4 create\n");
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    pthread_join(t4,NULL);
    return 0; 
}

void* writeThread(void* param_in) {
    //open write end of pipe
    int nums_generated = 0;
    param* p = (param*)param_in;
    int* pipe = p->pipe;
    //close(pipe[0]);
    while (nums_generated < p->num) {
        int r = rand() % 157;
        // write it to the pipe;
        printf("Generated: %d\n", r);
        write(pipe[1], &r, sizeof(r));
        nums_generated++;
    } 
    //close pipe  //reader does not block when reading a closed pipe
    close(pipe[1]);
    pthread_exit(NULL);
}

void* readThread(void* param_in) {
    
    param* p = (param*)param_in;
    int* j = p->pipe;
    //close(j[1]);
    int num;
    while (read(j[0], &num, sizeof(num)) > 0)  //there was data in the pipe
    {
        //printf("Checking number: %d\n", num);
        if (is_prime(num)) {
            printf("Reader %d %d is prime\n", p->num, num);
        }
        sleep(2);
    }
    pthread_exit(NULL);

}

// void* f1(void* param_in) 
// {

//   printf("entering function 1\n");
//   param* p = (param*)param_in;
//   FILE  *fp;
//   fp = fopen(p->file_name, "w"); 

//   int ct = 1;
//   int cand = 3; 
//   fprintf(fp,"%d\n",2);
//   while (ct < p->num)
//    { 
//     if (is_prime(cand))  
//       {
//        fprintf(fp,"%d\n",cand);
//        ct++;
//       }
//     cand++;
//    }
//   fclose(fp); 
//   printf("leaving function 1\n");
//   pthread_exit(NULL);
// }

int is_prime(int num)
{
 int i = 2;
 while (i < num)
 {
  if (num % i == 0)
     return 0;
  ++i;
 }
 return 1;
} 

