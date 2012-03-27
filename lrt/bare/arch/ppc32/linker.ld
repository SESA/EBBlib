ENTRY(_start);


SECTIONS
{
  /* Kernel Code starts at 0 and includes Read-Only Data sections */
  . = 0x00000000 ;
  .text ALIGN (0x1000)
  {
	*(.text)
  }
}