#include "MessageQueueLib.h"
#include "MessageQueue.h"


static messagequeue_t mqs[MAX_MQs];
static int mqs_map[MAX_MQs] = {0};     //indicate whether a slot is unsigned

void debug_open(void)         //used for debug
{
  int i;
  for(i = 0;i < MAX_MQs;i++)
  {
    if(mqs_map[i] == 1)
    {
      printf("mq%d: name %s,registcount %d\n",i,mqs[i].name,mqs[i].registcount);
    }
  }
}

void debug_send(void)
{
  int qi;
  for(qi = 0;qi < MAX_MQs;qi++)
  {
    if(mqs[qi].registcount > 0){
      message_t* qa[] = {mqs[qi].queue_p1,mqs[qi].queue_p2,mqs[qi].queue_p3,mqs[qi].queue_p4};
      int i4;
      for(i4 = 0;i4 < 4;i4++){
        int mi;
        for(mi = 0;mi < MAX_MESSAGE_PER_MQ;mi++){
          if(qa[i4][mi].to_receive_count > 0){
            printf("message in slot:%d priority:%d msg-slot:%d receiver pid:%d content:%s\n",qi,qa[i4][mi].priority,mi,qa[i4][mi].receivers[0],qa[i4][mi].content);
          }
        }
      }
    }
  }
}
mqd_t do_mq_open(void)
{
  /*
  try to create a new message queue.if no slot avaliable, returns -1
  otherwise return a mqd
  */
 size_t namelength = sizeof(char) * MAX_QUEUE_NAME_LENGTH;
 char *name = (char *)malloc(namelength);
 sys_datacopy(who_e, (vir_bytes)m_in.m7_p1, SELF, (vir_bytes)name, namelength);
 //search by name. check if it exist
 int i = 0;
 for(i = 0;i < MAX_MQs;i++)
 {
  if(mqs_map[i] == 1)
  {
   if(strncmp(mqs[i].name,name,MAX_QUEUE_NAME_LENGTH) == 0)   //name matches
   {
    mqs[i].registcount++;
    return i;
   }
  }
 }
 //need to create the mq
 for(i = 0;i < MAX_MQs;i++)  //find a usable slot
 {
  if(mqs_map[i] == 0)
  {
    mqs_map[i] = 1;  //mark it as used
    //initialize this slot
    bzero(&mqs[i],sizeof(messagequeue_t));
    //set name
    strncpy(mqs[i].name,name,namelength);
    //set attr
    mqs[i].attribute.mm = m_in.m7_i1;
    if(mqs[i].attribute.mm > MAX_MESSAGE_PER_MQ)
    {
      mqs[i].attribute.mm = MAX_MESSAGE_PER_MQ;
    }
    mqs[i].attribute.sb = m_in.m7_i2;
    mqs[i].attribute.rb = m_in.m7_i3;
    //add registcount
    mqs[i].registcount = 1;
    return i;
  }
 }
 //cannot find avaliable slot
 return -1;
}

int do_mq_close(void)
{
  pid_t pid = mproc[who_p].mp_pid;      //get pid of the caller
  if(pid < 1)
  {
    printf("Wrong pid\n");
    return -1;
  }
  int i = 0;
  for(i = 0;i < mqs[m_in.m7_i1].np_count; i++)     //delete from notify list
  {
    if(mqs[m_in.m7_i1].notify_pids[i].pid == pid)
    {
      mqs[m_in.m7_i1].notify_pids[i].pid = 0;
    }
  }
  mqs[m_in.m7_i1].registcount--;
  if(mqs[m_in.m7_i1].registcount == 0)      //free this slot
  {
    mqs_map[m_in.m7_i1] = 0;                //reset map value
    bzero(mqs[m_in.m7_i1].notify_pids,MAX_QUEUE_RECEIVERS_LENGTH);
    for(i = 0;i < MAX_MESSAGE_PER_MQ;i++)
    {
      if(mqs[m_in.m7_i1].queue_p1[i].to_receive_count != 0)    //free all unread messages
      {
        bzero(mqs[m_in.m7_i1].queue_p1[i].receivers,RECEIVER_LENGTH);
        bzero(mqs[m_in.m7_i1].queue_p1[i].content,MAX_MSG_LENGTH);
      }
    }
    for(i = 0;i < MAX_MESSAGE_PER_MQ;i++)
    {
      if(mqs[m_in.m7_i1].queue_p2[i].to_receive_count != 0)    //free all unread messages
      {
        bzero(mqs[m_in.m7_i1].queue_p2[i].receivers,RECEIVER_LENGTH);
        bzero(mqs[m_in.m7_i1].queue_p2[i].content,MAX_MSG_LENGTH);
      }
    }
    for(i = 0;i < MAX_MESSAGE_PER_MQ;i++)
    {
      if(mqs[m_in.m7_i1].queue_p3[i].to_receive_count != 0)    //free all unread messages
      {
        bzero(mqs[m_in.m7_i1].queue_p3[i].receivers,RECEIVER_LENGTH);
        bzero(mqs[m_in.m7_i1].queue_p3[i].content,MAX_MSG_LENGTH);
      }
    }
    for(i = 0;i < MAX_MESSAGE_PER_MQ;i++)
    {
      if(mqs[m_in.m7_i1].queue_p4[i].to_receive_count != 0)    //free all unread messages
      {
        bzero(mqs[m_in.m7_i1].queue_p4[i].receivers,RECEIVER_LENGTH);
        bzero(mqs[m_in.m7_i1].queue_p4[i].content,MAX_MSG_LENGTH);
      }
    }
  }
  return 0;
}

