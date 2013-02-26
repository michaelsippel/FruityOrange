ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
      . = 0x100000;
      kernel_start_phys = .;

      . += 0xC0000000;
      kernel_start_virt = .;

     .text :  AT(ADDR(.text) - 0xC0000000) {
          *(multiboot*)
          *(.text)
      }

      .data ALIGN(4096) : AT(ADDR(.data) - 0xC0000000) {
          *(.data)
	  *(.rodata*)
      }

      .bss ALIGN(4096) : AT(ADDR(.bss) - 0xC0000000) {
	  *(COMMON*)
	  *(.bss*)
      }

      kernel_end_virt = .;
      kernel_end_phys = . - 0xC0000000;
}
