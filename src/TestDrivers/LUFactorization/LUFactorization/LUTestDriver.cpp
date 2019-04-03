#include "LUFactorization.hpp"
#include <chrono>
#include <iostream>

int main()
{
   std::vector < std::vector < double > > A;
   A.resize(3, std::vector<double>(3));
   A[0][0] = 1;
   A[0][1] = 2;
   A[0][2] = 2;
   A[1][0] = 2;
   A[1][1] = 8;
   A[1][2] = 0;
   A[2][0] = 2;
   A[2][1] = 0;
   A[2][2] = 24;

   //Chrono lines used for calculating computation time

   //std::chrono::time_point<std::chrono::system_clock> start, end;
   //start = std::chrono::system_clock::now();

   //for (int i = 0; i < 100000; i++)
      //LUFactorization test(A, false);

   //end = std::chrono::system_clock::now();

   //std::chrono::duration<double> elapsedTime = end - start;

   //std::cout << "Total time: " << elapsedTime.count() << "\n";

   LUFactorization test(A, false);

   std::cout << "L and U of matrix: \n";
   for (int ii = 0; ii < int(A.size()); ii++)
   {
      for (int jj = 0; jj < int(A[0].size()); jj++)
         std::cout << A[ii][jj] << "     ";
      std::cout << "\n";
   }

   test.GetData(); //Used to view determinant

   return 0;
}