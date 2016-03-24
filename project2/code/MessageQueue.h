#include "pm.h"
#include "mproc.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>





#define MAX_MQs 8                //max number of messagequeues supported by default
