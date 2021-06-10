ENTRY(_start)

SECTIONS {
    . = 0x40000000;
    .text : { *(.text .text.*) }
    .rodata : {
      *(.rodata)
    }
    .data : {
      *(.data)
    }

    . = ALIGN(16);
    __bss_start = .;
    .bss : {
      *(.bss .bss.*)
    }
    __bss_end = .;
}

__bss_size = (__bss_end - __bss_start) >> 3;
