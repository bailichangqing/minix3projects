#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<MessageQueueLib.h>
#include<string.h>

int main(int argc,char** argv)
{
  printf("***************************************************\n");
  printf("this demo shows how mq_setattr&mq_getattr work\n");
  printf("the program will first create a new messagequeue\n");
  printf("and initiate attribute of it to 128,1,1\n");
  printf("then reset the attribute of it to 0,0,0\n");
  printf("finally it will using mq_getattr to check the attribute\n");
  printf("***************************************************\n");
  mq_attr_t attr;
  attr_set(&attr,128,1,1);
  mqd_t mqd  = mq_open("haha",&attr);
  printf("initial attr       :mm :%d sb:%d rb:%d\n",attr.mm,attr.sb,attr.rb);
  attr_set(&attr,0,0,0);
  mq_setattr(mqd,&attr);
  attr_set(&attr,1,1,1);
  mq_getattr(mqd,&attr);
  printf("after reseting attr:mm :%d sb:%d rb:%d\n",attr.mm,attr.sb,attr.rb);
  close(mqd);
  return 0;
}
