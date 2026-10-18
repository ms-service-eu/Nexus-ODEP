/* 
 * sandbox_linker.ld - Strikte, unprivilegierte Speichersegmentierung für ODEP-Code
 * Erzwingt harte Grenzen, die direkt von der Hardware-MPU/MMU überwacht werden.
 */

MEMORY
{
  /* Isoliertes Flash-Segment für unprivilegierten Code (rx = Read, Execute) */
  SANDBOX_TEXT (rx)  : ORIGIN = 0x08040000, LENGTH = 16K
  
  /* Isoliertes RAM-Segment für Variablen (rw = Read, Write) -> STRIKT KEIN EXECUTE! */
  SANDBOX_RAM  (rw)  : ORIGIN = 0x20010000, LENGTH = 4K
}

SECTIONS
{
  /* Code und schreibgeschützte Konstanten */
  .text :
  {
    . = ALIGN(32);
    /* 
     * Erzeugt einen festen Anker für die API-Sprungtabelle am Anfang des Sektors.
     * Hierüber ruft der Sandbox-Code sichere Host-Funktionen auf.
     */
    KEEP(*(.odep_api_table)) 
    
    *(.text)
    *(.text*)
    *(.rodata)
    *(.rodata*)
    . = ALIGN(32);
    
    /* Exportiert die exakte Code-Endgrenze für die MPU-Konfiguration des Kernels */
    _e_sandbox_text = .;
  } > SANDBOX_TEXT

  /* Initialisierte und uninitialisierte Datenstrukturen */
  .data :
  {
    . = ALIGN(32);
    _s_sandbox_data = .;
    *(.data)
    *(.data*)
    . = ALIGN(32);
    _e_sandbox_data = .;
  } > SANDBOX_RAM

  .bss :
  {
    . = ALIGN(32);
    _s_sandbox_bss = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(32);
    _e_sandbox_bss = .;
    
    /* Der Stack wächst vom Ende des RAMs nach unten, begrenzt durch das BSS-Ende */
    _sandbox_stack_limit = .;
  } > SANDBOX_RAM

  /* Globale Symbole für den Host-Sicherheits-Kernel zur MPU-Konfiguration */
  _sandbox_text_start = ORIGIN(SANDBOX_TEXT);
  _sandbox_text_size  = LENGTH(SANDBOX_TEXT);
  _sandbox_ram_start  = ORIGIN(SANDBOX_RAM);
  _sandbox_ram_size   = LENGTH(SANDBOX_RAM);
}
