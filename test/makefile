-include ../Makefile.param

SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

LIBSRC = ../lib
IMGSRC = ../build/image
SRC = test.bin
DEST = $(IMGSRC)/$(NAME)

INC = -I ../include

ASFLAGS = -m32 $(INC)
CFLAGS = -m32 -Wall -Werror -fno-stack-protector -nostdinc $(INC)
LDFLAGS = -melf_i386 -Ttest.ld

$(SRC): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
	cp $(SRC) $(DEST)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^
%.o: %.S
	$(CC) $(ASFLAGS) -c -o $@ $^
clean:
	rm $(OBJS)
.PHONY: clean
