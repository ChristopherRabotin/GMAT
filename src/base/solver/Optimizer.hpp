//$Header$
//------------------------------------------------------------------------------
//                         Optimizer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
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
   Optimizer(std::string name);
   virtual ~Optimizer();
   Optimizer(const Optimizer &opt);
   Optimizer&      operator=(const Optimizer& opt);
   
   virtual bool        Initialize();
   //virtual SolverState         AdvanceState();  // do I need this?

   // Solver interfaces used to talk to the Vary and Minimize commands
   virtual Integer     SetSolverVariables(Real *data, const std::string &name);
   virtual Real        GetSolverVariable(Integer id);
   virtual Integer     SetSolverResults(Real *data, const std::string &name);
   virtual void        SetResultValue(Integer id, Real value);


   // Access methods overriden from the base class
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   //virtual Integer     GetIntegerParameter(const Integer id) const;
   //virtual Integer     SetIntegerParameter(const Integer id,
   //                                        const Integer value);
   virtual bool        GetBooleanParameter(const Integer id) const;
   virtual bool        SetBooleanParameter(const Integer id,
                                           const bool value);
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value);
   virtual const StringArray&
                       GetStringArrayParameter(const Integer id) const;
   //virtual bool        TakeAction(const std::string &action,
   //                               const std::string &actionData = "");

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
      OptimizerParamCount = SolverParamCount,
   };
   
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
   /// names of the constraint variables
   StringArray       constraintNames;
   /// array of constraint values
   std::vector<Real> constraintValues;

   static const std::string    PARAMETER_TEXT[OptimizerParamCount -
                                              SolverParamCount];
   static const Gmat::ParameterType
                               PARAMETER_TYPE[OptimizerParamCount -
                                              SolverParamCount];

   // Methods from Solver
   //virtual void                RunNominal();
   //virtual void                RunPerturbation();
   //virtual void                CalculateParameters();
   //virtual void                CheckCompletion();
   //virtual void                RunComplete();

   // Methods used to perform optimization
   //void                        CalculateJacobian();
   //void                        InvertJacobian();

   void                        FreeArrays();
   //virtual std::string         GetProgressString();
   //virtual void                WriteToTextFile();
};

#endif // Optimizer_hpp
