
//$Header$
//------------------------------------------------------------------------------
//                                  ForFor
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurgnaus
// Created: 2004/01/22
// Modified: W. Shoan 2004.09.13 - updated for use in Build 3
//
/**
 * Definition for the For command class
 */
//------------------------------------------------------------------------------


#ifndef For_hpp
#define For_hpp
 

#include "BranchCommand.hpp"


/**
 * Command that manages processing for entry to the For loop
 *
 * The For command manages the for loop.  
 *
 */
class For : public BranchCommand
{
public:
  // default constructor
   For(void);
   // copy constructor
   For(const For& f);
   // destructor
   virtual ~For(void);

   // operator = 
   For&             operator=(const For& f);

   // Inherited methods that need some enhancement from the base class
   virtual bool        Append(GmatCommand *cmd);

   // Methods used to run the command
   virtual bool        Initialize(void);
   virtual bool        Execute(void);

   // inherited from GmatBase
   virtual GmatBase*   Clone(void) const;

   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
      GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   
   virtual Real        GetRealParameter(const Integer id) const;
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);
   virtual Real        GetRealParameter(const std::string &label) const;
   virtual Real        SetRealParameter(const std::string &label,
                                        const Real value);

    
protected:
   enum
   {
      START_VALUE = BranchCommandParamCount,
      END_VALUE,
      STEP,
      ForParamCount
   };

   static const std::string PARAMETER_TEXT[
      ForParamCount - BranchCommandParamCount];

   static const Gmat::ParameterType PARAMETER_TYPE[
      ForParamCount - BranchCommandParamCount];
   
   static const Real UNINITIALIZED_VALUE;
   static const Real DEFAULT_START;
   static const Real DEFAULT_END;
   static const Real DEFAULT_INCREMENT;

   std::string         forName;
   /// Start value for the For loop
   Real                startValue;
   /// End value for the For loop
   Real                endValue;
   /// Step value for the For loop
   Real                stepSize;
   /// Current value for the For loop counter
   Real                currentValue;
   
   // method to evaluate the counter to see if we are still looping
   bool StillLooping();
    
};
#endif  // For_hpp
