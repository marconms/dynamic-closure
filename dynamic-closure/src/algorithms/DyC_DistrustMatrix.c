#define DYC_FREE_WORK         \
    {                         \
        GrB_free(&type);      \
        GrB_free(&C);         \
        GrB_free(&R);         \
        GrB_free(&T);         \
        GrB_free(&a_val);     \
        GrB_free(&new_value); \
    }

#define DYC_FREE_ALL   \
    {                  \
        DYC_FREE_WORK; \
        GrB_free(&O);  \
    }

#include "DyC_Internal.h"

GrB_Info DyC_DistrustMatrix(GrB_Matrix* F, const DyC_Semiring semiring,
 const GrB_BinaryOp comparison,
    const GrB_Matrix A, const GrB_Matrix S, const GrB_Index* I,
    const GrB_Index ni, const GrB_Index* J, const GrB_Index nj, char* msg) {
    DYC_CLEAR_MSG;

    GrB_Index nrows, ncols;  // dimensions of matrix A
    GrB_Type type = NULL;    // type of matrix A
    GrB_Monoid add = NULL;   // addition monoid of semiring
    GrB_Semiring grb_semiring = NULL;
    GrB_BinaryOp addition,
        multiplication = NULL;  // binary operators of semiring
    GrB_Scalar a_val, new_value = NULL;
    GrB_Matrix C, R, T, O = NULL;
    GrB_Vector AD = NULL;  // diagonal of A

    // Getting information of A
    DYC_TRY(GrB_Matrix_nrows(&nrows, A));
    DYC_TRY(GrB_Matrix_ncols(&ncols, A));
    DYC_TRY(GxB_Matrix_type(&type, A));

    // Getting grb semiring and components
    DYC_TRY(DyC_Semiring_semiring(&grb_semiring, semiring));
    DYC_TRY(DyC_Semiring_add(&add, semiring));
    DYC_TRY(DyC_Semiring_multiply(&multiplication, semiring));
    DYC_TRY(GxB_Monoid_operator(&addition, add));

    // Initializing utils
    DYC_TRY(GrB_Scalar_new(&a_val, type));
    DYC_TRY(GrB_Scalar_new(&new_value, type));

    DYC_TRY(GrB_Matrix_new(&C, type, nrows, 1));
    DYC_TRY(GrB_Matrix_new(&R, type, 1, nrows));
    DYC_TRY(GrB_Matrix_new(&T, type, nrows, nrows));
    DYC_TRY(GrB_Matrix_new(&O, GrB_BOOL, nrows, nrows));

    // Checks that i,j and nvals are of the same length
    DYC_ASSERT_MSG(ni == nj, GrB_DIMENSION_MISMATCH,
        "The length of I, J and nvals must be the same.");

    for (int i = 0; i < ni; i++) {
        // Extracting column and row from matrix A to matrices C and R
        // respectively

        DYC_TRY(GrB_extract(
            C, GrB_NULL, GrB_NULL, S, GrB_ALL, nrows, &I[i], 1, GrB_NULL));
        DYC_TRY(GrB_extract(
            R, GrB_NULL, GrB_NULL, S, &J[i], 1, GrB_ALL, ncols, GrB_NULL));

        if (GxB_Matrix_isStoredElement(A, I[i], J[i]) == GrB_SUCCESS) {
            DYC_TRY(GrB_Matrix_extractElement(a_val, A, I[i], J[i]));
            DYC_TRY(GrB_apply(
                C, GrB_NULL, GrB_NULL, multiplication, a_val, C, GrB_NULL));
        }

        DYC_TRY(GrB_mxm(T, GrB_NULL, GrB_NULL, grb_semiring, C, R, GrB_NULL));

        DYC_TRY(
            GrB_eWiseMult(O, GrB_NULL, GrB_LOR, comparison, S, T, GrB_NULL));
    }

    // Drop all zero entries
    GxB_select(O, NULL, NULL, GxB_NONZERO, O, NULL, NULL) ;

    // Updating F
    *F = O;

    DYC_FREE_WORK;
    return GrB_SUCCESS;
}