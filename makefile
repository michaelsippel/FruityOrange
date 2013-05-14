KERNEL = ./kernel/kernel
IMAGE = ./build/image

all: lib kernel mods

lib:
	$(MAKE) -C lib

kernel:
	$(MAKE) -C kernel
	cp $(KERNEL) $(IMAGE)/kernel

mods:
	$(MAKE) -C concha
	$(MAKE) -C tractius
	$(MAKE) -C init	
	$(MAKE) -C test
	$(MAKE) -C utils
	$(MAKE) -C mkinitrd

floppy-img: all
	#TODO

cdrom-img: all
	cp ./mkinitrd/initrd.img ./build/image/initrd.img
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o cdrom.iso $(IMAGE)
	$(MAKE) -C mkinitrd clean
	rm $(IMAGE)/initrd.img

qemu: cdrom-img
	qemu-system-i386 -d int,cpu_reset -cdrom cdrom.iso

clean:
	$(MAKE) -C lib clean
	$(MAKE) -C kernel clean
	$(MAKE) -C test clean
	$(MAKE) -C utils clean
	$(MAKE) -C concha clean
	$(MAKE) -C tractius clean
	$(MAKE) -C init clean

.PHONY: all kernel lib clean qemu

