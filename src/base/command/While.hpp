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
#include "ConditionalBranch.hpp"
#include "Parameter.hpp"

/**
* Command that manages processing for entry to the While statement
 *
 * The While command manages the While statement.
 *
 */
class GMAT_API While : public ConditionalBranch
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
   virtual bool         Execute();

   // inherited from GmatBase
   virtual GmatBase*    Clone() const;

protected:

   /// Counter to track how deep the If nesting is
   Integer                  nestLevel;
   /// The object array used in GetRefObjectArray()
   ObjectArray              objectArray;

};
#endif  // While_hpp
