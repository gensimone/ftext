#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>

#define VERSION "0.0.1"
#define VERBOSE 0
#define LINES 20
#define COLUMNS 3
#define WIDTH 21
#define GAP 6

struct option longopts[] = {
        { "verbose",      no_argument,       NULL, 'v' },
        { "help",         no_argument,       NULL, 'h' },
        { "lines",        required_argument, NULL, 'l' },
        { "columns",      required_argument, NULL, 'c' },
        { "width",        required_argument, NULL, 'w' },
        { "gap",          required_argument, NULL, 'g' },
        { NULL,           0,                 NULL,  0  },
};

void help() 
{
        printf("textf (%s)\n", VERSION);
        for (int i = 0; i < sizeof(longopts)/sizeof(longopts[0]) - 1; i++) {
                printf("--%s -%c\n", longopts[i].name, longopts[i].val);
        }
}

int main(int argc, char* argv[])
{
        int verbose = VERBOSE;
        int lines   = LINES;
        int columns = COLUMNS;
        int width   = WIDTH;
        int gap     = GAP;

        int opt;
        while ((opt = getopt_long(argc, argv, "l:c:w:g:vh", longopts, NULL)) != -1) {
                switch (opt) {
                        case 'v':
                                verbose = 1;
                                break;
                        case 'h':
                                help();
                                return EXIT_SUCCESS;
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
                                return EXIT_FAILURE;
                }
        }
        
        /* Check options integrity. */
        if (!lines)   fputs("Argument of --lines must be greater than zero.\n", stderr);
        if (!width)   fputs("Argument of --width must be greater than zero.\n", stderr);
        if (!columns) fputs("Argument of --columns must be greater than zero.\n", stderr);
        if (!lines || !columns || !width) return EXIT_FAILURE;

        return 0;
}
