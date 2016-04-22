#include <lib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <ftw.h>

int inodemapwalker(char* mount_point){
  /* mount_point is a string with no more than 20 chars */
    message m;
    m.m1_p1 = mount_point;
    return _syscall(VFS_PROC_NR,108,&m);
}
int zonemapwalker(char* mount_point){
    message m;
    m.m1_p1 = mount_point;
    return _syscall(VFS_PROC_NR,109,&m);
}
int inodemapdamager(short i)
{
  message m;
  m.m5_s1 = i;
  m.m5_s2 = 0;
  return _syscall(VFS_PROC_NR,105,&m);
}
int zonemapdamager(short i)
{
  message m;
  m.m5_s1 = i;
  m.m5_s2 = 1;
  return _syscall(VFS_PROC_NR,105,&m);
}

int inodemapfixer(char* mount_point)
{
  message m;
  m.m1_p1 = mount_point;
  m.m1_i1 = 0;
  return _syscall(VFS_PROC_NR,103,&m);
}
int zonemapfixer(char* mount_point)
{
  message m;
  m.m1_p1 = mount_point;
  m.m1_i1 = 1;
  return _syscall(VFS_PROC_NR,103,&m);
}
