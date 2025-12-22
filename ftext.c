/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

#define PROGRAM_NAME "ftext"
#define VERSION "0.0.1"

#define COLS 3
#define LINES 24
#define WIDTH 21
#define GAP 6

#define NEW_PAGE "\n %%%\n\n"

int debug = 0;

/* function declarations */
char* next_word(void);
char* strspace(size_t n);
char** ftext(int cols, int lines, int gap, int width);
char** palloc(int n, int length);
int count_notascii(char* str);
int fpage(char** page, int cols, int lines, int width, int gap);
void die(const char *fmt, ...);
void emit_invalid_arg(char* opt);
void emit_try_help(void);
void emit_version(void);
void freepage(char** page, int n);
void str_border_align(char** words, char* str, int word_counter, int width, int current_width);
void str_left_align(char** words, char* str, int word_counter);
void usage(void);

static struct option const longopts[] = {
        {"version", no_argument, NULL, 'v'},
        {"debug", no_argument, NULL, 'd'},
        {"help", no_argument, NULL, 'h'},
        {"lines", required_argument, NULL, 'l'},
        {"columns", required_argument, NULL, 'c'},
        {"width", required_argument, NULL, 'w'},
        {"gap", required_argument, NULL, 'g'},
        {NULL, 0, NULL, 0},
};

void usage(void) 
{
        printf("Usage %s [OPTIONS]\n", PROGRAM_NAME);
        puts("Column the text provided in stdin and print it in stdout.");
        puts("The following options can be used to customize the formatting."); 
        printf("  --columns -c    number of columns (default %d)\n", COLS); 
        printf("  --lines   -l    number of lines (default %d)\n", LINES); 
        printf("  --width   -w    number of characters on a row of a column (default %d)\n", WIDTH);
        printf("  --gap     -l    number of spaces between columns (default %d)\n", GAP); 
        puts("  --help    -h    display this help and exit");
        puts("  --debug   -d    enable debug mode"); 
        puts("  --version -v    output version information and exit");

        exit(EXIT_SUCCESS);
}

void emit_version(void)
{
        printf("%s\n", VERSION);
        exit(EXIT_SUCCESS);
}

void emit_invalid_arg(char* opt)
{
        fprintf(stderr, "Invalid argument for option %s\n", opt);
        emit_try_help();
}

void emit_try_help(void) 
{
        fprintf(stderr, "Try '%s --help' for more information.\n", PROGRAM_NAME);
        exit(EXIT_FAILURE);
}

void die(const char *fmt, ...)
{
	va_list ap;
	int saved_errno;

	saved_errno = errno;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':')
		fprintf(stderr, " %s", strerror(saved_errno));
	fputc('\n', stderr);

	exit(1);
}

char* salloc(size_t n)
{
        char* str;
        if ((str = (char*) calloc(n, sizeof(char))) == NULL)
                die("salloc: cannot calloc %zu bytes", n);
        return str;
}

char** palloc(int n, int length)
{
        char** page;
        if (!(page = (char**) calloc(n, sizeof(char*))))
                die("palloc: cannot calloc %zu bytes", n * sizeof(char*));
        for (int i = 0; i < n; i++) {
                char* line;
                if (!(line = (char*) calloc(length, sizeof(char))))
                        die("palloc: cannot calloc %zu bytes", length);
                page[i] = line;
        }
        return page;
}

int isspace2(char c) 
{
        return c == '\f' || c == '\r' || c == '\t' || c == '\v' || c == ' ';
}

char* next_word(void) {
    int c;
    size_t len = 0;
    size_t cap = 16;
    char* buf = salloc(cap);

    while ((c = getchar()) != EOF && isspace(c)) {
        if (c == '\n') {
                buf[0] = '\0';   // stringa vuota
                return buf;
        }
    }

    if (c == EOF) {
        free(buf);
        return NULL;
    }

    while (c != EOF && !isspace(c)) {
        if (len + 1 >= cap) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) {
                free(buf);
                return NULL;
            }
            buf = tmp;
        }
        buf[len++] = (char)c;
        c = getchar();
    }

    buf[len] = '\0';
    return buf;
}

char* strspace(size_t n)
{
        char* str = salloc(n);
        memset(str, ' ', n);
        return str;
}

char** ftext(int cols, int lines, int width, int gap) 
{
        size_t page_width = (width * cols + (cols - 1) * gap) * 4;
        for (;;) {
                char** page = palloc(lines, page_width);
                int status = fpage(page, cols, lines, width, gap);
                for (int y = 0; y < lines; y++)
                        printf("%s\n", page[y]);
                freepage(page, lines);
                if (status == EOF) 
                        break; 
                printf("%s", NEW_PAGE);
        }
}

