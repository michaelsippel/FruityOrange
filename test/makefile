SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

LIBS = ../lib/common/commonlibc.a ../lib/user/userlibc.a
IMGSRC = ../build/image
SRC = test
DEST = $(IMGSRC)/$(NAME)

INC = -I ../include -I ../include/user

ASFLAGS = -m32 $(INC)
CFLAGS = -m32 -Wall -Werror -fno-stack-protector -fno-builtin -nostdinc $(INC)
LDFLAGS = -melf_i386 -T../lib/std_link.ld

$(SRC): $(OBJS) $(LIBS)
	$(LD) $(LDFLAGS) -o $@ $^ ../lib/start.o
	cp $(SRC) $(DEST)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.S
	$(CC) $(ASFLAGS) -c -o $@ $^

clean:
	rm $(OBJS)

.PHONY: clean

