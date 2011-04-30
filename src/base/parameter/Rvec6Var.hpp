//$Id$
//------------------------------------------------------------------------------
//                                  Rvec6Var
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
// Author: Linda Jun
// Created: 2004/03/12
//
/**
 * Declares base class of parameters returning Rvector6.
 */
//------------------------------------------------------------------------------
#ifndef Rvec6Var_hpp
#define Rvec6Var_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"
#include "Rvector6.hpp"

class GMAT_API Rvec6Var : public Parameter
{
public:

   Rvec6Var(const std::string &name = "",
            const std::string &typeStr = "Rvec6Var",
            GmatParam::ParameterKey key = GmatParam::USER_PARAM,
            GmatBase *obj = NULL, const std::string &desc = "",
            const std::string &unit = "",
            GmatParam::DepObject depObj = GmatParam::NO_DEP,
            Gmat::ObjectType ownerType = Gmat::UNKNOWN_OBJECT);
   Rvec6Var(const Rvec6Var &copy);
   Rvec6Var& operator= (const Rvec6Var& right);
   virtual ~Rvec6Var();
   
   bool operator==(const Rvec6Var &right) const;
   bool operator!=(const Rvec6Var &right) const;
   
   virtual std::string  ToString();
   
   virtual const        Rvector6& GetRvector6() const;
   virtual void         SetRvector6(const Rvector6 &val);
   virtual const        Rvector6& EvaluateRvector6();
   
protected:
   
   Rvector6 mRvec6Value;
   
private:

};
#endif // Parameter_hpp
