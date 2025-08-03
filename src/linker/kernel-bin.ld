ENTRY(kernel_main)
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 0x1000;

    .text : ALIGN(4K)
    {
        *(.text.kernel_main)
        *(.text)
        *(.text.*)
    }

    .rodata : ALIGN(4K)
    {
        *(.rodata)
        *(.rodata.*)
    }

    .data : ALIGN(4K)
    {
        *(.data)
        *(.data.*)
    }

    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }
}