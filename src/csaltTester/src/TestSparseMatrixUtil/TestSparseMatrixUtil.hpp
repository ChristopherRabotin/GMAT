#include "SparseMatrixUtil.hpp"
#include <iostream>

#ifndef TestSparseMatrixUtil_hpp
#define TestSparseMatrixUtil_hpp

class TestSparseMatrixUtil
{
public:
   static void RunTests()
   {
      // body 7: this is for LGR math util
      ////std::cout << "\nRadauMathUtil Unit-Tester: test begins" << std::endl;
      RSMatrix test1, test2;
      SparseMatrixUtil::SetSize(test1, 2, 2);

      test1(0, 0) = 1;
      test1(1, 1) = 2;

      test2 = SparseMatrixUtil::CopySparseMatrix(&test1);

      SparseMatrixUtil::PrintNonZeroElements(&test2);
   };

};
#endif // TestSparseMatrixUtil_hpp