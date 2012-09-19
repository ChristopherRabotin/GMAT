//$Id$
//------------------------------------------------------------------------------
//                                  MatlabFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Definition for the MatlabFunction class.
 */
//------------------------------------------------------------------------------


#ifndef MatlabFunction_hpp
#define MatlabFunction_hpp

#include "matlabinterface_defs.hpp"
#include "Function.hpp"

class MATLAB_API MatlabFunction : public Function
{
public:
   MatlabFunction(const std::string &nomme);
   virtual ~MatlabFunction();
   MatlabFunction(const MatlabFunction &copy);
   MatlabFunction& operator=(const MatlabFunction &right);
   
   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
};

#endif
