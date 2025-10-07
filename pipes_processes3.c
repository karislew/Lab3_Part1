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
  

  int pipe1[2];
  int pipe2[2];

  pid_t pid1,pid2;

  pipe(pipe1);
  pipe(pipe2);

  pid1 = fork();
  

  // make a pipe (fds go in pipefd[0] and pipefd[1])

  if (pid1 == 0)
    {
      
      pid2 = fork();

      if(pid2 ==0){
        dup2(pipe2[0],STDIN_FILENO);
        close(pipe2[1]);
        close(pipe1[0]);
        close(pipe1[1]);

        execlp("sort","sort",NULL);
        perror("exec sort failed");
        exit(1);
      }
      else{
        dup2(pipe1[0],STDIN_FILENO);
        dup2(pipe2[1],STDOUT_FILENO);

        close(pipe1[1]);
        close(pipe2[0]);

        char *grep_args[] = {"grep",argv[1],NULL};

        execvp("grep", grep_args);
        perror("exec grep failed");
        exit(1);
    }
      
  }
  else
    {
      dup2(pipe1[1],STDOUT_FILENO);

      close(pipe1[0]);
      close(pipe2[0]);
      close(pipe2[1]);

      char *cat_args[] = {"cat","scores",NULL};
      execvp("cat",cat_args);
      perror("exec cat failed");
      exit(1);
    }
  return 0;
}