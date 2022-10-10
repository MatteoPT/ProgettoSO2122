CC	 = gcc
CCOPTS	 = -g -c -w

OBJS	= main.o\
	  top.o
	  
SOURCE	= main.c\
	  top.c
	  
HEADER	= test.h

OUT	= top_test


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

main.o: main.c
	$(CC) $(CCOPTS) main.c 

top.o: top.c
	$(CC) $(CCOPTS) top.c 


clean:
	rm -rf *.o *~ $(LIBS) $(BINS)
