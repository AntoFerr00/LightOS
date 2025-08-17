; --- File: 32bit_print.asm ---
VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f

print_string_pm:
    pusha
    mov edx, VIDEO_MEMORY   ; Set edx to the start of video memory
.loop:
    mov al, [ebx]           ; Get a character from the string
    mov ah, WHITE_ON_BLACK  ; Set the color attribute
    cmp al, 0               ; Check for null terminator
    je .done

    mov [edx], ax           ; Write the character and attribute to video memory

    add ebx, 1              ; Move to the next character in the string
    add edx, 2              ; Move to the next cell in video memory (2 bytes per cell)
    jmp .loop
.done:
    popa
    ret