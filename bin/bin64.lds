OUTPUT_FORMAT("elf64-tradlittlemips", "elf64-tradlittlemips", "elf64-tradlittlemips")

OUTPUT_ARCH(mips)
ENTRY(_start)
SECTIONS
{
 . = 0;

 . = ALIGN(8);
 .text :
 {
   *(.text)
 }

 . = ALIGN(8);
 .rodata : { *(.rodata) }

 . = ALIGN(8);
 .data : { *(.data) }

 . = .;
 _gp = ALIGN(16) + 0x7ff0;

 .got : {
   __got_start = .;
   *(.got)
   __got_end = .;
 }

 .sdata : { *(.sdata) }

 .u_boot_cmd : {
   __u_boot_cmd_start = .;
   *(.u_boot_cmd)
   __u_boot_cmd_end = .;
 }

 uboot_end_data = .;
 num_got_entries = (__got_end - __got_start) >> 2;

 . = ALIGN(8);
 .sbss (NOLOAD) : { *(.sbss) }
 .bss (NOLOAD) : { *(.bss) . = ALIGN(8); }
 uboot_end = .;
 _heap = . ;
}
