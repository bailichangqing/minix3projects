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

int do_inodewalker(){
    struct vmnt *vmp;
    char mount_point[21] = {0};
    sys_datacopy(m_in.m_source,(vir_bytes)m_in.m1_p1,SELF,(vir_bytes)mount_point,20);
    printf("the mount_point is :%s\n",mount_point);

    for(vmp = &vmnt[0];vmp < &vmnt[NR_MNTS];++vmp)
    {
      if(strncmp(mount_point,vmp->m_mount_path,21) == 0)
      {
        message m;
        m.m_type = REQ_INODEWALKER;
        m.REQ_DEV = vmp->m_dev;
        return fs_sendrec(vmp->m_fs_e,&m);
      }
    }
    printf("cannot find valid mount_point\n");
    return 1;
}

int do_zonemapwalker(){
    // printf("successfully called vfs zonemapwalker...\n");
    //
    // struct vmnt *vmp;
    //
    // char * dest=(char *)m_in.m1_i1;
    // endpoint_t w=m_in.m_source;
    // int node=m_in.m1_i2;
    // printf("dest, entering vfs: %d\n",(int)dest);
    //
    // for (vmp = &vmnt[0]; vmp < &vmnt[NR_MNTS]; ++vmp) {
    // 	if ( strcmp("/home", vmp->m_mount_path) == 0 ) {
		// message m;
 	// 	m.m_type = REQ_ZONEMAPWALKER;
		// m.REQ_DEV = vmp->m_dev;
    //
		// RC_CODE = fs_sendrec(vmp->m_fs_e, &m);
    //
		// int* src=(int*)m.RES_DEV;
    //
		// int size=m.RES_NBYTES;
    //
		// int * blocks=calloc(size,1);
		// if(sys_datacopy(m.m_source, (vir_bytes)src, SELF, (vir_bytes)blocks, size)==OK)printf("Copy1 ok\n");
		// printf("test copy1: %ld %d  %d  %d\n",m.RES_DEV,blocks[0],blocks[1],blocks[2]);
    //
		// if(sys_datacopy(SELF, (vir_bytes)blocks, w , (vir_bytes)dest, size)==OK)printf("copy2 OK\n");
    //
    //
		// if (blocks[0] != 0){
		// 	printf("something is broken\n");
    //         m.m_type = REQ_INODEFIXER;
    //         m.REQ_INODE_NR=node;
		// 	m.REQ_DEV = vmp->m_dev;
		// 	RC_CODE = fs_sendrec(vmp->m_fs_e, &m);
		// }
    //
   // 	 }
    // }

    //*************************************************************************************************
    struct vmnt *vmp;
    char mount_point[21] = {0};
    sys_datacopy(m_in.m_source,(vir_bytes)m_in.m1_p1,SELF,(vir_bytes)mount_point,20);
    printf("the mount_point is :%s\n",mount_point);

    for(vmp = &vmnt[0];vmp < &vmnt[NR_MNTS];++vmp)
    {
      if(strncmp(mount_point,vmp->m_mount_path,21) == 0)
      {
        message m;
        m.m_type = REQ_ZONEMAPWALKER;
        m.REQ_DEV = vmp->m_dev;
        return fs_sendrec(vmp->m_fs_e,&m);
      }
    }
    printf("cannot find valid mount_point\n");
    return 1;
}

int do_directorywalker()
{
  // char namebuffer[129] = {0};
  // int namelength = 128 > m_in.m1_i1? m_in.m1_i1:128;
  // sys_datacopy(m_in.m_source,(vir_bytes)m_in.m1_p1,SELF,(vir_bytes)namebuffer,namelength);
  //
  // int rootdev = 0;
  // //find out mount_path
  // struct vmnt* vmp;
  // for(vmp = &vmnt[0];vmp < &vmnt[NR_MNTS];++vmp)
  // {
  //   if(strcmp("/",vmp->m_mount_path) == 0)
  //   {
  //     continue;
  //     rootdev = vmp->m_dev;
  //   }
  //   int prefixlength = strlen(vmp->m_mount_path);
  //   if (prefixlength > m_in.m1_i1)
  //   {
  //     continue;
  //   }
  //   if(strncmp(vmp->m_mount_path,namebuffer,prefixlength) == 0)
  //   {
  //     message m;
  //     m.m_type = REQ_DIRECTORYWALKER;
  //     m.m6_l1 = vmp->m_dev;
  //     m.m6_p1 = namebuffer + prefixlength;
  //     m.m6_s1 = strlen(namebuffer) - prefixlength;
  //     return fs_sendrec(vmp->m_fs_e,&m);
  //   }
  // }
  // if(namebuffer[0] == '/')
  // {
  //   message m;
  //   m.m_type = REQ_DIRECTORYWALKER;
  //   m.m6_l1 = rootdev;
  //   m.m6_p1 = namebuffer + 1;
  //   m.m6_s1 = strlen(namebuffer) - 1;
  //   return fs_sendrec(vmp->m_fs_e,&m);
  // }
  // printf("invalid pathname\n");
  // return 1;
  message m;
  m.m_type = REQ_DIRECTORYWALKER;
  m.m9_l1 = m_in.m9_l1;
  m.m9_l2 = m_in.m9_l2;
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
  //m_in.m5_s1 = inodenumber  m_in.m5_s2 = inodezoneflag
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
    return fs_sendrec(vmp->m_fs_e,&m);
  }
  printf("cannot find /home\n");
  return 1;
}

int do_inodebitmapfixer()
{
  struct vmnt *vmp;
  char mount_point[21] = {0};
  sys_datacopy(m_in.m_source,(vir_bytes)m_in.m1_p1,SELF,(vir_bytes)mount_point,20);
  printf("the mount_point is :%s\n",mount_point);

  for(vmp = &vmnt[0];vmp < &vmnt[NR_MNTS];++vmp)
  {
    if(strncmp(mount_point,vmp->m_mount_path,21) == 0)
    {
      message m;
      m.m_type = REQ_INODEBITMAPFIXER;
      m.REQ_DEV = vmp->m_dev;
      return fs_sendrec(vmp->m_fs_e,&m);
    }
  }
  printf("cannot find valid mount_point\n");
  return 1;
}
