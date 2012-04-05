//$Id$
//------------------------------------------------------------------------------
//                                  NoOp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2003/10/24
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Null operator for the command sequence -- typically used as a place holder
 */
//------------------------------------------------------------------------------



#ifndef NoOp_hpp
#define NoOp_hpp

#include "GmatCommand.hpp" // inheriting class's header file

/**
 * Default command used to initialize the command sequence lists in the 
 * Moderator
 */
class GMAT_API NoOp : public GmatCommand
{
public:
   NoOp();
   virtual ~NoOp();
   NoOp(const NoOp& noop);
   NoOp&                   operator=(const NoOp &noop);

   bool                    Execute();

   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS
};


#endif // NoOp_hpp
