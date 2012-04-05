//$Id$
//------------------------------------------------------------------------------
//                            EndFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// order 124.
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/08/30
//
/**
 * Definition code for the EndFunction command, a wrapper that marks the end
 * of the commands in a GMAT function.
 */
//------------------------------------------------------------------------------
 

#ifndef EndFunction_hpp
#define EndFunction_hpp


#include "GmatCommand.hpp"


class GMAT_API EndFunction : public GmatCommand
{
public:
   EndFunction();
   virtual ~EndFunction();
   EndFunction(const EndFunction& ef);
   EndFunction&         operator=(const EndFunction& ef);
   
   virtual GmatBase*    Clone() const;
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual bool         Execute();
   
   DEFAULT_TO_NO_CLONES

protected:   
   /// Name of the function
   std::string          functionName;
};

#endif /* EndFunction_hpp */
