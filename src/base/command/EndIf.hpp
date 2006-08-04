//$Header$
//------------------------------------------------------------------------------
//                             EndIf
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurganus 
// Created: 2004/01/30
//
/**
 * Definition for the closing line of the IF Statement
 */
//------------------------------------------------------------------------------


#ifndef EndIf_hpp
#define EndIf_hpp


#include "GmatCommand.hpp"


class GMAT_API EndIf : public GmatCommand
{
public:
   EndIf();
   virtual ~EndIf();
    
   EndIf(const EndIf& ic);
   EndIf&              operator=(const EndIf& ic);
    
   virtual bool            Initialize();
   virtual bool            Execute();

   virtual bool            Insert(GmatCommand *cmd, GmatCommand *prev);

   // inherited from GmatBase
   virtual bool            RenameRefObject(const Gmat::ObjectType type,
                                           const std::string &oldName,
                                           const std::string &newName);   
   virtual GmatBase*       Clone() const;
   const std::string&      GetGeneratingString(Gmat::WriteMode mode,
                                               const std::string &prefix,
                                               const std::string &useName);
};


#endif // EndIf_hpp
