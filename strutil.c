#include "alloc.h"
#include <math.h>
#include <string.h>

int count_notascii(char* str)
{
    int counter = 0;
    int in_special_char = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < 0) {
            if (in_special_char == 0)
                in_special_char = 1;
            else
                counter++;
        } else
            in_special_char = 0;
    }
    return counter;
}

void str_left_align(char** ws, char* str, int wc)
{
    strcat(str, ws[0]);
    for (int i = 1; i < wc; i++) {
        strcat(str, " ");
        strcat(str, ws[i]);
    }
}

char* strspace(size_t n)
{
    char* str = die_on_fail_calloc(n, sizeof(char));
    memset(str, ' ', n);
    return str;
}

void str_border_align(char** ws, char* str, int wc, int width, int current_width)
{
    strcpy(str, ws[0]);
    int internal_words;
    if (wc == 1)
        internal_words = 0;
    else
        internal_words = wc - 2;
    int total_spaces = width - current_width;
    for (int i = 1; i < wc; i++) {
        int nspaces = ceil((double) total_spaces / (internal_words + 1));
        total_spaces -= nspaces;
        internal_words -= 1;
        char* spaces = strspace(nspaces);
        strcat(str, spaces);
        strcat(str, ws[i]);
    }
}
