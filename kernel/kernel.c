#include "../display/screen.h"
#include "../cpu/idt.h"
#include "../drivers/keyboard.h"
#include "shell.h"

void kernel_main() {
    clear_screen();

    print_string("Kernel started\n", 0, 0);

    idt_init();
    print_string("IDT loaded\n", -1, -1);

    keyboard_install();

    shell_init();

    __asm__ __volatile__("sti");

    while (1) {
    }
}



