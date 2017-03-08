# Kernel

## Kernel VMA/LMA

Use objdump -h obj/kern/kernal, it shows the .text section has different VMA and
LMA address.

The VMA and LMA addresses are defined in the linker script: kern/kernal.ld

## Page Table

- Each process has its own page table.

- In a single process, the kernel and user memory share the same page table.
  This makes it convinient when switching from user code to kernel code during
  system calls and interrupts: such switches do not require page table switches.

## Page Translation

- 80386 transform a linear address into a physical address using page
  translation.
- A **Page Frame** is a 4K-byte unit of contiguous address of **physical**
  memory.
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
- The page dir is initiated in ```mem_init()```, then the PageInfo array is
  initiated.

Reference: [Intel 80386 Reference Programmer's Manual](https://pdos.csail.mit.edu/6.828/2016/readings/i386/s05_02.htm)

## Address Space

JOS devices the processor's linear address space into two parts.
- User environments (processes): has permission to memory below *UTOP*, has no
  permission to memory above *ULIM*.
- Kernel environments: has R/W permission to memory above *ULIM* and below
  *UTOP*.
- Common: For the address range *[UTOP,ULIM)*, both the kernel and the user
  environment have the same permission: they can read but not write this address
  range. This range of address is used to expose certain kernel data structures
  read-only to the user environment.
- Some important addresses:

| Address | Type |
|---------|------|
| ---kernel memory top--- | KERNBASE/KSTACKTOP 0xf0000000 4294MB |
| ---user memory top--- | ULIM 0xef800000 4018MB |
| *Cur. Page Table (User R-)* | |
| ---page table base--- | UVPT 0xef400000 4013MB |
| ---user stack top--- | UTOP/USTACKTOP 0xeec00000 4005MB |
| ---memory base--- | 0x00000000 0MB |

Reference: ```inc/memlayout.h``` 
