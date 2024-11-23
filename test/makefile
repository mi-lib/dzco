CFLAGS=`dzco-config --cflags`
LINK=`dzco-config -l`

CC=gcc
CFLAGS+=-ansi -Wall -O3 $(LIB) $(INCLUDE) -funroll-loops

%: %.c
	@$(CC) $(CFLAGS) -o $@ $< $(LINK)
clean :
	-@rm -f *.o *~ core *test
