ENTRY(_start)

SECTIONS {
    . = 0x80000;
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
      *(.bss*)
    }
    __bss_end = .;
}
