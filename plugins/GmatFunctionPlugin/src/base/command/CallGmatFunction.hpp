//$Id: CallFunction.hpp 6700 2009-05-22 19:17:23Z lindajun $
//------------------------------------------------------------------------------
//                                 CallGmatFunction
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
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Declaration for the CallGmatFunction command class
 */
//------------------------------------------------------------------------------
#ifndef CallGmatFunction_hpp
#define CallGmatFunction_hpp

#include "GmatFunction_defs.hpp"
#include "CallFunction.hpp"

class GMATFUNCTION_API CallGmatFunction : public CallFunction
{
public:
   CallGmatFunction();
   virtual ~CallGmatFunction();

   CallGmatFunction(const CallGmatFunction& cf);
   CallGmatFunction&    operator=(const CallGmatFunction& cf);
   
   // override GmatCommand methods
   virtual bool         Initialize();
   virtual bool         Execute();
   virtual void         RunComplete();
   
   // override GmatBase methods
   virtual GmatBase*    Clone() const;
   
protected:

   
};


#endif // CallGmatFunction_hpp
