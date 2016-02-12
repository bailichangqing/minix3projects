#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
  int fd;

  char* const param[] = {"/bin/ls", NULL};

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

      execvp("ls", param);
  }
  else {
      wait(NULL);
  }
}
