ENTRY(reset_handler)

MEMORY
{
	FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K
	ROMFS (rx) : ORIGIN = 0X08010000, LENGTH = 64K
	RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 40K
}

SECTIONS
{
	.text :
	{
		KEEP(*(.isr_vector))
		*(.text)
		*(.text.*)
		_sromdev = .;
		*(.rodata)
		*(.rodata.*)
		_eromdev = .;
		_sidata = .;
	} >FLASH

	__romfs_start__ =.;
	.romfs.data :
	 {
	    *(.romfs.data)
	} >ROMFS
	__romfs_end__ =.;

	.data : AT(_sidata)
	{
		_sdata = .;
		*(.data)
		*(.data*)
		_edata = .;
	} >RAM

	.bss :
	{
		_sbss = .;
		*(.bss)
		_ebss = .;
	} >RAM

	_estack = ORIGIN(RAM) + LENGTH(RAM);
}
