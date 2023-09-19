
#define DYC_FREE_WORK        \
    {                        \
        GrB_free(&type);     \
        GrB_free(&C);        \
        GrB_free(&R);        \
        GrB_free(&T);        \
        GrB_free(&B1);       \
        GrB_free(&B2);       \
        GrB_free(&OS);       \
    }

#define DYC_FREE_ALL   \
    {                  \
        DYC_FREE_WORK; \
        GrB_free(&O);  \
    }

#include "DyC_Internal.h"

GrB_Info DyC_DecreaseIdempotent(GrB_Matrix* F, const DyC_Semiring semiring,
    const GrB_Matrix A, const GrB_Matrix S, const GrB_Matrix D,
    const GrB_Scalar* X, const GrB_Index nx, const GrB_Index* I,
    const GrB_Index ni, const GrB_Index* J, const GrB_Index nj, char* msg) {
    DYC_CLEAR_MSG;

    GrB_Index nrows, ncols, loop_index;  // dimensions of matrix A, utils for
                                         // loops
    GrB_Type type = NULL;                // type of matrix A
    GrB_Monoid add = NULL;               // addition monoid of semiring
    GrB_Semiring grb_semiring = NULL;
    GrB_UnaryOp star = NULL;
    GrB_BinaryOp addition,
        multiplication = NULL;  // binary operators of semiring
    GrB_Scalar a_val, u_val, new_value = NULL;
    GrB_Matrix C, R, T, O, OS, B1, B2 = NULL;
    GrB_Scalar d_val = NULL;

    bool d_val_bool = false;

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
    DYC_TRY(GrB_Scalar_new(&a_val, type));
    DYC_TRY(GrB_Scalar_new(&u_val, type));
    DYC_TRY(GrB_Scalar_new(&d_val, GrB_BOOL));
    DYC_TRY(GrB_Matrix_new(&C, type, nrows, 1));
    DYC_TRY(GrB_Matrix_new(&R, type, 1, nrows));
    DYC_TRY(GrB_Matrix_new(&OS, type, 1, 1));
    DYC_TRY(GrB_Matrix_new(&T, GrB_BOOL, nrows, nrows));
    DYC_TRY(GrB_Matrix_new(&B1, type, nrows, nrows));
    DYC_TRY(GrB_Matrix_new(&O, type, nrows, nrows));

    // Checks that i,j and x are of the same length
    DYC_ASSERT_MSG(ni == nj && nx == ni && nx == nj, GrB_DIMENSION_MISMATCH,
        "The length of I, J and nvals must be the same.");

    // Compute trust T matrix from D
    DYC_TRY(GrB_Matrix_apply(T, GrB_NULL, GrB_NULL, GrB_LNOT, D, GrB_NULL));

    // Computing B1
    DYC_TRY(GrB_assign(
        B1, T, GrB_NULL, S, GrB_ALL, nrows, GrB_ALL, ncols, GrB_NULL));

    // Computing B2
    DYC_TRY(GrB_Matrix_dup(&B2, A));
    for (int i = 0; i < ni; i++) {
        // Assign u_val to B2
        DYC_TRY(GrB_assign(
            B2, GrB_NULL, GrB_NULL, X[i], &I[i], 1, &J[i], 1, GrB_NULL));
    }

    // Reassign B2 to B2 with mask D
    DYC_TRY(GrB_assign(
        B2, D, GrB_NULL, B2, GrB_ALL, nrows, GrB_ALL, ncols, GrB_NULL));

    // Computing O
    DYC_TRY(GrB_eWiseAdd(O, GrB_NULL, GrB_NULL, addition, B1, B2, GrB_NULL));

    for (int i = 0; i < nrows; i++) {
        loop_index = i;
        // Extracting column and row from matrix F to matrices C and R
        // respectively
        DYC_TRY(GrB_extract(C, GrB_NULL, GrB_NULL, O, GrB_ALL, nrows,
            &loop_index, 1, GrB_NULL));
        DYC_TRY(GrB_extract(R, GrB_NULL, GrB_NULL, O, &loop_index, 1, GrB_ALL,
            nrows, GrB_NULL));

        if (GxB_Matrix_isStoredElement(O, loop_index, loop_index) ==
            GrB_SUCCESS) {
            // Extract O(loop_index,loop_index) in OS
            DYC_TRY(GrB_Matrix_extract(OS, GrB_NULL, GrB_NULL, O, &loop_index,
                1, &loop_index, 1, GrB_NULL));

            // Apply unary operator to OS
            DYC_TRY(GrB_apply(OS, GrB_NULL, GrB_NULL, star, OS, GrB_NULL));

            DYC_TRY(GrB_mxm(C, GrB_NULL, GrB_NULL, grb_semiring, C, OS, GrB_NULL));
        }

        DYC_TRY(GrB_mxm(O, D, addition, grb_semiring, C, R, GrB_NULL));
    }

    // Updating F
    *F = O;

    DYC_FREE_WORK;
    return GrB_SUCCESS;
}