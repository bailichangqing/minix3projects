#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>


int main()
{
  for(int i = 0;i < 10;i++)
  {
    sleep(1);
    printf("alive\n");
  }
  return 0;
}
