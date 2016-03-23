#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<MessageQueueLib.h>
#include<string.h>


int main(int argc,char** argv)
{
  int family[3] = {0};
  family[0] = getpid();
  mq_attr_t attr;
  attr.mm = 128;
  attr.sb = 1;
  attr.rb = 1;
  message_t msg1,msg2;
  msg_init(&msg1,1,"AAAAA");
  msg_addreceiver(&msg1,family[0]);
  msg_init(&msg2,1,"BBBBB");
  msg_addreceiver(&msg2,family[0]);
  mqd_t mqd  = mq_open("haha",&attr);
  int pid = fork();
  if(pid == 0)      //this is child1
  {
    mq_open("haha",&attr);
    while(1)
    {
      mq_send(mqd,msg1);
    }
  }
  else
  {
    family[1] = pid;
    pid = fork();
    if(pid == 0)  //this is child2
    {
      mq_open("haha",&attr);
      while(1)
      {
        mq_send(mqd,msg2);
      }
    }
    else
    {
      family[2] = pid;
    }
  }
  void* buffer = malloc(MAX_MSG_LENGTH);
  bzero(buffer,MAX_MSG_LENGTH);
  while(1)    //this is father process
  {
    while(mq_receive(mqd,buffer) == 0)
    {
      printf("p:%d got msg:%s\n",family[0],buffer);
      bzero(buffer,MAX_MSG_LENGTH);
    }
  }
  // printf("mqd:%d\n",(int)mqd);
  // message_t msg;
  // msg_init(&msg,1,"AAAAA");
  // msg_addreceiver(&msg,getpid());
  // mq_send(mqd,msg);
  // void* buffer = malloc(MAX_MSG_LENGTH + 10);
  // bzero(buffer,MAX_MSG_LENGTH + 10);
  // mq_receive(mqd,buffer);
  // printf("content:%s\n",buffer);
  // if(mq_receive(mqd,buffer) == -1)
  // {
  //   printf("now nothing\n");
  // }
  mq_close(mqd);
  return 0;
}
