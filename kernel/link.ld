ENTRY(_start)

SECTIONS {
    . = 0x40000000;
    .text : { KEEP(*(.text.boot)) *(.text) }
    . = ALIGN(4096);
    PROVIDE(ktext_end = .);

    .rodata : {
      *(.rodata)
    }

    . = ALIGN(4096);
    .data : {
      *(.data)
    }

    . = ALIGN(4096);
    __bss_start = .;
    .bss : {
      *(.bss .bss.*)
    }
    __bss_end = .;

    . = ALIGN(4096);
    PROVIDE(kend = .);
}

__bss_size = (__bss_end - __bss_start) >> 3;
