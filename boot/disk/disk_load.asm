disk_load:
    pusha

    mov ah, 0x02
    int 0x13

    jc disk_error

    popa
    ret

disk_error:
    mov si, DISK_ERROR_MSG
    call print_string
    jmp $

DISK_ERROR_MSG db "Disk read error!", 0
