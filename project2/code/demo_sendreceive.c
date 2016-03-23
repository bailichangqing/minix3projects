#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<MessageQueueLib.h>
#include<string.h>


int main(int argc,char** argv)
{
  int sendtimes = 16;
  if(argc >= 2)
  {
    sendtimes = atoi(argv[1]);
  }
  mq_attr_t attr;
  attr_set(&attr,8,1,1);
  mqd_t mqd = mq_open("lisa",&attr);
  message_t msg1,msg2;
  msg_init(&msg1,1,"AAAAA");
  msg_init(&msg2,1,"BBBBB");
  msg_addreceiver(&msg1,getpid());
  msg_addreceiver(&msg2,getpid());

  if(fork() == 0)
  {
    //this is child1
    int sendcount = 0,i;
    for(i = 0;i < sendtimes;i++)
    {
      if(mq_send(mqd,&msg1) == 0)
      {
        sendcount ++;
      }
    }
    printf("child1 exits,sendcount is %d\n",sendcount);
    return 0;
  }
  else
  {
    //this is father
    if(fork() == 0)
    {
      //this is child2
      int sendcount = 0,i;
      for(i = 0;i < sendtimes;i++)
      {
        if(mq_send(mqd,&msg2) == 0)
        {
          sendcount ++;
        }
      }
      printf("child2 exits,sendcount is %d\n",sendcount);
      return 0;
    }
    else
    {
      char buffer[MAX_MSG_LENGTH];
      int mcount = 0;
      while(1)
      {
        bzero(buffer,MAX_MSG_LENGTH);
        if(mq_receive(mqd,buffer) == 0)
        {
          printf("%d father got msg:%s\n",mcount++,buffer);
        }
      }
    }
  }
  return 0;
}
