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

mqd_t do_mq_open(void)
{
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
    debug_open();     //TO delete
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
    if(mqs[m_in.m7_i1].notify_pids[i] == pid)
    {
      mqs[m_in.m7_i1].notify_pids[i] = -1;
    }
  }
  mqs[m_in.m7_i1].registcount--;
  if(mqs[m_in.m7_i1].registcount == 0)      //free this slot
  {
    mqs_map[m_in.m7_i1] = 0;                //reset map value
    free(mqs[m_in.m7_i1].notify_pids);
    for(i = 0;i < MAX_MESSAGE_PER_MQ;i++)
    {
      if(mqs[m_in.m7_i1].queue_p1[i].to_receive_count != 0)    //free all unread messages
      {
        free(mqs[m_in.m7_i1].queue_p1[i].receivers);
        free(mqs[m_in.m7_i1].queue_p1[i].content);
      }
    }
    for(i = 0;i < MAX_MESSAGE_PER_MQ;i++)
    {
      if(mqs[m_in.m7_i1].queue_p2[i].to_receive_count != 0)    //free all unread messages
      {
        free(mqs[m_in.m7_i1].queue_p2[i].receivers);
        free(mqs[m_in.m7_i1].queue_p2[i].content);
      }
    }
    for(i = 0;i < MAX_MESSAGE_PER_MQ;i++)
    {
      if(mqs[m_in.m7_i1].queue_p3[i].to_receive_count != 0)    //free all unread messages
      {
        free(mqs[m_in.m7_i1].queue_p3[i].receivers);
        free(mqs[m_in.m7_i1].queue_p3[i].content);
      }
    }
    for(i = 0;i < MAX_MESSAGE_PER_MQ;i++)
    {
      if(mqs[m_in.m7_i1].queue_p4[i].to_receive_count != 0)    //free all unread messages
      {
        free(mqs[m_in.m7_i1].queue_p4[i].receivers);
        free(mqs[m_in.m7_i1].queue_p4[i].content);
      }
    }
  }
  return 0;
}

int do_mq_send(void)
{
  return 0;
}
int do_mq_receive(void)
{
  return 0;
}
