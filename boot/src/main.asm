[org 0x7c00]
[bits 16]

KERNEL_OFFSET equ 0x1000

start:
    ; Set data segments
    xor ax, ax
    mov ds, ax
    mov es, ax

    ; Save boot drive
    mov [BOOT_DRIVE], dl

    ; Set stack
    mov bp, 0x9000
    mov sp, bp

    ; Set video mode 03h: text mode 80x25
    mov ah, 0x00
    mov al, 0x03
    int 0x10

    ; Enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Print boot messages
    mov si, message1
    call print_string

    mov si, switching_to_pm_msg
    call print_string

    ; ----------------------------------------------------
    ; Load kernel.
    ; Kernel has 19 sectors, so we load 20 sectors total.
    ;
    ; Floppy track has 18 sectors.
    ; From sector 2 to sector 18 = 17 sectors.
    ; Then continue on head 1, sector 1 for 3 sectors.
    ; ----------------------------------------------------

   
    
    mov bx, KERNEL_OFFSET
    mov al, 17
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [BOOT_DRIVE]
    call disk_load

    mov bx, KERNEL_OFFSET + 17 * 512
    mov al, 18
    mov ch, 0
    mov cl, 1
    mov dh, 1
    mov dl, [BOOT_DRIVE]
    call disk_load

    mov bx, KERNEL_OFFSET + 35 * 512
    mov al, 10
    mov ch, 1
    mov cl, 1
    mov dh, 0
    mov dl, [BOOT_DRIVE]
    call disk_load

    ; Switch to 32-bit protected mode
    call switch_to_pm

    jmp $

BOOT_DRIVE db 0

%include "../disk/disk_load.asm"
%include "../print/print_string.asm"
%include "../data/data.asm"
%include "../pmode/gdt.asm"
%include "../pmode/switch_to_pm.asm"
%include "../pmode/print_string_pm.asm"

[bits 32]

BEGIN_PM:
    mov ebx, pm_msg
    call print_string_pm

    jmp KERNEL_OFFSET

    jmp $

times 510-($-$$) db 0
dw 0xaa55
