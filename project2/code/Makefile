CC=clang
CFLAGS=-Wall
main:demo_reqnotify.o	demo_sendreceive.o demo_setgetattr.o

demo_reqnotify.o:demo_reqnotify.c
	$(CC)	$(CFLAGS) demo_reqnotify.c -o demo_reqnotify.o
demo_sendreceive.o:demo_sendreceive.c
	$(CC)	$(CFLAGS) demo_sendreceive.c -o demo_sendreceive.o
demo_setgetattr.o:demo_setgetattr.c
	$(CC)	$(CFLAGS) demo_setgetattr.c -o demo_setgetattr.o
clean:
	rm *.o
