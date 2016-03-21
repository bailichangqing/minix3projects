#ifndef _MESSAGEQUEUELIB_H_
#define _MESSAGEQUEUELIB_H_

#define MAX_MESSAGE_PER_MQ 1024
#include <lib.h>
#include <unistd.h>

//message structure
typedef struct {
 int sender;			       //send's pid
 int *receivers;		     //recievers' pids
 int receiver_count;     //number of receivers
 int to_receive_count;   //number of times to be received when set to 0, this message slot can be reused
 char *content;
 int contentlength;
} message_t;


//message queue attribute structure
typedef struct {
 int mm;                //max number of messages in the queue. it should be no more than 1024
 int sb;                //0:send is blocking, 1:send is non-blocking
 int rb;		              //0:recieve is blocking, 1:recieve is non-blocking
} mq_attr_t;

//message queue structure
typedef struct {
 char name[128];			                           //name of this message queue
 mq_attr_t attribute;	                      	 //attribute of this message queue
 int *notify_pids;		                          //pids of proesses that required notifycation
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
int mq_send(mqd_t mqd,message_t msg)
{
  message m;
  m.m7_i1 = msg.sender;
  m.m7_p1 = msg.receivers;
  m.m7_i2 = msg.receiver_count;
  m.m7_i3 = msg.to_receive_count;
  m.m7_p2 = msg.content;
  m.m7_i4 = msg.contentlength;
  return (_syscall(PM_PROC_NR, 45, &m));
}
int mq_receive(mqd_t mqd,void* buffer,size_t bufferlength)
{
  message m;
  m.m1_i1 = (int)mqd;
  m.m1_i2 = (int)bufferlength;
  m.m1_p1 = buffer;
  return (_syscall(PM_PROC_NR, 49, &m));
}
#endif
