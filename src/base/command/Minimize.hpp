//$Header$
//------------------------------------------------------------------------------
//                                  Minimize
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Daniel Hunter, Wendy Shoan (GSFC/MAB)
// Created: 2006.08.11
//
/**
 * Definition for the Minimize command class
 */
//------------------------------------------------------------------------------


#ifndef Minimize_hpp
#define Minimize_hpp
 

#include "GmatCommand.hpp"
#include "Solver.hpp"
#include "Parameter.hpp"

/**
 * Command that manages processing for targeter goals.
 */
class Minimize : public GmatCommand
{
public:
   Minimize();
   Minimize(const Minimize& m);
   Minimize&           operator=(const Minimize& m);
   virtual ~Minimize();

   // inherited from GmatBase
   virtual GmatBase*   Clone() const;

   virtual bool        RenameRefObject(const Gmat::ObjectType type,
                                       const std::string &oldName,
                                       const std::string &newName);
   
   // Parameter accessors
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   //virtual Real        GetRealParameter(const Integer id) const;
   //virtual Real        SetRealParameter(const Integer id,
   //                                     const Real value);
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value);
                                           
   virtual bool        SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                    const std::string &name = "");
    
    // Inherited methods overridden from the base class
   virtual bool        InterpretAction(void);
   virtual bool        Initialize(void);
   virtual bool        Execute(void);
   
   virtual const std::string&
                       GetGeneratingString(Gmat::WriteMode mode,
                                           const std::string &prefix,
                                           const std::string &useName);
    
protected:
   /// The name of the spacecraft that gets maneuvered
   std::string         optimizerName;
   /// Name of the variable to be minimized
   std::string         minName;
   // pointer to the Variable that is to be minimized
   Parameter           *minVar;
   /// vallue of the variable
   Real                varValue;
   /// The optimizer instance used to manage the optimizer state machine
   Solver              *optimizer;
   /// Flag used to finalize the targeter data during execution
   bool                optimizerDataFinalized;
   /// is it a parameter?
   bool                isMinimizeParm;
   /// Optimizer ID for the variable 
   Integer             varId;

   
   // Parameter IDs
   enum 
   {
      OPTIMIZER_NAME = GmatCommandParamCount,
      MINIMIZED_VARIABLE_NAME,
      MinimizeParamCount
   };
   static const std::string
                       PARAMETER_TEXT[MinimizeParamCount - GmatCommandParamCount];
   static const Gmat::ParameterType
                       PARAMETER_TYPE[MinimizeParamCount - GmatCommandParamCount];


   bool                InterpretParameter(const std::string text,
                                          std::string &paramType,
                                          std::string &paramObj,
                                          std::string &parmSystem);
   bool                ConstructGoal(const char* str);
};


#endif  // Minimize_hpp