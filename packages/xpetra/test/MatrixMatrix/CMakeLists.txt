
TRIBITS_ADD_EXECUTABLE_AND_TEST(
  MatrixMatrix_UnitTests
  SOURCES
    MatrixMatrix_UnitTests.cpp
    ../Xpetra_UnitTests.cpp
  COMM serial mpi
  STANDARD_PASS_OUTPUT
  )

TRIBITS_COPY_FILES_TO_BINARY_DIR(MatrixMatrix_UnitTests_cp
 SOURCE_FILES
   A.mat
   B.mat
   AB.mat
   AtB.mat
   ABt.mat
   AtBt.mat
)