void notify_helper(int mqd,message_t* msg)
{
  int mi,qi;
  for(mi = 0;(*msg).receivers[mi] > 0 && mi < MAX_RECEIVER;mi ++)
  {
    for(qi = 0;mqs[mqd].notify_pids[qi].pid > 0 && qi < MAX_QUEUE_RECEIVERS;qi ++)
    {
      if((*msg).receivers[mi] == mqs[mqd].notify_pids[qi].pid)
      {
        if(sys_kill(mqs[mqd].notify_pids[qi].receiver,mqs[mqd].notify_pids[qi].sig) < 0) //fail to send signal
        {
          printf("fails to send sig:%d to receiver:%d\n",mqs[mqd].notify_pids[qi].sig,mqs[mqd].notify_pids[qi].pid);
        }
        printf("I've sent it\n");
      }
    }
  }
}

int do_mq_send(void)
{
  /*
  first try to find the right slot.then try puting message into the correponding queue.if this is a sendblocking queue and,using while if cannot insert into
  return -1 if mdq is invalid or cannot insert now
  */
  //check mqd validation
  int mqd = m_in.m7_i5;
  int sender = m_in.m7_i1;
  int to_receive_count = m_in.m7_i2;
  int priority = m_in.m7_i4;

  if(mqs[mqd].registcount <= 0)  //wrong mqd
  {
    return -1;
  }
  message_t* pq = 0;
  switch (priority) {
    case 1:
      pq = mqs[mqd].queue_p1;
      break;
    case 2:
      pq = mqs[mqd].queue_p2;
      break;
    case 3:
      pq = mqs[mqd].queue_p3;
      break;
    default:
      pq = mqs[mqd].queue_p4;
  }
  if(mqs[mqd].attribute.sb == 0)//send blocking queue
  {
    while(1)
    {
      int i;
      for(i = 0;i < MAX_MESSAGE_PER_MQ && mqs[mqd].message_count < mqs[mqd].attribute.mm;i++)
      {
        if(pq[i].to_receive_count == 0 && mqs[mqd].message_count < mqs[mqd].attribute.mm) { //this slot is useable
          pq[i].sender = sender;      //set msg sender
          bzero(pq[i].receivers,RECEIVER_LENGTH); //reset receiverlist
          sys_datacopy(who_e, (vir_bytes)m_in.m7_p1, SELF, (vir_bytes)pq[i].receivers, RECEIVER_LENGTH);
          pq[i].to_receive_count = to_receive_count;    //set to_receive_count
          pq[i].priority = priority;
          bzero(pq[i].content,MAX_MSG_LENGTH);
          sys_datacopy(who_e, (vir_bytes)m_in.m7_p2, SELF, (vir_bytes)pq[i].content, MAX_MSG_LENGTH);   //copy message content
          mqs[mqd].message_count ++;
          notify_helper(mqd,&pq[i]);    //notify receivers
          //debug_send();
          return 0;
        }
      }
    }
  }
  else{ //send non-blocking queue
    int i;
    for(i = 0;i < MAX_MESSAGE_PER_MQ && mqs[mqd].message_count < mqs[mqd].attribute.mm;i++)
    {
      if(pq[i].to_receive_count == 0 && mqs[mqd].message_count < mqs[mqd].attribute.mm) { //this slot is useable
        pq[i].sender = sender;      //set msg sender
        bzero(pq[i].receivers,RECEIVER_LENGTH); //reset receiverlist
        sys_datacopy(who_e, (vir_bytes)m_in.m7_p1, SELF, (vir_bytes)pq[i].receivers, RECEIVER_LENGTH);
        pq[i].to_receive_count = to_receive_count;    //set to_receive_count
        pq[i].priority = priority;
        bzero(pq[i].content,MAX_MSG_LENGTH);          //reset content
        sys_datacopy(who_e, (vir_bytes)m_in.m7_p2, SELF, (vir_bytes)pq[i].content, MAX_MSG_LENGTH);   //copy message content
        mqs[mqd].message_count ++;
        notify_helper(mqd,&pq[i]);    //notify receivers
        return 0;
      }
    }
  }
  return -1;    //fail to do non-blocking send
}
int do_mq_receive(void)
{
  int mqd = m_in.m1_i1;
  int rpid = m_in.m1_i3;  //receiver's pid
  if(mqs[mqd].registcount <= 0)  //wrong mqd
  {
    return -1;
  }
  message_t* queuearray[] = {mqs[mqd].queue_p1,mqs[mqd].queue_p2,mqs[mqd].queue_p3,mqs[mqd].queue_p4};
  int qi;
  if(mqs[mqd].attribute.rb == 0)      //blocking receiver
  {
    while(1)
    {
      if(mqs[mqd].message_count > 0)    //there are msgs
      {
        for(qi = 0;qi < 4;qi++)
        {
          int mi;
          for(mi = 0;mi < MAX_MESSAGE_PER_MQ;mi++)
          {
            if(queuearray[qi][mi].to_receive_count > 0)  //am I a receiver?
            {
              int ri;
              for(ri = 0;ri < MAX_RECEIVER;ri++)
              {
                // printf("qi:%d mi:%d ri:%d rpid:%d\n",qi,mi,ri,rpid);
                if(queuearray[qi][mi].receivers[ri] == rpid)  //yes!
                {
                  //copy data
                  sys_datacopy(SELF, (vir_bytes)queuearray[qi][mi].content,who_e, (vir_bytes)m_in.m1_p1,MAX_MSG_LENGTH);
                  queuearray[qi][mi].receivers[ri] = -1;
                  queuearray[qi][mi].to_receive_count --;
                  if(queuearray[qi][mi].to_receive_count == 0)
                  {
                    mqs[mqd].message_count --;
                  }
                  return 0;
                }
              }
            }
          }
        }
      }
    }
  }
  else      //non-blocking receiver
  {
    if(mqs[mqd].message_count <= 0)   //no msgs
    {
      return -1;
    }
    for(qi = 0;qi < 4;qi++)
    {
      int mi;
      for(mi = 0;mi < MAX_MESSAGE_PER_MQ;mi++)
      {
        if(queuearray[qi][mi].to_receive_count > 0)  //am I a receiver?
        {
          int ri;
          for(ri = 0;ri < MAX_RECEIVER;ri++)
          {
            if(queuearray[qi][mi].receivers[ri] == rpid)  //yes!
            {
              //copy data
              sys_datacopy(SELF, (vir_bytes)queuearray[qi][mi].content,who_e, (vir_bytes)m_in.m1_p1,MAX_MSG_LENGTH);
              queuearray[qi][mi].receivers[ri] = -1;
              queuearray[qi][mi].to_receive_count --;
              if(queuearray[qi][mi].to_receive_count == 0)
              {
                mqs[mqd].message_count --;
              }
              return 0;
            }
          }
        }
      }
    }
  }
  return -1;    //no msg to receive
}

