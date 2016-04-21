#include <lib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>


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


int inodefixer(int n){
    message m;
    m.m1_i1=NULL;
    int x=_syscall(VFS_PROC_NR,108,&m);

    message m2;
    m2.m1_i1=NULL;
    m2.m1_i2=n;
    int y=_syscall(VFS_PROC_NR,109,&m2);
    return y;
}
