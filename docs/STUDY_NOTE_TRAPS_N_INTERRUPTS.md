# Traps And Interrupts

## General

There are three cases when control must be transferred from a user program to
the kernel.

- First, a system call: when a user program asks for an operating system
  service, as we saw at the end of the last chapter.
- Second, an exception: when a program performs an illegal action. Examples of
  illegal actions include divide by zero, attempt to access memory for a PTE
  that is not present, and so on.
- Third, an interrupt: when a device generates a signal to indicate that it
  needs attention from the operating system. For example, a clock chip may
  generate an interrupt every 100 msec to allow the kernel to implement time
  sharing. As another example, when the disk has read a block from disk, it
  generates an interrupt to alert the operating system that the block is ready
  to be retrieved.

The kernel handles all interrupts, rather than processes handling them, because
in most cases only the kernel has the required privilege and state. 

## Terminology

Although the official x86 term is interrupt, xv6 refers to all of these as
traps, largely because it was the term used by the PDP11/40 and therefore is the
conventional Unix term. This chapter uses the terms trap and interrupt
interchangeably, but it is important to remember that traps are caused by the
current process running on a processor (e.g., the process makes a system call
and as a result generates a trap), and interrupts are caused by devices and may
not be related to the currently running process. For example, a disk may
generate an interrupt when it is done retrieving a block for one process, but at
the time of the interrupt some other process may be running. This property of
interrupts makes thinking about interrupts more difficult than thinking about
traps, because interrupts happen concurrently with other activities. Both rely,
however, on the same hardware mechanism to transfer control between user and
kernel mode securely, which we will discuss next.

## Interrupt Descriptor Table (IDT)

On the x86, interrupt handlers are defined in the interrupt descriptor table
(IDT). The IDT has 256 entries, each giving the %cs and %eip to be used when
handling the corresponding interrupt.

In xv6, it is set up in *main.c* file by calling `tvint()` method. The entry
point of all the interrupt descriptors are defined in vectors.S. The IDT is
then loaded to processor in *main.c* file by calling `mpmain()->idtinit()`.

## System Call

In xv6, system call methods are declared in *user.h* file. Their impl are in
*usys.S* file.

## Interrupt

An interrupt is a signal to the processor emitted by hardware or software
indicating an event that needs immediate attention. An interrupt alerts the
processor to a high-priority condition requiring the interruption of the current
code the processor is executing. The processor responds by suspending its
current activities, saving its state, and executing a function called an
interrupt handler (or an interrupt service routine, ISR) to deal with the event.
This interruption is temporary, and, after the interrupt handler finishes, the
processor resumes normal activities.[1] There are two types of interrupts:
hardware interrupts and software interrupts.

Two types:
- Software interrupt: used by devices to communicate that they require attention
  from the operating system.
- Hardware interrupt: caused either by an exceptional condition in the processor
  itself, or a special instruction in the instruction set which causes an
  interrupt when it is executed.

### Software Interrupt Workflow

1.Set up the IDT table and load it to the processor. See the [Interrupt
  Descriptor Table](## Interrupt Descriptor Table (IDT)) section.
2.Call `int IDT_INDEX` instruction. This will invoke the corresponding entry
  point defined in IDT.
  E.g., for system call, it is in *usys.S* file.
3.All the IDT entry points (which are defined in *vectors.S*) will call
  `alltraps` method defined in trapasm.S, which builds a `struct trapframe` in
  the kernel stack. The trap frame contains all the info necessary to restore
  the user mode processor registers when the kernel returns to the current
  process.
4.`trap()` method in *trap.c* is called from `alltraps`. This is the c entry
  point for handling all the interrupts.
5.In `trap()` method, there are `if..else..` and `switch` condition to determine
  how to handle the given interrupt.
6.For system calls, `trap()` invokes `syscall()` in *syscall.c*. `syscall()`
  loads the system call number from the trap frame, which contains the saved
  5=%eax, and indexes into the system call tables defined by `syscalls[]` in
  *syscall.c* file. The helper functions argint, argptr, argstr, and argfd
  retrieve the n’th system call argument, as either an integer, pointer, a
  string, or a file descriptor.

### Hardware Interrupts Workflow

Devices on the motherboard can generate interrupts. Interrupts are usually
optional in the sense that the kernel could instead periodically check (or
"poll") the device hardware to check for new events. Interrupts are preferable
to polling if the events are relatively rare, so that polling would waste CPU
time. Interrupt handling shares some of the code already needed for system calls
and exceptions.

Most modern general purpose microprocessors handle the interrupts the same way.
When a hardware interrupt occurs the CPU stops executing the instructions that
it was executing and jumps to a location in memory that either contains the
interrupt handling code or an instruction branching to the interrupt handling
code. This code usually operates in a special mode for the CPU, interrupt mode,
and, normally, no other interrupts can happen in this mode.

Interrupts are similar to system calls, except devices generate them at any
time. There is hardware on the motherboard to signal the CPU when a device needs
attention (e.g., the user has typed a character on the keyboard). The hardware
includes:
- PIC (old)
- IO APIC/Local APIC (new)

On multi-processor platform, usually different cores handles a subset of the
interrupts. E.g., core 1 may handle the timer, core 2 may handle the disk.

Workflow difference compared to software interrupt:
- PIC cannot call CPU `int` instruction. Instead, it sends a signal to INT pin
  of processor. (step 2 in Software Interrupt Workflow) The rest parts are
  similar: when a hardware interrupt occurs the CPU stops executing the
  instructions that it was executing and jumps to a location in memory that
  either contains the interrupt handling code or an instruction branching to the
  interrupt handling code. e.g., the same c `trap()` method is used to handle
  both hardware and software interrupts.

### Interrupt and Lock in Kernel

Interrupts can cause concurrency even on a single processor: if interrupts are
enabled, kernel code can be stopped at any moment to run an interrupt handler
instead. Suppose iderw held the idelock and then got interrupted to run ideintr.
Ideintr would try to lock idelock, see it was held, and wait for it to be
released. In this situation, idelock will never be released—only iderw can
release it, and iderw will not continue running until ideintr returns—so the
processor, and eventually the whole system, will deadlock.

To avoid this situation, if a lock is used by an interrupt handler, a processor
must never hold that lock with interrupts enabled. 

More explanation:

Imagine if some shared data is used by various parts of the kernel, including
operations such as interrupt handlers that need to be fast and cannot block.
Let's say system call foo is currently active and has acquired a lock to
use/access shared data bar, and interrupts are not disabled when/before
acquiring said lock.

Now a (hardware) interrupt handler, e.g. the keyboard, kicks in and also needs
access to bar (hardware interrupts have higher priority than system calls).
Since bar is currently being locked by syscall foo, the interrupt handler cannot
do anything. Interrupt handlers do need to be fast & not be blocked though, so
they just keep spinning while trying to acquire the lock, which would cause a
deadlock (i.e. system freeze) since syscall foo never gets a chance to finish
and release its lock.

If you disable interrupts before trying to acquire the lock in foo, though, then
foo will be able to finish up whatever it's doing and ultimately release the
lock (and restore interrupts). Any interrupts trying to come in while foo holds
the spinlock will be left on the queue, and will be able to start when the lock
is released. This way, you won't run into the problem described above. However,
care must also be taken to ensure that the lock for bar is held for as short as
possible, so that other higher priority operations can take over whenever
required.

## Driver

A driver is the code in an operating system that manages a particular device:
it tells the device hardware to perform operations, configures the device to
generate interrupts when done, and handles the resulting interrupts. 
