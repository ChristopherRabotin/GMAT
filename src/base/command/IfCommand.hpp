//$Header$
//------------------------------------------------------------------------------
//                                IfCommand 
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurgnaus
// Created: 2004/01/30
//
/**
 * Definition for the IF command class
 */
//------------------------------------------------------------------------------


#ifndef IfCommand_hpp
#define IfCommand_hpp

#include "gmatdefs.hpp"
#include "BranchCommand.hpp"
#include "Parameter.hpp"

/**
 * Command that manages processing for entry to the IF statement 
 *
 * The IfCommand command manages the If statement.  
 *
 */
class GMAT_API IfCommand : public BranchCommand
{
public:
   // default constructor
   IfCommand();
   // copy constructor
   IfCommand(const IfCommand &ic);
   // operator =
   IfCommand& operator=(const IfCommand &ic);
   // destructor
   virtual ~IfCommand(void);
   
   // Inherited methods that need some enhancement from the base class
   virtual bool         Append(GmatCommand *cmd);
   
   // Methods used to run the command
   virtual bool         Initialize();
   virtual bool         Execute();
   
   // Method to set up the condition(s) for the If command
   virtual bool         SetCondition(std::string lhs, std::string operation,
                                       std::string rhs);
   
   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   // Reference object accessor methods ----- wcs: 2004.07.08 added
   //virtual std::string  GetRefObjectName(const Gmat::ObjectType type) const;
   //virtual bool         SetRefObjectName(const Gmat::ObjectType type,
   //                                       const std::string &name);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                       const std::string &name,
                                       const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                       const std::string &name,
                                       const Integer index);
   //virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type);
   
    
    
protected:

   bool                 EvaluateCondition(Integer which);

   enum OpType
   {
      EQUAL_TO = 0,
      NOT_EQUAL,
      GREATER_THAN,
      LESS_THAN,
      GREATER_OR_EQUAL,
      LESS_OR_EQUAL,
      NumberOfOperators
   };

   static const std::string OPTYPE_TEXT[NumberOfOperators];

   /// Number of consitions for the If
   Integer                  numberOfConditions;
   /// Counter to track how deep the If nesting is
   Integer                  nestLevel;
   /// The object array used in GetRefObjectArray()
   ObjectArray              objectArray;
   /// Arrays representing conditions
   StringArray              lhsList;
   std::vector<OpType>      opList;
   StringArray              rhsList;
   /// list of parameter objects used by the conditions
   std::vector<Parameter*>   params;
   
};
#endif  // IfCommand_hpp
