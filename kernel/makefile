SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = gcc
LD = ld

INC = -Iinclude -I../include

ASFLAGS = -m32 $(INC)
CFLAGS = -m32 -Wall -g -fno-stack-protector -fno-builtin -nostdinc $(INC)
LDFLAGS = -melf_i386 -Tkernel.ld

KERNEL = kernel

$(KERNEL): $(OBJS) ../lib/libc.a
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.S
	$(CC) $(ASFLAGS) -c -o $@ $^

clean:
	rm $(OBJS)
	rm $(KERNEL)

.PHONY: clean
