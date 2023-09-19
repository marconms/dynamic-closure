#include <gtest/gtest.h>

extern "C" {
#include "DynamicClosure.h"
#include "LAGraph.h"
}

// Create new star function
void star_f(void *z, const void *x) { *((double *)z) = 0; }

namespace {
class GeneralizedFloydWarshallTest : public ::testing::Test {
   protected:
    GrB_Semiring grb_semiring;
    DyC_Semiring dyc_semiring;

    GeneralizedFloydWarshallTest() {
        LAGraph_Init(NULL);
        DyC_Init(NULL);

        GrB_Semiring_new(&grb_semiring, GrB_MIN_MONOID_FP64, GrB_PLUS_FP64);
        DyC_Semiring_new(&dyc_semiring, grb_semiring, DyC_STAR_MIN_PLUS_FP64);
    }

    ~GeneralizedFloydWarshallTest() {
        DyC_Finalize(NULL);
        LAGraph_Finalize(NULL);
    }
};

TEST_F(GeneralizedFloydWarshallTest, DimensionsMismatch) {
    char msg[DYC_MSG_LEN];
    GrB_Matrix F, A;
    GrB_Matrix_new(&A, GrB_FP64, 4, 5);

    GrB_Info info = DyC_GeneralizedFloydWarshall(&F, dyc_semiring, A, msg);
    EXPECT_EQ(info, GrB_DIMENSION_MISMATCH);
}

TEST_F(GeneralizedFloydWarshallTest, Empty) {
    char msg[DYC_MSG_LEN];
    GrB_Matrix F, A;
    GrB_Matrix_new(&A, GrB_FP64, 4, 4);

    GrB_Info info = DyC_GeneralizedFloydWarshall(&F, dyc_semiring, A, msg);
    ASSERT_EQ(info, GrB_SUCCESS);

    GrB_Index nvals;
    GrB_Matrix_nvals(&nvals, F);
    EXPECT_EQ(nvals, 0);
}

TEST_F(GeneralizedFloydWarshallTest, MinPlus) {
    char msg[DYC_MSG_LEN];
    GrB_Matrix F, S, A;

    // Load the matrix
    FILE *fa = fopen("assets/min-plus-A.mtx", "r");
    LAGraph_MMRead(&A, fa, NULL);
    fclose(fa);
    FILE *fs = fopen("assets/min-plus-S.mtx", "r");
    LAGraph_MMRead(&S, fa, NULL);
    fclose(fs);

    GrB_Info info = DyC_GeneralizedFloydWarshall(&F, dyc_semiring, A, msg);
    ASSERT_EQ(info, GrB_SUCCESS);

    bool result = NULL;
    LAGraph_Matrix_IsEqual(&result, F, S, NULL);
    EXPECT_EQ(result, true);
}

TEST_F(GeneralizedFloydWarshallTest, ExtendedMinPlus) {
    char msg[DYC_MSG_LEN];
    GrB_Matrix F, S, A;

    // Load the matrix
    FILE *fa = fopen("assets/extended-min-plus-A.mtx", "r");
    LAGraph_MMRead(&A, fa, NULL);
    fclose(fa);
    FILE *fs = fopen("assets/extended-min-plus-S.mtx", "r");
    LAGraph_MMRead(&S, fa, NULL);
    fclose(fs);

    GrB_Info info = DyC_GeneralizedFloydWarshall(&F, dyc_semiring, A, msg);
    printf("%s\n", msg);
    ASSERT_EQ(info, GrB_SUCCESS);

    bool result = NULL;
    LAGraph_Matrix_IsEqual(&result, F, S, NULL);
    EXPECT_EQ(result, true);
}
}  // namespace
