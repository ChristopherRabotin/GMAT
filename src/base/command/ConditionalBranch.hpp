//$Header$
//------------------------------------------------------------------------------
//                                ConditionalBranch 
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Wendy Shoan
// Created: 2004/09/27
//
/**
 * Declaration for the ConditionalBranch command class
 */
//------------------------------------------------------------------------------


#ifndef ConditionalBranch_hpp
#define ConditionalBranch_hpp

#include "gmatdefs.hpp"
#include "BranchCommand.hpp"
#include "Parameter.hpp"

/**
 * Command that manages processing for entry to the conditional
 * branch commands. 
 *
 * The ConditionalBranch command manages the conditional branch
 * commands.  
 *
 */
class GMAT_API ConditionalBranch : public BranchCommand
{
public:
   // default constructor
   ConditionalBranch(const std::string &typeStr);
   // copy constructor
   ConditionalBranch(const ConditionalBranch &cb);
   // operator =
   ConditionalBranch& operator=(const ConditionalBranch &cb);
   // destructor
   virtual ~ConditionalBranch();
         
   // Method to set up the condition(s) for the conditional branch commands
   virtual bool         SetCondition(std::string lhs, std::string operation,
                                       std::string rhs);
   virtual bool         SetConditionOperator(std::string& op);

   virtual bool         Initialize();
   
   // inherited from GmatBase
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);
    
protected:

   virtual bool         EvaluateCondition(Integer which);
   virtual bool         EvaluateAllConditions();

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
   
   enum LogicalOpType
   {
      AND = 0,
      OR,
      NumberOfLogicalOperators
   };

   static const std::string OPTYPE_TEXT[NumberOfOperators];
   static const std::string LOGICAL_OPTYPE_TEXT[NumberOfLogicalOperators];

   /// Number of conditions for the conditional branch commands
   Integer                    numberOfConditions;
   /// Number  of separating logical operators (should be numberOfConditions - 1)
   Integer                    numberOfLogicalOps;
   /// Arrays representing conditions
   StringArray                lhsList;
   std::vector<OpType>        opList;
   StringArray                rhsList;
   std::vector<LogicalOpType> logicalOpList;
   /// list of parameter objects used by the conditions
   std::vector<Parameter*>    params;
   
};
#endif  // ConditionalBranch_hpp
