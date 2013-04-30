SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

LIBS = ../../lib/user/userlibc.a ../../lib/common/commonlibc.a 
IMGSRC = ../../build/image
SRC = cp
DEST = $(IMGSRC)/$(SRC)

INC = -I ../../include -I ../../include/user

ASFLAGS = -m32 $(INC)
CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin -nostdinc $(INC)
LDFLAGS = -melf_i386 -flto -T../../lib/std_link.ld

$(SRC): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ ../../lib/start.o $(LIBS)
	cp $(SRC) $(DEST)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.S
	$(CC) $(ASFLAGS) -c -o $@ $^

clean:
	rm $(OBJS)
	rm $(SRC)

.PHONY: clean

