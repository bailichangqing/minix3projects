#include <stdio.h>
#include <stdlib.h>
#include <repair.h>
#include <dirwalker.h>

int main(int argc,char** argv)
{
  /*
  argv[1] inode numbers
  argv[2] 0 or 1
  argv[3] inode or zone
  */
  if(argc < 4)
  {
    return 1;
  }
  if(atoi(argv[3]) == 0)  //test inode bit map
  {
    inodemapdamager(atoi(argv[1]),atoi(argv[2]));
    inodemapwalker("/home");
    inodemapfixer("/home");
    inodemapwalker("/home");
  }
  else if(atoi(argv[3]) == 1) //test zone bit map
  {
    zonemapdamager(atoi(argv[1]),atoi(argv[2]));
    zonemapwalker("/home");
    zonemapfixer("/home");
    zonemapwalker("/home");
  }
  return 0;
}
