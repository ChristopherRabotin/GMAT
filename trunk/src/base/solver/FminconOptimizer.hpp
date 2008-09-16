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
   virtual StringArray AdvanceNestedState(std::vector<Real> vars);
   virtual bool        Optimize();

   // inherited from GmatBase
   virtual GmatBase*   Clone() const;
   virtual void        Copy(const GmatBase* orig);
   //virtual bool        ExecuteCallback();// *TEMPORARY ******************************//

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
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value);
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
   //virtual bool        TakeAction(const std::string &action,
   //                               const std::string &actionData = "");
/*
   virtual const std::string&  
                       GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                           const std::string &prefix = "",
                                           const std::string &useName = "");
*/
   virtual void        WriteParameters(Gmat::WriteMode mode, std::string &prefix, 
                                       std::stringstream &stream);
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
   
   Integer      fminconExitFlag;
 
   static const std::string    PARAMETER_TEXT[FminconOptimizerParamCount -
                                              ExternalOptimizerParamCount];
   static const Gmat::ParameterType
                               PARAMETER_TYPE[FminconOptimizerParamCount -
                                              ExternalOptimizerParamCount];
   static const std::string    ALLOWED_OPTIONS[12];
   static const std::string    DEFAULT_OPTION_VALUES[12];
   static const Integer        NUM_MATLAB_OPTIONS;
   static const Integer        MATLAB_OPTIONS_OFFSET;
                                              
 
   // Methods from Solver
   virtual void                CompleteInitialization();
   virtual void                RunExternal();
   virtual void                RunNominal();
   virtual void                CalculateParameters();
   virtual void                RunComplete();

   virtual void                FreeArrays();
   //virtual std::string         GetProgressString(); // moved to Optimizer
   virtual void                WriteToTextFile(
                                  SolverState stateToUse = UNDEFINED_STATE);
   
   virtual bool                OpenConnection();
   virtual void                CloseConnection();
   
   virtual bool                IsAllowedOption(const std::string &str);
   virtual bool                IsAllowedValue(const std::string &opt,
                                              const std::string &val);
};

#endif // FminconOptimizer_hpp
