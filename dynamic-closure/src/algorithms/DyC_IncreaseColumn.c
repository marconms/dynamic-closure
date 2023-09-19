#define DYC_FREE_WORK     \
    {                     \
        GrB_free(&type);  \
        GrB_free(&V);     \
        GrB_free(&T1);     \
         GrB_free(&T2);     \
          GrB_free(&T3);     \
        GrB_free(&M);     \
        GrB_free(&SR);    \
        GrB_free(&VC);    \
        GrB_free(&t_val); \
    }

#define DYC_FREE_ALL   \
    {                  \
        DYC_FREE_WORK; \
        GrB_free(&O);  \
    }

#include "DyC_Internal.h"

GrB_Info DyC_IncreaseColumn(GrB_Matrix* F, const DyC_Semiring semiring,
    const GrB_Matrix A, const GrB_Matrix S, const GrB_Scalar* X,
    const GrB_Index nx, const GrB_Index* I, const GrB_Index ni,
    const GrB_Index* J, const GrB_Index nj, char* msg) {
    DYC_CLEAR_MSG;

    GrB_Index nrows, ncols, loop_index;  // dimensions of matrix A, utils for
                                         // loops
    GrB_Type type = NULL;                // type of matrix A
    GrB_Monoid add = NULL;               // addition monoid of semiring
    GrB_Semiring grb_semiring = NULL;
    GrB_UnaryOp star = NULL;
    GrB_BinaryOp addition,
        multiplication = NULL;  // binary operators of semiring
    GrB_Matrix O, SR, V, VC, T1, T2, T3,
        M = NULL;  // matrices for storing intermediate
                   // values and results, plus output
    GrB_Scalar t_val = NULL;
    GrB_Index affected_column;

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
    DYC_TRY(GrB_Matrix_new(&T1, type, 1, nrows));
    DYC_TRY(GrB_Matrix_new(&T2, type, nrows, 1));
    DYC_TRY(GrB_Matrix_new(&T3, type, nrows, nrows));
    DYC_TRY(GrB_Matrix_new(&V, type, nrows, nrows));
    DYC_TRY(GrB_Matrix_new(&SR, type, 1, nrows));
    DYC_TRY(GrB_Matrix_new(&VC, type, nrows, 1));
    DYC_TRY(GrB_Matrix_new(&M, type, 1, 1));
    DYC_TRY(GrB_Matrix_new(&O, type, nrows, nrows));
    DYC_TRY(GrB_Scalar_new(&t_val, type));

    // Checks that i,j and x are of the same length
    DYC_ASSERT_MSG(ni == nj && nx == ni && nx == nj, GrB_DIMENSION_MISMATCH,
        "The length of I, J and nvals must be the same.");

    // Checks that all i are the same or all j are the same
    bool same_j = true;
    for (int k = 1; k < ni; k++) {
        if (J[k] != J[0]) {
            same_j = false;
        }
    }
    DYC_ASSERT_MSG(
        same_j, GrB_INVALID_INDEX, "The update must affect a single column.");

    affected_column = J[0];

    // Setup of O
    DYC_TRY(GrB_Matrix_dup(&O, S));

    // Computing V as u(A) = V + A
    DYC_TRY(GrB_Matrix_dup(&V, A));
    for (int k = 0; k < nx; k++) {
        // Perform addition between X[k] and V(I[k], J[k]) and store it in V
        DYC_TRY(GrB_assign(
            V, GrB_NULL, addition, X[k], &I[k], 1, &J[k], 1, GrB_NULL));
    }

    // Extracting column of V to VC
    DYC_TRY(GrB_extract(VC, GrB_NULL, GrB_NULL, V, GrB_ALL, nrows,
        &affected_column, 1, GrB_NULL));

    // Extracting row of S to SR
    DYC_TRY(GrB_extract(SR, GrB_NULL, GrB_NULL, S, &affected_column, 1, GrB_ALL,
        ncols, GrB_NULL));

    // Computing SR * VC
    DYC_TRY(GrB_mxm(M, GrB_NULL, GrB_NULL, grb_semiring, SR, VC, GrB_NULL));

    // Starring M
    DYC_TRY(GrB_Matrix_apply(M, GrB_NULL, GrB_NULL, star, M, GrB_NULL));

    // Computing SC * VR * S
    DYC_TRY(GrB_mxm(T1, GrB_NULL, GrB_NULL, grb_semiring, M, SR, GrB_NULL));
    DYC_TRY(GrB_mxm(T2, GrB_NULL, GrB_NULL, grb_semiring, S, VC, GrB_NULL));
    DYC_TRY(GrB_mxm(T3, GrB_NULL, GrB_NULL, grb_semiring, T2, T1, GrB_NULL));
    DYC_TRY(GrB_mxm(O, GrB_NULL, addition, grb_semiring, S, T3, GrB_NULL));

    // Updating F
    *F = O;

    DYC_FREE_WORK;
    return GrB_SUCCESS;
}