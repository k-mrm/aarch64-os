ENTRY(_start)

SECTIONS {
    /* . = 0x40000000; */
    . = 0xffffff8040000000;

    .text.boot : AT(ADDR(.text.boot) - 0xffffff8000000000) {
      KEEP(*(.text.boot))
    }

    . = ALIGN(4096);

    .text : AT(ADDR(.text) - 0xffffff8000000000) {
      *(.text) *(.text.*)
    }
    ktext_end = .;

    .rodata : AT(ADDR(.rodata) - 0xffffff8000000000) {
      *(.rodata) *(.rodata.*)
    }

    .data : AT(ADDR(.data) - 0xffffff8000000000) {
      *(.data) *(.data.*)
    }

    . = ALIGN(4096);
    .bss : AT(ADDR(.bss) - 0xffffff8000000000) {
      __bss_start = .;
      *(.bss .bss.*)
      __bss_end = .;
    }

    . = ALIGN(4096);
    . += 4096;
    __kstack = .;

    . = ALIGN(4096);
    kend = .;

    /DISCARD/ : {
      *(.comment)
    }
}

__bss_size = (__bss_end - __bss_start) >> 3;
