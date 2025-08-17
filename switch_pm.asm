; --- File: switch_pm.asm ---
switch_to_pm:
    cli                     ; 1. Disable interrupts
    lgdt [gdt_descriptor]   ; 2. Load the GDT register with our GDT descriptor

    mov eax, cr0            ; 3. Get the value of the control register 0
    or eax, 0x1             ; 4. Set the protected mode bit (bit 0)
    mov cr0, eax            ; 5. Write it back to switch to protected mode

    ; 6. Make a far jump to a 32-bit code segment to flush the CPU pipeline
    ;    and force it to use our new segment descriptors.
    jmp CODE_SEG:BEGIN_PM