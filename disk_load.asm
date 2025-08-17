; --- File: disk_load.asm ---
disk_load:
    pusha
    push dx

    mov ah, 0x02    ; BIOS read sector(s) into memory function
    mov al, dh      ; Number of sectors to read
    mov ch, 0x00    ; Cylinder 0
    mov dh, 0x00    ; Head 0
    mov cl, 0x02    ; Start reading from sector 2 (sector 1 is the bootloader itself)

    int 0x13        ; BIOS disk interrupt
    jc disk_error   ; If carry flag is set, there was an error

    pop dx
    cmp al, dh      ; Compare actual sectors read (al) with requested sectors (dh)
    jne disk_error
    popa
    ret

disk_error:
    mov bx, DISK_ERROR_MSG
    call print_string
    jmp $           ; Hang forever

DISK_ERROR_MSG db "Disk read error!", 0