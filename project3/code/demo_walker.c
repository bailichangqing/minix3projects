#include <stdio.h>
#include <stdlib.h>
#include <repair.h>
#include <dirwalker.h>

int main(int argc,char** argv)
{
  /*
  arg 1: pathname
  arg 2:walkmode
  */
  if(argc < 3)
  {
    return 1;
  }
  if(atoi(argv[2]) == 0)
  {
    inodemapwalker(argv[1]);
  }
  else if(atoi(argv[2]) == 1)
  {
    zonemapwalker(argv[1]);
  }
  else
  {
    directorywalker(argv[1]);
  }
}
