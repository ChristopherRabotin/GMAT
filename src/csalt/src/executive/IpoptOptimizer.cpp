//------------------------------------------------------------------------------
//                              IpoptOptimizer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Jeremy Knittel / NASA
// Created: 2016.08.09 
//
/**
 * From original MATLAB prototype:
 * Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include "IpoptOptimizer.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_OPTIMIZER

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
IpoptOptimizer::IpoptOptimizer(Trajectory* trajectory_in) :
	traj    (trajectory_in),
	iGfun   (),
	jGvar   (),
   Problem ()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
IpoptOptimizer::IpoptOptimizer(const IpoptOptimizer &copy) :
   traj    (copy.traj),
   iGfun   (copy.iGfun),
   jGvar   (copy.jGvar),
   Problem (copy.Problem)

{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
IpoptOptimizer& IpoptOptimizer::operator=(const IpoptOptimizer &copy)
{
   if (&copy == this)
      return *this;
   
   traj    = copy.traj;
   iGfun   = copy.iGfun;
   jGvar   = copy.jGvar;
   Problem = copy.Problem;

   return *this;   
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
IpoptOptimizer::~IpoptOptimizer()
{
   // nothing to do here 
}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the IpoptOptimizer
 *
 */
//------------------------------------------------------------------------------
void IpoptOptimizer::Initialize()
{
    // Configure the optimizer and prepare for use
	// Create the SNOPT problem and initialize the inputs
	
	// Set the derivative option          
   Problem.setIntParameter((char*)"Derivative Option",2);
   // Set the derivative verification level
   Problem.setIntParameter((char*)"Verify Level",-1);
   // Print CPU times at bottom of SNOPT print file
   Problem.setIntParameter((char*)"Timing level",3);
   //  Set the SNOPT internal scaling option
   Problem.setIntParameter((char*)"Scale Option", 1);
   //  Set tolerance on optimality
   Problem.setRealParameter((char*)"Major optimality tolerance",1e-8);
   //  Set tolerance on feasibility
   Problem.setRealParameter((char*)"Major feasibility tolerance",1e-8);
   //  Set number of iterations
   Problem.setIntParameter((char*)"Major iterations limit",3000);
   //  Set number of iterations
   Problem.setIntParameter((char*)"Iterations limit",300000);
   //  Set objective row
   Problem.setIntParameter((char*)"Objective row",1);
   //  Set the user function
   Problem.setUserFun(SNOPTFunctionWrapper);
   //  Set the problem name
   Problem.setProbName((char*)"CSALT");
}

//------------------------------------------------------------------------------
// void SetScaling(bool ifScaling)
//------------------------------------------------------------------------------
/**
 * Sets the flag indicating whether or not use scaling
 *
 * @param <ifScaling>   is scaling requested?
 *
 */
//------------------------------------------------------------------------------
void IpoptOptimizer::SetScaling(bool ifScaling)
{
	// Whether to use SNOPT's internal scaling or whether the 
	// problem has already been scaled
	if (ifScaling)	
	    Problem.setIntParameter((char*)"Scale Option",0);	
	else
	    Problem.setIntParameter((char*)"Scale Option",1);	
}

//------------------------------------------------------------------------------
// void SetIpoptOptimizerOutputFile(const std::string &optFile)
//------------------------------------------------------------------------------
/**
 * Sets the output file for the SNOPT optimizer
 *
 * @param optFile The name of the output file
 */
//------------------------------------------------------------------------------
void IpoptOptimizer::SetIpoptOptimizerOutputFile(const std::string &optFile)
{
   Problem.setPrintFile(optFile.c_str());
}

//------------------------------------------------------------------------------
// void Optimize(Rvector       &decVec,   const Rvector  &decVecLB,
//               const Rvector &decVecUB, const Rvector  &funLB,
//               const Rvector &funUB,    const RSMatrix &sPatternMat,
//               Rvector       &F,        Rvector        &xmul,
//               Rvector       &Fmul)
//------------------------------------------------------------------------------
/**
 * Performs the optimization
 *
 * @param <decVec>        the decision vector
 * @param <decVecLB>      the decision vector lower bounds
 * @param <decVecUB>      the decision vector upper bounds
 * @param <funLB>         the objective and constraint function lower bounds
 * @param <funUB>         the objective and constraint function upper bounds
 * @param <sPatternMat>   the dsparsity pattern
 * @param <F>             the objective and constraint function values
 * @param <xmul>          the lagrange multipliers for the KKT optimality 
 *                        conditions of the state bounds
 * @param <Fmul>          the lagrange multipliers for the KKT optimality 
 *                        conditions of the objective and constraint functions
 *
 */
