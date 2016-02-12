#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define profilename ".myshellprofile"
#define TOKEN_BUFFER_SIZE (128)
#define TOKEN_DILIMITER " \r\t\v"

char PATH[1024];      //init by profile
char HOME[1024];      //init by profile


int readprofile()
{
  char profilepath[1024] = "/home/";
  char* username = getlogin();
  if(username == NULL)      //error happend in getlogin()
  {
    printf("cannot get username\n");
    return 1;
  }
  strncat(profilepath,username,1018);
  strncat(profilepath,profilename,30);
  //prepare to read profile

  int file;
  char profilebuffer[1024];
  if((file = open(profilepath,O_RDONLY)) < 0)   //cannot open profile
  {
    printf("cannot open profile\n");
    return 1;
  }
  int readn = 0;
  readn = read(file,profilebuffer,1024);      //read all content to buffer
  if(readn = -1)           //error happend
  {
    close(file);
    printf("error happend in profile reading\n");
    return 1;
  }

}

char* readcommand()
{
  //read anything from std input
  char* commandline;
  ssize_t linesize = 0;
  getline(&commandline,&linesize,stdin);
  return commandline;
}

char** parsecommands(char* commandline)
{
  //slipe commandsline by TOKEN_DILIMITER
  int bufferindex = 0;
  int buffersize = TOKEN_BUFFER_SIZE;
  char* token;
  char** tokens = malloc(buffersize * sizeof(char));
  token = strtok(commandline, TOKEN_DILIMITER);
  while (token != NULL) {
  tokens[bufferindex] = token;
  bufferindex++;

  if (bufferindex >= buffersize) {
    buffersize += TOKEN_BUFFER_SIZE;
    tokens = realloc(tokens, buffersize * sizeof(char*));
  }
  token = strtok(NULL, TOKEN_DILIMITER);
}
tokens[bufferindex] = NULL;
tokens[bufferindex - 1][strlen(tokens[bufferindex - 1]) - 1] = 0;
return tokens;
}

int executecommands(char** commands)
{
  char programname[1024] = "./";
  int i = 0;
  while(commands[i] != NULL)
  {
    strncat(programname,commands[i],1024);
    int pid = fork();
    if(pid == 0)  //child process
    {
      execl(programname,"",NULL);
      printf("cant find excutablefile:%s\n",programname);
    }
    i++;
  }
  return 0;
}
int shell()
{
  // readprofile();
  char* commandline;
  char** commands;
  while(1)
  {
    printf(">");
    commandline = readcommand();
    commands = parsecommands(commandline);
    executecommands(commands);
    free(commandline);
    free(commands);
  }
}

int main()
{
  shell();
  return 0;
}
