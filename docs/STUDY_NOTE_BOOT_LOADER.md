# Boot Loader

## Protection Mode

Once entering the protected mode, all memory references are interpreted as
virtual address and translated by the MMU, which means all pointers in C are
virtual address. And one can only dereference virtual address.

## Entry Point

Defined in kern/entry.S. Its LMA can be viewed by `objdump -f obj/kern/kernel`,
VMA can be viewed by `nm obj/kern/kernel | grep entry`.

The makefile in the kern/ folder defines the compile/link rule for it.
