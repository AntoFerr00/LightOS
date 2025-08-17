# Makefile for the barebones LightOS

# Source files
ASM_SOURCES = boot.asm
C_SOURCES = kernel.c
OBJECTS = boot.o kernel.o

# Tools
AS = nasm
CC = gcc
LD = ld

# Compiler flags - Added more conservative flags
CFLAGS = -m32 -ffreestanding -O0 -Wall -Wextra -nostdlib -fno-builtin -fno-pic -fno-stack-protector -fno-asynchronous-unwind-tables

# Assembler flags
ASFLAGS = -f elf32

# Linker flags - Simplified and more explicit
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

# Build a bootable ISO
all: lightos.iso

lightos.iso: kernel.bin grub.cfg
	mkdir -p isodir/boot/grub
	cp kernel.bin isodir/boot/
	cp grub.cfg isodir/boot/grub/
	grub-mkrescue -o lightos.iso isodir
	rm -r isodir

kernel.bin: $(OBJECTS) linker.ld
	@echo "Linking kernel..."
	$(LD) $(LDFLAGS) -o kernel.bin $(OBJECTS)
	@echo "Kernel linked successfully"

kernel.o: kernel.c
	@echo "Compiling kernel.c..."
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

boot.o: boot.asm
	@echo "Assembling boot.asm..."
	$(AS) $(ASFLAGS) boot.asm -o boot.o

# Alternative run targets for debugging
run: lightos.iso
	qemu-system-i386 -cdrom lightos.iso -nographic -m 256M -machine pc-i440fx-2.8 -cpu pentium -no-acpi -no-hpet

run-gui: lightos.iso
	qemu-system-i386 -cdrom lightos.iso -m 256M -machine pc-i440fx-2.8 -cpu pentium -no-acpi -no-hpet

run-simple: lightos.iso
	qemu-system-i386 -cdrom lightos.iso -m 256M

debug: lightos.iso
	qemu-system-i386 -cdrom lightos.iso -nographic -m 256M -s -S

clean:
	rm -f *.bin *.o *.iso
	rm -rf isodir

# Show object file info for debugging
info: $(OBJECTS)
	@echo "=== Object file information ==="
	file $(OBJECTS)
	@echo "=== Sections in boot.o ==="
	objdump -h boot.o
	@echo "=== Sections in kernel.o ==="
	objdump -h kernel.o

.PHONY: all run debug clean info