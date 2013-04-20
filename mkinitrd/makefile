SRCS = $(shell find -name '*.c')

IMGSRC = ../build/image
SRC = initrd.img
DEST = $(IMGSRC)/$(SRC)

CFLAGS = -Wall

mkinitrd: $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^
	./mkinitrd $(SRC) $(IMGSRC)

clean:
	rm mkinitrd
	rm initrd.img

.PHONY: clean

