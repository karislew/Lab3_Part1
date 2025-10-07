// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
  
int main() 
{ 
    // We use two pipes 
    // First pipe to send input string from parent 
    // Second pipe to send concatenated string from child 
  
    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 
  
    char fixed_str1[] = "howard.edu"; 
    char fixed_str2[] = "gobison.org";
    char input_str[100]; 
    char concat_str[200];    
    pid_t p; 
  
    if (pipe(fd1)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    if (pipe(fd2)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
  
    printf("Enter a string to concatenate:");
    scanf("%s", input_str); 
    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed" ); 
        return 1; 
    } 
  
    // Parent process 
    else if (p > 0) 
    { 
  
        close(fd1[0]);  // Close reading end of pipes 
        close(fd2[1]);
  
        // Write input string and close writing end of first 
        // pipe. 
        write(fd1[1], input_str, strlen(input_str)+1); 
        
  
        // Wait for child to print the concatenated string 
        wait(NULL); 
        read(fd2[0],concat_str, sizeof(concat_str));
        strcat(concat_str,fixed_str2);
        printf("Final concatenated string: %s\n",concat_str);

        close(fd2[1]); // Close writing end of pipes 
        close(fd1[0]); 
    } 
  
    // child process 
    else
    { 
        close(fd1[1]);  // Close writing end of first pipes 
        close(fd2[0]); 
      
        // Read a string using first pipe 
        char recv_str[200];
        char user_input[100];

        read(fd1[0],recv_str,sizeof(recv_str));
        strcat(recv_str,fixed_str1);
        printf("Concatenated String so far %s\n",recv_str);
        //char concat_str[100]; 
        //read(fd1[0], concat_str, 100); 
        printf("Enter Another String to Append: ");
        scanf("%s",user_input);

        strcat(recv_str,user_input);
        write(fd2[1],recv_str,strlen(recv_str)+1);
 
  
        // Close both reading ends 
        close(fd1[0]); 
        close(fd2[1]); 

  
        exit(0); 
    } 
    return 0;
} 