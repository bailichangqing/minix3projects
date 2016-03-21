#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#define profilename ".nprofile"
#define TOKEN_BUFFER_SIZE (128)
#define PATH_ENTRY_NUM (1024)

char* PATH[PATH_ENTRY_NUM];      //init by profile
int inextpathslot;       //number of path entry
int maxdirectorylength;  //the max length of PATH entry
int flag_ctrld;

void setdefaultpath()
{
  inextpathslot = 3;
  PATH[0] = malloc(10);
  bzero(PATH[0],10);
  strcpy(PATH[0],"/usr/");
  PATH[1] = malloc(10);
  bzero(PATH[1],10);
  strcpy(PATH[1],"/usr/bin/");
  PATH[2] = malloc(10);
  bzero(PATH[2],10);
  strcpy(PATH[2],"/bin/");
}
int readprofile()
{
  /*
  this function try to read .nprofile from $(HOME). If cannot find it, PATH&HOME will be set to default value
  PATH entries should be seperated by ';'
  */
  char* defaulthomepath = "/home/";
  inextpathslot = 0;
  bzero(PATH,1024);
  char profilepath[1024];
  strncpy(profilepath,defaulthomepath,strlen(defaulthomepath));
  profilepath[strlen(defaulthomepath)] = '\0';
  char* username = getlogin();
  if(username == NULL)      //error happend in getlogin()
  {
    printf("cannot get username\n");
    setdefaultpath();
    return 1;
  }
  strncat(profilepath,username,1007);
  strcat(profilepath,"/");
  strncat(profilepath,profilename,9);

  //prepare to read profile
  int file;
  char* profilecontent = (char*) malloc(1048576 * sizeof(char));       // the profile should be no more than 1mb
  bzero(profilecontent,1048576 * sizeof(char));
  if((file = open(profilepath,O_RDONLY)) < 0)   //cannot open profile
  {
    printf("cannot open profile:%s\n",profilepath);
    setdefaultpath();
    free(profilecontent);
    return 1;
  }
  int readn = 0;
  readn = read(file,profilecontent,1048576 * sizeof(char));      //read all content to buffer
  if(readn == -1)           //error happend
  {
    close(file);
    printf("error happend in profile reading\n");
    setdefaultpath();
    return 1;
  }
  close(file);
  //start to extract PATH&HOME
  char* ptrHOME = NULL;
  char* ptrPATH = NULL;
  ptrHOME = strstr(profilecontent,"HOME");
  if(ptrHOME == NULL)     //set to default value
  {
    char homepath[1024];
    strncpy(homepath,defaulthomepath,strlen(defaulthomepath));
    homepath[strlen(defaulthomepath)] = '\0';
    strncat(homepath,username,1017);
    if(chdir(homepath) == -1)    //failed to change to home directory
    {
      printf("failed to change to user's home directory:%s\n",homepath);
    }
  }
  else
  {
    char* delim = " \n\t\0";
    strtok(ptrHOME,"=");
    char* dir = strtok(NULL,delim);
    if(chdir(dir) == -1)
    {
      printf("cannot change to %s\n",dir);
    }
  }

  ptrPATH = strstr(profilecontent,"PATH");
  if(ptrPATH == NULL) //set default value
  {
    setdefaultpath();
  }
  else
  {
    int i;
    for(i = 4;ptrPATH[i] != '\0' && ptrPATH[i] != ' ' && ptrPATH[i] != '\t' && ptrPATH[i] != '\n';i++)  //find out the end of PATH
    {
      ;
    }
    char* strPATH = (void*) malloc(sizeof(char) * i + 1);
    bzero(strPATH,sizeof(char) * i + 1);
    strncpy(strPATH,ptrPATH,i);       //extract the entire PATH into a new buffer
    char* delim = ";";
    char* entry = NULL;
    strtok(strPATH,"=");
    entry = strtok(NULL,delim);
    while(entry != NULL && inextpathslot != PATH_ENTRY_NUM)                //extract all PATH entry
    {
      int entrylen = strlen(entry);
      if(entry[entrylen - 1] != '/')
      {
        PATH[inextpathslot] = (char*)malloc(entrylen + 2);  //one for '/' one for '\0''
        bzero(PATH[inextpathslot],entrylen + 2);
        strncpy(PATH[inextpathslot],entry,entrylen);
        strcat(PATH[inextpathslot],"/");
      }
      else
      {
        PATH[inextpathslot] = (char*)malloc(entrylen + 1);  //one for '\0''
        bzero(PATH[inextpathslot],entrylen);
        strncpy(PATH[inextpathslot],entry,entrylen);
      }
      inextpathslot ++;
      entry = strtok(NULL,delim);
    }
    free(strPATH);
  }
  free(profilecontent);
  // //debug
  // for(int i = 0;i < inextpathslot;i++)
  // {
  //   printf("%s\n",PATH[i]);
  // }
  maxdirectorylength = 0;
  for(int i = 0;i < inextpathslot;i++)
  {
    int maxlength = strlen(PATH[i]);
    if(maxdirectorylength < maxlength)
    {
      maxdirectorylength = maxlength;
    }
  }
  // printf("maxlength:%d",maxdirectorylength);
  return 0;
}

