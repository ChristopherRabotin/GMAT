//$Id$
//------------------------------------------------------------------------------
//                             InternalOptimizer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2007/03/27
//
/**
 * Definition of the internal optimizer. 
 */
//------------------------------------------------------------------------------


#ifndef InternalOptimizer_hpp
#define InternalOptimizer_hpp

#include "Optimizer.hpp"

/**
 * Defines the interfaces used by Optimizers that run integral to GMAT.  
 */
class GMAT_API InternalOptimizer : public Optimizer
{
public:
	InternalOptimizer(std::string type, std::string name);
	virtual ~InternalOptimizer();
   InternalOptimizer(const InternalOptimizer &opt);
   InternalOptimizer&      operator=(const InternalOptimizer& opt);

   virtual bool        Initialize();
   
protected:
   /// The itemized parameters for internal optimizers
   enum
   {
      InternalOptimizerParamCount = OptimizerParamCount
   };
};

#endif /*InternalOptimizer_hpp*/
