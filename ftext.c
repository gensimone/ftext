/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

#define PROGRAM_NAME "ftext"
#define VERSION "0.0.1"

#define COLS 3
#define COL_LINES 24
#define COL_WIDTH 21
#define COL_GAP 6

#define NEW_PAGE "\n %%%\n\n"

int debug = 0;

/* function declarations */
char* align_strings(char* dst, int n);
char* next_word(void);
char* strspace(size_t n);
char** ftext(int cols, int col_lines, int col_gap, int col_width);
char** palloc(int n, int length);
int fpage(char** page, int cols, int col_lines, int col_width, int col_gap);
void die(const char *fmt, ...);
void emit_invalid_arg(char* opt);
void emit_try_help(void);
void emit_version(void);
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
        printf("  --lines   -l    number of lines (default %d)\n", COL_LINES); 
        printf("  --width   -w    number of characters on a row of a column (default %d)\n", COL_WIDTH);
        printf("  --gap     -l    number of spaces between columns (default %d)\n", COL_GAP); 
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

char** palloc(int n, int length)
{
        char** page;
        if (!(page = (char**) malloc(n * sizeof(char*))))
                die("palloc: cannot malloc %zu bytes", n * sizeof(char*));
        for (int i = 0; i < n; i++) {
                char* line;
                if (!(line = (char*) malloc(length)))
                        die("palloc: cannot malloc %zu bytes", length);
                page[i] = line;
        }
        return page;
}

char* next_word(void)
{
        size_t size = 100;
        char* word;
        if (!(word = (char*) malloc(size)))
                die("cannot malloc %zu bytes:", size);
        int in_word = 0;
        size_t size_c = 0;
        int c;
        for (int i = 0; (c = getchar()) != EOF; i++) {
                if (c == ' ' || c == '\n' || c == '\t' && in_word) 
                        break;
                else {
                        if (size_c >= size) {
                                size += 100;
                                if (!(word = realloc(word, size)))
                                        die("cannot realloc %zu bytes:", size);
                        }
                        word[i] = c;
                        in_word = 1;
                        size_c++;
                }
        }
        return word;
}

char* strspace(size_t n)
{
        char* dst;
        if (!(dst = (char*) malloc(n + 1)))
                die("cannot malloc %zu bytes:", n + 1);
        memset(dst, ' ', n);
        dst[n] = '\0';
        return dst;
}

char** ftext(int cols, int col_lines, int col_width, int col_gap) 
{
        int width_page_line = col_width * cols + (cols - 1) * col_gap;
        for (;;) {
                char** page = palloc(col_lines, width_page_line);
                int finished = fpage(page, cols, col_lines, col_width, col_gap);
                for (int y =0; y < col_lines; y++)
                        printf("%s\n", page[y]);
                free(page);
                if (finished == EOF) break; 
                else printf("%s", NEW_PAGE);
        }
}

int fpage(char** page, int cols, int lines, int width, int gap) 
{
        for (int c = 0; c < cols; c++) {
                for (int l = 0; l < lines; l++) {
                        word = next_word();
                }
        }
}

char* align_strings(char* dst, int n)
{
        int t = 0;
        int string_count = 0;

        char** words_list;
        if (!(words_list = (char**) malloc(sizeof(char*) * 10)))
                die("cannot malloc %zu bytes:", sizeof(char*) * 10);

        for (int i = 0; t < n; i++) {
                char* word = next_word();
                int length = strlen(word);
                if (length == 0)
                        break;
                if (string_count > 0 && (t + length + string_count) > n) {
                        strcpy(dst, word);
                        break;
                }
                words_list[i] = word;
                string_count++;
                t += strlen(word);
        }

        if (string_count == 1) {
                if (t > n) {
                        char* substring;
                        if (!(substring = (char*) malloc(n + 1)))
                                die("cannot malloc %zu bytes:", n);
                        strncpy(substring, words_list[0], n);
                        strcpy(dst, words_list[0] + n);
                        return substring;
                }
                return words_list[0];
        }

        // do not consider last word.
        // we already loaded that word in dst.
        if (t > n) string_count -= 1;

        char* str_output;
        if (!(str_output = (char*) malloc(n + 1)))
                die("cannot malloc %zu bytes:", n + 1);

        strcpy(str_output, words_list[0]);

        int n_internal_words = string_count - 2;
        int n_tot_spaces = n - t;

        for (int i = 1; i < string_count; i++) {
                double res = (double) n_tot_spaces / (n_internal_words + 1);
                int n_spaces = ceil(res);
                n_tot_spaces -= n_spaces;
                n_internal_words -= 1;
                char* space = strspace(n_spaces);

                strcat(str_output, space);
                strcat(str_output, words_list[i]);
                free(words_list[i]);
        }
        free(words_list);

        return str_output;
}

int main(int argc, char* argv[])
{
        int cols = COLS;
        int col_lines = COL_LINES;
        int col_width = COL_WIDTH;
        int col_gap = COL_GAP;

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
                                col_lines = atoi(optarg);
                                break;
                        case 'w':
                                col_width = atoi(optarg);
                                break;
                        case 'g':
                                // FIXME: use strtol instead of atoi
                                col_gap = atoi(optarg);
                                break;
                        case '?':
                                emit_try_help();
                }
        
        if (col_lines <= 0) emit_invalid_arg("--lines");
        if (col_width <= 0) emit_invalid_arg("--width");
        if (cols <= 0) emit_invalid_arg("--columns");

        ftext(cols, col_lines, col_width, col_gap);

        return EXIT_SUCCESS;
}

