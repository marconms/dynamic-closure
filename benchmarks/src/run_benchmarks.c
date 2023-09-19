#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "DyC_Benchmarks.h"
#include "DynamicClosure.h"

int main(int argc, char* argv[]) {
    //--------------------------------------------------------------------------
    // Parsing arguments
    //--------------------------------------------------------------------------

    char *dataset, *s_dataset, *out_dir = NULL;

    int opt;
    while ((opt = getopt(argc, argv, "f:o:s:")) != -1) {
        switch (opt) {
            case 'f':
                dataset = optarg;
                break;
            case 'o':
                out_dir = optarg;
                break;
            case 's':
                s_dataset = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-fn] [file|number...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (out_dir == NULL) {
        printf(ANSI_COLOR_RED "!" ANSI_COLOR_RESET
                              " - Output directory not specified\n");
        exit(EXIT_FAILURE);
    }
    if (dataset == NULL) {
        printf(
            ANSI_COLOR_RED "!" ANSI_COLOR_RESET " - Dataset not specified\n");
        exit(EXIT_FAILURE);
    }

    //--------------------------------------------------------------------------
    // Setup
    //--------------------------------------------------------------------------

    char* cleaned_dataset_name;
    char* specific_out_dir;
    char* closure;
    char* command;
    int status;
    int xnvals;

    // Remove path and extension from dataset name
    cleaned_dataset_name = DYC_CleanFilename(dataset);

    // Create out dir
    DYC_MakeOutDir(out_dir);

    FILE* fsd = fopen(s_dataset, "r");

    //--------------------------------------------------------------------------
    // Running floyd warshall
    //--------------------------------------------------------------------------

    if (fsd == NULL) {
        asprintf(&specific_out_dir, "%s/floyd-warshall_%s", out_dir,
            cleaned_dataset_name);
        asprintf(&command, "./generalized-floyd-warshall -f %s -o %s", dataset,
            specific_out_dir);
        status = system(command);
        if (status != 0) {
            printf(ANSI_COLOR_RED "!" ANSI_COLOR_RESET
                                " - Error running benchmark "
                                "generalized-floyd-warshall, status %d\n",
                status);
            exit(EXIT_FAILURE);
        }

        asprintf(&closure, "%s/F.mtx", specific_out_dir);
    } else {
        asprintf(&closure, "%s", s_dataset);
    }

    //--------------------------------------------------------------------------
    // Decrease idempotent
    //--------------------------------------------------------------------------

    for (int k = 0, xnvals = 10; k < 3; k++, xnvals *= 10) {
        asprintf(&specific_out_dir, "%s/decrease-idempotent_%s_%d", out_dir,
            cleaned_dataset_name, xnvals);
        asprintf(&command, "./decrease-idempotent -f %s -s %s -n %d -o %s",
            dataset, closure, xnvals, specific_out_dir);
        status = system(command);
        if (status != 0) {
            printf(ANSI_COLOR_RED "!" ANSI_COLOR_RESET
                                  " - Error running benchmark "
                                  "decrease-idempotent, status %d\n",
                status);
            exit(EXIT_FAILURE);
        }
    }

    //--------------------------------------------------------------------------
    // Increase column
    //--------------------------------------------------------------------------

    for (int k = 0, xnvals = 10; k < 3; k++, xnvals *= 10) {
        asprintf(&specific_out_dir, "%s/increase-column_%s_%d", out_dir,
            cleaned_dataset_name, xnvals);
        asprintf(&command, "./increase-column -f %s -s %s -n %d -o %s", dataset,
            closure, xnvals, specific_out_dir);
        status = system(command);
        if (status != 0) {
            printf(ANSI_COLOR_RED "!" ANSI_COLOR_RESET
                                  " - Error running benchmark "
                                  "increase-column, status %d\n",
                status);
            exit(EXIT_FAILURE);
        }
    }

    //--------------------------------------------------------------------------
    // Increase Row
    //--------------------------------------------------------------------------

    for (int k = 0, xnvals = 10; k < 3; k++, xnvals *= 10) {
        asprintf(&specific_out_dir, "%s/increase-row_%s_%d", out_dir,
            cleaned_dataset_name, xnvals);
        asprintf(&command, "./increase-row -f %s -s %s -n %d -o %s", dataset,
            closure, xnvals, specific_out_dir);
        status = system(command);
        if (status != 0) {
            printf(ANSI_COLOR_RED "!" ANSI_COLOR_RESET
                                  " - Error running benchmark "
                                  "increase-row, status %d\n",
                status);
            exit(EXIT_FAILURE);
        }
    }
}