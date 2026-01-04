#include <stddef.h>

char* strspace(size_t n);
int count_notascii(char* str);
void str_left_align(char** ws, char* str, int wc);
void str_border_align(char** ws, char* str, int wc, int width, int current_width);
