//$Header$
//------------------------------------------------------------------------------
//                                  Achieve
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/22
//
/**
 * Definition for the Achieve command class
 */
//------------------------------------------------------------------------------


#ifndef Achieve_hpp
#define Achieve_hpp
 

#include "Command.hpp"
#include "Solver.hpp"
#include "Parameter.hpp"

/**
 * Command that manages processing for targeter goals.
 */
class Achieve : public GmatCommand
{
public:
   Achieve(void);
   virtual ~Achieve(void);
    
   Achieve(const Achieve& t);
   Achieve&             operator=(const Achieve& t);


   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;

   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);
   
   // Parameter accessors
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual Real        GetRealParameter(const Integer id) const;
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value);
                                           
   virtual bool        SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                    const std::string &name = "");
    
    // Inherited methods overridden from the base class
   virtual bool        InterpretAction(void);
   virtual bool        Initialize(void);
   virtual bool        Execute(void);
    
protected:
   /// The name of the spacecraft that gets maneuvered
   std::string         targeterName;
   /// Name of the goal
   std::string         goalName;
   /// Target value for the goal
   Real                goal;
   /// String form of target value for the goal
   std::string         goalString;
   /// Parameter used for floating end point goals
   Parameter           *goalTarget;
   /// Accuracy needed for the goal
   Real                tolerance;
   /// Pointer to the object that owns the goal
   GmatBase            *goalObject;
   /// Object ID for the parameter
   Integer             parmId;
   /// Targeter ID for the goal 
   Integer             goalId;
   /// The targeter instance used to manage the targeter state machine
   Solver              *targeter;
   /// Flag used to finalize the targeter data during execution
   bool                targeterDataFinalized;
   /// Class that performs the goal calculation
   Parameter           *goalParm;
    
   // Parameter IDs
   enum {
      targeterNameID = GmatCommandParamCount,
      goalNameID,
      goalID,
      toleranceID,
      AchieveParamCount
   };
   static const std::string
                     PARAMETER_TEXT[AchieveParamCount - GmatCommandParamCount];
   static const Gmat::ParameterType
                     PARAMETER_TYPE[AchieveParamCount - GmatCommandParamCount];


   bool                InterpretParameter(const std::string text,
                                 std::string &paramType,
                                 std::string &paramObj,
                                 std::string &parmSystem);
   bool                ConstructGoal(const char* str);
};


#endif  // Achieve_hpp
