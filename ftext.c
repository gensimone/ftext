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
        printf("%s %s", PROGRAM_NAME, VERSION);
        exit(EXIT_SUCCESS);
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
        printf("  --help    -h    display this help and exit");
        printf("  --version -v    output version information and exit\n");

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
                                gap = atoi(optarg);
                                break;
                        case '?':
                                emit_try_help();
                }
        
        if (!lines)   
                fputs("Argument of --lines must be greater than zero.\n", stderr);
        if (!width)   
                fputs("Argument of --width must be greater than zero.\n", stderr);
        if (!columns) 
                fputs("Argument of --columns must be greater than zero.\n", stderr);
        if (!lines || !columns || !width) 
                return EXIT_FAILURE;

        return 0;
}
