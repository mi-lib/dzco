CC=gcc
CFLAGS=-ansi -Wall -O3 $(LIB) $(INCLUDE) -funroll-loops -g `dzco-config --cflags`
LINK=`dzco-config -l`

TARGET=$(shell ls *.c | xargs -i basename {} .c | tr -s "\n" " ")

all: $(TARGET)
%: %.c
	$(shell test -d log || mkdir -p log)
	$(CC) $(CFLAGS) -o $@ $< $(LINK)
clean :
	rm -fr *.o *~ core *test log *.dat $(TARGET)
