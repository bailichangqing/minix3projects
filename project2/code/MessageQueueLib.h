#ifndef _MESSAGEQUEUELIB_H_
#define _MESSAGEQUEUELIB_H_

#include <lib.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#define MAX_QUEUE_NAME_LENGTH 64  //max length of the name of a mq
#define MAX_MESSAGE_PER_MQ 16
#define MAX_RECEIVER  8
#define MAX_MSG_LENGTH (64 * sizeof(char))
#define RECEIVER_LENGTH (MAX_RECEIVER * sizeof(int))
#define MAX_QUEUE_RECEIVERS (MAX_MESSAGE_PER_MQ * MAX_RECEIVER)
#define MAX_QUEUE_RECEIVERS_LENGTH (MAX_QUEUE_RECEIVERS * sizeof(pns_t))

//message structure
typedef struct {
 int sender;			       //send's pid
 int receivers[MAX_RECEIVER];		     //recievers' pids
 int to_receive_count;   //number of times to be received when set to 0, this message slot can be reused
 char content[MAX_MSG_LENGTH - 1];
 int priority;          //high -> low   1->4
} message_t;


//message queue attribute structure
typedef struct {
 int mm;                //max number of messages in the queue. it should be no more than 1024
 int sb;                //0:send is blocking, 1:send is non-blocking
 int rb;		              //0:recieve is blocking, 1:recieve is non-blocking
} mq_attr_t;

//A receiver's pid and that singnal that it want to be used to notify it when msg comes
typedef struct {
  endpoint_t receiver;
  int pid;
  int sig;
} pns_t;
//message queue structure
typedef struct {
 char name[MAX_QUEUE_NAME_LENGTH];			                           //name of this message queue
 mq_attr_t attribute;	                      	 //attribute of this message queue
 pns_t notify_pids[MAX_QUEUE_RECEIVERS];		                          //pids of proesses that required notifycation
 int np_count;                                //length of notify_pids
 int message_count;	                          //the total number of messages
 int registcount;                             //how many processes have opened this queue
 message_t queue_p1[MAX_MESSAGE_PER_MQ];		   //messages that has priority level 1   this is the highest priority
 message_t queue_p2[MAX_MESSAGE_PER_MQ];		   //messages that has priority level 2
 message_t queue_p3[MAX_MESSAGE_PER_MQ];		   //messages that has priority level 3
 message_t queue_p4[MAX_MESSAGE_PER_MQ];     //messages that has priority level 4
} messagequeue_t;

//message queue discriptor
typedef int mqd_t;

//*********************************************************prototype***************************************************************************
//open a queue
mqd_t mq_open(const char *name, const mq_attr_t* attr)
{
 message m;
 m.m7_p1 = name;
 m.m7_i1 = (*attr).mm;
 m.m7_i2 = (*attr).sb;
 m.m7_i3 = (*attr).rb;
 return (_syscall(PM_PROC_NR, 35, &m));
}
int mq_close(mqd_t mqd)
{
  message m;
  m.m7_i1 = (int)mqd;
  return (_syscall(PM_PROC_NR, 44, &m));
}
int mq_send(mqd_t mqd,message_t* msg)
{
  message m;
  m.m7_i1 = (*msg).sender;
  m.m7_p1 = (char*)((*msg).receivers);
  // m.m7_i2 = msg.receiver_count;
  m.m7_i2 = (*msg).to_receive_count;
  m.m7_p2 = (*msg).content;
  m.m7_i4 = (*msg).priority;
  m.m7_i5 = (int)mqd;
  return (_syscall(PM_PROC_NR, 45, &m));
}
int mq_receive(mqd_t mqd,void* buffer)
{
  /*
  the buffer should be at least as big as MAX_MSG_LENGTH
  */
  message m;
  m.m1_i1 = (int)mqd;
  m.m1_i3 = getpid();
  m.m1_p1 = buffer;
  return (_syscall(PM_PROC_NR, 49, &m));
}
int mq_setattr(mqd_t mqd,const mq_attr_t* attr)
{
  /*
  the mm should be no more than MAX_MESSAGE_PER_MQ otherwise its useless
  */
  message m;
  m.m7_i1 = (*attr).mm;
  m.m7_i2 = (*attr).sb;
  m.m7_i3 = (*attr).rb;
  m.m7_i4 = (int) mqd;
  return (_syscall(PM_PROC_NR, 56, &m));
}
int mq_getattr(mqd_t mqd,mq_attr_t* attr)
{
  message m;
  int buffer[3];
  m.m7_i4 = (int) mqd;
  m.m7_p1 = buffer;
  int result = 0;
  result = _syscall(PM_PROC_NR, 57, &m);
  (*attr).mm = buffer[0];
  (*attr).sb = buffer[1];
  (*attr).rb = buffer[2];
  return result;
}
int mq_reqnotify(mqd_t mqd,int sig)
{/*
  To register for notifying, the user must provide a valid mqd and the signal number that it want to receive when msg arrives
  if the mqd/sig is invalid or the notifying queueu is full, returns -1
  */
  message m;
  m.m1_i1 = (int) mqd;
  m.m1_i2 = sig;
  m.m1_i3 = getpid();
  return (_syscall(PM_PROC_NR,58,&m));
}
void msg_init(message_t* msg,int priority,char* content)
{
  bzero(msg,sizeof(message_t));
  (*msg).priority = priority;
  (*msg).sender = getpid();
  strncpy((*msg).content,content,MAX_MSG_LENGTH);
}
void msg_addreceiver(message_t* msg,int pid)
{
  (*msg).receivers[(*msg).to_receive_count] = pid;
  (*msg).to_receive_count ++;
}
void attr_set(mq_attr_t* attr,int mm,int sb,int rb)
{
  (*attr).mm = mm;
  (*attr).sb = sb;
  (*attr).rb = rb;
}
#endif
