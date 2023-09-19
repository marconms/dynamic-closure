#include "DyC_Internal.h"

GrB_UnaryOp DyC_STAR_MIN_PLUS_FP64 = NULL;

GrB_Info DyC_Init(char* msg) {
    DYC_CLEAR_MSG;
    // Initialising star operators
    DYC_TRY(GrB_UnaryOp_new(
        &DyC_STAR_MIN_PLUS_FP64, star_min_plus_fp64, GrB_FP64, GrB_FP64));

    return GrB_SUCCESS;
}