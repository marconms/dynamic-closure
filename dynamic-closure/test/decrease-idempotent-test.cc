
#include <gtest/gtest.h>

extern "C" {
#include "DynamicClosure.h"
#include "LAGraph.h"
}

namespace {
class DescreaseIdempotentTest : public ::testing::Test {
   protected:
    GrB_Semiring grb_semiring;
    DyC_Semiring dyc_semiring;

    DescreaseIdempotentTest() {
        LAGraph_Init(NULL);
        DyC_Init(NULL);

        GrB_Semiring_new(&grb_semiring, GrB_MIN_MONOID_FP64, GrB_PLUS_FP64);
        DyC_Semiring_new(&dyc_semiring, grb_semiring, DyC_STAR_MIN_PLUS_FP64);
    }

    ~DescreaseIdempotentTest() {
        DyC_Finalize(NULL);
        LAGraph_Finalize(NULL);
    }
};

TEST_F(DescreaseIdempotentTest, DimensionsMismatch) {
    char msg[DYC_MSG_LEN];
    GrB_Matrix F, A, S, D;
    GrB_Matrix_new(&A, GrB_FP64, 4, 4);
    GrB_Matrix_new(&S, GrB_FP64, 4, 4);
    GrB_Matrix_new(&D, GrB_FP64, 4, 4);

    // Declaring update
    GrB_Index I = 1;
    GrB_Index* J = (GrB_Index*)malloc(2 * sizeof(GrB_Index));
    J[0] = 1;
    J[1] = 1;
    GrB_Scalar X;
    GrB_Scalar_new(&X, GrB_FP64);
    GrB_Scalar_setElement_FP64(X, 3);

    // Calling decrease
    GrB_Info di_info = DyC_DecreaseIdempotent(
        &F, dyc_semiring, A, S, D, &X, 1, &I, 1, J, 2, msg);
    EXPECT_EQ(di_info, GrB_DIMENSION_MISMATCH);
}

TEST_F(DescreaseIdempotentTest, MinPlus) {
    char msg[DYC_MSG_LEN];
    GrB_Matrix F, A, D, US, S;

    // Load the distrust matrix
    FILE* fa = fopen("assets/min-plus-A.mtx", "r");
    LAGraph_MMRead(&A, fa, NULL);
    fclose(fa);
    FILE* fs = fopen("assets/min-plus-S.mtx", "r");
    LAGraph_MMRead(&S, fs, NULL);
    fclose(fs);
    FILE* fd = fopen("assets/min-plus-D.mtx", "r");
    LAGraph_MMRead(&D, fd, NULL);
    fclose(fd);
    FILE* fus = fopen("assets/min-plus-US-decrease.mtx", "r");
    LAGraph_MMRead(&US, fus, NULL);
    fclose(fus);

    // Declaring update
    GrB_Index* I = (GrB_Index*)malloc(2 * sizeof(GrB_Index));
    I[0] = 0;
    I[1] = 0;
    GrB_Index* J = (GrB_Index*)malloc(2 * sizeof(GrB_Index));
    J[0] = 1;
    J[1] = 2;
    GrB_Scalar* X = (GrB_Scalar*)malloc(2 * sizeof(GrB_Scalar));
    GrB_Scalar_new(&X[0], GrB_FP64);
    GrB_Scalar_new(&X[1], GrB_FP64);
    GrB_Scalar_setElement_FP64(X[0], 2);
    GrB_Scalar_setElement_FP64(X[1], 2);

    GrB_Info di_info = DyC_DecreaseIdempotent(
        &F, dyc_semiring, A, S, D, X, 2, I, 2, J, 2, msg);
    printf("%s\n", msg);
    ASSERT_EQ(di_info, GrB_SUCCESS);

    // Checking result
    bool result = NULL;
    LAGraph_Matrix_IsEqual(&result, F, US, NULL);
    EXPECT_EQ(result, true);
}

TEST_F(DescreaseIdempotentTest, ExtendedMinPlus) {
    char msg[DYC_MSG_LEN];
    GrB_Matrix F, S, A, D, US;

    // Load the matrix
    FILE* fa = fopen("assets/extended-min-plus-A.mtx", "r");
    LAGraph_MMRead(&A, fa, NULL);
    fclose(fa);
    FILE* fs = fopen("assets/extended-min-plus-S.mtx", "r");
    LAGraph_MMRead(&S, fa, NULL);
    fclose(fs);
    FILE* fd = fopen("assets/extended-min-plus-D.mtx", "r");
    LAGraph_MMRead(&D, fd, NULL);
    fclose(fd);
    FILE* fus = fopen("assets/extended-min-plus-US-decrease.mtx", "r");
    LAGraph_MMRead(&US, fus, NULL);
    fclose(fus);

    // Declaring update
    GrB_Index* I = (GrB_Index*)malloc(1 * sizeof(GrB_Index));
    I[0] = 0;
    GrB_Index* J = (GrB_Index*)malloc(1 * sizeof(GrB_Index));
    J[0] = 0;
    GrB_Scalar* X = (GrB_Scalar*)malloc(1 * sizeof(GrB_Scalar));
    GrB_Scalar_new(&X[0], GrB_FP64);
    GrB_Scalar_setElement_FP64(X[0], -2);

    GrB_Info di_info = DyC_DecreaseIdempotent(
        &F, dyc_semiring, A, S, D, X, 1, I, 1, J, 1, msg);
    ASSERT_EQ(di_info, GrB_SUCCESS);

    // Checking result
    bool result = NULL;
    LAGraph_Matrix_IsEqual(&result, F, US, NULL);
    EXPECT_EQ(result, true);
}
}  // namespace
