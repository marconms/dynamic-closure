#include "GraphBLAS.h"

#define DYC_MSG_LEN 256  // Maximum length of an error message

//------------------------------------------------------------------------------
// Error codes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Predefined star operators
//------------------------------------------------------------------------------

extern GrB_UnaryOp DyC_STAR_MIN_PLUS_FP64;
static inline void star_min_plus_fp64(void* z, const void* x) {
    if (*((double*)x) >= 0) {
        *((double*)z) = 0;
    } else {
        *((double*)z) = -INFINITY;
    }
}

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

GrB_Info DyC_Init(char* msg);
GrB_Info DyC_Finalize(char* msg);

//------------------------------------------------------------------------------
// Semiring
//------------------------------------------------------------------------------

typedef struct DyC_Semiring {
    GrB_Semiring semiring;
    GrB_UnaryOp star;
} DyC_Semiring;

GrB_Info DyC_Semiring_new(DyC_Semiring* dyc_semiring,
    const GrB_Semiring grb_semiring, const GrB_UnaryOp star);
GrB_Info DyC_Semiring_semiring(
    GrB_Semiring* grb_semiring, const DyC_Semiring dyc_semiring);
GrB_Info DyC_Semiring_multiply(
    GrB_BinaryOp* multiply, const DyC_Semiring dyc_semiring);
GrB_Info DyC_Semiring_add(GrB_Monoid* add, const DyC_Semiring dyc_semiring);
GrB_Info DyC_Semiring_star(GrB_UnaryOp* star, const DyC_Semiring dyc_semiring);
GrB_Info DyC_Semiring_free(DyC_Semiring* dyc_semiring);

//------------------------------------------------------------------------------
// Algorithms
//------------------------------------------------------------------------------

GrB_Info DyC_GeneralizedFloydWarshall(
    GrB_Matrix* F, const DyC_Semiring semiring, const GrB_Matrix A, char* msg);

GrB_Info DyC_DistrustMatrix(GrB_Matrix* F, const DyC_Semiring semiring,
    const GrB_BinaryOp comparison, const GrB_Matrix A, const GrB_Matrix S, const GrB_Index* I,
    const GrB_Index ni, const GrB_Index* J, const GrB_Index nj, char* msg);

GrB_Info DyC_DecreaseIdempotent(GrB_Matrix* F, const DyC_Semiring semiring,
    const GrB_Matrix A, const GrB_Matrix S, const GrB_Matrix D,
    const GrB_Scalar* X, const GrB_Index nx, const GrB_Index* I,
    const GrB_Index ni, const GrB_Index* J, const GrB_Index nj, char* msg);

GrB_Info DyC_IncreaseRow(GrB_Matrix* F, const DyC_Semiring semiring,
    const GrB_Matrix A, const GrB_Matrix S, const GrB_Scalar* X,
    const GrB_Index nx, const GrB_Index* I, const GrB_Index ni,
    const GrB_Index* J, const GrB_Index nj, char* msg);

GrB_Info DyC_IncreaseColumn(GrB_Matrix* F, const DyC_Semiring semiring,
    const GrB_Matrix A, const GrB_Matrix S, const GrB_Scalar* X,
    const GrB_Index nx, const GrB_Index* I, const GrB_Index ni,
    const GrB_Index* J, const GrB_Index nj, char* msg);
