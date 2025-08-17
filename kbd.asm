; --- File: keyboard.asm ---
; This function reads a single byte (a scancode) from the keyboard's data port.

[bits 32]

global read_scancode ; Make this function visible to our C code

read_scancode:
    ; The keyboard controller sends data to port 0x60.
    ; The 'in' instruction reads a byte from a hardware port into the 'al' register.
    in al, 0x60
    ret