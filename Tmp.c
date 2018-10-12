#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>


int my_strlen(char*);



int main(int argc, char* argv[])
{
  //A pointer to an array of pointers to char.  In essence, an array of 
  //arrays.  Each of the second level arrays is a c-string. These hold
  //the command line arguments entered by the user.
  //as exactly the same structure as char* argv[]
  printf("Hello. Your string: %s\n Length: %d\n", argv[1], my_strlen(argv[1]));
   return 0;
}

int my_strlen(char* stuff) {
    char* start = stuff;
    int length = 0;
    while(*start) {
        length++;
        *start++;
    }
    return length;
}
