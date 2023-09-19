#define DYC_FREE_WORK     \
    {                     \
        GrB_free(&type);  \
        GrB_free(&C);     \
        GrB_free(&R);     \
        GrB_free(&T);     \
        GrB_free(&OS);    \
    }

#define DYC_FREE_ALL   \
    {                  \
        DYC_FREE_WORK; \
        GrB_free(&O);  \
    }

#include "DyC_Internal.h"

GrB_Info DyC_GeneralizedFloydWarshall(
    GrB_Matrix* F, const DyC_Semiring semiring, const GrB_Matrix A, char* msg) {
    DYC_CLEAR_MSG;

    GrB_Index loop_index, nrows, ncols, u_row, u_col;  // dimensions of matrix
                                                       // A, utils for loops
    GrB_Type type = NULL;   // type of matrix A
    GrB_Monoid add = NULL;  // addition monoid of semiring
    GrB_Semiring grb_semiring = NULL;
    GrB_UnaryOp star = NULL;
    GrB_BinaryOp addition,
        multiplication = NULL;  // binary operators of semiringiagonal of A
    GrB_Matrix O, C, R, OS,
        T;  // matrices for storing intermediate values and results

    // Getting information of A
    DYC_TRY(GrB_Matrix_nrows(&nrows, A));
    DYC_TRY(GrB_Matrix_ncols(&ncols, A));
    DYC_TRY(GxB_Matrix_type(&type, A));

    // Getting components of semiring
    DYC_TRY(DyC_Semiring_semiring(&grb_semiring, semiring));
    DYC_TRY(DyC_Semiring_add(&add, semiring));
    DYC_TRY(GxB_Monoid_operator(&addition, add));
    DYC_TRY(DyC_Semiring_multiply(&multiplication, semiring));
    DYC_TRY(DyC_Semiring_star(&star, semiring));

    // Initializing utils
    DYC_TRY(GrB_Matrix_new(&C, type, nrows, 1));
    DYC_TRY(GrB_Matrix_new(&R, type, 1, nrows));
    DYC_TRY(GrB_Matrix_new(&T, GrB_BOOL, nrows, nrows));
    DYC_TRY(GrB_Matrix_new(&OS, type, 1, 1));
    DYC_TRY(GrB_Matrix_new(&O, type, nrows, nrows));

    // Checking that A is square
    DYC_ASSERT_MSG(
        nrows == ncols, GrB_DIMENSION_MISMATCH, "The matrix A must be square");

    // Copying A to O
    DYC_TRY(GrB_Matrix_dup(&O, A));

    for (int i = 0; i < nrows; i++) {
        loop_index = i;

        // Extracting column and row from matrix A to matrices C and R
        // respectively
        DYC_TRY(GrB_extract(C, GrB_NULL, GrB_NULL, O, GrB_ALL, nrows,
            &loop_index, 1, GrB_NULL));
        DYC_TRY(GrB_extract(R, GrB_NULL, GrB_NULL, O, &loop_index, 1, GrB_ALL,
            nrows, GrB_NULL));

        DYC_TRY(GrB_mxm(T, GrB_NULL, addition, grb_semiring, C, R, GrB_NULL));
        if (GxB_Matrix_isStoredElement(O, loop_index, loop_index) ==
            GrB_SUCCESS) {
            // Extract O(loop_index,loop_index) in OS
            DYC_TRY(GrB_Matrix_extract(OS, GrB_NULL, GrB_NULL, O, &loop_index,
                1, &loop_index, 1, GrB_NULL));

            // Apply unary operator to OS
            DYC_TRY(GrB_apply(OS, GrB_NULL, GrB_NULL, star, OS, GrB_NULL));

            DYC_TRY(GrB_mxm(C, GrB_NULL, GrB_NULL, grb_semiring, C, OS, GrB_NULL));
        }

        DYC_TRY(GrB_mxm(O, GrB_NULL, addition, grb_semiring, C, R, GrB_NULL));
    }

    // Updating F
    *F = O;

    DYC_FREE_WORK;
    return GrB_SUCCESS;
}