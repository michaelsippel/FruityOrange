SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = gcc
LD = ld

INC = -Iinclude -I../include

ASFLAGS = -m32
CFLAGS = -m32 -Wall -O3 -fno-stack-protector -fno-builtin -nostdinc $(INC)
LDFLAGS = -melf_i386 -Tkernel.ld

LIB = libc.a

$(LIB): $(OBJS)
	ar -rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.S
	$(CC) $(ASFLAGS) -c -o $@ $^

clean:
	rm $(OBJS)
	rm $(LIB)

.PHONY: clean
