ENTRY(_start)

SECTIONS {
    . = 0x40000000;
    /* . = 0xffffff8040000000; */
    .text : /*AT(ADDR(.text) - 0xffffff8000000000)*/ {
      KEEP(*(.text.boot)) *(.text)
    }
    . = ALIGN(4096);
    ktext_end = .;

    .rodata : /*AT(ADDR(.rodata) - 0xffffff8000000000)*/ {
      *(.rodata)
    }

    . = ALIGN(4096);
    .data : /*AT(ADDR(.data) - 0xffffff8000000000)*/ {
      *(.data)
    }

    . = ALIGN(4096);
    __bss_start = .;
    .bss : /*AT(ADDR(.bss) - 0xffffff8000000000)*/ {
      *(.bss .bss.*)
    }
    __bss_end = .;

    . = ALIGN(4096);
    . += 4096;
    __kstack = .;

    . = ALIGN(4096);
    kend = .;
}

__bss_size = (__bss_end - __bss_start) >> 3;
