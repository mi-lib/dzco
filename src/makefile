include ../config

ROOTDIR=..
LIBDIR=$(ROOTDIR)/lib
INCLUDE=-I$(ROOTDIR)/include -I$(PREFIX)/include

CC=gcc
CFLAGS=-ansi -Wall -fPIC -O3 $(INCLUDE) -funroll-loops
LD=gcc
LDFLAGS=-shared
SIGNUP=echo "DZco ver."$(VERSION)" Copyright (C) 2000 Tomomichi Sugihara (Zhidao)" >>

OBJ=dz_tf.o dz_lin.o\
	dz_sys.o\
	dz_sys_misc.o dz_sys_pid.o dz_sys_lag.o\
	dz_sys_lin.o dz_sys_tf.o\
	dz_sys_filt_maf.o dz_sys_filt_bw.o\
	dz_sys_gen.o\
	dz_ident_lag.o
DLIB=libdzco.so

TARGET=$(DLIB)

all: $(TARGET)

$(DLIB): $(OBJ)
	@echo " LD	" $^
	@$(LD) $(LDFLAGS) -o $@ $^ > /dev/null; $(SIGNUP) $@
	-@mv $@ $(LIBDIR)
%.o: %.c
	@echo " CC	" $<
	@$(CC) $(CFLAGS) -c $< > /dev/null
clean:
	@echo " CLEAN"
	-@rm -f *.o *~ $(TARGET)
