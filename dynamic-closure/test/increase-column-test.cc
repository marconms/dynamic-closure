
#include <gtest/gtest.h>

extern "C" {
#include "DynamicClosure.h"
#include "LAGraph.h"
}

namespace {
class IncreaseColumnTest : public ::testing::Test {
   protected:
    GrB_Semiring grb_semiring;
    DyC_Semiring dyc_semiring;

    IncreaseColumnTest() {
        LAGraph_Init(NULL);
        DyC_Init(NULL);

        GrB_Semiring_new(&grb_semiring, GrB_MIN_MONOID_FP64, GrB_PLUS_FP64);
        DyC_Semiring_new(&dyc_semiring, grb_semiring, DyC_STAR_MIN_PLUS_FP64);
    }

    ~IncreaseColumnTest() {
        DyC_Finalize(NULL);
        LAGraph_Finalize(NULL);
    }
};

TEST_F(IncreaseColumnTest, DimensionsMismatch) {
    char msg[DYC_MSG_LEN];
    GrB_Matrix F, A, S;
    GrB_Matrix_new(&A, GrB_FP64, 1, 1);
    GrB_Matrix_new(&S, GrB_FP64, 1, 1);

    // Declaring update
    GrB_Index I = 0;
    GrB_Index* J = (GrB_Index*)malloc(2 * sizeof(GrB_Index));
    J[0] = 1;
    J[1] = 1;
    GrB_Scalar X;
    GrB_Scalar_new(&X, GrB_FP64);
    GrB_Scalar_setElement_FP64(X, 2);

    // Calling decrease
    GrB_Info info =
        DyC_IncreaseColumn(&F, dyc_semiring, A, S, &X, 1, &I, 1, J, 2, msg);
    EXPECT_EQ(info, GrB_DIMENSION_MISMATCH);
}

TEST_F(IncreaseColumnTest, InvalidIndex) {
    char msg[DYC_MSG_LEN];
    GrB_Matrix F, A, S;
    GrB_Matrix_new(&A, GrB_FP64, 4, 4);
    GrB_Matrix_new(&S, GrB_FP64, 4, 4);

    // Declaring update
    GrB_Index* I = (GrB_Index*)malloc(2 * sizeof(GrB_Index));
    I[0] = 1;
    I[1] = 1;
    GrB_Index* J = (GrB_Index*)malloc(2 * sizeof(GrB_Index));
    J[0] = 0;
    J[1] = 1;
    GrB_Scalar* X = (GrB_Scalar*)malloc(2 * sizeof(GrB_Scalar));
    GrB_Scalar_new(&X[0], GrB_FP64);
    GrB_Scalar_new(&X[1], GrB_FP64);
    GrB_Scalar_setElement_FP64(X[0], 2);
    GrB_Scalar_setElement_FP64(X[1], 2);

    // Calling decrease
    GrB_Info info =
        DyC_IncreaseColumn(&F, dyc_semiring, A, S, X, 2, I, 2, J, 2, msg);
    EXPECT_EQ(info, GrB_INVALID_INDEX);
}

TEST_F(IncreaseColumnTest, MinPlus) {
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
    FILE* fus = fopen("assets/min-plus-US-increase.mtx", "r");
    LAGraph_MMRead(&US, fus, NULL);
    fclose(fus);

    // Transpose A, S and US
    GrB_transpose(A, GrB_NULL, GrB_NULL, A, GrB_NULL);
    GrB_transpose(S, GrB_NULL, GrB_NULL, S, GrB_NULL);
    GrB_transpose(US, GrB_NULL, GrB_NULL, US, GrB_NULL);

    // Declaring first update
    GrB_Index* I = (GrB_Index*)malloc(2 * sizeof(GrB_Index));
    I[0] = 1;
    I[1] = 2;
    GrB_Index* J = (GrB_Index*)malloc(2 * sizeof(GrB_Index));
    J[0] = 0;
    J[1] = 0;
    GrB_Scalar* X = (GrB_Scalar*)malloc(2 * sizeof(GrB_Scalar));
    GrB_Scalar_new(&X[0], GrB_FP64);
    GrB_Scalar_new(&X[1], GrB_FP64);
    GrB_Scalar_setElement_FP64(X[0], 0);
    GrB_Scalar_setElement_FP64(X[1], 0);

    GrB_Info info =
        DyC_IncreaseColumn(&F, dyc_semiring, A, S, X, 2, I, 2, J, 2, msg);
    printf("%s\n", msg);
    ASSERT_EQ(info, GrB_SUCCESS);

    // Checking result
    bool result = NULL;
    LAGraph_Matrix_IsEqual(&result, F, US, NULL);
    EXPECT_EQ(result, true);
}

TEST_F(IncreaseColumnTest, ExtendedMinPlus) {
    char msg[DYC_MSG_LEN];
    GrB_Matrix F, A, US, S;

    // Load the distrust matrix
    FILE* fa = fopen("assets/extended-min-plus-A.mtx", "r");
    LAGraph_MMRead(&A, fa, NULL);
    fclose(fa);
    FILE* fs = fopen("assets/extended-min-plus-S.mtx", "r");
    LAGraph_MMRead(&S, fs, NULL);
    fclose(fs);
    FILE* fus = fopen("assets/extended-min-plus-US-increase.mtx", "r");
    LAGraph_MMRead(&US, fus, NULL);
    fclose(fus);

    // Transpose A, S and US
    GrB_transpose(A, GrB_NULL, GrB_NULL, A, GrB_NULL);
    GrB_transpose(S, GrB_NULL, GrB_NULL, S, GrB_NULL);
    GrB_transpose(US, GrB_NULL, GrB_NULL, US, GrB_NULL);

    // Declaring update
    GrB_Index* I = (GrB_Index*)malloc(2 * sizeof(GrB_Index));
    I[0] = 1;
    I[1] = 2;
    GrB_Index* J = (GrB_Index*)malloc(2 * sizeof(GrB_Index));
    J[1] = 0;
    J[2] = 0;
    GrB_Scalar* X = (GrB_Scalar*)malloc(2 * sizeof(GrB_Scalar));
    GrB_Scalar_new(&X[0], GrB_FP64);
    GrB_Scalar_new(&X[1], GrB_FP64);
    GrB_Scalar_setElement_FP64(X[0], -2);
    GrB_Scalar_setElement_FP64(X[1], -2);

    GrB_Info info =
        DyC_IncreaseColumn(&F, dyc_semiring, A, S, X, 2, I, 2, J, 2, msg);
    ASSERT_EQ(info, GrB_SUCCESS);

    // Checking result
    bool result = NULL;
    LAGraph_Matrix_IsEqual(&result, F, US, NULL);
    EXPECT_EQ(result, true);
}
}  // namespace
