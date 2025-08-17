; --- File: boot.asm ---
; This is the bootloader. It's the very first thing that runs when the computer starts.
; Its job is to switch the processor from 16-bit real mode to 32-bit protected mode,
; and then call our C kernel.

[org 0x7c00] ; The BIOS loads us at address 0x7c00

KERNEL_OFFSET equ 0x1000 ; We will load our kernel at 1MB

    mov [BOOT_DRIVE], dl ; The BIOS stores the boot drive in DL

    mov bp, 0x9000  ; Set up the stack
    mov sp, bp

    mov bx, MSG_REAL_MODE
    call print_string
    
    call load_kernel

    call switch_to_pm

    jmp $ ; This should never be reached

; Include our printing and disk loading functions
%include "boot_print.asm"
%include "disk_load.asm"
%include "gdt.asm"
%include "32bit_print.asm"
%include "switch_pm.asm"

[bits 16]
load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call print_string

    mov bx, KERNEL_OFFSET ; Read from disk into memory at 0x1000
    mov dh, 2             ; FIXED: Read 2 sectors instead of 15
    mov dl, [BOOT_DRIVE]
    call disk_load
    
    mov bx, MSG_KERNEL_LOADED ; ADDED: Success message
    call print_string
    ret

[bits 32]
BEGIN_PM:
    mov ebx, MSG_PROT_MODE
    call print_string_pm ; Use our 32-bit print function
    
    call KERNEL_OFFSET ; Jump to the address where we loaded our kernel
    
    jmp $ ; Hang here

BOOT_DRIVE db 0
MSG_REAL_MODE db "Started in 16-bit Real Mode", 0x0D, 0x0A, 0
MSG_PROT_MODE db "Landed in 32-bit Protected Mode", 0x0D, 0x0A, 0
MSG_LOAD_KERNEL db "Loading kernel into memory...", 0x0D, 0x0A, 0
MSG_KERNEL_LOADED db "Kernel loaded successfully.", 0x0D, 0x0A, 0

; Padding and magic number
times 510-($-$$) db 0
dw 0xaa55