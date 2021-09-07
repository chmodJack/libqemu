ENTRY(_start)
MEMORY
{
	RAM(rwx)  : ORIGIN = 0x00000000 , LENGTH = 0x00400000
}
SECTIONS
{
	.text :
	{
		start.o(.text*)
		*(.text*)
	} > RAM

	. = ALIGN(4);

	.rodata :
	{
		*(.rodata*)
	} > RAM

	. = ALIGN(4);

	data_start = .;

	.data :
	{
		*(.data*)
	} > RAM

	. = ALIGN(4);

	bss_start = .;

	.bss :
	{
		*(.bss*)
	} > RAM

	. = ALIGN(4);

	bss_end = .;
}
