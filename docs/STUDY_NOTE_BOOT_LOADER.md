# Boot Loader

## Protection Mode

Once entering the protected mode, all memory references are interpreted as
virtual address and translated by the MMU, which means all pointers in C are
virtual address. And one can only dereference virtual address.

## Entry Point

Defined in kern/entry.S. Its LMA can be viewed by `objdump -f obj/kern/kernel`,
VMA can be viewed by `nm obj/kern/kernel | grep entry`.

The makefile in the kern/ folder defines the compile/link rule for it.

## Load Address

The boot loader loads the xv6 kernel into memory at physical address 0x100000.
The reason it doesn't load the kernel at 0x80100000, where the kernel expects to
find its instructions and deta, is that there may not be any physical memory at
such a high address on a small machine. The reason it places the kernel at
0x100000 rather than 0x0 is because the address range 0xa0000:0x100000 contains
I/O devices.

## Page Dir/Table

To allow the rest of the kernel to run, ```entry``` sets up a page table that
maps virtual address starting at 0x80000000 (called ```KERNBASE```) to physical
addresses starting at 0x0.

This is a temporary page dir/table. After the control flow is handed over to the
kernel, it will set up a new page dir/table to fit the kernel's needs.
