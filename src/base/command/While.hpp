//$Header$
//------------------------------------------------------------------------------
//                                While
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Wendy Shoan/GSFC/MAB
// Created: 2004/08/10
//
/**
* Definition for the While command class
 */
//------------------------------------------------------------------------------


#ifndef While_hpp
#define While_hpp

#include "gmatdefs.hpp"
#include "BranchCommand.hpp"
#include "Parameter.hpp"

/**
* Command that manages processing for entry to the While statement
 *
 * The While command manages the While statement.
 *
 */
class GMAT_API While : public BranchCommand
{
public:
   // default constructor
   While();
   // copy constructor
   While(const While &wc);
   // operator =
   While& operator=(const While &wc);
   // destructor
   virtual ~While(void);

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
#endif  // While_hpp