int do_mq_setattr(void)
{
  int mqd = m_in.m7_i4;
  if (mqs[mqd].registcount <= 0)  //invalid mqd
  {
    return -1;
  }
  else
  {
    mqs[mqd].attribute.mm = m_in.m7_i1;
    mqs[mqd].attribute.sb = m_in.m7_i2;
    mqs[mqd].attribute.rb = m_in.m7_i3;
    return 0;
  }
}

int do_mq_getattr(void)
{
  int mqd = m_in.m7_i4;
  if (mqs[mqd].registcount <= 0)  //invalid mqd
  {
    return -1;
  }
  int attr[3] = {0};
  attr[0] = mqs[mqd].attribute.mm;
  attr[1] = mqs[mqd].attribute.sb;
  attr[2] = mqs[mqd].attribute.rb;
  sys_datacopy(SELF, (vir_bytes)attr,who_e, (vir_bytes)m_in.m7_p1,sizeof(attr));
  printf("done\n");
  return 0;
}

int do_mq_reqnotify(void)
{
  int mqd = m_in.m1_i1;
  int sig = m_in.m1_i2;
  int pid = m_in.m1_i3;
  //check if mqd is valid
  if(mqs[mqd].registcount < 1)
  {
    printf("fail to register for notify because of registcount:%d\n",mqs[mqd].registcount );
    return -1;
  }
  if(sig < 1 || sig > 32 || sig == SIGKILL)
  {
    //invalid signal number
    printf("fail to register for notify because of invalid sig%d\n",sig);
    return -1;
  }
  //try find a useable slot
  int i;
  for(i = 0;i < MAX_QUEUE_RECEIVERS;i++)
  {
    if(mqs[mqd].notify_pids[i].pid == 0) //useable
    {
      mqs[mqd].notify_pids[i].pid = pid;
      mqs[mqd].notify_pids[i].sig = sig;
      mqs[mqd].notify_pids[i].receiver = who_e;
      return 0;
    }
  }
  return -1;  //when reaches here, no useable slot for registering
}
