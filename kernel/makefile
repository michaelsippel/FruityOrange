SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = gcc
LD = ld

INC = include

ASFLAGS = -m32
CFLAGS = -m32 -Wall -g -fno-stack-protector -nostdinc -I$(INC)
LDFLAGS = -melf_i386 -Tkernel.ld

kernel: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.S
	$(CC) $(ASFLAGS) -c -o $@ $^

clean:
	rm $(OBJS)
.PHONY: clean
