CC=clang
CFLAGS=-Wall
main:demo_walker.o	demo_fixer.o

demo_walker.o:demo_walker.c
	$(CC)	$(CFLAGS) demo_walker.c -o demo_walker.o
demo_fixer.o:demo_bitmaprepair.c
	$(CC)	$(CFLAGS) demo_bitmaprepair.c -o demo_fixer.o
clean:
	rm *.o