//------------------------------------------------------------------------------
void IpoptOptimizer::Optimize(Rvector       &decVec,    const Rvector  &decVecLB,
			  		   	    const Rvector &decVecUB,  const Rvector  &funLB,
			 			       const Rvector &funUB,     const RSMatrix &sPatternMat,
						       Rvector       &F,         Rvector        &xmul,
			 		          Rvector       &Fmul)
{

   #ifdef DEBUG_OPTIMIZER
      MessageInterface::ShowMessage("--- decVec passed in to Optimize = %s\n",
                                    decVec.ToString(12).c_str());
      MessageInterface::ShowMessage("--- decVecLB passed in to Optimize = %s\n",
                                    decVecLB.ToString(12).c_str());
      MessageInterface::ShowMessage("--- decVecUB passed in to Optimize = %s\n",
                                    decVecUB.ToString(12).c_str());
      MessageInterface::ShowMessage("--- funLB passed in to Optimize = %s\n",
                                    funLB.ToString(12).c_str());
      MessageInterface::ShowMessage("--- funUB passed in to Optimize = %s\n",
                                    funUB.ToString(12).c_str());
      Integer rSparse = sPatternMat.size1();
      Integer cSparse = sPatternMat.size2();
      MessageInterface::ShowMessage(
            " ----- sparsity pattern passed in to Optimize() size (%d, %d): \n",
            rSparse, cSparse);
      for (Integer cc = 0; cc < cSparse; cc++)
      {
         for (Integer rr = 0; rr < rSparse; rr++)
         {
            Real jacTmp = sPatternMat(rr,cc);
            if (jacTmp != 0.0)
               MessageInterface::ShowMessage(
                                    "      sPatternMat(%d, %d) =  %12.10f\n",
                                    rr+1, cc+1, jacTmp);
         }
      }
   #endif

   // TODO: Better GMAT-ish way to do this without converting
	// the Rvectors to std::vectors?
	std::vector<SNOPT_DOUBLE>    x(decVec.GetSize());
	std::vector<SNOPT_DOUBLE>    xLB(decVec.GetSize());
	std::vector<SNOPT_DOUBLE>    xUB(decVec.GetSize());
	std::vector<SNOPT_DOUBLE>    xmul_local(decVec.GetSize(),0.0);
	std::vector<SNOPT_INTEGER>   xstate(decVec.GetSize(),0);
	std::vector<SNOPT_DOUBLE>    F_local(F.GetSize());
	std::vector<SNOPT_DOUBLE>    F_localLB(F.GetSize());
	std::vector<SNOPT_DOUBLE>    F_localUB(F.GetSize());
	std::vector<SNOPT_DOUBLE>    Fmul_local(F.GetSize(),0.0);
	std::vector<SNOPT_INTEGER>   Fstate(F.GetSize(),0);
	
	// Loop through the decision vector and put the RVector 
	// data into the std::vectors
	for (Integer ii = 0; ii < decVec.GetSize(); ii++)	
	{
		x[ii]   = decVec[ii];
		xLB[ii] = decVecLB[ii];
		xUB[ii] = decVecUB[ii];
	}	
	
	// Loop through the objective function and constraints and put 
	// the RVector data into the std::vectors
	for (Integer ii = 0; ii < F.GetSize(); ii++)	
	{
		F_local[ii]   = F[ii];
		F_localLB[ii] = funLB[ii];
		F_localUB[ii] = funUB[ii];
	}	
	
	// Pass pointers to SNOPT for the decision vector and its bounds
    Problem.setX(x.data(), xLB.data(), xUB.data(),
				     xmul_local.data(), xstate.data());
				 
	// Pass pointers to SNOPT for the objective function and constraints
    Problem.setF(F_local.data(), F_localLB.data(),F_localUB.data(),
				     Fmul_local.data(), Fstate.data());
				 
	// We don't know what constraints are linear, so assume they are
   // all nonlinear - Or maybe there is no need to do this??
    
    SNOPT_INTEGER lenA = 0, neA = 0;
    std::vector<Integer> iAfun, jAvar;
    std::vector<double> A;
    Problem.setA(lenA, neA, iAfun.data(), jAvar.data(), A.data());
    

   // Loop over the sparsity pattern and determine where non-zeros are and
   // constuct vectors that SNOPT requires for the sparsity pattern
   // (iGfun,jGvar)

   // Sparse Matrix Utility Style Coding by YK
   Integer numNonLinearNonZeros =
           SparseMatrixUtil::GetNumNonZeroElements(&sPatternMat);

   iGfun.clear();
   jGvar.clear();

   SparseMatrixUtil::GetSparsityPattern(&sPatternMat, iGfun, jGvar);

   // Probably should cast iGfun and jGVar to std::vector<SNOPT_INTEGER>
   // here -> N/N
   Integer numTotalNonZeros = numNonLinearNonZeros;  // No linear constraints
   #ifdef DEBUG_OPTIMIZER
      MessageInterface::ShowMessage("--- numTotalNonZeros     = %d\n",
                                    numTotalNonZeros);
      MessageInterface::ShowMessage("--- numNonLinearNonZeros = %d\n",
                                    numNonLinearNonZeros);
      MessageInterface::ShowMessage("--- iGfun:\n");
      for (Integer ii = 0; ii < iGfun.size(); ii++)
         MessageInterface::ShowMessage("------ (%d)  = %d\n",
                                       ii+1, iGfun.at(ii));
      MessageInterface::ShowMessage("--- jGvar:\n");
      for (Integer ii = 0; ii < jGvar.size(); ii++)
         MessageInterface::ShowMessage("------ (%d)  = %d\n",
                                       ii+1, jGvar.at(ii));
   #endif
   Problem.setG(numNonLinearNonZeros,numTotalNonZeros,
                iGfun.data(),jGvar.data());
	// Tell SNOPT how many decision variables there are and how many constraints
	Problem.setProblemSize(decVec.GetSize(), F.GetSize());

   // Pass a pointer to this object so that the user function can call it
   Problem.setUserR((SNOPT_DOUBLE*)this,500);
		
	// Run SNOPT!
	//SNOPT_INTEGER status = Problem.solve(0); // Do we care about exit status??
   Problem.solve(0);
	
	// Put the std::vector data back into the Rvectors
	for (Integer ii = 0; ii < decVec.GetSize(); ii++)	
	{
		decVec[ii]   = x[ii];
		xmul[ii]     = xmul_local[ii];
	}	
	for (Integer ii = 0; ii < F.GetSize(); ii++)	
	{
		F[ii] = F_local[ii];
		Fmul[ii] = Fmul_local[ii];
	}			  	
   #ifdef DEBUG_OPTIMIZER
      MessageInterface::ShowMessage(
                                 " ----- EXITING IpoptOptimizer::Optimize<<<< \n");
   #endif
}
