CC=gcc
CCOPTS=--std=gnu99 -Wall -I include/
AR=ar

HEADERS=include/disastrOS.h\
	include/disastrOS_constants.h\
	include/disastrOS_globals.h\
	include/disastrOS_pcb.h\
	include/disastrOS_syscalls.h\
	include/linked_list.h\
	include/pool_allocator.h\

OBJS=pool_allocator.o\
     linked_list.o\
     disastrOS_pcb.o\
     disastrOS.o\
     disastrOS_wait.o\
     disastrOS_fork.o\
     disastrOS_spawn.o\
     disastrOS_exit.o\
     disastrOS_shutdown.o\
     disastrOS_schedule.o\
     disastrOS_preempt.o

LIBS=libs/libdisastrOS.a\
     libs/lib_prova.so

BINS=disastrOS_test

#disastros_test

.phony: clean all


all:	$(LIBS) $(BINS)

%.o:	source/%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

libdisastrOS.a: $(OBJS) 
	$(AR) -rcs $@ $^
	$(RM) $(OBJS)
	

disastrOS_test:		source/disastrOS_test.c $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^

clean:
	rm -rf *.o *~ $(LIBS) $(BINS)
