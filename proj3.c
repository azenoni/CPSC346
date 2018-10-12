/*
Class: CPSC 346-02
Team Member 1: Andrew Zenoni
Team Member 2: N/A 
GU Username of project lead: azenoni
Pgm Name: proj3.c 
Pgm Desc: exploraiton of the proc file system 
Usage: 1) standard:  ./a.out -s 
Usage: 2) history:  ./a.out -h 
Usage: 3) load:  ./a.out -l 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void standard();
void history();
void load();

int main(int argc, char* argv[])
{
 if (argc != 2)
  {
   fprintf(stderr, "Error: Options required\n"); 
   fprintf(stderr, "usage: ./a.out -s|-h|-l\n\n"); 
   exit(EXIT_FAILURE);
  }
  
 if (!strcmp(argv[1],"-s")) {
  standard();  
 } else if (!strcmp(argv[1],"-h")) {
  history();  
 } else if (!strcmp(argv[1],"-l")) {
  load();  
 } else {
  fprintf(stderr, "Error: Unrecognized token '%s'\n", argv[1]);
 }
}

/*
pre: none
post: displays CPU vendor_id, model name, and OS version
*/
void standard()
{
 char ch;
 FILE* ifp;
 char str[80];

 /*
 I've deliberately used two different mechanisms for writing to the console.  
 Use whichever suits you.
 strstr locates a substring
 */

 ifp = fopen("/proc/cpuinfo","r");
 while (fgets(str,80,ifp) != NULL)
  if (strstr(str,"vendor_id") || strstr(str,"model name"))
   puts(str); 
 fclose (ifp);

 ifp = fopen("/proc/version","r");
 while ((ch = getc(ifp)) != EOF)
  putchar(ch);
 fclose (ifp);
}

/*
pre: none
post: displays time since the last reboot (DD:HH:MM:SS), time when the system was last 
      booted, number of processes that have been creates since the last reboot 
      Hint: strftime could be useful
*/
void history()
{
  char ch;
  FILE* ifp;
  char str[80];
  char buffer[80];
  int mins;
  int hours;
  int days;
  int leftover;
  int seconds;
  time_t now;
  struct tm* info;


  ifp = fopen("/proc/uptime", "r");
  while(fgets(str, 9, ifp) != NULL) {
    days = atoi(str)/60/60/24;
    leftover = atoi(str)/60/60%24;
    hours = leftover;
    leftover = atoi(str)/60%60;
    mins = leftover;
    seconds = atoi(str)%60;
    
    printf("Time since last reboot: %d:%d:%d:%d\n",days, hours, mins, seconds);
    break;
  }
  fclose(ifp);

  time(&now);
  now = now - atoi(str);
  info = localtime(&now);
  strftime(buffer, 80, "%x at %I:%M%p\n", info);
  printf("System booted on %s", buffer);

  ifp = fopen("/proc/stat", "r");
  while(fgets(str,80, ifp) != NULL) {
    if(strstr(str,"process")) {
      puts(str);
    }
  }
  fclose(ifp);
}

/*
pre: none
post: displays total memory, available memory, load average (avg. number of processes over the last minute) 
*/
void load()
{
  FILE* ifp;
  char str[80];

  ifp = fopen("/proc/meminfo", "r");
  while(fgets(str, 80, ifp) != NULL) {
    if(strstr(str, "MemTotal:") || strstr(str, "MemAvailable:")){
      puts(str);
    }
  }
  fclose(ifp);

  ifp = fopen("/proc/loadavg", "r");
  while(fgets(str, 4, ifp) != NULL) {
    printf("Load average in last minute: %s\n", str);
    break;
  }
  fclose(ifp);
}
