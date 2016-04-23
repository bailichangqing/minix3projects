#include "fs.h"
#include <assert.h>
#include <minix/vfsif.h>
#include <minix/bdev.h>
#include "inode.h"
#include "clean.h"
#include <stdlib.h>

/*===========================================================================*
 *				fs_sync					     *
 *===========================================================================*/
int fs_sync()
{
/* Perform the sync() system call.  Flush all the tables.
 * The order in which the various tables are flushed is critical.  The
 * blocks must be flushed last, since rw_inode() leaves its results in
 * the block cache.
 */
  struct inode *rip;

  assert(lmfs_nr_bufs() > 0);

  /* Write all the dirty inodes to the disk. */
  for(rip = &inode[0]; rip < &inode[NR_INODES]; rip++)
	  if(rip->i_count > 0 && IN_ISDIRTY(rip)) rw_inode(rip, WRITING);

  /* Write all the dirty blocks to the disk. */
  lmfs_flushall();

  return(OK);		/* sync() can't fail */
}


/*===========================================================================*
 *				fs_flush				     *
 *===========================================================================*/
int fs_flush()
{
/* Flush the blocks of a device from the cache after writing any dirty blocks
 * to disk.
 */
  dev_t dev = (dev_t) fs_m_in.REQ_DEV;
  if(dev == fs_dev) return(EBUSY);

  lmfs_flushall();
  lmfs_invalidate(dev);

  return(OK);
}


/*===========================================================================*
 *				fs_new_driver				     *
 *===========================================================================*/
