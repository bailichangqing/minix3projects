#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<MessageQueueLib.h>
#include<string.h>

int main(int argc,char** argv)
{

  mq_attr_t attr;
  attr.mm = 128;
  attr.sb = 1;
  attr.rb = 1;
  mqd_t mqd  = mq_open("haha",&attr);
  attr.sb = 0;
  attr.rb = 0;
  mq_setattr(mqd,&attr);
  attr.sb = 1;
  attr.rb = 1;
  mq_getattr(mqd,&attr);
  printf("sb:%d rb:%d\n",attr.sb,attr.rb);
  close(mqd);
}
