
MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x00040000
  SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00010000
}

SECTIONS
{
  .text : ALIGN(8)
  {
    FILL(0xFF)
    _text = .;
    KEEP(*(.isr_vector))
    *(.text*)
    *(.rodata*)
    . = ALIGN(8);
  } > FLASH

  .ARM.extab : ALIGN(8)
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > FLASH

  __exidx_start = .;

  .ARM.exidx : ALIGN(8)
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > FLASH

  __exidx_end = .;

  . = ALIGN(8);
  _etext = .;

  .data : ALIGN(8)
  {
    _data = .;
    *(vtable)
    *(.data*)
    . = ALIGN(8);
    _edata = .;
  } > SRAM AT>FLASH

  .bss : ALIGN(8)
  {
    _bss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(8);
    _ebss = .;
    _end = .;
  } > SRAM

  PROVIDE(_HEAP_START = _end);
  PROVIDE(_HEAP_END = ALIGN(ORIGIN(SRAM) + LENGTH(SRAM) - 256, 8));
  PROVIDE(_STACK_TOP = ORIGIN(SRAM) + LENGTH(SRAM));
}
