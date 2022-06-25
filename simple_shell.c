#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void handle(int);
int checkInput(char*);
void checkMemoryFailure(char*);

char* filepath = NULL;

int main() {
  char* path = getcwd(NULL, 0);  //get current path and allocates space according to the path's size
  filepath = (char*) malloc(strlen(path) + 9); //allocate space for filepath of "log.txt"
  strcpy(filepath, path);
  strcat(filepath, "/log.txt");  //full file path

  signal(SIGCHLD, handle);  //Child processes send SIGCHLD when terminated

  while(1) {
    char input[100];
    char* args[100];   //command arguments to be executed in child process
    int argc = 0, flag = 0;

    do {
      printf("Shell >> ");
      fgets(input, sizeof(input), stdin);
    } while((strcmp(input, "\n") == 0) || checkInput(input));
    //Print "Shell >>" again if the user presses "enter" only or entered a 	    string containing spaces only

    char* token = strtok(input, "\n");   //remove "\n" from input

    if(token[strlen(token)-1] == '&') {  //If the last letter is '&'
      flag = 1;  //Parent will not wait for the child
      token[strlen(token)-1] = ' ';//replace '&' with space (new delimeter)
    }

    token = strtok(token, " "); //get the first token until the first space

    if(strcmp(token, "exit") == 0){
      exit(0);  //Exit the shell if token = "exit"
    }

    while(token != NULL) {
      args[argc] = (char*)malloc(strlen(token));
      checkMemoryFailure(args[argc]);
      strcpy(args[argc], token);  //set arguments array: args[]
      token = strtok(NULL, " ");  //get next argument
      argc++;  //index of arguments in args[]
    }
    args[argc] = NULL;

    if(strcmp(args[0], "cd") == 0 && args[1] != NULL) {  //cd command
      char* dir = (char*) malloc(strlen(args[1]));
      checkMemoryFailure(dir);
      strcpy(dir, args[1]);  //put arg[1] in dir then cocat. the rest
      for(int i = 2; i < argc; i++) {
        //allocate space for the entered dir: size of (arg[1]+arg[2]+...)
        dir = (char*) realloc(dir, strlen(dir) + strlen(args[i]));
        checkMemoryFailure(dir);
        //Concatenate to the first argument to get the full path (dir)
        strcat(dir, " ");  //E.x: folder name is "Simple Shell"
        strcat(dir, args[i]);
      }
      chdir(dir);   //execute cd command
      free(dir);  //free the allocated space for dir and arguments
      for(int i = 0; args[i] != NULL; i++) {
        free(args[i]);
      }
      continue;  //Take another input from user
    }

    pid_t pid = fork();   //creates a child process
    if(pid < 0){  //Process creation failure
      printf("Fork failed ! Unable to create Child process.");
      return -1;
    } else if (pid == 0) {  //Child Process
      int failChecker = execvp(args[0], args);
      if(failChecker < 0) {   //If execution failed
        printf("\"%s\" few arguments or undefined command in the terminal !\n", input);
        kill(getpid(), SIGKILL);  //kill this child process
        exit(EXIT_FAILURE);
      }
    } else {  //Parent Process
      if(flag == 0) {   //Parent waits for the child to execute
        waitpid(pid, NULL, 0);
      } else {  //Parent doesn't wait for the child
        printf("[%d]\n", pid); //Child that is running in the background
      }
    }
    for(int i = 0; args[i] != NULL; i++) {
      free(args[i]);  //free the allocated space for arguments
    }
  }
  free(path);
  free(filepath);  //free the allocated space for path and filepath
}

void handle(int signum) {  //handler that catches SIGCHLD signal on child process termination
  waitpid(-1, NULL, WNOHANG);  //To get rid of zombie childs
  FILE* fp;
  fp = fopen(filepath, "a");
  if(!fp) {   //file couldn't open
    printf("Error in opening the file !\n");
    exit(-1);
  }
  fprintf(fp, "Child process was terminated\n");  //append in log.txt file
  fclose(fp);
}

int checkInput(char* input) {
  for(int i =0; i < strlen(input)-1; i++) {  //strlen()-1 to ignore '\n'
    if(input[i] != ' ' && input[i] != '\t') {  //If the input has 							 characters other than spaces
      return 0;
    }
  }
  return 1;  //If the input consists of spaces or tabs only
}

void checkMemoryFailure(char*str) {  //Check if malloc() or realloc() fails
  if(str == NULL) {
    printf("Failure in memory allocation\n");
    exit(-1);
  }
}
