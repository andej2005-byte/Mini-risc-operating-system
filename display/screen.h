#ifndef SCREEN_H
#define SCREEN_H

void print_char(char c, int col, int row);
void print_string(char* string,int col, int row);
void clear_screen();
int get_cursor_offset();
void set_cursor_offset(int offset);
int get_screen_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);
int handle_scrolling(int offset);
void memory_copy(char* source, char* dest, int no_bytes);

#endif

