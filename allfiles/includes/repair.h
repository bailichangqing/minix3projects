#include <lib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <ftw.h>

int inodewalker(char* mount_point){
  /* mount_point is a string with no more than 20 chars */
    message m;
    m.m1_p1 = mount_point;
    int x=_syscall(VFS_PROC_NR,108,&m);
    return x;
}


int zonemapwalker(char* mount_point){
    message m;
    m.m1_p1 = mount_point;
    int x=_syscall(VFS_PROC_NR,109,&m);
    return x;
}


int bitmapdamager(short inodenumber,short inodezoneflag)
{
  message m;
  m.m5_s1 = inodenumber;
  m.m5_s2 = inodezoneflag;
  return _syscall(VFS_PROC_NR,105,&m);
}

int inodebitmapfixer(char* mount_point)
{
  message m;
  m.m1_p1 = mount_point;
  int x=_syscall(VFS_PROC_NR,103,&m);
  return x;
}
