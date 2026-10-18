/* 
 * sandbox_linker.ld - Strikte Speichersegmentierung für geladenen ODEP-Code
 * Definiert harte Grenzen, die von der Hardware-MPU überwacht werden.
 */

MEMORY
{
  /* Dem Programm fest zugewiesener, isolierter Flash-Bereich für den Code */
  SANDBOX_TEXT (rx)  : ORIGIN = 0x08040000, LENGTH = 16K
  
  /* Dem Programm fest zugewiesener, isolierter RAM-Bereich für Variablen/Stack */
  SANDBOX_RAM  (xrw) : ORIGIN = 0x20010000, LENGTH = 4K
}

SECTIONS
{
  /* Code und Konstanten wandern in den geschützten Text-Sektor */
  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } > SANDBOX_TEXT

  /* Daten und Stack wandern in das isolierte RAM-Segment */
  .data :
  {
    . = ALIGN(4);
    *(.data)
    *(.data*)
    *(.bss)
    *(.common)
    . = ALIGN(4);
  } > SANDBOX_RAM
}
