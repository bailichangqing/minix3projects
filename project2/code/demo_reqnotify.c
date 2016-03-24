#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<MessageQueueLib.h>
#include<string.h>
#include<signal.h>

mqd_t mqd;
int count = 0;
void myhandler(int sig)
{
  char msg[MAX_MSG_LENGTH];
  if(sig == SIGALRM)
  {
    printf("%d  new msg coming!\n",count);
    count++;
    while(mq_receive(mqd,&msg) == 0)
    {
      printf("msg:%s\n",msg);
      bzero(msg,MAX_MSG_LENGTH);
    }
  }
}

int main(int argc,char** argv)
{
    printf("********************************************************\n");
    printf("this demo shows how mq_reqnotify works\n");
    printf("the father process will create a new message queue\n");
    printf("and then register for message notifying.after that\n");
    printf("it will fork two children preocesses to send msgs to \n");
    printf("itself.If the mq_reqnotify works,the father process should\n");
    printf("print out all the msgs it receives\n");
    printf("********************************************************\n");
    mq_attr_t attr;
    attr_set(&attr,8,1,1);
    mqd = mq_open("hakaze",&attr);
    message_t msg1,msg2;
    msg_init(&msg1,1,"greeting from child1!");
    msg_init(&msg2,1,"greeting from child2!");
    msg_addreceiver(&msg1,getpid());
    msg_addreceiver(&msg2,getpid());

    if(signal(SIGALRM,myhandler) != 0)
    {
      printf("cannot catch sig: %d\n",SIGALRM);
      return 0;
    }
    if(mq_reqnotify(mqd,SIGALRM) != 0)
    {
      printf("cannot register for notify with sig: %d\n",SIGALRM);
      return 0;
    }

    if(fork() == 0)
    {
      //this is child1
      int i;
      int sendcount = 0;
      for(i = 0;i < 5;i++)   //send msg 16 times
      {
        if(mq_send(mqd,&msg1) == 0)
        {
          sendcount++;
        }
      }
      printf("child1 exit, sendcount:%d\n",sendcount);
      return 0;
    }
    else
    {
      //this is father
      if(fork() == 0)
      {
        //this is child2
        int i;
        int sendcount = 0;
        for(i = 0;i < 5;i++)   //send msg 16 times
        {
          if(mq_send(mqd,&msg2) == 0)
          {
            sendcount++;
          }
        }
        printf("child2 exit, sendcount:%d\n",sendcount);
        return 0;
      }
    }
    while(1)
    {
      ;
    }
    mq_close(mqd);
    return 0;
}
