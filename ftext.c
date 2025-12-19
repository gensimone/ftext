#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>

#define PROGRAM_NAME "ftext"
#define VERSION "0.0.1"

#define COLS 3
#define COL_LINES 24
#define COL_WIDTH 21
#define COL_GAP 6

#define NEW_PAGE "\n %%%\n\n"

int debug = 0;

/* function declarations */
void emit_version(void);
void emit_invalid_arg(char* opt);
void emit_try_help(void);
void usage(void);
char** ftext(int cols, int col_lines, int col_gap, int col_width);

static struct option const longopts[] = 
{
        {"version", no_argument, NULL, 'v'},
        {"debug", no_argument, NULL, 'd'},
        {"help", no_argument, NULL, 'h'},
        {"lines", required_argument, NULL, 'l'},
        {"columns", required_argument, NULL, 'c'},
        {"width", required_argument, NULL, 'w'},
        {"gap", required_argument, NULL, 'g'},
        {NULL, 0, NULL, 0},
};

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

char** palloc(int lines, int line_length) 
{
        char** page = (char**) malloc(lines * sizeof(char*));
        if (!page) 
                return NULL;

        for (int i = 0; i < lines; i++) {
                char* line = (char*) malloc((1 + line_length) * sizeof(char));
                if (!line)
                        return NULL;
                page[i] = line;
        }

        return page;
}

int fpage(char** page, int cols, int col_lines, int col_width, int col_gap) 
{
        for (int col = 0; col < cols; col++) {
                int col_alignment = col * (col_width + col_gap);
                for (int y = 0; y < col_lines; y++) {
                        int x;
                        for (int rx = 0; rx < col_width; rx++) { 
                                /* SAMPLE:               */
                                /* Unicode       (UTF-8) */
                                /* contenente  un  testo */
                                /* in           italiano */
                                /* strutturato        in */
                                x = rx + col_alignment;      
                                char c = getchar();
                                if (c == EOF) return 1;
                                // Substitute control characters.
                                if (c < 32) c = ' ';
                                page[y][x] = c;
                        }
                        // In gaps, substitute '\0' with a whitespace.
                        for (int i = x + 1; i < x + col_gap + 1; i++)
                                page[y][i] = ' ';
                }
        }
        return 0;
}

char** ftext(int cols, int col_lines, int col_width, int col_gap) 
{
        int width_page_line = col_width * cols + (cols - 1) * col_gap;

        for (;;) {
                char** page = palloc(col_lines, width_page_line);
                if (!page) {
                        perror("palloc");
                        exit(EXIT_FAILURE);
                }

                int finished = fpage(page, cols, col_lines, col_width, col_gap);

                for (int y =0; y < col_lines; y++) {
                        printf("%s\n", page[y]);
                }

                free(page);

                if (finished) break; 
                else printf("%s", NEW_PAGE);
        }
}
