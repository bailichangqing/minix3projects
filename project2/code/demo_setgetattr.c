#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<MessageQueueLib.h>
#include<string.h>

int main(int argc,char** argv)
{

  mq_attr_t attr;
  attr_set(&attr,128,1,1);
  mqd_t mqd  = mq_open("haha",&attr);
  attr_set(&attr,0,0,0);
  mq_setattr(mqd,&attr);
  attr_set(&attr,1,1,1);
  mq_getattr(mqd,&attr);
  printf("mm :%d sb:%d rb:%d\n",attr.mm,attr.sb,attr.rb);
  close(mqd);
  return 0;
}
