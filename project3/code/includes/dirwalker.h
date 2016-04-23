#include <lib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <ftw.h>

int returnzones(const char* fpath,const struct stat* sb,int typeflag)
{
  printf("fpath:%s  inodenumber:%d\n",fpath,sb->st_ino);
  message m;
  m.m9_l1 = sb->st_dev;
  m.m9_l2 = sb->st_ino;
  return _syscall(VFS_PROC_NR,106,&m);
}
int directorywalker(char* path)
{
  ftw(path,returnzones,30);
}
