//------------------------------------------------------------------------------
// LAGRAPH_CATCH: catch an error from a LAGraph/GraphBLAS method
//------------------------------------------------------------------------------

#define LAGRAPH_CATCH(status)                                                 \
    {                                                                         \
        printf("LAGraph failure (file %s, line %d): status: %d, message: %s", \
            __FILE__, __LINE__, status, msg);                                 \
        DYC_FREE_ALL;                                                         \
        return (status);                                                      \
    }

//------------------------------------------------------------------------------
// External headers
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "LAGraph.h"

//------------------------------------------------------------------------------
// ANSI colors
//------------------------------------------------------------------------------

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

//------------------------------------------------------------------------------
// DYC_FREE_ALL: free all workspace and all output arguments, on error
//------------------------------------------------------------------------------

#ifndef DYC_FREE_ALL
#define DYC_FREE_ALL ;
#endif

//------------------------------------------------------------------------------
// DYC_GenerateUpdates: generate randomic updates on a matrix
//------------------------------------------------------------------------------

static inline GrB_Info DYC_GenerateUpdates(GrB_Scalar** X, GrB_Index** I,
    GrB_Index** J, const GrB_Index nvals, const int xmin,
    const int xmax, const int ijmax) {
    GrB_Index* i = (GrB_Index*)malloc(nvals * sizeof(GrB_Index));
    GrB_Index* j = (GrB_Index*)malloc(nvals * sizeof(GrB_Index));
    GrB_Scalar* x = (GrB_Scalar*)malloc(nvals * sizeof(GrB_Scalar));

    // Initialize random number generator
    srand(time(NULL));

    double range = (double)(xmax - xmin);
    double div = RAND_MAX / range;

    for (int k = 0; k < nvals; k++) {
        i[k] = rand() % (ijmax - 1);
        j[k] = rand() % (ijmax - 1);

        if (i[k] == j[k]) {
            k--;
            continue;
        }

        GrB_Scalar_new(&x[k], GrB_FP64);
        GrB_Scalar_setElement_FP64(x[k], xmin + (rand() / div));
    }

    *I = i;
    *J = j;
    *X = x;

    return GrB_SUCCESS;
}

//------------------------------------------------------------------------------
// DYC_GenerateUpdatesColumn: generate randomic updates on a column
//------------------------------------------------------------------------------

static inline GrB_Info DYC_GenerateUpdatesColumn(GrB_Scalar** X, GrB_Index** I,
    GrB_Index** J, const GrB_Index nvals, const int xmin,
    const int xmax, const int ijmax) {
    GrB_Index* i = (GrB_Index*)malloc(nvals * sizeof(GrB_Index));
    GrB_Index* j = (GrB_Index*)malloc(nvals * sizeof(GrB_Index));
    GrB_Scalar* x = (GrB_Scalar*)malloc(nvals * sizeof(GrB_Scalar));

    // Initialize random number generator
    srand(time(NULL));

    int column = rand() % (ijmax - 1);

    double range = (double)(xmax - xmin);
    double div = RAND_MAX / range;

    for (int k = 0; k < nvals; k++) {
        i[k] = rand() % ijmax;
        j[k] = column;

        if (i[k] == j[k]) {
            k--;
            continue;
        }

        GrB_Scalar_new(&x[k], GrB_FP64);
        GrB_Scalar_setElement_FP64(x[k], xmin + (rand() / div));
    }

    *I = i;
    *J = j;
    *X = x;

    return GrB_SUCCESS;
}

//------------------------------------------------------------------------------
// DYC_GenerateUpdatesRow: generate randomic updates on a column
//------------------------------------------------------------------------------

static inline GrB_Info DYC_GenerateUpdatesRow(GrB_Scalar** X, GrB_Index** I,
    GrB_Index** J, const GrB_Index nvals, const int xmin,
    const int xmax, const int ijmax) {
    GrB_Index* i = (GrB_Index*)malloc(nvals * sizeof(GrB_Index));
    GrB_Index* j = (GrB_Index*)malloc(nvals * sizeof(GrB_Index));
    GrB_Scalar* x = (GrB_Scalar*)malloc(nvals * sizeof(GrB_Scalar));

    // Initialize random number generator
    srand(time(NULL));

    int row = rand() % (ijmax - 1);

    double range = (double)(xmax - xmin);
    double div = RAND_MAX / range;

    for (int k = 0; k < nvals; k++) {
        i[k] = row;
        j[k] = rand() % ijmax;

        if (i[k] == j[k]) {
            k--;
            continue;
        }

        GrB_Scalar_new(&x[k], GrB_FP64);
        GrB_Scalar_setElement_FP64(x[k], xmin + (rand() / div));
    }

    *I = i;
    *J = j;
    *X = x;

    return GrB_SUCCESS;
}

//------------------------------------------------------------------------------
// DYC_MakeOutDir: generate output directory for abenchmark
//------------------------------------------------------------------------------

static inline void DYC_MakeOutDir(char* out) {
    struct stat st = {0};
    if (stat(out, &st) == -1) {
        mkdir(out, 0700);
    }
}

//------------------------------------------------------------------------------
// DYC_MMWrite: write matrix to file in Matrix Market format
//------------------------------------------------------------------------------

static inline void DYC_MMWrite(GrB_Matrix A, char* filename, char* out) {
    FILE* f = fopen(filename, "ab+");
    if (f == NULL) {
        printf(ANSI_COLOR_RED "!" ANSI_COLOR_RESET " - Error opening file %s\n",
            filename);
        exit(EXIT_FAILURE);
    }
    LAGraph_MMWrite(A, f, NULL, NULL);
    fclose(f);
}

//------------------------------------------------------------------------------
// DYC_MMPrint: write matrix info to file
//------------------------------------------------------------------------------

static inline void DYC_MMPrint(GrB_Matrix A, char* filename, char* out) {
    FILE* f = fopen(filename, "ab+");
    if (f == NULL) {
        printf(ANSI_COLOR_RED "!" ANSI_COLOR_RESET " - Error opening file %s\n",
            filename);
        exit(EXIT_FAILURE);
    }
    LAGraph_Matrix_Print(A, LAGraph_SHORT_VERBOSE, f, NULL);
    fclose(f);
}

//------------------------------------------------------------------------------
// DYC_AnalyticsWrite: write analytics to file
//------------------------------------------------------------------------------

static inline void DYC_AnalyticsWrite(char* content, char* out) {
    char* filename = malloc(strlen(out) + strlen("/analytics.csv"));
    asprintf(&filename, "%s/analytics.csv", out);

    FILE* f = fopen(filename, "ab+");
    if (f == NULL) {
        printf(ANSI_COLOR_RED "!" ANSI_COLOR_RESET " - Error opening file %s\n",
            filename);
        exit(EXIT_FAILURE);
    }
    fprintf(f, "%s", content);
    fclose(f);
}

//------------------------------------------------------------------------------
// DYC_CleanFilename: removes path and extension from a filename
//------------------------------------------------------------------------------

static inline char* DYC_CleanFilename(char* filename) {
    char* cleaned_filename = malloc(strlen(filename));
    strcpy(cleaned_filename, filename);

    char* token = strtok(cleaned_filename, "/");
    while (token != NULL) {
        cleaned_filename = token;
        token = strtok(NULL, "/");
    }

    char* dot = strrchr(cleaned_filename, '.');
    if (dot != NULL) {
        *dot = '\0';
    }

    return cleaned_filename;
}
