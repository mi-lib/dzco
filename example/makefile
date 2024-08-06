INCLUDE=`dzco-config -I`
LIB=`dzco-config -L`
LINK=`dzco-config -l`

TARGET=$(shell ls *.c | xargs -i basename {} .c | tr -s "\n" " ")

CC=gcc
CFLAGS=-ansi -Wall -O3 $(LIB) $(INCLUDE) -funroll-loops -g

all: $(TARGET)
%: %.c
	$(shell test -d log || mkdir -p log)
	$(CC) $(CFLAGS) -o $@ $< $(LINK)
clean :
	rm -fr *.o *~ core *test log *.dat $(TARGET)
