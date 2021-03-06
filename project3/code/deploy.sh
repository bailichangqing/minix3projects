#!/bin/sh
cp -pf includes/repair.h /usr/include/repair.h
cp -pf includes/dirwalker.h /usr/include/
cp -pf includes/vfsif.h /usr/src/include/minix/vfsif.h
cp -pf mfs/misc.c /usr/src/servers/mfs/misc.c
cp -pf mfs/proto.h /usr/src/servers/mfs/proto.h
cp -pf mfs/table.c /usr/src/servers/mfs/table.c
cp -pf vfs/repair.c /usr/src/servers/vfs/repair.c
cp -pf vfs/proto.h /usr/src/servers/vfs/proto.h
cp -pf vfs/table.c /usr/src/servers/vfs/table.c
cp -pf vfs/Makefile /usr/src/servers/vfs/Makefile

cd /usr/src/releasetools
make hdboot && sync && reboot
