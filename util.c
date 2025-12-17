#include <stdio.h>

int count_new_lines(char* s) 
{
        int c = 0;
        while (*s) {
                if (*s == '\n') 
                        c++;
                *s++;
        }
        return c;
}
