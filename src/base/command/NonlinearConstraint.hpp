//$Header$
//------------------------------------------------------------------------------
//                                  NonlinearConstraint
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy Shoan (GSFC/MAB)
// Created: 2006.08.14
//
/**
 * Definition for the NonlinearConstraint command class
 */
//------------------------------------------------------------------------------


#ifndef NonlinearConstraint_hpp
#define NonlinearConstraint_hpp
 

#include "GmatCommand.hpp"
#include "Solver.hpp"
#include "Parameter.hpp"

/**
 * Command that manages processing for targeter goals.
 */
class NonlinearConstraint : public GmatCommand
{
public:
   NonlinearConstraint();
   NonlinearConstraint(const NonlinearConstraint& nlc);
   NonlinearConstraint&           operator=(const NonlinearConstraint& nlc);
   virtual ~NonlinearConstraint();

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
   virtual bool        Initialize();
   virtual bool        Execute();
   
   virtual const std::string&
                       GetGeneratingString(Gmat::WriteMode mode,
                                           const std::string &prefix,
                                           const std::string &useName);
    
protected:
   // Parameter IDs
   enum 
   {
      OPTIMIZER_NAME = GmatCommandParamCount,
      CONSTRAINT_VARIABLE_NAME,
      OPERATOR,
      CONSTRAINT_VALUE,
      TOLERANCE,
      NonlinearConstraintParamCount
   };
   static const std::string
                       PARAMETER_TEXT[NonlinearConstraintParamCount - GmatCommandParamCount];
   static const Gmat::ParameterType
                       PARAMETER_TYPE[NonlinearConstraintParamCount - GmatCommandParamCount];

   enum Operator
   {
      LESS_THAN_OR_EQUAL = 0,
      EQUAL,
      GREATER_THAN_OR_EQUAL
   };

   /// The name of the spacecraft that gets maneuvered
   std::string         optimizerName;
   /// The optimizer instance used to manage the optimizer state machine
   Solver              *optimizer;
   /// Name of the variable to be constrained
   std::string         constraintName;
   // pointer to the Variable that is to be minimized
   Parameter           *constraint;
   /// most recent value of the variable
   Real                constraintValue;
   /// name of the parameter part of the right-hand-side
   std::string         nlcParmName;
   Parameter           *nlcParm;

   /// String of value array name
   std::string         nlcArrName;
   /// constraint array row index variable name
   std::string         nlcArrRowStr;
   /// constraint array column index variable name
   std::string         nlcArrColStr;
   /// constraint array row index
   Integer             nlcArrRow;
   /// constraint array column index
   Integer             nlcArrCol;
   Parameter           *nlcArrRowParm;
   Parameter           *nlcArrColParm;

   /// flag indicating whether or not the constraint is an inequality
   /// constraint
   bool                isInequality;
   /// string to send into the optimizer, based on isInequality
   std::string         isIneqString;
   /// the desired value (right hand side of the constraint equation)
   Real                desiredValue;
   /// indicates what type of operator was passed in for the generating
   /// string
   Operator            op;
   /// tolerance for the constraint <future>
   Real                tolerance;  // <future>
   /// Flag used to finalize the targeter data during execution
   bool                optimizerDataFinalized;
   /// ID for this constraint (returned from the optimizer)
   Integer             constraintId;
   /// is the right hand side a parameter?
   bool                isNlcParm;
   /// is the right hand side an array?
   bool                isNlcArray;
   /// Pointer to the object that owns the goal
   GmatBase            *constraintObject;
   /// Object ID for the parameter
   Integer             parmId;
   /// flag indicating whether or not the generating string has been interpreted
   bool                interpreted;
   
   bool                InterpretParameter(const std::string text,
                                          std::string &paramType,
                                          std::string &paramObj,
                                          std::string &parmSystem);
                                          

   bool                ConstructConstraint(const char* str);

};


#endif  // NonlinearConstraint_hpp

