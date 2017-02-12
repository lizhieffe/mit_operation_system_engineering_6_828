# Study Notes for MIT 6.828

## *Boot Loader*

### *Protection Mode*

Once entering the protected mode, all memory references are interpreted as
virtual address and translated by the MMU, which means all pointers in C are
virtual address. And one can only dereference virtual address.

## *Kernel*

### *Entry Point*

Defined in kern/entry.S. Its LMA can be viewed by `objdump -f obj/kern/kernel`,
VMA can be viewed by `nm obj/kern/kernel | grep entry`.

The makefile in the kern/ folder defines the compile/link rule for it.

### *Kernel VMA/LMA*

Use objdump -h obj/kern/kernal, it shows the .text section has different VMA and
LMA address.

The VMA and LMA addresses are defined in the linker script: kern/kernal.ld

### *Page Translation*

- 80386 transform a linear address into a physical address using page
  translation.
- A **Page Prame** is a 4K-byte unit of contiguous address of **physical** memory.
- Linear address:

| Range | Field |
|-------|-------|
| 31-22 bits | DIR |
| 21-12 bits | PAGE |
| 11-0  bits | OFFSET |

- CR3 (Page Directory Address) + DIR -> DIR Entry
- DIR Entry + PAGE -> Page Table Entry
- Page Table Entry + OFFSET -> Page Frame (physical address)
- The **page frame address** specifies the physical starting address of a page.
  Because pages are located on 4K boundary, the low-order 12 bits are always
  zero. The 12 bits are used to store the page's metadata. E.g., the lowest bit
  is **Present** bit, representing whether the page can be used.

Reference: [Intel 80386 Reference Programmer's Manual](https://pdos.csail.mit.edu/6.828/2016/readings/i386/s05_02.htm)
