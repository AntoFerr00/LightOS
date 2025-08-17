; --- File: boot_print.asm ---
print_string:
    pusha           ; Save all general-purpose registers
    mov ah, 0x0e    ; BIOS teletype output function
.loop:
    mov al, [bx]    ; Get a character from the string (pointed to by bx)
    cmp al, 0       ; Check if it's the null terminator
    je .done        ; If so, we're done
    int 0x10        ; Otherwise, call the BIOS video interrupt to print it
    inc bx          ; Move to the next character
    jmp .loop       ; Repeat
.done:
    popa            ; Restore all registers
    ret             ; Return from the function