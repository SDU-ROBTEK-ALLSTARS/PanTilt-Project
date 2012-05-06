
MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x00040000
  SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00010000
}

GROUP(
  libgcc.a
  libc.a
  libm.a
)

SECTIONS
{
.text :
  {
    _text = .;
    KEEP(*(.isr_vector))
    *(.text*)
    *(.rodata*)
    _etext = .;
  } > FLASH

  /*
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
  */

.data :
  /* AT(ADDR(.text) + SIZEOF(.text)) */
  {
    _data = .;
    *(vtable)
    *(.data*)
    _edata = .;
  } > SRAM AT>FLASH

.bss :
  {
    _bss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
    . = ALIGN (8);
    _end = .;
  } > SRAM
}

PROVIDE(_HEAP_START = _end);
PROVIDE(_HEAP_END = ALIGN(ORIGIN(SRAM) + LENGTH(SRAM), 8));
