ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
      .  = 0x00100000;
      kernel_start_phys = .;

      . += 0xC0000000;
      kernel_start_virt = .;

      .text : AT(kernel_start_phys) {
          *(multiboot*)
          *(.text)
      }

      .data ALIGN(4096) : {
          *(.data)
	  *(.rodata*)
      }

      .bss ALIGN(4096) : {
	  *(COMMON*)
	  *(.bss*)
      }

      kernel_end_virt = .;
      kernel_end_phys = . - 0xC0000000;
}


