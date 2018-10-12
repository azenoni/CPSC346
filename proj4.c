/*
Class: CPSC 346-02
Team Member 1: Andrew Zenoni
Team Member 2: N/A 
GU Username of project lead: azenoni
Pgm Name: proj4.c 
Pgm Desc: Simulates a full shell
Usage: 1) any shell command located in /bin 
*/

#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 80
#define TRUE 80

char** getInput();
char** parseInput(char*);
void dispOutput(char**);
int wc(char*);
char* modifyFirstWord(char*);
void addToHistory(char**, char**, int);
char* retrieveLine(char**);
void dispHistory(char**, int);

int main(int argc, char* argv[])
{
  //A pointer to an array of pointers to char.  In essence, an array of 
  //arrays.  Each of the second level arrays is a c-string. These hold
  //the command line arguments entered by the user.
  //as exactly the same structure as char* argv[]
  char **args; 
  pid_t pid;
  char** history = (char**)calloc(10, sizeof(char**));
  int historyLoc = -1; 
  int addHistory = 1;

  while (TRUE)
  {
    addHistory = 1;
    printf("myShell> ");
    fflush(stdout);
    args = getInput();

    //if the user has entered "quit" break out of the loop.
    if(args[0]) {
      if(strcmp(args[0], "quit") == 0) {
        break;
      }
      if(strcmp(args[0], "!!") == 0) { // if the user is using most recent command
        addHistory = 0;
        if(historyLoc != -1)
          args = parseInput(history[historyLoc % 10]);
        else
          printf("No history to execute\n");
      }
      if(args[0][0] == '!') { // if user is using Nth most recent command
        addHistory = 0;
        if(args[0][1] - '0' == 1 && args[0][2] && args[0][2] - '0' != 0) { // ensure N is less than 11
          printf("Error, number exceeds history capacity\n");
        } else {
          // find the command to use
          int newArgLoc = historyLoc;
          newArgLoc = newArgLoc - args[0][1] + '0' + 1;
          if(newArgLoc < 0) {
            newArgLoc += 10;
          }
          if(history[newArgLoc])
            args = parseInput(history[newArgLoc]);
          else
            printf("Erorr, not that much history is not found\n");
        }
        
      } 
      // see if the command was history, if so, display it
      if(strcmp(args[0], "history") == 0 || addHistory == 0) {
        dispHistory(history, historyLoc);
      } else { // if not, add command to history
        addToHistory(args, history, historyLoc);
        historyLoc += 1;
        historyLoc = historyLoc % 10;
      }

      // create child process
      pid = fork();

      if (pid < 0) { // fork failed
        printf("Fork failed\n");
      } else {
        if (pid == 0) { // child process
          execvp(args[0],args);
          break; //exit child process as soon as it has executed command
        } else if (args[argc-1][0] != '&') { // parent process
          wait(NULL); // wait till the child process is done
        }
      }
    }
  }
   return 0;
}
// add passed in args into the history array. history loc represents the las location in history
void addToHistory(char** args, char** history, int historyLoc) {
  historyLoc += 1;
  historyLoc = historyLoc % 10;
  history[historyLoc] = retrieveLine(args);
}

// change the args back into a single line
char* retrieveLine(char** args) {
  char* output = (char*) malloc(MAX_LINE);
  char* outputStart = output;
  int i = 0;
  while(args[i]) {
    char* arg = args[i];
    while(*arg) {
      *outputStart = *arg++;
      *outputStart++;
    }
     
    i++;
    *outputStart = ' ';
    *outputStart++;
  }
  *outputStart = '\0';
  return output;
}

/*
Reads input string from the key board.   
invokes parseInput and returns the parsed input to main
*/
char** getInput()
{
  char* inp = (char*) malloc(MAX_LINE);
  char* start = inp;
  char c;

  while ((c = getchar()) != '\n')
    *inp++ = c; //weird, yes? First add the character to the dereffed pointer
              //then go to the next position in dynamic memory 
  *inp = '\0'; 
  return parseInput(start);
}

/*
inp is a cstring holding the keyboard input
returns an array of cstrings, each holding one of the arguments entered at
the keyboard. The structure is the same as that of argv
Here the user has entered three arguements:
myShell>cp x y
*/ 
char** parseInput(char* inp)
{
  int wordCount = wc(inp);
  char** array = (char**)calloc(wordCount+1,sizeof(char**));
  int i;
  
  char* word; 
  for (i = 0; i < wordCount; i++) {
    word = (char*) malloc(20);
    array[i] = word;
    if (*inp != '&') {
      while (*inp && *inp != ' ') {
        *word = *inp++;
        *word++;
      }
    } else {
      array[i] = NULL;
    }
    
    *word = '\0';
    *inp++;
  }
  // if(wordCount > 0)
  //   array[0] = modifyFirstWord(array[0]);
  array[wordCount] = NULL;
  
  return array;
}

// modify the passed in word to be /bin/<inp>
// legacy code
char* modifyFirstWord(char* inp) {
  char* tmp = (char*) malloc(MAX_LINE);
  strcat(tmp, "/bin/");
  strcat(tmp, inp);
  return inp;
}
 
/*
Displays the arguments entered by the user and parsed by getInput
*/
void dispOutput(char** args)
{
  int i = 0;
  while(args[i]) {
    char* start = args[i];
    while(*start)
      putchar(*start++);
    putchar('\n');
    i = i + 1;
  }
} 

// finds how many args in the user input
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

// displays the history from most recent to last (top to bottom)
void dispHistory(char** history, int historyLoc) {
  int i = historyLoc;
  while(history[i]) {
    char* start = history[i];
    while(*start)
      putchar(*start++);
    putchar('\n');
    i = (i - 1);
    if(i < 0) {
      i += 10;
    }
    if (i == historyLoc)
      break;
  }
}
