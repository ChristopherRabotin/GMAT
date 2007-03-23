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
 

#include "GmatCommand.hpp"
#include "Solver.hpp"
#include "Parameter.hpp"
#include "ElementWrapper.hpp"

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
   virtual GmatBase* Clone() const;

   virtual bool        RenameRefObject(const Gmat::ObjectType type,
                                       const std::string &oldName,
                                       const std::string &newName);
   
   virtual const ObjectTypeArray&
                       GetRefObjectTypeArray();
   virtual const StringArray&
                       GetRefObjectNameArray(const Gmat::ObjectType type);
   
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
   virtual bool        InterpretAction();
   virtual const StringArray& 
                       GetWrapperObjectNameArray();
   virtual bool        SetElementWrapper(ElementWrapper* toWrapper,
                                         const std::string &withName);
   virtual void        ClearWrappers();

   virtual bool        Initialize();
   virtual bool        Execute();
   virtual void        RunComplete();
   
   virtual const std::string&
                       GetGeneratingString(Gmat::WriteMode mode,
                                           const std::string &prefix,
                                           const std::string &useName);
    
protected:
   /// The name of the targeter
   std::string         targeterName;
   /// Name of the goal
   std::string         goalName;
   /// Target value for the goal
   //Real                goal;
   ElementWrapper      *goal;   // can be any kind of wrapper except a NumberWrapper
   /// String form of target value for the goal
   std::string         achieveName;  // arg1
   /// Parameter used for floating end point goals
   //Parameter           *achieveParm;
   ElementWrapper      *achieve;   // arg1
   /// Accuracy needed for the goal
   //Real                tolerance;
   std::string         toleranceName;
   /// the tolerance object
   ElementWrapper      *tolerance;
   /// Pointer to the object that owns the goal
   //GmatBase            *goalObject;
   /// Targeter ID for the goal 
   Integer             goalId;
   /// The targeter instance used to manage the targeter state machine
   Solver              *targeter;
   /// Flag used to finalize the targeter data during execution
   bool                targeterDataFinalized;
   /// String of goal array name
   //std::string         achieveArrName;
   /// Goal array row index variable name
   //std::string         achieveArrRowStr;
   /// Goal array column index variable name
   //std::string         achieveArrColStr;
   /// Goal array row index
   //Integer             achieveArrRow;
   /// Goal array row index
   //Integer             achieveArrCol;
   //Parameter           *achieveArrRowParm;
   //Parameter           *achieveArrColParm;
   /// Flag use to indicate goal target is a parameter
   //bool                isAchieveParm;
   /// Flag use to indicate goal target is a array
   //bool                isAchieveArray;
   /// Object ID for the parameter
   //Integer             parmId;
   /// Class that performs the goal calculation
   //Parameter           *goalParm;
   
   void SetTolerance(Real value);
   
   // Parameter IDs
   enum {
      targeterNameID = GmatCommandParamCount,
      goalNameID,
      goalValueID,
      toleranceID,
      AchieveParamCount
   };
   static const std::string
                     PARAMETER_TEXT[AchieveParamCount - GmatCommandParamCount];
   static const Gmat::ParameterType
                     PARAMETER_TYPE[AchieveParamCount - GmatCommandParamCount];


   //bool                InterpretParameter(const std::string text,
   //                              std::string &paramType,
   //                              std::string &paramObj,
   //                              std::string &parmSystem);
   //bool                ConstructGoal(const char* str);
};


#endif  // Achieve_hpp
