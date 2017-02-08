# Study Notes for MIT 6.828

## Kernel

### Entry point

Defined in kern/entry.S. Its LMA can be viewed by `objdump -f obj/kern/kernel`, VMA can be viewed by `nm obj/kern/kernel | grep entry`.

The makefile in the kern/ folder defines the compile/link rule for it.

### Kernel VMA/LMA

Use objdump -h obj/kern/kernal, it shows the .text section has different VMA and
LMA address.

The VMA and LMA addresses are defined in the linker script: kern/kernal.ld
