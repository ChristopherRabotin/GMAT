//$Header$
//------------------------------------------------------------------------------
//                                If 
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
 * Declaration for the IF command class
 */
//------------------------------------------------------------------------------


#ifndef If_hpp
#define If_hpp

#include "gmatdefs.hpp"
#include "ConditionalBranch.hpp"
#include "Parameter.hpp"

/**
 * Command that manages processing for entry to the IF statement 
 *
 * The If command manages the If statement.  
 *
 */
class GMAT_API If : public ConditionalBranch
{
public:
   // default constructor
   If();
   // copy constructor
   If(const If &ic);
   // operator =
   If& operator=(const If &ic);
   // destructor
   virtual ~If(void);
   
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
#endif  // If_hpp
