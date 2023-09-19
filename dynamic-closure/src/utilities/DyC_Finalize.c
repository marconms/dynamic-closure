#include "DyC_Internal.h"

GrB_Info DyC_Finalize(char *msg) {
    DYC_CLEAR_MSG;

    DYC_TRY(GrB_free(&DyC_STAR_MIN_PLUS_FP64));

    return GrB_SUCCESS;
}