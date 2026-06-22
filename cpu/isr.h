#ifndef ISR_H
#define ISR_H

typedef struct registers {
    unsigned int int_no;
    unsigned int err_code;
} registers_t;

void isr_handler(registers_t* regs);

#endif
