//$Header$
//------------------------------------------------------------------------------
//                                EndTarget
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/23
//
/**
 * Definition for the closing line of a Targeter loop
 */
//------------------------------------------------------------------------------


#ifndef EndTarget_hpp
#define EndTarget_hpp


#include "GmatCommand.hpp"


class GMAT_API EndTarget : public GmatCommand
{
public:
   EndTarget();
   virtual ~EndTarget();
    
   EndTarget(const EndTarget& et);
   EndTarget&              operator=(const EndTarget& et);
    
   virtual bool            Initialize();
   virtual bool            Execute();
    
   virtual bool            Insert(GmatCommand *cmd, GmatCommand *prev);

   // inherited from GmatBase
   virtual bool            RenameRefObject(const Gmat::ObjectType type,
                                           const std::string &oldName,
                                           const std::string &newName);   
   virtual GmatBase*       Clone() const;
   virtual const std::string&
                           GetGeneratingString(Gmat::WriteMode mode,
                                               const std::string &prefix,
                                               const std::string &useName);
};


#endif // EndTarget_hpp
