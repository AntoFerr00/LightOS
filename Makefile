# Makefile for LightOS (Windows Version - Cross-Compiler)

# The prefix for our cross-compiler tools
TARGET_PREFIX = i686-elf-

# Tools
AS = nasm
CC = $(TARGET_PREFIX)gcc
LD = $(TARGET_PREFIX)ld
OBJCOPY = $(TARGET_PREFIX)objcopy

# Flags for the C compiler
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra

# Compile C code to 32-bit object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build process
all: lightos.bin

lightos.bin: boot.bin kernel.bin
	copy /b boot.bin + kernel.bin lightos.bin

boot.bin: boot.asm boot_print.asm disk_load.asm gdt.asm 32bit_print.asm switch_pm.asm
	$(AS) boot.asm -f bin -o boot.bin

kernel.bin: kernel.o linker.ld
	$(LD) -T linker.ld -o kernel.elf --oformat=elf32-i386 kernel.o
	$(OBJCOPY) -O binary kernel.elf kernel.bin

# Run in QEMU
run: lightos.bin
	qemu-system-i386 -fda lightos.bin

# Clean up build files
clean:
	rm -f *.bin *.o *.elf