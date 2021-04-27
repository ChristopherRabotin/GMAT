//------------------------------------------------------------------------------
//                              OptimizerBase
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Youngkwang Kim / Yonsei University
// Created: 2017.09.28
//
/**
* the base class for SNOPT/IPOPT optimizer classes
*/
//------------------------------------------------------------------------------

#ifndef OptimizerBase_hpp
#define OptimizerBase_hpp


#include "Trajectory.hpp"
#include "csaltdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "SparseMatrixUtil.hpp"

//class Trajectory;

class OptimizerBase
{
public:
	
	// OptimizerBase();
	OptimizerBase(Trajectory * trajectory_in);
	
	OptimizerBase(const OptimizerBase &copy);
	
	OptimizerBase& operator=(const OptimizerBase &copy);
	
	virtual ~OptimizerBase();
	
	virtual void Initialize() = 0;
	
	virtual void SetScaling(bool ifScaling) = 0;

	virtual void SetOptimizerOutputFile(const std::string &optFile) = 0;

	virtual void Optimize(Rvector        &decVec,
                         const Rvector  &decVecLB,
                         const Rvector  &decVecUB,
                         const Rvector  &funLB,
                         const Rvector  &funUB,
                         const RSMatrix &spMat,
                         Rvector        &F,
                         Rvector        &xmul,
                         Rvector        &Fmul) = 0;
				
	// TODO: Should the following things be made protected 
	// or private with accessor functions?
   // WCS - for performance we can leave it as is for now
							  
  	// Pointer to the Owner class
  	Trajectory * traj;

protected:
		
};

#endif // OptimizerBase_hpp
