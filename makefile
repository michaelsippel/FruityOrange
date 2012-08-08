all: lib kernel

kernel:
	$(MAKE) -C kernel
lib:
	$(MAKE) -C lib

qemu: all
	qemu -kernel kernel/kernel

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C lib clean

.PHONY: all kernel lib clean qemu
