#!/bin/sh

cp -pf Makefile_pm /usr/src/servers/pm/Makefile
cp -pf callnr.h /usr/src/include/minix/callnr.h
cp -pf proto.h /usr/src/servers/pm/proto.h
cp -pf table.c /usr/src/servers/pm/table.c
cp -pf MessageQueueLib.h /usr/src/include/MessageQueueLib.h
cp -pf MessageQueue.h /usr/src/servers/pm/MessageQueue.h
cp -pf MessageQueue.c /usr/src/servers/pm/MessageQueue.c
cp -pf Makefile_include /usr/src/include/Makefile

cd /usr/src/releasetools

make hdboot && sync && reboot
