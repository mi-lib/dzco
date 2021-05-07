include ../config

BINDIR=$(PREFIX)/bin

INSTALL=install -m 755
UNINSTALL=rm -f

INCLUDE=`zm-config -I`
LIB=`zm-config -L`
DEF=`zm-config -D`

LINK=-ldzco `zm-config -l`

CONFIGGEN=`which zeda-config-gen`
CONFIG=dzco-config

#####################################################################
all: $(CONFIG)

$(CONFIG):
	@echo " GENERATE	" $@
	-@rm -f $@
	@$(CONFIGGEN) -I "$(INCLUDE)" -L "$(LIB)" -D "$(DEF)" -l "$(LINK)" -v $(VERSION) > $@
install: $(CONFIG)
	@echo " INSTALL	" $^
	@$(INSTALL) $^ $(BINDIR)
clean:
	@echo " CLEAN"
	-@rm -f *.o *~ core $(CONFIG)
uninstall:
	@echo " UNINSTALL	" $(CONFIG)
	@$(UNINSTALL) $(BINDIR)/$(CONFIG)
