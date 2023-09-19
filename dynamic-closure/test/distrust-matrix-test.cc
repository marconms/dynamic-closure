#include <gtest/gtest.h>

extern "C" {
#include "DynamicClosure.h"
#include "LAGraph.h"
}

namespace {
class DistrustMatrixTest : public ::testing::Test {
   protected:
    GrB_Semiring grb_semiring;
    DyC_Semiring dyc_semiring;

    DistrustMatrixTest() {
        LAGraph_Init(NULL);
        DyC_Init(NULL);

        GrB_Semiring_new(&grb_semiring, GrB_MIN_MONOID_FP64, GrB_PLUS_FP64);
        DyC_Semiring_new(&dyc_semiring, grb_semiring, DyC_STAR_MIN_PLUS_FP64);
    }

    ~DistrustMatrixTest() {
        DyC_Finalize(NULL);
        LAGraph_Finalize(NULL);
    }
};

TEST_F(DistrustMatrixTest, DimensionsMismatch) {
    char msg[DYC_MSG_LEN];
    GrB_Matrix F, A, S;

    FILE* fa = fopen("assets/min-plus-A.mtx", "r");
    LAGraph_MMRead(&A, fa, NULL);
    fclose(fa);
    FILE* fs = fopen("assets/min-plus-S.mtx", "r");
    LAGraph_MMRead(&S, fs, NULL);
    fclose(fs);

    // Set update
    GrB_Index I = 1;
    GrB_Index J = 1;

    // Calling distrust matrix
    GrB_Info info =
        DyC_DistrustMatrix(&F, dyc_semiring, GrB_EQ_FP64, A, S, &I, 1, &J, 2, msg);
    ASSERT_EQ(info, GrB_DIMENSION_MISMATCH);
}

TEST_F(DistrustMatrixTest, MinPlus) {
    char msg[DYC_MSG_LEN];
    GrB_Matrix F, A, S, D;

    FILE* fa = fopen("assets/min-plus-A.mtx", "r");
    LAGraph_MMRead(&A, fa, NULL);
    fclose(fa);
    FILE* fs = fopen("assets/min-plus-S.mtx", "r");
    LAGraph_MMRead(&S, fs, NULL);
    fclose(fs);
    FILE* fd = fopen("assets/min-plus-D.mtx", "r");
    LAGraph_MMRead(&D, fd, NULL);
    fclose(fd);

    // Declaring update
    GrB_Index* I = (GrB_Index*)malloc(2 * sizeof(GrB_Index));
    I[0] = 0;
    I[1] = 0;
    GrB_Index* J = (GrB_Index*)malloc(2 * sizeof(GrB_Index));
    J[0] = 1;
    J[1] = 2;

    // Calling distrust matrix
    GrB_Info info = DyC_DistrustMatrix(&F, dyc_semiring, GrB_EQ_FP64, A, S, I, 2, J, 2, msg);
    EXPECT_EQ(info, GrB_SUCCESS);

    bool result = NULL;
    LAGraph_Matrix_IsEqual(&result, F, D, NULL);
    EXPECT_EQ(result, true);
}

TEST_F(DistrustMatrixTest, ExtendedMinPlus) {
    char msg[DYC_MSG_LEN];
    GrB_Matrix F, A, S, D;

    FILE* fa = fopen("assets/min-plus-A.mtx", "r");
    LAGraph_MMRead(&A, fa, NULL);
    fclose(fa);
    FILE* fs = fopen("assets/min-plus-S.mtx", "r");
    LAGraph_MMRead(&S, fs, NULL);
    fclose(fs);
    FILE* fd = fopen("assets/extended-min-plus-D.mtx", "r");
    LAGraph_MMRead(&D, fd, NULL);
    fclose(fd);

    // Declaring update
    GrB_Index* I = (GrB_Index*)malloc(1 * sizeof(GrB_Index));
    I[0] = 0;
    GrB_Index* J = (GrB_Index*)malloc(1 * sizeof(GrB_Index));
    J[0] = 0;

    // Calling distrust matrix
    GrB_Info info = DyC_DistrustMatrix(&F, dyc_semiring, GrB_EQ_FP64, A, S, I, 1, J, 1, msg);
    EXPECT_EQ(info, GrB_SUCCESS);

    bool result = NULL;
    LAGraph_Matrix_IsEqual(&result, F, D, NULL);
    EXPECT_EQ(result, true);
}

}  // namespace