void freepage(char** page, int n) 
{
        for (int i = 0; i < n; i++)
                free(page[i]);
        free(page);
}

int fpage(char** page, int cols, int lines, int width, int gap) 
{
        int max_words = 100; 
        char** words;
        if ((words = (char**) calloc(max_words, sizeof(char*))) == NULL)
                die("fpage: cannot calloc %zu bytes", max_words * sizeof(char*));
        int current_width = 0;
        int word_counter = 0;
        for (int c = 0; c < cols; c++) {
                for (int l = 0; l < lines; l++) {
                        while (1) {
                                char* word = next_word();

                                // EOF Reached.
                                if (word == NULL) {
                                        if (word_counter > 0) {
                                                char* line = salloc(width);
                                                // alignment
                                                str_left_align(words, line, word_counter);
                                                strcat(page[l], line);
                                                free(line);
                                        }
                                        free(word);
                                        freepage(words, max_words);
                                        return EOF;
                                }

                                int word_length = strlen(word) - count_notascii(word);

                                // New line found.
                                if (word_length == 0) {
                                        next_word();
                                        char* line = salloc(width);
                                        str_left_align(words, line, word_counter);
                                        strcat(page[l], line);
                                        strcat(page[l], strspace(gap + width - current_width - word_counter + 1));
                                        if (l < lines - 1) {
                                                strcat(page[l + 1], strspace(width + gap));
                                        }
                                        current_width = 0; 
                                        word_counter = 0;
                                        words[0] = NULL;
                                        l++;
                                        free(line);
                                        break;
                                }

                                // Buffer has no space for 'word'.
                                else if (word_length + current_width + word_counter > width) {
                                        char* line = salloc(width);
                                        str_border_align(words, line, word_counter, width, current_width);
                                        strcat(page[l], line);
                                        strcat(page[l], strspace(gap));
                                        current_width = word_length; 
                                        word_counter = 1;
                                        words[0] = word;
                                        free(line);
                                        break;
                                } 
                                
                                // Buffer has enough space for 'word'.
                                else {
                                        if (word_counter >= max_words) {
                                                max_words += 100;
                                                if ((words = (char**) realloc(words, max_words * sizeof(char*))) == NULL)
                                                        die("fpage: cannot realloc %zu bytes", max_words * sizeof(char*));
                                        }
                                        words[word_counter] = word;
                                        word_counter++;
                                        current_width += word_length;
                                }
                        }
                }
        }
        freepage(words, lines);
}

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
                } else {
                        in_special_char = 0;
                }
        }
        return counter;
}

void str_left_align(char** words, char* str, int word_counter)
{
        strcat(str, words[0]);
        for (int i = 1; i < word_counter; i++) {
                strcat(str, " ");
                strcat(str, words[i]);
        }
}

void str_border_align(char** words, char* str, int word_counter, int width, int current_width)
{
        strcpy(str, words[0]);
        int internal_words;
        if (word_counter == 1)
                internal_words = 0;
        else
                internal_words = word_counter - 2;
        int total_spaces = width - current_width;
        for (int i = 1; i < word_counter; i++) {
                int nspaces = ceil((double) total_spaces / (internal_words + 1));
                total_spaces -= nspaces;
                internal_words -= 1;
                char* spaces = strspace(nspaces);
                strcat(str, spaces);
                strcat(str, words[i]);
        }
}

int main(int argc, char* argv[])
{
        int cols = COLS;
        int lines = LINES;
        int width = WIDTH;
        int gap = GAP;

        int opt;
        while ((opt = getopt_long(argc, argv, "c:l:w:g:vhd", longopts, NULL)) != -1)
                switch (opt) 
                {
                        case 'v':
                                emit_version();
                        case 'd':
                                debug = 1;
                                break;
                        case 'h':
                                usage();
                        case 'c':
                                cols = atoi(optarg);
                                break;
                        case 'l':
                                lines = atoi(optarg);
                                break;
                        case 'w':
                                width = atoi(optarg);
                                break;
                        case 'g':
                                // FIXME: use strtol instead of atoi
                                gap = atoi(optarg);
                                break;
                        case '?':
                                emit_try_help();
                }
        
        if (cols <= 0) emit_invalid_arg("--columns");
        if (lines <= 0) emit_invalid_arg("--lines");
        if (width <= 0) emit_invalid_arg("--width");

        ftext(cols, lines, width, gap);

        return EXIT_SUCCESS;
}
