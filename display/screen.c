#include "screen.h"
#include "../cpu/ports.h"
#include "../libc/memory.h"
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

#define VIDEO_MEMORY 0xb8000
#define WHITE_ON_BLACK 0x0f

/* koristi se za ispis 1 slova */
void print_char(char c, int col, int row) {
    char* video_memory = (char*) VIDEO_MEMORY;
    int offset;

    if (col >= 0 && row >= 0) {
        offset = get_screen_offset(col, row);
    } else {
        offset = get_cursor_offset();
    }

    if (c == '\b') {
        if (offset > 0) {
            offset -= 2;

            video_memory[offset] = ' ';
            video_memory[offset + 1] = WHITE_ON_BLACK;

            set_cursor_offset(offset);
        }

        return;
    }

    if (c == '\n') {
        int row = get_offset_row(offset);
        offset = get_screen_offset(0, row + 1);
    } else {
        video_memory[offset] = c;
        video_memory[offset + 1] = WHITE_ON_BLACK;
        offset += 2;
    }

    /*provera da li treba skrolovati */
    offset = handle_scrolling(offset);
    set_cursor_offset(offset);
}

/*pisanje stringa */
void print_string(char* string, int col, int row) {
    int i = 0;

    if (col >= 0 && row >= 0) {
        set_cursor_offset(get_screen_offset(col, row));
    }

    while (string[i] != 0) {
        print_char(string[i], -1, -1);
        i++;
    }
}

/*funkcija za brisanje celog ekrana*/
void clear_screen() {
    int row;
    int col;

    for (row = 0; row < 25; row++) {
        for (col = 0; col < 80; col++) {
            print_char(' ', col, row);
        }
    }
}

/*funkcija koja cita gde se kursor nalazi*/
int get_cursor_offset() {
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;

    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);

    return offset * 2;
}


void set_cursor_offset(int offset) {
    offset /= 2;

    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));

    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

/* funkcija za pretvaranje u bajt */
int get_screen_offset(int col, int row) {
    return 2 * (row * 80 + col);
}

/* iz pomeraja vracanje reda */
int get_offset_row(int offset) {
    return offset / (2 * 80);
}

/* iz pomeraja vracanje kolone */
int get_offset_col(int offset) {
    return (offset - (get_offset_row(offset) * 2 * 80)) / 2;
}

int handle_scrolling(int offset) {
    if (offset < 80 * 25 * 2) {
        return offset;
    }

    int i;

    for (i = 1; i < 25; i++) {
        memory_copy(
            (char*)(VIDEO_MEMORY + get_screen_offset(0, i)),
            (char*)(VIDEO_MEMORY + get_screen_offset(0, i - 1)),
            80 * 2
        );
    }

    char* last_line = (char*)(VIDEO_MEMORY + get_screen_offset(0, 24));

    for (i = 0; i < 80 * 2; i++) {
        last_line[i] = 0;
    }

    offset -= 2 * 80;

    return offset;
}



