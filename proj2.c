/*
Class: CPSC 346-02
Team Member 1: Andrew Zenoni
Team Member 2: N/A 
GU Username of project lead: azenoni
Pgm Name: proj2.c
Pgm Desc: Asks the user for input and reuturns the number of words they inputted
Usage: ./a.out
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 80

char* get_inp();

void disp_inp(char*);

int wc(char*);

int main()
{
 char* inp = get_inp();
 printf("here is your input : %s\n",inp);
 
 disp_inp(inp); 
 printf("%d\n", wc(inp));
 free(inp);  //return dynamially allocated memory to the heap
 
 return 0;
}


char* get_inp()
{
 char* inp = (char*) malloc(MAX_LINE);
 char* start = inp;
 char c;

 printf("Enter text <= 80 characters in length\n");
 while ((c = getchar()) != '\n')
  *inp++ = c; //weird, yes? First add the character to the dereffed pointer
              //then go to the next position in dynamic memory 
 *inp = '\0'; 
 return start;
}

void disp_inp(char* out)
{
 while(*out)  //continue until the null character is encountered
  putchar(*out++);
 putchar('\n');
}

/*
pre:  inp is the address of an 80 byte block of memory 
post: returns the number of words stored in the block of memory.  A word
      is any sequence of characters that 
      1) are stored at the beginning of the block or 
      2) are stored at end of the block or
      3) are terminated by the end-of-line character or
      4) are preceded by and ended by one or more spaces
        
*/
int wc(char* inp)
{
	int i;
	int numWords = 0;
	int inWord = 0; // 0 false, 1 true
	for (i = strlen(inp) - 1; i >= 0; i--) {
   		if (inp[i] == ' ' && inWord == 1) {
   			numWords++;
   			inWord = 0;
   		} else if (inp[i] != ' ') {
   			inWord = 1;
   		}
	}
	if (inWord) {
		numWords++;
	}
	return numWords;
}
