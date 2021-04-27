//$Id$
//------------------------------------------------------------------------------
//                               TestOptimizer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Jeremy Knittel
// Created: 2016.08.25
//
/**
 * Test driver for Optimizer class and its interfaces.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "Rvector.hpp"
#include "gmatdefs.hpp"
#include "Optimizer.hpp"
#include "SparseMatrixUtil.hpp"

using namespace std;
using namespace GmatMathConstants;
using namespace GmatMathUtil;

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   
	Optimizer Opt = Optimizer();
	
	Opt.Initialize();
    Opt.SetScaling(0);
	
	Rvector decVec(1,10.0);
	Rvector decVecLB(1,-100.0);
	Rvector decVecUB(1,100.0);
	Rvector funLB(2,-200.0,-200.0);
	Rvector funUB(2,200.0,0.0);
	Rvector F(2,0.0,0.0);
	Rvector xmul(1,0.0,0.0);
	Rvector Fmul(2,0.0,0.0);
	
	RSMatrix spMat;
	SparseMatrixUtil::SetSize(spMat,2,1);
	SparseMatrixUtil::SetElement(spMat,0,0,1.0);
	SparseMatrixUtil::SetElement(spMat,1,0,1.0);
	
	Opt.Optimize(decVec,
			     decVecLB,
			  	 decVecUB,
			 	 funLB,
			 	 funUB,
				 spMat,
				 F,
			 	 xmul,	
			 	 Fmul);
				 
	cout<<"Optimal value is "<<F[0]<<endl;
	cout<<"With a decision variable of "<<decVec[0]<<endl;

}

