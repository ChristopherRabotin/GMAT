//$Id$
//------------------------------------------------------------------------------
//                                If 
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
 * Declaration for the IF command class
 */
//------------------------------------------------------------------------------


#ifndef If_hpp
#define If_hpp

#include "gmatdefs.hpp"
#include "ConditionalBranch.hpp"
#include "Parameter.hpp"

/**
 * Command that manages processing for entry to the IF statement 
 *
 * The If command manages the If statement.  
 *
 */
class GMAT_API If : public ConditionalBranch
{
public:
   // default constructor
   If();
   // copy constructor
   If(const If &ic);
   // operator =
   If& operator=(const If &ic);
   // destructor
   virtual ~If();
   
   // Inherited methods that need some enhancement from the base class
   virtual bool         Append(GmatCommand *cmd);
   
   // Methods used to run the command
   virtual bool         Execute();
   
   // inherited from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);

   virtual GmatBase*    Clone() const;
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName);

   DEFAULT_TO_NO_CLONES

protected:
   enum
   {
      NEST_LEVEL = ConditionalBranchParamCount,
      IfParamCount
   };

   static const std::string PARAMETER_TEXT[IfParamCount - ConditionalBranchParamCount];
   
   static const Gmat::ParameterType PARAMETER_TYPE[IfParamCount - ConditionalBranchParamCount];
   
   
   /// Counter to track how deep the If nesting is
   Integer                  nestLevel;
};
#endif  // If_hpp
