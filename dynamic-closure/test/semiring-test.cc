#include <gtest/gtest.h>

extern "C" {
#include "DynamicClosure.h"
#include "LAGraph.h"
}

namespace {

class SemiringTest : public ::testing::Test {
   protected:
    GrB_Semiring grb_semiring;
    DyC_Semiring dyc_semiring;

    SemiringTest() {
        LAGraph_Init(NULL);
        DyC_Init(NULL);

        GrB_Semiring_new(&grb_semiring, GrB_MIN_MONOID_FP64, GrB_PLUS_FP64);
        DyC_Semiring_new(&dyc_semiring, grb_semiring, DyC_STAR_MIN_PLUS_FP64);
    }

    ~SemiringTest() {
        DyC_Finalize(NULL);
        LAGraph_Finalize(NULL);
    }
};

TEST_F(SemiringTest, Add) {
    // Get the addition monoid from the DcS semiring
    GrB_Monoid semiring_add;
    DyC_Semiring_add(&semiring_add, dyc_semiring);

    // Check that the addition monoid is the same as the original
    EXPECT_EQ(semiring_add, GrB_MIN_MONOID_FP64);
}

TEST_F(SemiringTest, Multiply) {
    // Get the multiplication monoid from the DcS semiring
    GrB_BinaryOp semiring_multiply;
    DyC_Semiring_multiply(&semiring_multiply, dyc_semiring);

    // Check that the multiplication monoid is the same as the original
    EXPECT_EQ(semiring_multiply, GrB_PLUS_FP64);
}

TEST_F(SemiringTest, Star) {
    // Get the star function from the DcS semiring
    GrB_UnaryOp semiring_star;
    DyC_Semiring_star(&semiring_star, dyc_semiring);

    // Check that the star function is the same as the original
    EXPECT_EQ(semiring_star, DyC_STAR_MIN_PLUS_FP64);
}
}  // namespace
