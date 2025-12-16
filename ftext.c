#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>

#define PROGRAM_NAME "ftext"
#define VERSION "0.0.1"
#define VERBOSE 0
#define LINES 20
#define COLUMNS 3
#define WIDTH 21
#define GAP 6

/* function declarations */
void emit_version(void);
void emit_invalid_arg(char* opt);
void emit_try_help(void);
void usage(void);

static struct option const longopts[] = 
{
        {"version", no_argument, NULL, 'v'},
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
        printf("  --columns -c    number of columns (default %d)\n", COLUMNS); 
        printf("  --lines   -l    number of lines (default %d)\n", LINES); 
        printf("  --gap     -l    number of spaces between columns (default %d)\n", GAP); 
        printf("  --width   -w    number of characters on a row of a column (default %d)\n", WIDTH);
        puts("  --help    -h    display this help and exit");
        puts("  --version -v    output version information and exit");

        exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
        int verbose = VERBOSE;
        int lines = LINES;
        int columns = COLUMNS;
        int width = WIDTH;
        int gap = GAP;

        int opt;
        while ((opt = getopt_long(argc, argv, "l:c:w:g:vh", longopts, NULL)) != -1)
                switch (opt) 
                {
                        case 'v':
                                emit_version();
                        case 'h':
                                usage();
                        case 'l':
                                lines = atoi(optarg);
                                break;
                        case 'c':
                                columns = atoi(optarg);
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
        
        if (lines <= 0)   emit_invalid_arg("--lines");
        if (width <= 0)   emit_invalid_arg("--width");
        if (columns <= 0) emit_invalid_arg("--columns");

        return EXIT_SUCCESS;
}
