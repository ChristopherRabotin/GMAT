//$Id$
//------------------------------------------------------------------------------
//                         Optimizer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy C. Shoan, GSFC
// Created: 2006.07.12
//
/**
 * Definition for the Optimizer base class. 
 */
//------------------------------------------------------------------------------


#ifndef Optimizer_hpp
#define Optimizer_hpp

#include "Solver.hpp"

class GMAT_API Optimizer : public Solver
{
public:
   Optimizer(std::string typeName, std::string name);
   virtual ~Optimizer();
   Optimizer(const Optimizer &opt);
   Optimizer&      operator=(const Optimizer& opt);
   
   virtual bool        Initialize();
   //virtual SolverState         AdvanceState();  // do I need this?

   // Solver interfaces used to talk to the Vary and Minimize commands
   //virtual Integer     SetSolverVariables(Real *data, const std::string &name);
   //virtual Real        GetSolverVariable(Integer id);
   virtual Integer     SetSolverResults(Real *data, const std::string &name,
                                        const std::string &type = "");
   virtual void        SetResultValue(Integer id, Real value,
                                      const std::string &resultType = "");

   virtual std::string GetProgressString();

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;


   // Access methods overriden from the base class

   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual Real        GetRealParameter(const Integer id) const;
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);

   //virtual Integer     GetIntegerParameter(const Integer id) const;
   //virtual Integer     SetIntegerParameter(const Integer id,
   //                                        const Integer value);
   //virtual bool        GetBooleanParameter(const Integer id) const;
   //virtual bool        SetBooleanParameter(const Integer id,
   //                                        const bool value);
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value);
   // compiler complained again - so here they are ....
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool        SetStringParameter(const std::string &label,
                                          const std::string &value);
   virtual std::string GetStringParameter(const Integer id,
                                          const Integer index) const;
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value,
                                          const Integer index);
   virtual std::string GetStringParameter(const std::string &label,
                                          const Integer index) const;
   virtual bool        SetStringParameter(const std::string &label, 
                                          const std::string &value,
                                          const Integer index);
   virtual const StringArray&
                       GetStringArrayParameter(const Integer id) const;
   virtual bool        TakeAction(const std::string &action,
                                  const std::string &actionData = "");


//------------------------------------------------------------------------------
//  bool  Optimize()    <pure virtual>
//------------------------------------------------------------------------------
/**
 * This method performs the optimization.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
   virtual bool        Optimize() = 0;

protected:
   // Parameter IDs
   enum
   {
      OBJECTIVE_FUNCTION = SolverParamCount,
      OPTIMIZER_TOLERANCE,
      EQUALITY_CONSTRAINT_NAMES,
      INEQUALITY_CONSTRAINT_NAMES,
      PLOT_COST_FUNCTION,
      OptimizerParamCount
   };
   
   /// Flag that is toggled when a Minimize command registers itself
   bool              objectiveDefined;
   /// name of the objective function
   std::string       objectiveFnName;
   /// latest value obtained for the objective function
   Real              cost;
   /// tolerance for determining convergence - the optimizer has converged 
   /// on a solution when the magnitude of the gradient of the cost function 
   /// is smaller than this tolerance
   Real              tolerance;
   /// flag indicating when the optimizer has reached an acceptable 
   /// value for the objective function
   bool              converged;
   /// the number of equality constraints defined for this optimization
   Integer           eqConstraintCount;
   /// the number of inequality constraints defined for this optimization
   Integer           ineqConstraintCount;
   /// current value of the objective function
   //Real              objectiveValue; - this is cost
   /// names of the equality constraint variables
   StringArray       eqConstraintNames;
   /// names of the inequality constraint variables
   StringArray       ineqConstraintNames;
   /// array of equality constraint values
   std::vector<Real> eqConstraintValues;
   /// array of inequality constraint values
   std::vector<Real> ineqConstraintValues;
   /// most recently calculated gradient of the objective function
   std::vector<Real> gradient;   // ********* future ***********
   /// most recently calculated Jacobian of the equality constraints
   Rmatrix           eqConstraintJacobian;  // size of this?
   /// most recently calculated Jacobian of the inequality constraints
   Rmatrix           ineqConstraintJacobian;  // size of this?

   static const std::string    PARAMETER_TEXT[OptimizerParamCount -
                                              SolverParamCount];
   static const Gmat::ParameterType
                               PARAMETER_TYPE[OptimizerParamCount -
                                              SolverParamCount];
   static const Integer        EQ_CONST_START;
   static const Integer        INEQ_CONST_START;
 
   virtual void      FreeArrays();
};

#endif // Optimizer_hpp
