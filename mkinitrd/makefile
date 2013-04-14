SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

IMGSRC = ../build/image
SRC = initrd.img
DEST = $(IMGSRC)/$(SRC)

CFLAGS = -Wall

mkinitrd: $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^
	./mkinitrd $(SRC) $(IMGSRC)

clean:
	rm $(OBJS)
	rm mkinitrd

.PHONY: clean

