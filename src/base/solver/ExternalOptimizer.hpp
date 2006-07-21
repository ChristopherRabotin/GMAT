//$Header$
//------------------------------------------------------------------------------
//                         ExternalOptimizer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy C. Shoan, GSFC
// Created: 2006.07.13
//
/**
 * Definition for the ExternalOptimizer base class. 
 */
//------------------------------------------------------------------------------


#ifndef ExternalOptimizer_hpp
#define ExternalOptimizer_hpp

#include "Optimizer.hpp"
#include "GmatInterface.hpp"    // a singleton  - currently no Interface base class

class GMAT_API ExternalOptimizer : public Optimizer
{
public:
   ExternalOptimizer(std::string type, std::string name);
   virtual ~ExternalOptimizer();
   ExternalOptimizer(const ExternalOptimizer &opt);
   ExternalOptimizer&      operator=(const ExternalOptimizer& opt);

   virtual bool        Initialize();

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
   virtual const StringArray&
                       GetStringArrayParameter(const Integer id) const;
   //virtual bool        TakeAction(const std::string &action,
   //                               const std::string &actionData = "");

//------------------------------------------------------------------------------
//  bool  Optimize()    <pure virtual> <from Optimizer>
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
      PARAMETER_LIST = OptimizerParamCount,
      SOURCE_TYPE,
      ExternalOptimizerParamCount
   };
   

   /// list of parameters used to configure the external optimizer
   StringArray         parmList;
   /// array of values for the optimizer - value of the objective funcion;
   /// elements of the gradient (if calculated); values of the constraints
   std::vector<Real>   costConstraintArray;
   /// type of external interface used (as of 2006.07.13, only MATLAB is 
   /// supported)
   std::string         sourceType;
   /// flag indicating whether or not the interface was opened successfully
   /// and the supporting structures needed by the interface were found
   bool                sourceReady;
   /// pointer to the interface object (as of 2006.07.13, two pointers
   /// are needed - one to a MatlabInterface object, one to a GmatServer,
   /// per GMAT Architectural Specification document)
   //std::vector<Interface*> source;   -- cannot do this yet
   GmatInterface       *gmatServer;
   // MatlabInterface is currently an all-static class, so just include
   // the header

   static const std::string    PARAMETER_TEXT[ExternalOptimizerParamCount -
                                              OptimizerParamCount];
   static const Gmat::ParameterType
                               PARAMETER_TYPE[ExternalOptimizerParamCount -
                                              OptimizerParamCount];
 
   // Methods from Solver
   //virtual void                RunNominal();
   //virtual void                RunPerturbation();
   //virtual void                CalculateParameters();
   //virtual void                CheckCompletion();
   //virtual void                RunComplete();

   // Methods used to perform optimization
   //void                        CalculateJacobian();
   //void                        InvertJacobian();

   //void                        FreeArrays();
   //virtual std::string         GetProgressString();
   //virtual void                WriteToTextFile();
   
   virtual bool                  OpenConnection() = 0;
   virtual void                  CloseConnection() = 0;
};

#endif // ExternalOptimizer_hpp
