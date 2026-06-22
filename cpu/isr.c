#include "isr.h"
#include "ports.h"
#include "../display/screen.h"
#include "../drivers/keyboard.h"

void isr_handler(registers_t* regs) {

    if (regs->int_no == 32) {
    }
    
    if (regs->int_no == 33) {
    keyboard_callback();
	}


    if (regs->int_no == 0) {
        print_string("Received interrupt: 0 Divide by zero\n", -1, -1);

    } else if (regs->int_no == 13) {
        print_string("Received interrupt: 13 General protection fault\n", -1, -1);

    } else if (regs->int_no == 14) {
        print_string("Received interrupt: 14 Page fault\n", -1, -1);
    }


    if (regs->int_no >= 32 && regs->int_no <= 47) {

        if (regs->int_no >= 40) {
            port_byte_out(0xA0, 0x20);
        }

        port_byte_out(0x20, 0x20);
    }
}
