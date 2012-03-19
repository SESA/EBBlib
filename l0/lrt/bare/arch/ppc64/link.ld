OUTPUT_ARCH(powerpc:common64)
/* OUTPUT_FORMAT(elf64-powerpc) */
ENTRY(_start)

SECTIONS
{
	. = 0x0;
	kstart = .;

	.init :
	{
		*(.init.text);
		*(.init.stack);
	}

        /* Prism <= DD2.0 requies us to avoid 0-256K for coherency */
	/* first 256k are not coherent on this board version */
	. = (256 << 10);
	.isr :
	{
		*(.isr);
	}

	.text ALIGN (4K) :
	{
		*(.text);
	}

	.rodata ALIGN (4K) : 
	{
		*(.rodata)
	}
	.data ALIGN (4K) : 
	{
		*(.data)
	}

	.bss : 
	{
		sbss = .;
		*(COMMON)
		*(.bss)
		ebss = .;
	}

	/DISCARD/ :
	{
		*(.eh_frame);
		*(.note*);
		*(.comment);
	}
	kend = .;	
}	