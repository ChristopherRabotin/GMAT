//$Id$
//------------------------------------------------------------------------------
//                                  Stop
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
 * Stop function for the command sequence -- typically used while debugging
 */
//------------------------------------------------------------------------------



#ifndef Stop_hpp
#define Stop_hpp

#include "GmatCommand.hpp" // inheriting class's header file

/**
 * Default command used to initialize the command sequence lists in the 
 * Moderator
 */
class GMAT_API Stop : public GmatCommand
{
public:
   Stop();
   virtual ~Stop();
   Stop(const Stop& noop);
   Stop&                   operator=(const Stop &noop);

   bool                    Execute();

   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   virtual const std::string&
                     GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                         const std::string &prefix = "",
                                         const std::string &useName = "");

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS
};


#endif // Stop_hpp
