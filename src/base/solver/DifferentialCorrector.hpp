//$Id$
//------------------------------------------------------------------------------
//                         DifferentialCorrector
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/29
//
/**
 * Definition for the differential corrector targeter. 
 */
//------------------------------------------------------------------------------


#ifndef DifferentialCorrector_hpp
#define DifferentialCorrector_hpp


#include "Solver.hpp"
#include <fstream>          // for std::ofstream

/**
 * This class implements the first targeter in GMAT.
 * 
 * @todo refactor this class with the Solver class so that elements common to
 *       targeting, scanning, estimating and optimizing are all in the base
 *       class.  This task should be done when the first instance of one of the
 *       other approaches is implemented.
 */
class GMAT_API DifferentialCorrector : public Solver
{
public:
   DifferentialCorrector(std::string name);
   virtual ~DifferentialCorrector();
   DifferentialCorrector(const DifferentialCorrector &dc);
   DifferentialCorrector& operator=(const DifferentialCorrector& dc);
   
   virtual bool        Initialize();
   virtual SolverState AdvanceState();
   
   // inherited from GmatBase
   virtual GmatBase*   Clone() const;
   virtual void        Copy(const GmatBase* orig);
   
   // Access methods overriden from the base class
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual Integer     GetIntegerParameter(const Integer id) const;
   virtual Integer     SetIntegerParameter(const Integer id,
                                           const Integer value);
   virtual bool        GetBooleanParameter(const Integer id) const;
   virtual bool        SetBooleanParameter(const Integer id,
                                           const bool value);
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value);
   virtual const StringArray&
                       GetStringArrayParameter(const Integer id) const;
   virtual bool        TakeAction(const std::string &action,
                                  const std::string &actionData = "");

   // Solver interfaces used to talk to the Vary and Achieve commands
   //virtual Integer     SetSolverVariables(Real *data, const std::string &name);
   //virtual Real        GetSolverVariable(Integer id);
   virtual Integer     SetSolverResults(Real *data, const std::string &name,
                                        const std::string &type = "");
   virtual bool        UpdateSolverGoal(Integer id, Real newValue);
   virtual bool        UpdateSolverTolerance(Integer id, Real newValue);
   virtual void        SetResultValue(Integer id, Real value,
                                      const std::string &resultType = "");

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   // Core data members used for the targeter numerics
   /// The number of goals in the targeting problem
   Integer                     goalCount;
   /// Value desired for the goals
   Real                        *goal;
   /// Accuracy for the goals
   Real                        *tolerance;
   /// Array used to track the achieved value during a nominal run
   Real                        *nominal;
   /// Array used to track the achieved values when variables are perturbed
   Real                        **achieved;
   /// Array used to track the backwards perts for central differencing
   Real                        **backAchieved;
   /// The sensitivity matrix
   Real                        **jacobian;
   /// The inverted sensitivity matrix
   Real                        **inverseJacobian;

   /// Permutation vector used by the LU Decomposition routine.
   Integer                     *indx;
   /// Vector used for the back substitution
   Real                        *b;
    
   // Control parameters

   /// Text describing how differences should be generated
   std::string                 derivativeMethod;
   /// Mode flag for differencing
   Integer                     diffMode; // 1 for forward, -1 for backward,
                                         // 0 for central
   /// Flag used to track place in central differencing
   bool                        firstPert;
   /// Flag used to indicate if it is time to move to next pert
   bool                        incrementPert;

   /// List of goals
   StringArray                 goalNames;
    
   // Parameter IDs
   enum
   {
      goalNamesID = SolverParamCount,
      derivativeMethodID,
      DifferentialCorrectorParamCount
   };

   static const std::string    PARAMETER_TEXT[DifferentialCorrectorParamCount -
                                              SolverParamCount];
   static const Gmat::ParameterType
                               PARAMETER_TYPE[DifferentialCorrectorParamCount -
                                              SolverParamCount];

   // Methods
   virtual void                RunNominal();
   virtual void                RunPerturbation();
   virtual void                CalculateParameters();
   virtual void                CheckCompletion();
   virtual void                RunComplete();

   // Methods used to perform differential correction
   void                        CalculateJacobian();
   void                        InvertJacobian();

   void                        FreeArrays();
   virtual std::string         GetProgressString();
   virtual void                WriteToTextFile(
                                  SolverState stateToUse = UNDEFINED_STATE);
};

#endif // DifferentialCorrector_hpp
