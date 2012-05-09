
MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x00040000
  SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00010000
}

SECTIONS
{
  .text : ALIGN(4)
  {
    _text = .;
    KEEP(*(.isr_vector))
    *(.text*)
    *(.rodata*)
    . = ALIGN(4);
  } > FLASH

	.ARM.extab : ALIGN(4)
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > FLASH

	__exidx_start = .;

	.ARM.exidx : ALIGN(4)
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > FLASH

	__exidx_end = .;

  . = ALIGN(4);
	_etext = .;

  .data : ALIGN(4)
  {
    _data = .;
    *(vtable)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } > SRAM AT>FLASH

  .bss : ALIGN(4)
  {
    _bss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    _end = .;
  } > SRAM

  /* These are for _sbrk(). We'll give it whatever memory is
   * left as heap to play with. */
  PROVIDE(_HEAP_START = _end);
  PROVIDE(_HEAP_END = ALIGN(ORIGIN(SRAM) + LENGTH(SRAM), 4));
}
