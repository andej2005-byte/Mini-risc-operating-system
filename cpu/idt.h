#ifndef IDT_H
#define IDT_H

#define IDT_ENTRIES 256

struct idt_entry {
    unsigned short base_low;
    unsigned short selector;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_high;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

void set_idt_gate(int n, unsigned int handler);
void load_idt();
void idt_init();

#endif
