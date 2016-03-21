#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<MessageQueueLib.h>


int main(int argc,char** argv)
{
  mq_attr_t attr;
  attr.mm = 128;
  attr.sb = 0;
  attr.rb = 0;
  mqd_t mqd  = mq_open("haha",&attr);
  printf("mqd:%d\n",(int)mqd);
  return 0;
}
