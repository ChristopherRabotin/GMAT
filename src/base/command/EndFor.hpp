//$Header$
//------------------------------------------------------------------------------
//                             EndFor
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurganus 
// Created: 2004/01/29
// Modified: W. Shoan 2004.09.13 - Updated for use in Build 3
//
/**
 * Definition for the closing line of a for loop
 */
//------------------------------------------------------------------------------


#ifndef EndFor_hpp
#define EndFor_hpp


#include "GmatCommand.hpp"


class GMAT_API EndFor : public GmatCommand
{
public:
   EndFor(void);
   virtual ~EndFor(void);
   
   EndFor(const EndFor& ef);
   EndFor&          operator=(const EndFor& ef);
   
   virtual bool            Initialize(void);
   virtual bool            Execute(void);
   
   virtual bool            Insert(GmatCommand *cmd, GmatCommand *prev);
   
   // inherited from GmatBase
   virtual bool            RenameRefObject(const Gmat::ObjectType type,
                                           const std::string &oldName,
                                           const std::string &newName);   
   virtual GmatBase*       Clone(void) const;
   const std::string&      GetGeneratingString(Gmat::WriteMode mode,
                                               const std::string &prefix,
                                               const std::string &useName);
};


#endif // EndFor_hpp