char* readcommand()
{
  /*
  read anything from std input
  the returned pointer should be free by caller
  */
  size_t linesize = 2048;
  char* commandline = (char*) malloc(linesize);    //input content should be no longer than 2048
  bzero(commandline,linesize);
  int i;
  char content;
  for(i = 0;i < linesize - 1;i++)
  {
    content = getchar();
    if(content != EOF && content != '\n')
    {
      commandline[i] = content;
      continue;
    }
    if(content == '\n')
    {
      if(i == 0)
      {
        i = -1;
        printf(">");
        continue;
      }
      return commandline;
    }
    if(content == EOF)
    {
      if(flag_ctrld == 0)
      {
        flag_ctrld = 1;
        printf("turn off alarm\n");
      }
      else
      {
        flag_ctrld = 0;
        printf("turn on alarm\n");
      }
      printf(">");
      bzero(commandline,linesize);
      // int fd = dup(0);
      // close(0);
      // dup2(fd,0);
      i = -1;
    }
  }
  fgets(commandline,linesize - 1,stdin);        //make sure the string has a '\0'
  return commandline;
}

char** parsecommands(char* commandline)
{
  /*
  slit commandsline by TOKEN_DILIMITER
  caller should free the returned ptr
  ptr content:[[entry],...[entry],NULL]
  */
  static char* token_delimer = " \r\t\v";
  int bufferindex = 0;
  int buffersize = 128;
  char* token = NULL;
  char** tokens = (char**) malloc(buffersize);
  bzero(tokens,buffersize);
  token = strtok(commandline, token_delimer);
  while (token != NULL)
  {
    if (bufferindex >= buffersize)
    {
      buffersize += 10;
      tokens = realloc(tokens, buffersize);
    }
    tokens[bufferindex] = token;
    bufferindex++;
    token = strtok(NULL, token_delimer);
  }
  if (bufferindex >= buffersize)
  {
    buffersize += 10;
    tokens = realloc(tokens, buffersize);
  }
  tokens[bufferindex] = NULL;
  //tokens[bufferindex - 1][strlen(tokens[bufferindex - 1]) - 1] = '\0';    //delete '\n'
  return tokens;
}

int executecommands(char** commands)
{
  /*
  commands must contain at least 1 item, it could be "" if the user just typed enter
  if user use relative path, this functon should look up that command in directories specified by PATH
  */
  if(commands[0][0] == '\0')
  {
    return 0;
  }
  char programname[strlen(commands[0]) + maxdirectorylength];
  int flag_executable = 0;  //cannot find command:-1 otherwise:0
  //check it is relative path or absolute path
  if(commands[0][0] == '.' || commands[0][0] == '~' || commands[0][0] == '/')     //absolute path
  {
    strncat(programname,commands[0],strlen(commands[0]));
    flag_executable = access(programname,F_OK);
  }
  else
  {
    for(int i = 0;i < inextpathslot;i++)      //find out which directory in PATH contains command file
    {
      strncat(programname,PATH[i],strlen(PATH[i]));
      strncat(programname,commands[0],strlen(commands[0]));
      if((flag_executable = access(programname,F_OK)) != 0)   //cannot find file in this directory
      {
        bzero(programname,strlen(commands[0]) + maxdirectorylength);
      }
      else
      {
        break;
      }
    }
  }
  if(flag_executable != 0)  //command file doesn't exist
  {
    printf("cannot find command:%s\n",commands[0]);
    return 1;
  }
  else if((flag_executable = access(programname,X_OK)) != 0)
  {
    printf("permission denied:%s\n",programname);
  }
  else      //try to execute command
  {
    int sleeptime = 5;
    int pid = fork();
    if(0 == pid)    //this is child process
    {
      for(int i = 0;commands[i] != NULL;i++)
      {
        if(strcmp(commands[i],"=>") == 0)   //redirect stdout for child process
        {
          commands[i] = NULL;
          if(commands[i + 1]  == '\0')
          {
            printf("=> : donnot know where the stander output should be redirected to\n");
          }
          int fd = open(commands[i + 1], O_WRONLY | O_CREAT);
          if(fd < 0)    //cannot redirect
          {
            break;
          }
          else
          {
            dup2(fd, STDOUT_FILENO);
            close(fd);
          }
          break;
        }
      }
      printf("programname:%s",programname);
      execv(programname,commands);
      printf("failed to execute %s\n",programname);
      return 1;
    }
    else      // main process
    {
      if(flag_ctrld == 0)
      {
        sleep(sleeptime);
      }
      //check if child process is still running
      if(0 == waitpid(pid,NULL,WNOHANG))//still running
      {
        // kill(pid,SIGSTOP);
        if(flag_ctrld == 0)
        {
          char yn = 'y';
          printf("do you want to terminate this process? y/n [Y]\n");
          yn = getchar();
          if(yn == '\n' || yn == 'y' || yn == 'Y')  //default option yes
          {
            kill(pid,SIGKILL);
          }
          if(yn != '\n')
          {
            getchar();      //delete '\n'
          }
        }
      }
      wait(NULL);
    }
  }
  return 0;
}
int shell()
{
  // readprofile();
  char* commandline = NULL;
  char** commands = NULL;
  if (readprofile() != 0)
  {
    printf("error happend in .nprofile reading\n");
  }
  while(1)
  {
    printf(">");
    commandline = readcommand();
    // //debug
    // printf("commandline:%s\n",commandline);
    commands = parsecommands(commandline);
    // for(int i = 0;commands[i] != NULL;i++)
    // {
    //   printf("%s",commands[i]);
    // }
    // printf("parameter zero%s\n",commands[0]);
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
