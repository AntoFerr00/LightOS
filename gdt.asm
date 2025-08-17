; --- File: gdt.asm ---
gdt_start:
    ; The mandatory null descriptor
    gdt_null:
        dd 0x0
        dd 0x0

    ; Code segment descriptor. base=0, limit=4GB
    gdt_code:
        dw 0xffff    ; Limit (bits 0-15)
        dw 0x0       ; Base (bits 0-15)
        db 0x0       ; Base (bits 16-23)
        db 0x9a      ; Access byte: present, ring 0, code, executable, readable
        db 0xcf      ; Granularity: 4KB pages, 32-bit operand size, limit (bits 16-19)
        db 0x0       ; Base (bits 24-31)

    ; Data segment descriptor. Same as code segment.
    gdt_data:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0x92      ; Access byte: present, ring 0, data, writable
        db 0xcf
        db 0x0

gdt_end:

; GDT descriptor: a pointer to the GDT itself (its size and address)
gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; Size of the GDT
    dd gdt_start               ; Start address of the GDT

; Constants to be used in other files
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start