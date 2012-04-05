//$Id$
//------------------------------------------------------------------------------
//                              SteepestDescent
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2007/05/23
//
/**
 * Implementation for the steepest descent optimizer. 
 */
//------------------------------------------------------------------------------


#ifndef SteepestDescent_hpp
#define hpp

#include "InternalOptimizer.hpp"
#include "Gradient.hpp"
#include "Jacobian.hpp"

/**
 * The SteepestDescent optimizer is the prototypical optimization method.  While
 * not the most efficient method, it is the simplest to implement, since all it 
 * needs to do is run nominal trajectories, calculate gradients (via finite 
 * differences if no analytic form exists), scan in the "downhill" direction, 
 * and repeat until the magnitude of the gradient is small enough to declare
 * victory.
 * 
 * @note The steepest descent optimizer is not yet implemented; once a line 
 * search utility exists, it can be completed.
 */
class GMAT_API SteepestDescent : public InternalOptimizer
{
public:
	SteepestDescent(const std::string &name);
	virtual ~SteepestDescent();
   SteepestDescent(const SteepestDescent& sd);
   SteepestDescent& operator=(const SteepestDescent& sd);
   
   virtual Integer      SetSolverResults(Real *data,
                                        const std::string &name,
                                        const std::string &type = "");
   virtual void         SetResultValue(Integer id, Real value,
                                      const std::string &resultType = "");
   virtual GmatBase*    Clone() const;
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");
   virtual bool         Initialize();
   virtual Solver::SolverState
                        AdvanceState();
   virtual bool         Optimize();

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   std::string          objectiveName;
   Real                 objectiveValue;
   
   Gradient             gradientCalculator;
   std::vector<Real>    gradient;
   Jacobian             jacobianCalculator;
   std::vector<Real>    jacobian;
   
   enum
   {
      goalNameID = SolverParamCount,
//      constraintNameID,
      useCentralDifferencesID,
      SteepestDescentParamCount
   };

   static const std::string      PARAMETER_TEXT[SteepestDescentParamCount -
                                              SolverParamCount];
   static const Gmat::ParameterType
                                 PARAMETER_TYPE[SteepestDescentParamCount -
                                              SolverParamCount];

   // State machine methods
   virtual void                  RunNominal();
   virtual void                  RunPerturbation();
   virtual void                  CalculateParameters();
   virtual void                  CheckCompletion();
   virtual void                  RunComplete();

   // Methods used to evaluate the gradient and line search
   void                          CalculateJacobian();
   void                          LineSearch();
   void                          FreeArrays();
   
   virtual void                  WriteToTextFile(
                                    SolverState stateToUse = UNDEFINED_STATE);
};

#endif /*STEEPESTDESCENT_HPP_*/
