#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>


int main()
{
  char* const param[] = {"./Ahh",NULL};
  int pid = fork();
  if(pid == 0)
  {
    execl("./Ahh","",NULL);
    printf("error\n");
    return 1;
  }
  sleep(3);
  int ret;
  ret = kill(pid,SIGSTOP);
  printf("ret : %d\n",ret);
  sleep(2);
  ret = kill(pid,SIGCONT);
  printf("ret : %d\n",ret);
  sleep(2);
  kill(pid,SIGKILL);
  return 0;
}
