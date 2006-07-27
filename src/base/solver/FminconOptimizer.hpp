//$Header$
//------------------------------------------------------------------------------
//                         FminconOptimizer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy C. Shoan, GSFC
// Created: 2006.07.21
//
/**
 * Definition for the FminconOptimizer class. 
 */
//------------------------------------------------------------------------------


#ifndef FminconOptimizer_hpp
#define FminconOptimizer_hpp


#include "gmatdefs.hpp"
#include "ExternalOptimizer.hpp"
#include "GmatInterface.hpp"    // a singleton  - currently no Interface base class
//#include "MatlabInterface.hpp"

class GMAT_API FminconOptimizer : public ExternalOptimizer
{
public:
   FminconOptimizer(std::string name);
   virtual ~FminconOptimizer();
   FminconOptimizer(const FminconOptimizer &opt);
   FminconOptimizer&   operator=(const FminconOptimizer& opt);

   virtual bool        Initialize();
   virtual SolverState AdvanceState();
   virtual bool        Optimize();

   // inherited from GmatBase
   virtual GmatBase*   Clone() const;

   // Access methods overriden from the base class
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   //virtual Integer     GetIntegerParameter(const Integer id) const;
   //virtual Integer     SetIntegerParameter(const Integer id,
   //                                        const Integer value);
   //virtual bool        GetBooleanParameter(const Integer id) const;
   //virtual bool        SetBooleanParameter(const Integer id,
   //                                        const bool value);
   //virtual std::string GetStringParameter(const Integer id) const;
   //virtual bool        SetStringParameter(const Integer id,
   //                                       const std::string &value);
   virtual const StringArray&
                       GetStringArrayParameter(const Integer id) const;
   //virtual bool        TakeAction(const std::string &action,
   //                               const std::string &actionData = "");


protected:
   // Parameter IDs
   enum
   {
      OPTIONS = ExternalOptimizerParamCount,
      OPTION_VALUES,
      FminconOptimizerParamCount
   };
   
   StringArray  options;
   StringArray  optionValues;
 
   static const std::string    PARAMETER_TEXT[FminconOptimizerParamCount -
                                              ExternalOptimizerParamCount];
   static const Gmat::ParameterType
                               PARAMETER_TYPE[FminconOptimizerParamCount -
                                              ExternalOptimizerParamCount];
 
   // Methods from Solver
   virtual void                RunNominal();
   //virtual void                RunPerturbation();
   virtual void                CalculateParameters();
   //virtual void                CheckCompletion();
   virtual void                RunComplete();


   virtual void                FreeArrays();
   virtual std::string         GetProgressString();
   virtual void                WriteToTextFile();
   
   virtual bool                OpenConnection();
   virtual void                CloseConnection();
};

#endif // FminconOptimizer_hpp
