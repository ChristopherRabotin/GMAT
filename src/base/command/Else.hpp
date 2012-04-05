//$Id$
//------------------------------------------------------------------------------
//                              Else 
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
// Author:  Joey Gurgnaus
// Created: 2004/01/30
//
/**
 * Definition for the ELSE command class
 */
//------------------------------------------------------------------------------


#ifndef Else_hpp
#define Else_hpp
 
#include "GmatCommand.hpp"

/**
 * Command that manages processing for entry to the ELSE statement 
 *
 *
 */
class GMAT_API Else : public GmatCommand
{
public:
   Else();
   virtual ~Else();
    
   Else(const Else& ec);
   Else&               operator=(const Else& ec);
    
   // Inherited methods that need some enhancement from the base class
   virtual bool        Insert(GmatCommand *cmd, GmatCommand *prev);
    
   // Methods used to run the command
   virtual bool        Initialize();
   virtual bool        Execute(); 

   // inherited from GmatBase
   virtual bool        RenameRefObject(const Gmat::ObjectType type,
                                       const std::string &oldName,
                                       const std::string &newName);   
   virtual GmatBase* Clone() const;
   virtual const std::string&
                       GetGeneratingString(Gmat::WriteMode mode,
                                           const std::string &prefix,
                                           const std::string &useName);
   DEFAULT_TO_NO_CLONES
   
protected:
    
};
#endif  // Else_hpp
