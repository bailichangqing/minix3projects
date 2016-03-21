#include "pm.h"
#include "mproc.h"
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>



#define MAX_MQs 128                //max number of messagequeues supported by default
#define MAX_QUEUE_NAME_LENGTH 128  //max length of the name of a mq

mqd_t do_mq_open(void);
int do_mq_close(void);
int do_mq_send(void);
int do_mq_receive(void);
