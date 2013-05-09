OBJS = $(shell find -name '*.o')

all:
	make -C common
	make -C user
	ar -rcs libc.a $(OBJS)

clean:
	make -C common clean
	make -C user clean
	rm libc.a

.PHONY: clean

