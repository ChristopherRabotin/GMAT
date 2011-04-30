//$Id$
//------------------------------------------------------------------------------
//                                  Rmat66Var
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun (GSFC/NASA)
// Created: 2009.03.30
//
/**
 * Declares base class of parameters returning Rmatrix.
 */
//------------------------------------------------------------------------------
#ifndef Rmat66Var_hpp
#define Rmat66Var_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"
#include "Rmatrix66.hpp"

class GMAT_API Rmat66Var : public Parameter
{
public:

   Rmat66Var(const std::string &name = "",
             const std::string &typeStr = "Rmat66Var",
             GmatParam::ParameterKey key = GmatParam::USER_PARAM,
             GmatBase *obj = NULL, const std::string &desc = "",
             const std::string &unit = "",
             GmatParam::DepObject depObj = GmatParam::NO_DEP,
             Gmat::ObjectType ownerType = Gmat::UNKNOWN_OBJECT,
             bool isSettable = false);
   Rmat66Var(const Rmat66Var &copy);
   Rmat66Var& operator= (const Rmat66Var& right);
   virtual ~Rmat66Var();
   
   bool operator==(const Rmat66Var &right) const;
   bool operator!=(const Rmat66Var &right) const;
   
   virtual std::string ToString();
   
   virtual const Rmatrix& GetRmatrix() const;
   virtual void  SetRmatrix(const Rmatrix &val);
   virtual const Rmatrix& EvaluateRmatrix();
   
protected:
   
   Rmatrix66 mRmat66Value;
   
private:

};
#endif // Rmat66Var_hpp
