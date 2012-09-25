//$Id$
//------------------------------------------------------------------------------
//                             EndFor
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurganus 
// Created: 2004/01/29
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
   EndFor();
   EndFor(const EndFor& ef);
   EndFor&                 operator=(const EndFor& ef);
   virtual ~EndFor();

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

   DEFAULT_TO_NO_CLONES
};


#endif // EndFor_hpp
