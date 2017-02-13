# XV6

## Build
```bash
cd xv6-public
make
```

## Run
First build the xv6. Then
```bash
cd xv6-public
make qemu-nox
```

For debug, run
```bash
cd xv6-public
make qemu-nox-debug
```

## Kernel Organization

A key design question is what part of the operation system should run in kernel
mode.

### Monolithic Kernel (xv6 is this)

The entire operating system resides in the kernel.
- The impl of all system calls run in kernel mode.
- Convenient, because the OS designer doesn't have to decide which part of the
  operating system doesn't need full hardware privilege.
- Furthermore, it is easy for different parts of the operating system to
  cooperate. For example, an operating system might have a buffer cache that can
  be shared both by the file system and the virtual memory system.
- Complex interface between different parts of the operating system. And thus
  easy to make mistake for coding.
- Code mistake is fatal. System may need to reboot.

### Micro Kernel

Minimize the amount of operating system code that runs in kernel mode, and
execute the buld of the operating system in user mode.
- OS servieces running as processes (in user mode) are called servers.
- Kernel provides IPC to send messages from one user-mode process to another.
- The kernel interface consists of a few low-level functions for starting
  applications, sending messages, accessing device hardware, etc. This
  organization allows the kernel to be relatively simple, as most of the
  operating system resides in user-level servers.
