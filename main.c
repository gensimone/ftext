#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>


struct option longopts[] = {
        { "verbose", no_argument,       NULL, 'v' },
        { "help",    no_argument,       NULL, 'h' },
        { "debug",   no_argument,       NULL, 'd' },
        { "input",   required_argument, NULL, 'i' },
        { "output",  required_argument, NULL, 'o' },
        { NULL,      0,                 NULL,  0 },
};

void help() 
{
        // TODO: make dinamic version
        puts("textf (0.0.1)");
        // Iterate over longopts and generate the Help message.
        for (int i = 0; i < sizeof(longopts)/sizeof(longopts[0]); i++) {
                printf("--%s -%c\n", longopts[i].name, longopts[i].val);
        }
}

int main(int argc, char *argv[]) 
{
        char *input = NULL;
        char *output = NULL;
        int debug = 0;
        int verbose = 0;
        int opt;

        while ((opt = getopt_long(argc, argv, "vhi:o:", longopts, NULL)) != -1) {
                switch (opt) {
                        case 'v':
                                verbose = 1;
                                break;
                        case 'h':
                                help();
                                return EXIT_SUCCESS;
                        case 'i':
                                input = optarg;
                                break;
                        case 'o':
                                output = optarg;
                                break;
                        case 'd':
                                debug = 1;
                                break;
                        case '?':
                                return EXIT_FAILURE;
                }
        }

        if (input == NULL || output == NULL) {
                dprintf(2, "Missing required option.\n");
                return EXIT_FAILURE;
        }

        FILE *finput =  fopen(input,  "r");
        if (finput == NULL) {
                dprintf(2, "Cannot open file: %s\n", input);
                dprintf(2, "Errno: %d\n", errno);
                return EXIT_FAILURE;
        }

        FILE *foutput = fopen(output, "w");
        if (foutput == NULL) {
                dprintf(2, "Cannot open file: %s\n", input);
                dprintf(2, "Errno: %d\n", errno);
                return EXIT_FAILURE;
        }

        fclose(finput);
        fclose(foutput);

        return 0;
}

