//$Id$
//------------------------------------------------------------------------------
//                                  RvectorVar
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
 * Declares base class of parameters returning Rvector.
 */
//------------------------------------------------------------------------------
#ifndef RvectorVar_hpp
#define RvectorVar_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"
#include "Rvector.hpp"

class GMAT_API RvectorVar : public Parameter
{
public:

   RvectorVar(const std::string &name = "",
              const std::string &typeStr = "RvectorVar",
              GmatParam::ParameterKey key = GmatParam::USER_PARAM,
              GmatBase *obj = NULL, const std::string &desc = "",
              const std::string &unit = "",
              GmatParam::DepObject depObj = GmatParam::NO_DEP,
              Gmat::ObjectType ownerType = Gmat::UNKNOWN_OBJECT,
              bool isTimeParam = false, bool isSettable = false,
              Gmat::ObjectType ownedObjType = Gmat::UNKNOWN_OBJECT,
              Integer size = 0);
   RvectorVar(const RvectorVar &param);
   RvectorVar& operator= (const RvectorVar& param);
   virtual ~RvectorVar();
   
   bool operator==(const RvectorVar &param) const;
   bool operator!=(const RvectorVar &param) const;
   
   virtual std::string  ToString();
   
   virtual const        Rvector& GetRvector() const;
   virtual void         SetRvector(const Rvector &val);
   virtual const        Rvector& EvaluateRvector();
   
   // methods inherited from GmatBase
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   
protected:

   Integer mVectorSize;
   Rvector mRvectorValue;
   
   enum
   {
      VECTOR_SIZE = ParameterParamCount,
      RvectorVarParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[RvectorVarParamCount - ParameterParamCount];
   static const std::string
      PARAMETER_TEXT[RvectorVarParamCount - ParameterParamCount];
   
private:

};
#endif // RvectorVar_hpp
