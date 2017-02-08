# Study Notes for MIT 6.828

## Kernel

### Entry point

Defined in kern/entry.S

### Kernel VMA/LMA

Use objdump -h obj/kern/kernal, it shows the .text section has different VMA and
LMA address.

The VMA and LMA addresses are defined in the linker script: kern/kernal.ld
