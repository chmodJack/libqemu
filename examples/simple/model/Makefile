LIBQEMU_HOME=../../..

TARGET:=main
CROSS:=
CC:=$(CROSS)gcc
ECHO:=

OBJS+=main.o

CFLAGS+=-I. -I$(LIBQEMU_HOME)/include
CFLAGS+=-DDEBUG=1
CFLAGS+=-c -Ofast

CSFLAGS+=$(CFLAGS)
CCFLAGS+=$(CFLAGS) -std=c11
CCPPFLAGS+=$(CFLAGS) -std=c++14

LDFLAGS+=-L. -L$(LIBQEMU_HOME)/lib
LDFLAGS+=-Ofast
LDFLAGS+=-Wl,-rpath=. -L.
LDFLAGS+=-lqemu-system-arm -pthread -lstdc++ -lm -lc -ldl
STATIC_LIBS+=

.PHONY:all clean
all:$(TARGET).elf
	@echo -e '[33m[GO] [32m$<[0m'
	$(ECHO) ./$<
$(TARGET).elf:$(OBJS) $(STATIC_LIBS)
	@echo -e '[33m[LD] [32m$@[0m'
	$(ECHO)$(CC) -o $@ $^ $(LDFLAGS)
%.o:%.cpp
	@echo -e '[33m[CC] [32m$@[0m'
	$(ECHO)$(CC) -o $@ $^ $(CCPPFLAGS)
clean:
	@echo -e '[33m[RM] [32m$(TARGET).elf $(OBJS)[0m'
	$(ECHO)rm -rf $(TARGET).elf $(OBJS)
