#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>

#define VERSION "0.0.1"

/* Format default values. */
#define LINES 20
#define COLUMNS 3
#define WIDHT 21
#define GAP 6
#define MPROC 0

struct option longopts[] = {
        { "verbose",      no_argument,       NULL, 'v' },
        { "help",         no_argument,       NULL, 'h' },
        { "debug",        no_argument,       NULL, 'd' },
        { "input",        required_argument, NULL, 'i' },
        { "output",       required_argument, NULL, 'o' },
        { "lines",        required_argument, NULL, 'l' },
        { "columns",      required_argument, NULL, 'c' },
        { "width",        required_argument, NULL, 'w' },
        { "gap",          required_argument, NULL, 'g' },
        { "multiprocess", required_argument, NULL, 'm' },
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
        int verbose = 0;
        int debug = 0;
        char* input = NULL;
        char* output = NULL;
        int lines = LINES;
        int columns = COLUMNS;
        int width = WIDHT;
        int gap = GAP;
        int mproc = MPROC;

        int opt;
        while ((opt = getopt_long(argc, argv, "vhi:o:", longopts, NULL)) != -1) {
                switch (opt) {
                        case 'v':
                                verbose = 1;
                                break;
                        case 'h':
                                help();
                                return EXIT_SUCCESS;
                        case 'd':
                                debug = 1;
                                break;
                        case 'i':
                                input = optarg;
                                break;
                        case 'o':
                                output = optarg;
                                break;
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
                        case 'm':
                                mproc = 1;
                                break;
                        case '?':
                                return EXIT_FAILURE;
                }
        }
        
        /* Check format options. */

        if (!lines)   fputs("lines cannot be zero.",   stderr);
        if (!columns) fputs("columns cannot be zero.", stderr);
        if (!width)   fputs("width cannot be zero.",   stderr);

        if (!lines || !columns || !width) return EXIT_FAILURE;

        /* Check IO files. */

        if (input == NULL || output == NULL) {
                fprintf(stderr, "Missing --input or --output options.\n");
                return EXIT_FAILURE;
        }

        FILE* finput =  fopen(input,  "r");
        if (!finput) {
                perror("fopen");
                return EXIT_FAILURE;
        }

        FILE* foutput = fopen(output, "w");
        if (!foutput) {
                perror("fopen");
                return EXIT_FAILURE;
        }

        if (debug) {
                printf("ftext (%s)\n", VERSION);
                printf("input:   %s\n", input);
                printf("output:  %s\n", output);
                printf("lines:   %d\n", lines);
                printf("columns: %d\n", columns);
                printf("width:   %d\n", width);
                printf("gap:     %d\n", gap);
                printf("mproc:   %d\n", mproc);
        }

        fclose(finput);
        fclose(foutput);

        return 0;
}
