all: kernel

kernel:
	$(MAKE) -C kernel

qemu: kernel
	qemu -kernel kernel/kernel

clean:
	$(MAKE) -C kernel clean

.PHONY: all kernel clean qemu
