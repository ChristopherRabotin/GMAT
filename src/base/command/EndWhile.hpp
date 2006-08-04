//$Header$
//------------------------------------------------------------------------------
//                             EndWHile
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Wendy Shoan/GSFC/MAB
// Created: 2004/08/11
//
/**
* Definition for the closing line of the While Statement
 */
//------------------------------------------------------------------------------


#ifndef EndWhile_hpp
#define EndWhile_hpp


#include "GmatCommand.hpp"


class EndWhile : public GmatCommand
{
public:
   EndWhile();
   virtual ~EndWhile();

   EndWhile(const EndWhile& ew);
   EndWhile&              operator=(const EndWhile& ew);

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


#endif // EndWhile_hpp
