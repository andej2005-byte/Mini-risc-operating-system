#include "shell.h"
#include "../display/screen.h"
#include "../assembler/assembler.h"
#include "../assembler/encoder.h"
#include "../assembler/decoder.h"

#define INPUT_BUFFER_SIZE 128
#define ASM_MEMORY_SIZE 256

static char input_buffer[INPUT_BUFFER_SIZE];
static int input_index = 0;

static int asm_mode = 0;

static u32 asm_memory[ASM_MEMORY_SIZE];
static u32* asm_output = asm_memory;

static int asm_memory_index = 0;
static int asm_using_custom_address = 0;
static u32 asm_base_address = 0;

static void print_prompt() {
    if (asm_mode) {
        print_string("asm> ", -1, -1);
    } else {
        print_string("MyOS> ", -1, -1);
    }
}

static int str_equal(char* a, char* b) {
    int i = 0;

    while (a[i] != 0 && b[i] != 0) {
        if (a[i] != b[i]) {
            return 0;
        }

        i++;
    }

    return a[i] == 0 && b[i] == 0;
}

static int is_space(char c) {
    return c == ' ' || c == '\t';
}

static void skip_spaces(char** p) {
    while (is_space(**p)) {
        (*p)++;
    }
}

static int read_word(char** p, char* out, int max) {
    int i = 0;

    skip_spaces(p);

    while (**p != 0 && !is_space(**p)) {
        if (i >= max - 1) {
            return 0;
        }

        out[i] = **p;
        i++;
        (*p)++;
    }

    out[i] = 0;

    return i > 0;
}

static int hex_value(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }

    if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }

    if (c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
    }

    return -1;
}

static int parse_number(char* text, u32* out) {
    int i = 0;
    u32 value = 0;

    if (text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) {
        i = 2;

        if (text[i] == 0) {
            return -1;
        }

        while (text[i] != 0) {
            int digit = hex_value(text[i]);

            if (digit < 0) {
                return -1;
            }

            value = (value << 4) | digit;
            i++;
        }

        *out = value;
        return 0;
    }

    if (text[0] < '0' || text[0] > '9') {
        return -1;
    }

    while (text[i] >= '0' && text[i] <= '9') {
        value = value * 10 + (text[i] - '0');
        i++;
    }

    if (text[i] != 0) {
        return -1;
    }

    *out = value;
    return 0;
}

static void print_hex_digit(unsigned int value) {
    value = value & 0xF;

    if (value < 10) {
        print_char('0' + value, -1, -1);
    } else {
        print_char('A' + (value - 10), -1, -1);
    }
}

static void print_hex_u32(u32 value) {
    int i;

    print_string("0x", -1, -1);

    for (i = 7; i >= 0; i--) {
        print_hex_digit(value >> (i * 4));
    }
}

static void clear_input_buffer() {
    int i;

    for (i = 0; i < INPUT_BUFFER_SIZE; i++) {
        input_buffer[i] = 0;
    }

    input_index = 0;
}

static void start_assembler_mode(u32* output, int custom_address, u32 base_address) {
    asm_mode = 1;
    asm_output = output;
    asm_memory_index = 0;
    asm_using_custom_address = custom_address;
    asm_base_address = base_address;

    print_string("Assembler mode. Type '.' to finish.\n", -1, -1);

    if (custom_address) {
        print_string("Output address: ", -1, -1);
        print_hex_u32(base_address);
        print_string("\n", -1, -1);
    } else {
        print_string("Output: internal asm_memory\n", -1, -1);
    }
}

