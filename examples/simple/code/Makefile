TARGET=main
CROSS=arm-none-eabi-
CC=$(CROSS)gcc
ECHO=@

OBJS=start.o main.o

CFLAGS+=-I.
CFLAGS+=-DDEBUG=1
CFLAGS+=-c -O0 -ggdb -mtune=cortex-a9

CSFLAGS+=$(CFLAGS)
CCFLAGS+=$(CFLAGS) -std=gnu11
CCPPFLAGS+=$(CFLAGS) -std=gnu++11

LDFLAGS+=-O0 -ggdb -nostartfiles -T./link.ld
LDFLAGS+=-L.
LDFLAGS+=-lm -lc

.PHONY:all clean
all:$(TARGET).elf
	$(ECHO)$(CROSS)objdump -D $< > $(TARGET).dis
	$(ECHO)$(CROSS)objcopy -O binary -S $< $(TARGET).bin
$(TARGET).elf:$(OBJS)
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
	$(ECHO)rm -rf $(TARGET).elf $(OBJS) $(TARGET).dis $(TARGET).bin


debug:
	arm-none-eabi-gdb -x ./gdb
