/* See LICENSE file for copyright and license details. */
#include "core.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#define PROGRAM_NAME "ftext"
#define VERSION "0.0.1"
#define COLS 3
#define LINES 24
#define WIDTH 21
#define GAP 6

void emit_invalid_arg(char* opt);
void emit_try_help(void);
void emit_version(void);
void usage(void);

/* available options */
static struct option const longopts[] = {
    {"version", no_argument, NULL, 'v'},
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
    printf("  --width   -w    number of characters on a row of a column "
           "(default "
           "%d)\n",
           WIDTH);
    printf("  --gap     -l    number of spaces between columns (default %d)\n", GAP);
    puts("  --help    -h    display this help and exit");
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

int main(int argc, char* argv[])
{
    int cols = COLS;
    int lines = LINES;
    int width = WIDTH;
    int gap = GAP;

    int opt;
    // FIXME: use strol instead of atoi
    while ((opt = getopt_long(argc, argv, "c:l:w:g:vhd", longopts, NULL)) != -1)
        switch (opt) {
        case 'v':
            emit_version();
        case 'h':
            usage();
        case 'c':
            cols = strtol(optarg, NULL, 10);
            break;
        case 'l':
            lines = atoi(optarg);
            break;
        case 'w':
            width = atoi(optarg);
            break;
        case 'g':
            gap = atoi(optarg);
            break;
        case '?':
            emit_try_help();
        }
    if (cols <= 0)
        emit_invalid_arg("--columns");
    if (lines <= 0)
        emit_invalid_arg("--lines");
    if (width <= 0)
        emit_invalid_arg("--width");

    ftext(cols, lines, width, gap);
}