static void handle_normal_command(char* line) {
    char* p = line;
    char command[16];
    char arg[32];
    u32 value;

    if (!read_word(&p, command, 16)) {
        return;
    }

   if (str_equal(command, "help")) {
    print_string("Available commands:\n", -1, -1);
    print_string(" help              - show this help message\n", -1, -1);
    print_string(" clear             - clear the screen\n", -1, -1);
    print_string(" about             - show system information\n", -1, -1);
    print_string(" asm               - enter assembler mode\n", -1, -1);
    print_string(" asm 0x100000      - assemble to memory address\n", -1, -1);
    print_string(" dump              - show assembled instructions\n", -1, -1);
    print_string(" decode 0xHEX      - decode RISC-V instruction\n", -1, -1);
    print_string("\n", -1, -1);
    print_string("Assembler mode:\n", -1, -1);
    print_string(" Supported: add, sub, addi, lw, sw, beq, jal\n", -1, -1);
    print_string(" Use '.' to exit assembler mode\n", -1, -1);

   } else if (str_equal(command, "clear")) {
        clear_screen();

   } else if (str_equal(command, "about")) {
    print_string("MyOS - 32-bit x86 toy operating system\n", -1, -1);
    print_string("Features:\n", -1, -1);
    print_string(" - Bootloader and protected mode kernel\n", -1, -1);
    print_string(" - VGA text mode output\n", -1, -1);
    print_string(" - IDT, PIC remap, IRQ and keyboard driver\n", -1, -1);
    print_string(" - Interactive shell\n", -1, -1);
    print_string(" - Mini RISC-V assembler / monitor\n", -1, -1);
    print_string(" - Encoder and decoder for RV32I instructions\n", -1, -1);
    print_string("Supported RISC-V instructions:\n", -1, -1);
    print_string(" add, sub, addi, lw, sw, beq, jal\n", -1, -1);

} else if (str_equal(command, "asm")) { 
        if (read_word(&p, arg, 32)) {
            if (parse_number(arg, &value) != 0) {
                print_string("Invalid address\n", -1, -1);
                return;
            }

            start_assembler_mode((u32*) value, 1, value);
        } else {
            start_assembler_mode(asm_memory, 0, 0);
        }

    } else if (str_equal(command, "dump")) {
    int i;

    print_string("Assembled instructions:\n", -1, -1);

    for (i = 0; i < asm_memory_index; i++) {
        if (asm_using_custom_address) {
            print_hex_u32(asm_base_address + i * 4);
            print_string(": ", -1, -1);
        }

        print_hex_u32(asm_output[i]);
        print_string("  ", -1, -1);

        decode_and_print(asm_output[i]);
    }

    } else if (str_equal(command, "decode")) {
        if (!read_word(&p, arg, 32)) {
            print_string("Usage: decode 0x003100B3\n", -1, -1);
            return;
        }

        if (parse_number(arg, &value) != 0) {
            print_string("Invalid instruction\n", -1, -1);
            return;
        }

        decode_and_print(value);

    } else {
        print_string("Unknown command\n", -1, -1);
    }
}

static void handle_asm_command(char* line) {
    u32 instruction;

    if (line[0] == 0) {
        return;
    }

    if (str_equal(line, ".")) {
        asm_mode = 0;

        print_string("Leaving assembler mode\n", -1, -1);
        print_string("Stored instructions: ", -1, -1);

        if (asm_memory_index == 0) {
            print_string("0\n", -1, -1);
        } else {
            print_string("use dump\n", -1, -1);
        }

        return;
    }

    if (asm_memory_index >= ASM_MEMORY_SIZE) {
        print_string("ASM memory full\n", -1, -1);
        return;
    }

    if (assemble_line(line, &instruction) == 0) {
        asm_output[asm_memory_index] = instruction;
        asm_memory_index++;

        print_string("ASM OK: ", -1, -1);
        print_hex_u32(instruction);
        print_string("\n", -1, -1);

    } else {
        print_string("ASM ERROR\n", -1, -1);
    }
}

static void handle_line() {
    input_buffer[input_index] = 0;

    if (asm_mode) {
        handle_asm_command(input_buffer);
    } else {
        handle_normal_command(input_buffer);
    }

    clear_input_buffer();
    print_prompt();
}

void shell_init() {
    clear_input_buffer();

    print_string("Shell ready\n", -1, -1);
    print_string("Type 'help' for commands\n", -1, -1);

    print_prompt();
}

void shell_input(char c) {
    if (c == '\n') {
        print_string("\n", -1, -1);
        handle_line();
        return;
    }

    if (c == '\b') {
        if (input_index > 0) {
            input_index--;
            input_buffer[input_index] = 0;

            print_char('\b', -1, -1);
        }

        return;
    }

    if (input_index < INPUT_BUFFER_SIZE - 1) {
        input_buffer[input_index] = c;
        input_index++;

        print_char(c, -1, -1);
    }
}
