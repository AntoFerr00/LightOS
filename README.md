# LightOS v0.2

A simple 32-bit hobby operating system for the x86 architecture, built from scratch for educational purposes.

## About The Project

LightOS is a minimalist kernel and shell that demonstrates fundamental operating system concepts. It boots using GRUB, sets up its own stack, and provides a basic command-line interface that interacts directly with hardware (VGA text buffer and PS/2 keyboard polling).

The entire system is contained in a handful of files, making it easy to understand the boot process, kernel entry, and basic I/O handling.

---

## Features

LightOS includes a simple shell with a set of built-in commands:

* `help`: Displays the list of available commands.
* `info`: Shows system details like OS name, version, and architecture.
* `clear`: Clears the entire screen.
* `echo [text]`: Prints the provided text back to the console.
* `uptime`: Shows the (approximate) time since the system was booted.
* `reboot`: Restarts the computer by sending a command to the keyboard controller.
* `calc [a+b]`: A simple calculator that can add two single-digit numbers.
* `colors`: Displays a test pattern of all 16 available VGA text mode colors.

---

## Getting Started

Follow these instructions to build and run LightOS on your own machine.

### Prerequisites

You will need a Linux environment with the following tools installed. On Debian/Ubuntu, you can install them with this command:

```sh
sudo apt-get update
sudo apt-get install build-essential nasm qemu-system-x86 grub-common grub-pc-bin
```

* make, gcc, ld: Core build tools.
* nasm: The Netwide Assembler, for boot.asm.
* qemu-system-i386: An emulator to run the operating system.
* grub-mkrescue: A tool to create a bootable ISO image.

Building
To compile the kernel and create the bootable ISO, simply run make:

```sh
make
```

This will assemble boot.asm, compile kernel.c, link them into kernel.bin, and finally package everything into a bootable lightos.iso file.

Running
The Makefile provides several targets to run LightOS in QEMU:

To run in a graphical window (recommended):

```sh
make run-gui
```

To run in your terminal (no graphics):

```sh
make run
```
(To exit QEMU in this mode, press Ctrl+A, then X)

Cleaning Up
To remove all compiled object files and the ISO image, run:

```sh
make clean
```

Technical Details
Architecture: 32-bit x86 (i386)

Bootloader: GRUB with a Multiboot v1 compliant header.

Kernel Entry: The boot.asm file sets up the Multiboot header, initializes the stack, and then calls the kernel_main function in kernel.c.

Screen Output: The kernel writes directly to the VGA text mode memory buffer at address 0xB8000.

Keyboard Input: The system uses polling (inb/outb) to read scancodes directly from the PS/2 keyboard controller ports (0x60 and 0x64). Interrupts are not used.

Project Structure
.
├── boot.asm          # Assembly code for CPU setup and kernel entry
├── grub.cfg          # GRUB configuration file
├── kernel.c          # The main C kernel, shell, and commands
├── linker.ld         # Linker script to define memory layout
├── Makefile          # Build automation script
└── README.md         # This file