#include "fs.h"
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/statvfs.h>
#include <minix/vfsif.h>
#include <minix/const.h>
#include <minix/endpoint.h>
#include <stdlib.h>
#include <stdio.h>
#include <minix/callnr.h>
#include <signal.h>
#include <sys/svrctl.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <minix/com.h>
#include <minix/config.h>
#include <minix/sysinfo.h>
#include <minix/type.h>
#include <minix/vm.h>
#include <string.h>
#include <machine/archtypes.h>
#include <lib.h>
#include <assert.h>
#include <string.h>

#include "fproc.h"
#include "vmnt.h"
#include "vnode.h"
#include "path.h"
#include "param.h"

#include <repair.h>

int RC_CODE;

int do_inodemapwalker()
{
    struct vmnt *vmp;
    char mount_point[21] = {0};
    sys_datacopy(m_in.m_source,(vir_bytes)m_in.m1_p1,SELF,(vir_bytes)mount_point,20);
    // printf("the mount_point is :%s\n",mount_point);

    for(vmp = &vmnt[0];vmp < &vmnt[NR_MNTS];++vmp)
    {
      if(strncmp(mount_point,vmp->m_mount_path,21) == 0)
      {
        message m;
        m.m_type = REQ_WALKER;
        m.m9_l1 = vmp->m_dev;
        m.m9_l3 = 0;
        return fs_sendrec(vmp->m_fs_e,&m);
      }
    }
    printf("cannot find valid mount_point\n");
    return 1;
}

int do_zonemapwalker()
{
    struct vmnt *vmp;
    char mount_point[21] = {0};
    sys_datacopy(m_in.m_source,(vir_bytes)m_in.m1_p1,SELF,(vir_bytes)mount_point,20);
    // printf("the mount_point is :%s\n",mount_point);

    for(vmp = &vmnt[0];vmp < &vmnt[NR_MNTS];++vmp)
    {
      if(strncmp(mount_point,vmp->m_mount_path,21) == 0)
      {
        message m;
        m.m_type = REQ_WALKER;
        m.m9_l1 = vmp->m_dev;
        m.m9_l3 = 1;
        return fs_sendrec(vmp->m_fs_e,&m);
      }
    }
    printf("cannot find valid mount_point\n");
    return 1;
}

int do_directorywalker()
{
  message m;
  m.m_type = REQ_WALKER;
  m.m9_l1 = m_in.m9_l1;
  m.m9_l2 = m_in.m9_l2;
  m.m9_l3 = 2;
  struct vmnt* vmp;
  for(vmp = &vmnt[0];vmp < &vmnt[NR_MNTS];++vmp)
  {
    if(vmp->m_dev == m.m9_l1)
    return fs_sendrec(vmp->m_fs_e,&m);
  }
  return 1;
}

int do_bitmapdamager()
{
  /*
  m_in.m5_s1 = inodenumber
  m_in.m5_s2 = inodezoneflag
  m_in.m5_i1 = 0 or 1
  */
  struct vmnt* vmp;
  for(vmp = &vmnt[0];vmp < &vmnt[NR_MNTS];++vmp)
  {
    if(strcmp("/home",vmp->m_mount_path) != 0)
    {
      continue;
    }
    message m;
    m.m_type = REQ_BITMAPDAMAGER;
    m.m9_s1 = vmp->m_dev;
    m.m9_s2 = m_in.m5_s1;
    m.m9_s3 = m_in.m5_s2;
    m.m9_s4 = m_in.m5_i1;
    return fs_sendrec(vmp->m_fs_e,&m);
  }
  printf("cannot find /home\n");
  return 1;
}

int do_bitmapfixer()
{
  struct vmnt *vmp;
  char mount_point[21] = {0};
  sys_datacopy(m_in.m_source,(vir_bytes)m_in.m1_p1,SELF,(vir_bytes)mount_point,20);
  // printf("the mount_point is :%s\n",mount_point);

  for(vmp = &vmnt[0];vmp < &vmnt[NR_MNTS];++vmp)
  {
    if(strncmp(mount_point,vmp->m_mount_path,21) == 0)
    {
      message m;
      m.m_type = REQ_BITMAPFIXER;
      m.m9_l1 = vmp->m_dev;
      m.m9_l3 = m_in.m1_i1;
      return fs_sendrec(vmp->m_fs_e,&m);
    }
  }
  printf("cannot find valid mount_point\n");
  return 1;
}
