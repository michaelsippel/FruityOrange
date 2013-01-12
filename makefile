KERNEL = ./kernel/kernel
IMAGE = ./build/image

all: lib kernel mods

kernel:
	$(MAKE) -C kernel
	cp $(KERNEL) $(IMAGE)/kernel

mods:
	$(MAKE) -C test

lib:
	$(MAKE) -C lib

floppy-img: all
	#TODO

cdrom-img: all
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o cdrom.iso $(IMAGE)

qemu: cdrom-img
	qemu-system-i386 -d int,cpu_reset -cdrom cdrom.iso

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C lib clean

.PHONY: all kernel lib clean qemu
