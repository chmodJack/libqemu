TARGET:=main
CROSS:=
CC:=$(CROSS)gcc
ECHO:=

SSRC+=$(shell find . -name '*.s')
CSRC+=$(shell find . -name '*.c')
CPPSRC+=$(shell find . -name '*.cpp')
OBJS+=$(SSRC:%.s=%.o)
OBJS+=$(CSRC:%.c=%.o)
OBJS+=$(CPPSRC:%.cpp=%.o)

ALLPATHS:=$(shell ls -R | grep :)
CFLAGS+=$(ALLPATHS:%:=-I%)
CFLAGS+=-I. $(SYSTEMC_CXXFLAGS) $(LIBVVS_CXXFLAGS) $(LIBQEMU_CXXFLAGS) $(AMBA_PV_CXXFLAGS)
CFLAGS+=-c -O2 -fPIC

CSFLAGS+=$(CFLAGS)
CCFLAGS+=$(CFLAGS) -std=c11
CCPPFLAGS+=$(CFLAGS) -std=c++11

LDFLAGS+=$(ALLPATHS:%:=-L%)
LDFLAGS+=-O2 -fPIC
LDFLAGS+=-L. $(SYSTEMC_LDFLAGS) $(LIBVVS_LDFLAGS) $(LIBQEMU_LDFLAGS)
LDFLAGS+=-pthread -lstdc++ -lm -lc -ldl
STATIC_LIBS+=

.PHONY:all clean
all:$(TARGET).elf
	@echo -e '[33m[GO] [32m$<[0m'
	$(ECHO) ./$<
$(TARGET).elf:$(OBJS) $(STATIC_LIBS)
	@echo -e '[33m[LD] [32m$@[0m'
	$(ECHO)$(CC) -o $@ $^ $(LDFLAGS)
%.o:%.s
	@echo -e '[33m[CC] [32m$@[0m'
	$(ECHO)$(CC) -o $@ $^ $(CSFLAGS)
%.o:%.c
	@echo -e '[33m[CC] [32m$@[0m'
	$(ECHO)$(CC) -o $@ $^ $(CCFLAGS)
%.o:%.cpp
	@echo -e '[33m[CC] [32m$@[0m'
	$(ECHO)$(CC) -o $@ $^ $(CCPPFLAGS)
clean:
	@echo -e '[33m[RM] [32m$(TARGET).elf $(OBJS)[0m'
	$(ECHO)rm -rf $(TARGET).elf $(OBJS)
