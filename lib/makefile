OBJS = $(shell find -name '*.o')

all:
	make -C common
	make -C user
	make -C . libc.a

libc.a: $(OBJS)
	ar -rcs $@ $^

clean:
	make -C common clean
	make -C user clean
	rm libc.a

.PHONY: clean

