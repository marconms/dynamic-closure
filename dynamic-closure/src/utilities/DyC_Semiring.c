#include "DyC_Internal.h"

GrB_Info DyC_Semiring_new(DyC_Semiring* dyc_semiring,
    const GrB_Semiring grb_semiring, const GrB_UnaryOp star) {
    dyc_semiring->semiring = grb_semiring;
    dyc_semiring->star = star;
    return GrB_SUCCESS;
}

GrB_Info DyC_Semiring_semiring(
    GrB_Semiring* grb_semiring, const DyC_Semiring dyc_semiring) {
    *grb_semiring = dyc_semiring.semiring;
    return GrB_SUCCESS;
}

GrB_Info DyC_Semiring_multiply(
    GrB_BinaryOp* multiply, const DyC_Semiring dyc_semiring) {
    GrB_Semiring grb_semiring = dyc_semiring.semiring;
    GxB_Semiring_multiply(multiply, grb_semiring);
    return GrB_SUCCESS;
}

GrB_Info DyC_Semiring_add(GrB_Monoid* add, const DyC_Semiring dyc_semiring) {
    GrB_Semiring grb_semiring = dyc_semiring.semiring;
    GxB_Semiring_add(add, grb_semiring);
    return GrB_SUCCESS;
}

GrB_Info DyC_Semiring_star(GrB_UnaryOp* star, const DyC_Semiring dyc_semiring) {
    *star = dyc_semiring.star;
    return GrB_SUCCESS;
}

GrB_Info DyC_Semiring_free(DyC_Semiring* dyc_semiring) {
    GrB_Semiring grb_semiring = dyc_semiring->semiring;
    GrB_UnaryOp star = dyc_semiring->star;

    GrB_free(&grb_semiring);

    return GrB_SUCCESS;
}
