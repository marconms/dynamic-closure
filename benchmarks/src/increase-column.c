#define DYC_FREE_ALL                      \
    {                                     \
        GrB_free(&A);                     \
        GrB_free(&S);                     \
        GrB_free(&F);                     \
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

    char *dataset, *s_dataset, *out_dir = NULL;
    int xnvals = 1;

    int opt;
    while ((opt = getopt(argc, argv, "f:n:s:o:")) != -1) {
        switch (opt) {
            case 'f':
                dataset = optarg;
                break;
            case 'n':
                xnvals = atoi(optarg);
                break;
            case 's':
                s_dataset = optarg;
                break;
            case 'o':
                out_dir = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-fn] [file|number...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    printf(ANSI_COLOR_YELLOW
        "o" ANSI_COLOR_RESET
        " - DynamicClosure: benchmarking DyC_IncreaseColumn.c, Semiring: "
        "MIN PLUS FP64\n"
        "\t - Dataset: %s\n"
        "\t - Number of updates: %d\n",
        dataset, xnvals);

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
    bool closure_run = true;
    double dynamic_closure_wall, closure_wall = 0;

    double wall_start, wall_end = 0;

    GrB_Semiring grb_semiring;
    DyC_Semiring dyc_semiring;
    GrB_Index nodes;
    GrB_UnaryOp star = NULL;
    GrB_Matrix A, S, F = NULL;

    GrB_Index *I, *J;
    GrB_Scalar* X;

    FILE* fd = fopen(dataset, "r");
    if (fd == NULL) {
        printf(ANSI_COLOR_RED "!" ANSI_COLOR_RESET " - Error opening file %s\n",
            fd);
        exit(EXIT_FAILURE);
    }
    LAGRAPH_TRY(LAGraph_MMRead(&A, fd, msg));
    fclose(fd);

    FILE* fsd = fopen(s_dataset, "r");
    if (fsd != NULL) {
        LAGRAPH_TRY(LAGraph_MMRead(&S, fsd, msg));
    }
    fclose(fsd);

    LAGRAPH_TRY(GrB_Matrix_nrows(&nodes, A));

    LAGRAPH_TRY(
        GrB_Semiring_new(&grb_semiring, GrB_MIN_MONOID_FP64, GrB_PLUS_FP64));
    LAGRAPH_TRY(
        DyC_Semiring_new(&dyc_semiring, grb_semiring, DyC_STAR_MIN_PLUS_FP64));

    //--------------------------------------------------------------------------
    // Generating update
    //--------------------------------------------------------------------------

    fprintf(stdout,
        ANSI_COLOR_BLUE "i" ANSI_COLOR_RESET " - Generating update...\n");

    LAGRAPH_TRY(
        DYC_GenerateUpdatesColumn(&X, &I, &J, xnvals, -100, -10, nodes));

    //--------------------------------------------------------------------------
    // Computing closure of A
    //--------------------------------------------------------------------------

    // Check if S is already computed
    if (s_dataset == NULL) {
        printf(ANSI_COLOR_BLUE "i" ANSI_COLOR_RESET
                               " - Computing closure of A...\n");

        wall_start = LAGraph_WallClockTime();

        LAGRAPH_TRY(DyC_GeneralizedFloydWarshall(&S, dyc_semiring, A, msg));

        wall_end = LAGraph_WallClockTime();
        closure_wall = wall_end - wall_start;

    } else {
        closure_run = false;
        printf(ANSI_COLOR_BLUE
            "i" ANSI_COLOR_RESET
            " - Closure of A already computed. Skipping step...\n");
    }

    //--------------------------------------------------------------------------
    // Benchmark start
    //--------------------------------------------------------------------------

    printf(ANSI_COLOR_BLUE "i" ANSI_COLOR_RESET
                           " - Computing dynamic closure...\n");

    wall_start = LAGraph_WallClockTime();

    //--------------------------------------------------------------------------
    // Dynamic closure
    //--------------------------------------------------------------------------

    LAGRAPH_TRY(DyC_IncreaseColumn(
        &F, dyc_semiring, A, S, X, xnvals, I, xnvals, J, xnvals, msg));

    //--------------------------------------------------------------------------
    // Benchmark end
    //--------------------------------------------------------------------------

    wall_end = LAGraph_WallClockTime();
    dynamic_closure_wall = wall_end - wall_start;

    //--------------------------------------------------------------------------
    // Write results to file
    //--------------------------------------------------------------------------

    if (out_dir != NULL) {
        printf(ANSI_COLOR_BLUE "i" ANSI_COLOR_RESET " - Saving output...\n");

        DYC_MakeOutDir(out_dir);
        char* filename;

        // Writing matrices to file
        asprintf(&filename, "%s/S.txt", out_dir);
        DYC_MMPrint(S, filename, out_dir);

        asprintf(&filename, "%s/F.txt", out_dir);
        DYC_MMPrint(F, filename, out_dir);

        // Writing analytics to file
        DYC_AnalyticsWrite(
            "matrix_name,algorithm,has_run,wall_time\n", out_dir);
        char* content;
        asprintf(&content, "%s,DyC_GeneralizedFloydWarshall,%d,%f\n", "S",
            closure_run, closure_wall);
        DYC_AnalyticsWrite(content, out_dir);
        asprintf(&content, "%s,DyC_IncreaseColumn,%d,%f\n", "F", true,
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