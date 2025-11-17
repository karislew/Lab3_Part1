#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

/**
 * Executes the command "cat scores | grep Lakers".  In this quick-and-dirty
 * implementation the parent doesn't wait for the child to finish and
 * so the command prompt may reappear before the child terminates.
 *
 */

int main(int argc, char **argv)
{
  
  //pipe1[1] = write end (from cat), pipe1[0] = read end (to grep)
  int pipe1[2];
  //pipe2[1] = write end (from grep), pipe2[0] = read end (to sort)
  int pipe2[2];

  pid_t pid1,pid2;
  //create both pipes
  pipe(pipe1);
  pipe(pipe2);

  //first fork - creates the grep/sort branch
  pid1 = fork();
  

  // make a pipe (fds go in pipefd[0] and pipefd[1])

  if (pid1 == 0)
    {
      //child process 1-> will run either grep or sort 
      //second fork inside child- creates the sort process
      pid2 = fork();

      if(pid2 ==0){
        //child 2 runs sort 
        //connect pipe2 read end to STDIN 
        dup2(pipe2[0],STDIN_FILENO);
        //close unused FDs 
        close(pipe2[1]);
        close(pipe1[0]);
        close(pipe1[1]);
        //execute sort 
        execlp("sort","sort",NULL);
        perror("exec sort failed");
        exit(1);
      }
      else{
        //child 1 -> runs grep
        //connect stdin to pipe1 output (from cat)
        dup2(pipe1[0],STDIN_FILENO);
        //connect stdout to pipe2 input (to sort)
        dup2(pipe2[1],STDOUT_FILENO);

        //close unused FDs
        close(pipe1[1]);
        close(pipe2[0]);

        //grep <pattern>
        char *grep_args[] = {"grep",argv[1],NULL};

        execvp("grep", grep_args);
        perror("exec grep failed");
        exit(1);
    }
      
  }
  else
    {
      //parent runs cat
      //cat writes into pipe1's write end
      dup2(pipe1[1],STDOUT_FILENO);

      //close unused pipe ends 
      close(pipe1[0]);
      close(pipe2[0]);
      close(pipe2[1]);

      //execute cat scores
      char *cat_args[] = {"cat","scores",NULL};
      execvp("cat",cat_args);
      perror("exec cat failed");
      exit(1);
    }
  return 0;
}
