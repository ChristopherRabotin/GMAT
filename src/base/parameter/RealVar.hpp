//$Id$
//------------------------------------------------------------------------------
//                                  RealVar
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
// Created: 2004/01/08
//
/**
 * Declares base class of parameters returning Real.
 */
//------------------------------------------------------------------------------
#ifndef RealVar_hpp
#define RealVar_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"

class GMAT_API RealVar : public Parameter
{
public:

   RealVar(const std::string &name = "", const std::string &valStr = "",
           const std::string &typeStr = "RealVar",
           GmatParam::ParameterKey key = GmatParam::USER_PARAM,
           GmatBase *obj = NULL, const std::string &desc = "",
           const std::string &unit = "",
           GmatParam::DepObject depObj = GmatParam::NO_DEP,
           Gmat::ObjectType ownerType = Gmat::UNKNOWN_OBJECT,
           bool isTimeParam = false, bool isSettable = false,
           bool isPlottable = true, bool isReportable = true,
           Gmat::ObjectType ownedObjType = Gmat::UNKNOWN_OBJECT);
   RealVar(const RealVar &copy);
   RealVar& operator= (const RealVar& right);
   virtual ~RealVar();
   
   bool operator==(const RealVar &right) const;
   bool operator!=(const RealVar &right) const;
   
   // methods inherited from Parameter
   virtual bool Initialize();
   virtual std::string ToString();
   
   virtual Real GetReal() const;
   virtual void SetReal(Real val);
   
   virtual Integer GetParameterID(const std::string &str) const;
   
   virtual Real GetRealParameter(const Integer id) const;
   virtual Real GetRealParameter(const std::string &label) const;
   
   virtual Real SetRealParameter(const Integer id, const Real value);
   virtual Real SetRealParameter(const std::string &label, const Real value);
   
   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value);
protected:

   enum
   {
      VALUE = ParameterParamCount,
      RealVarParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[RealVarParamCount - ParameterParamCount];
   static const std::string
      PARAMETER_TEXT[RealVarParamCount - ParameterParamCount];
   
   bool mValueSet;
   bool mIsNumber;
   Real mRealValue;
   
private:

};
#endif // RealVar_hpp
