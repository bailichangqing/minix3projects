#include "pm.h"
#include "mproc.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>



#define MAX_MQs 8                //max number of messagequeues supported by default
