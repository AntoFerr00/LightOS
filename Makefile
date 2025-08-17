# Makefile for LightOS (Linux/WSL Version)

# Target for C files
C_SOURCES = kernel.c
C_OBJECTS = $(C_SOURCES:.c=.o)

# Tools
AS = nasm
CC = gcc
LD = ld

# Flags for the C compiler
# We use -m32 to create a 32-bit binary
# -ffreestanding means we are not linking against a standard library
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra

# Build process
all: lightos.bin

lightos.bin: boot.bin kernel.bin
	cat boot.bin kernel.bin > lightos.bin

boot.bin: boot.asm boot_print.asm disk_load.asm gdt.asm 32bit_print.asm switch_pm.asm
	$(AS) boot.asm -f bin -o boot.bin

kernel.bin: kernel.o linker.ld
	$(LD) -m elf_i386 -o kernel.elf -Tlinker.ld $(C_OBJECTS)
	objcopy -O binary kernel.elf kernel.bin

# Compile C code
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run in QEMU without a GUI window
run: lightos.bin
	qemu-system-i386 -fda lightos.bin -nographic

# Clean up build files
clean:
	rm -f *.bin *.o *.elf