int fs_new_driver(void)
{
/* Set a new driver endpoint for this device. */
  dev_t dev;
  cp_grant_id_t label_gid;
  size_t label_len;
  char label[sizeof(fs_dev_label)];
  int r;

  dev = (dev_t) fs_m_in.REQ_DEV;
  label_gid = (cp_grant_id_t) fs_m_in.REQ_GRANT;
  label_len = (size_t) fs_m_in.REQ_PATH_LEN;

  if (label_len > sizeof(label))
	return(EINVAL);

  r = sys_safecopyfrom(fs_m_in.m_source, label_gid, (vir_bytes) 0,
	(vir_bytes) label, label_len);

  if (r != OK) {
	printf("MFS: fs_new_driver safecopyfrom failed (%d)\n", r);
	return(EINVAL);
  }

  bdev_driver(dev, label);

  return(OK);
}
int fs_walker()
{
  if(fs_m_in.m9_l3 == 0)  //inodemapwalker
  {
    printf("enter mfs inodewalker!\n");
    struct super_block* sp = get_super(fs_m_in.m9_l1);
    //read inodebitmap blocks
    int i;
    for(i = 0;i < sp->s_imap_blocks;i++)
    {
      struct buf* bitmapblock = get_block(fs_m_in.m9_l1, 2+i, NORMAL);
      bitchunk_t* mapchunks = (bitchunk_t*)bitmapblock->data;
      int k = 0;
      if(i == 0)
      {
        k = 1;
      }
      for(;k < 32768;k++)
      {
        if((mapchunks[k / (sizeof(bitchunk_t) * 8)] & (bitchunk_t)1 << (k % (sizeof(bitchunk_t) * 8))) != 0)  //used inode
        {
          printf("inode:%d\n",i*32768 + k);

        }
      }
      put_block(bitmapblock,0);
    }
    // int i;
    // struct super_block* sp = get_super(fs_m_in.m9_l1);
    // for(i = 1;i <= sp->s_ninodes;i++)
    // {
    //   struct inode inodetmp;
    //   inodetmp.i_dev = sp->s_dev;
    //   inodetmp.i_num = i;
    //   rw_inode(&inodetmp,READING);
    //   if(inodetmp.i_nlinks != 0)
    //   {
    //     printf("inode%d used\t",inodetmp.i_num);
    //     int k;
    //     for(k = 0;k < V2_NR_TZONES;k++)
    //     {
    //       if(inodetmp.i_zone[k] != 0)
    //       {
    //         printf("%u\t",inodetmp.i_zone[k]);
    //       }
    //     }
    //     printf("\n");
    //   }
    // }
    return 0;
  }
  else if(fs_m_in.m9_l3 == 1)  //zonemapwalker
  {
    printf("fs_zonemapwalker\n");
    struct super_block* sp = get_super(fs_m_in.m9_l1);
    int i;
    for(i = 0;i < sp->s_zmap_blocks;i++)
    {
      struct buf* bitmapblock = get_block(fs_m_in.m9_l1, 2+ sp->s_imap_blocks + i, NORMAL);
      bitchunk_t* mapchunks = (bitchunk_t*)bitmapblock->data;
      int k = 1;
      if(i != 0)
      {
        k = 0;
      }
      for(;k < 32768;k++)
      {
        if((mapchunks[k / (sizeof(bitchunk_t) * 8)] & (bitchunk_t)1 << (k % (sizeof(bitchunk_t) * 8))) != 0)  //used zone
        {
          printf("zone:%d\n",i*32768 + k);
        }
      }
      put_block(bitmapblock,0);
    }
    return 0;
  }
  else  //dirwalker
  {
    struct inode* tmpinode= get_inode(fs_m_in.m9_l1,fs_m_in.m9_l2);
    printf("Zones of inode %ld are:",fs_m_in.m9_l2);
    int i;
    for(i = 0;i < V2_NR_TZONES;i++)
    {
      if(tmpinode->i_zone[i] != 0)
        printf("%u  ",tmpinode->i_zone[i]);
    }
    printf("\n");
    put_inode(tmpinode);
    printf("first data zone:%u\n",get_super(fs_m_in.m9_l1)->s_firstdatazone);
    return 0;
  }
}
int fs_bitmapdamager()
{
  /*
  m9_s1:dev
  m9_s2:inodenumber
  m9_s3:inode/zone flag
  */
  struct super_block* sp = get_super(fs_m_in.m9_s1);
  int inodezoneflag = fs_m_in.m9_s3 == 0? 0:sp->s_zmap_blocks;
  int inodenumber = fs_m_in.m9_s2;
  if(fs_m_in.m9_s3 == 1)
  {
    inodenumber -= (sp->s_firstdatazone - 1);
  }
  int blockoff = inodenumber / (sp->s_block_size * 8);
  struct buf* bitmapblock = get_block(fs_m_in.m9_s1,2 + inodezoneflag + blockoff,NORMAL);
  bitchunk_t* mapchunks = (bitchunk_t*)bitmapblock->data;
  int chunkindex = inodenumber / (sizeof(bitchunk_t) * 8);
  int bitindex = inodenumber % (sizeof(bitchunk_t) * 8);
  mapchunks[chunkindex] &= (~((bitchunk_t)1 << (bitindex)));
  printf("blockoff:%d chunkindex:%d bitindex:%d\n",blockoff,chunkindex,bitindex);
  put_block(bitmapblock,0);
  return 0;
}
int fs_bitmapfixer()
{
  /*
  m9_l1 dev
  m9_l3 fixertype
  */
  if(fs_m_in.m9_l3 == 0)  //fix inode bitmapblock
  {
    printf("enter mfs inode bitmap fixer!\n");
    // int errorcount = 0;
    struct super_block* sp = get_super(fs_m_in.m9_l1);
    //read inodebitmap blocks
    int i;
    for(i = 0;i < sp->s_imap_blocks;i++)
    {
      struct buf* bitmapblock = get_block(sp->s_dev, 2+i, NORMAL);
      bitchunk_t* mapchunks = (bitchunk_t*)bitmapblock->data;
      int k = 0;
      if(i == 0)
      {
        k = 1;
      }
      for(;k < 32768;k++)
      {
        if(i*32768 + k > sp->s_ninodes)
        {
          break;
        }
        if((mapchunks[k / (sizeof(bitchunk_t) * 8)] & (bitchunk_t)1 << (k % (sizeof(bitchunk_t) * 8))) != 0)  //used inode
        {
          struct inode inodetmp;
          inodetmp.i_dev = sp->s_dev;
          inodetmp.i_num = i*32768 + k;
          rw_inode(&inodetmp,READING);
          if(inodetmp.i_nlinks == 0)
          {
            mapchunks[k / (sizeof(bitchunk_t) * 8)] &= ~((bitchunk_t)1 << (k % (sizeof(bitchunk_t) * 8)));
            printf("Inode %d fixed!\n",i*32768 + k);
          }
        }
        else
        {
          struct inode inodetmp;
          inodetmp.i_dev = sp->s_dev;
          inodetmp.i_num = i*32768 + k;
          rw_inode(&inodetmp,READING);
          if(inodetmp.i_nlinks != 0)
          {
            mapchunks[k / (sizeof(bitchunk_t) * 8)] |= ((bitchunk_t)1 << (k % (sizeof(bitchunk_t) * 8)));
            printf("Inode %d fixed!\n",i*32768 + k);
          }
        }
        // struct inode inodetmp;
        // inodetmp.i_dev = sp->s_dev;
        // inodetmp.i_num = i*32768 + k;
        // rw_inode(&inodetmp,READING);
        //
        // if(
        //   (inodetmp.i_nlinks !=0 && (mapchunks[k / (sizeof(bitchunk_t) * 8)] & (bitchunk_t)1 << (k % (sizeof(bitchunk_t) * 8))) == 0) ||
        //   (inodetmp.i_nlinks ==0 && (mapchunks[k / (sizeof(bitchunk_t) * 8)] & (bitchunk_t)1 << (k % (sizeof(bitchunk_t) * 8))) != 0)
        // )
        // {
        //   errorcount ++;
        // }
      }
      put_block(bitmapblock,0);
    }
    printf("inode bitmap fixer return\n");
    // printf("error_count:%d\n",errorcount);
    return 0;
  }
  else if(fs_m_in.m9_l3 == 1) //fix zone bitmap
  {
    printf("enter mfs zone bitmap fixer!\n");
    struct super_block* sp = get_super(fs_m_in.m9_l1);
    //first erase all zone bit map
    int logicalzonebitmapblockindex;
    for(logicalzonebitmapblockindex = 0;logicalzonebitmapblockindex < sp->s_zmap_blocks;logicalzonebitmapblockindex++)
    {
      struct buf* tmpbitmapblock = get_block(sp->s_dev,logicalzonebitmapblockindex + 2 + sp->s_imap_blocks,NORMAL);
      bitchunk_t* tmpchunktable = (bitchunk_t*)tmpbitmapblock->data;
      int chunkindex;
      for(chunkindex = 0;chunkindex < (sp->s_block_size / sizeof(bitchunk_t));chunkindex++)
      {
        if(logicalzonebitmapblockindex == 0 && chunkindex == 0)
        {
          tmpchunktable[chunkindex] &= (bitchunk_t) 1;
        }
        else
        {
          tmpchunktable[chunkindex] &= (bitchunk_t) 0;
        }
      }
      put_block(tmpbitmapblock,0);
    }
    //check all inode and fix zonebitmap
    // int i;
    // for(i = 0;i < sp->s_imap_blocks;i++)
    // {
    //   struct buf* bitmapblock = get_block(sp->s_dev, 2+i, NORMAL);
    //   bitchunk_t* mapchunks = (bitchunk_t*)bitmapblock->data;
    //   int k = 0;
    //   if(i == 0)
    //   {
    //     k = 1;
    //   }
    //   for(;k < 32768;k++)
    //   {
    //     if(i*32768 + k > sp->s_ninodes)
    //     {
    //       break;
    //     }
    //     if((mapchunks[k / (sizeof(bitchunk_t) * 8)] & (bitchunk_t)1 << (k % (sizeof(bitchunk_t) * 8))) != 0)  //used inode
    //     {
    //       struct inode* inodetmp = NULL;
    //       inodetmp = get_inode(sp->s_dev,i * 32768 + k);
    //       int inodezoneindex;
    //       printf("fix inode%d zones\n",i*32768 + k);
    //       for(inodezoneindex = 0;inodezoneindex < V2_NR_TZONES;inodezoneindex++);
    //       {
    //         if(inodetmp->i_zone[inodezoneindex] != 0)
    //         {
    //           unsigned int bitsperblock = sp->s_block_size * 8;
    //           unsigned int bitsperchunk = sizeof(bitchunk_t) * 8;
    //           unsigned int logicalzoneindex = inodetmp->i_zone[inodezoneindex] - sp->s_firstdatazone;
    //           unsigned int physicalblockindex = (logicalzoneindex / bitsperblock) + 2 + sp->s_imap_blocks;
    //           unsigned int chunkindex = (logicalzoneindex % bitsperblock) / bitsperchunk;
    //           unsigned int bitindex = logicalzoneindex % bitsperchunk;
    //           struct buf* tmpzonebitmapblock = get_block(sp->s_dev,physicalblockindex,NORMAL);
    //           bitchunk_t* tmpchunktable = (bitchunk_t*)tmpzonebitmapblock->data;
    //           tmpchunktable[chunkindex] |= ((bitchunk_t)1 << bitindex);
    //           printf("%u  %u  %u  %u  %u  %u\n",bitsperblock,bitsperchunk,logicalzoneindex,physicalblockindex,chunkindex,bitindex);
    //           put_block(tmpzonebitmapblock,0);
    //         }
    //       }
    //       put_inode(inodetmp);
    //     }
    //   }
    //   put_block(bitmapblock,0);
    // }

    int i;
    for(i = 1;i <= sp->s_ninodes;i++)
    {
      struct inode inodetmp;
      inodetmp.i_dev = sp->s_dev;
      inodetmp.i_num = i;
      rw_inode(&inodetmp,READING);
      if(inodetmp.i_nlinks != 0)
      {
        printf("inode%d used\t",inodetmp.i_num);
        int k;
        for(k = 0;k < V2_NR_TZONES;k++)
        {
          if(inodetmp.i_zone[k] != 0)
          {
            printf("fix zone %u\t",inodetmp.i_zone[k]);
            unsigned int bitsperblock = sp->s_block_size * 8;
            unsigned int bitsperchunk = sizeof(bitchunk_t) * 8;
            unsigned int logicalzoneindex = inodetmp.i_zone[k] - sp->s_firstdatazone + 1;
            unsigned int physicalblockindex = (logicalzoneindex / bitsperblock) + 2 + sp->s_imap_blocks;
            unsigned int chunkindex = (logicalzoneindex % bitsperblock) / bitsperchunk;
            unsigned int bitindex = logicalzoneindex % bitsperchunk;
            struct buf* tmpzonebitmapblock = get_block(sp->s_dev,physicalblockindex,NORMAL);
            bitchunk_t* tmpchunktable = (bitchunk_t*)tmpzonebitmapblock->data;
            tmpchunktable[chunkindex] |= ((bitchunk_t)1 << bitindex);
            put_block(tmpzonebitmapblock,0);
          }
        }
        printf("\n");
      }
    }
    printf("zone bitmap fixer return\n");
    return 0;
  }
  else
  {
    return 0;
  }
}
