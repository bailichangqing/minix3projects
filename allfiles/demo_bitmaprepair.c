#include <stdio.h>
#include <stdlib.h>
#include <repair.h>
#include <dirwalker.h>

int main(int argc,char** argv){
  if(argc < 3)
  {
    return 1;
  }
  if(atoi(argv[2]) == 0)  //test inode bit map
  {
    inodemapdamager(atoi(argv[1]));
    inodemapwalker("/home");
    inodemapfixer("/home");
    inodemapwalker("/home");
  }
  else if(atoi(argv[2]) == 1) //test zone bit map
  {
    zonemapdamager(atoi(argv[1]));
    zonemapwalker("/home");
    zonemapfixer("/home");
    zonemapwalker("/home");
  }
  return 0;
}
