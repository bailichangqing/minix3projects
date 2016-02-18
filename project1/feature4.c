#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
  int fd;

  char* param[2];
  param[1] = NULL;
  param[0] = "/bin/ls";

  fd = open("outfile", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

  if (fd < 0) {
      perror("open failed\n");
      exit(0);
  }

  pid_t pid = fork();

  if (pid < 0) {
      perror("fork failed\n");
      exit(0);
  }
  else if (pid == 0) {
      dup2(fd, STDOUT_FILENO);

      close(fd);

      execv("cat",param);
      printf("failed");
  }
  else {
      wait(NULL);
  }
}
