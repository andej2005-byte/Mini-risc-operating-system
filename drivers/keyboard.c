#include "keyboard.h"
#include "../cpu/ports.h"
#include "../display/screen.h"
#include "../kernel/shell.h"

static int shift_pressed = 0;

char scancode_to_char(unsigned char scancode) {
    switch (scancode) {
        case 0x02: return '1';
        case 0x03: return '2';
        case 0x04: return '3';
        case 0x05: return '4';
        case 0x06: return '5';
        case 0x07: return '6';
        case 0x08: return '7';
        case 0x09: return '8';
        case 0x0A: return '9';
        case 0x0B: return '0';

        case 0x10: return 'q';
        case 0x11: return 'w';
        case 0x12: return 'e';
        case 0x13: return 'r';
        case 0x14: return 't';
        case 0x15: return 'y';
        case 0x16: return 'u';
        case 0x17: return 'i';
        case 0x18: return 'o';
        case 0x19: return 'p';

        case 0x1E: return 'a';
        case 0x1F: return 's';
        case 0x20: return 'd';
        case 0x21: return 'f';
        case 0x22: return 'g';
        case 0x23: return 'h';
        case 0x24: return 'j';
        case 0x25: return 'k';
        case 0x26: return 'l';

        case 0x2C: return 'z';
        case 0x2D: return 'x';
        case 0x2E: return 'c';
        case 0x2F: return 'v';
        case 0x30: return 'b';
        case 0x31: return 'n';
        case 0x32: return 'm';

        case 0x39: return ' ';
        case 0x1C: return '\n';
        case 0x0E: return '\b';

        case 0x33: return ',';
        case 0x34: return '.';
        case 0x35: return '/';
        case 0x0C: return '-';
        case 0x0D: return '=';

        default: return 0;
    }
}

char scancode_to_shift_char(unsigned char scancode) {
    char c;

    switch (scancode) {
        case 0x02: return '!';
        case 0x03: return '@';
        case 0x04: return '#';
        case 0x05: return '$';
        case 0x06: return '%';
        case 0x07: return '^';
        case 0x08: return '&';
        case 0x09: return '*';
        case 0x0A: return '(';
        case 0x0B: return ')';

        case 0x0C: return '_';
        case 0x0D: return '+';

        case 0x33: return '<';
        case 0x34: return '>';
        case 0x35: return '?';

        default:
            break;
    }

    c = scancode_to_char(scancode);

    if (c >= 'a' && c <= 'z') {
        return c - 32;
    }

    return c;
}

void keyboard_callback() {
    unsigned char scancode;
    char c;

    scancode = port_byte_in(0x60);

    
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return;
    }

    /* Left Shift or Right Shift released */
    if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = 0;
        return;
    }

    /* Ignore key release events */
    if (scancode & 0x80) {
        return;
    }

    if (shift_pressed) {
        c = scancode_to_shift_char(scancode);
    } else {
        c = scancode_to_char(scancode);
    }

    if (c != 0) {
        shell_input(c);
    }
}

void keyboard_install() {
    print_string("Keyboard installed\n", -1, -1);
}
