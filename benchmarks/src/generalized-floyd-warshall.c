#define DYC_FREE_ALL                      \
    {                                     \
        GrB_free(&A);                     \
        GrB_free(&F);                     \
        GrB_free(&u_val);                 \
        DyC_Semiring_free(&dyc_semiring); \
    }

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

    char *dataset, *out_dir;

    int opt;
    while ((opt = getopt(argc, argv, "f:o:")) != -1) {
        switch (opt) {
            case 'f':
                dataset = optarg;
                break;
            case 'o':
                out_dir = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-f] [file...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    printf(ANSI_COLOR_YELLOW "o" ANSI_COLOR_RESET
                             " - DynamicClosure: benchmarking "
                             "DyC_GeneralizedFloydWarchall.c, Semiring: "
                             "MIN PLUS FP64\n"
                             "\t - Dataset: %s\n",
        dataset);

    //--------------------------------------------------------------------------
    // Initialization
    //--------------------------------------------------------------------------

    char msg[LAGRAPH_MSG_LEN];
    LAGraph_Init(NULL);
    DyC_Init(NULL);

    //--------------------------------------------------------------------------
    // Setup
    //--------------------------------------------------------------------------

    // Analytics
    double dynamic_closure_wall = 0;

    GrB_Semiring grb_semiring;
    DyC_Semiring dyc_semiring;
    GrB_Index u_row, u_col, nodes;  // dimensions of matrix
    GrB_UnaryOp star = NULL;
    GrB_Matrix A, S, F = NULL;
    GrB_Scalar u_val = NULL;

    FILE* fd = fopen(dataset, "r");
    if (fd == NULL) {
        printf(ANSI_COLOR_RED "!" ANSI_COLOR_RESET " - Error opening file %s\n",
            fd);
        exit(EXIT_FAILURE);
    }
    LAGRAPH_TRY(LAGraph_MMRead(&A, fd, msg));
    fclose(fd);

    LAGRAPH_TRY(GrB_Matrix_nrows(&nodes, A));

    LAGRAPH_TRY(
        GrB_Semiring_new(&grb_semiring, GrB_MIN_MONOID_FP64, GrB_PLUS_FP64));
    LAGRAPH_TRY(
        DyC_Semiring_new(&dyc_semiring, grb_semiring, DyC_STAR_MIN_PLUS_FP64));

    //--------------------------------------------------------------------------
    // Benchmark start
    //--------------------------------------------------------------------------

    printf(ANSI_COLOR_BLUE
        "i" ANSI_COLOR_RESET
        " - Computing closure...\n");  // TODO: move before benchmark start

    double wall_start = LAGraph_WallClockTime();

    //--------------------------------------------------------------------------
    // Closure of A
    //--------------------------------------------------------------------------

    LAGRAPH_TRY(DyC_GeneralizedFloydWarshall(&F, dyc_semiring, A, msg));

    //--------------------------------------------------------------------------
    // Benchmark end
    //--------------------------------------------------------------------------

    double wall_end = LAGraph_WallClockTime();
    dynamic_closure_wall = wall_end - wall_start;

    //--------------------------------------------------------------------------
    // Write results to file
    //--------------------------------------------------------------------------

    if (out_dir != NULL) {
        printf(ANSI_COLOR_BLUE "i" ANSI_COLOR_RESET " - Saving output...\n");

        DYC_MakeOutDir(out_dir);
        char* filename;

        // Writing matrices to file
        asprintf(&filename, "%s/F.mtx", out_dir);
        DYC_MMWrite(F, filename, out_dir);

        // Writing analytics to file
        DYC_AnalyticsWrite(
            "matrix_name,algorithm,has_run,wall_time\n", out_dir);
        char* content;
        asprintf(&content, "%s,DyC_GeneralizedFloydWarshall,%d,%f\n", "F", true,
            dynamic_closure_wall);
        DYC_AnalyticsWrite(content, out_dir);
    }

    //--------------------------------------------------------------------------
    // Cleanup
    //--------------------------------------------------------------------------

    printf(ANSI_COLOR_GREEN "o" ANSI_COLOR_RESET
                            " - Benchmark completed. Cleaning up.\n");

    DYC_FREE_ALL;
    DyC_Finalize(NULL);
    LAGraph_Finalize(NULL);
}