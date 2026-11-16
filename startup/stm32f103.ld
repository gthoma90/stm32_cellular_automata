/* STM32F103x6 memory layout; keep the startup stack address consistent with RAM. */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 32K
    RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 10K
}

SECTIONS
{
    .text :
    {
        /* Reset vectors must be first in flash, even when unused sections are removed. */
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
    } > FLASH 

    /* .data runs in RAM, but its initial values are stored in flash. */
    _sidata = LOADADDR(.data);
    .data :
    {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > RAM AT > FLASH

    .bss :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > RAM
}