//$Header$
//------------------------------------------------------------------------------
//                              CurrentTimeParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/09/22
//
/**
 * Declares A1Mjd type operations.
 */
//------------------------------------------------------------------------------
#ifndef CurrentTimeParam_hpp
#define CurrentTimeParam_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"
#include "A1Mjd.hpp"

class GMAT_API CurrentTimeParam : public Parameter
{
public:

   CurrentTimeParam(const std::string &name, const std::string &desc,
                    const A1Mjd &a1Mjd);
   CurrentTimeParam(const std::string &name, const std::string &desc,
                    const Real val);
   CurrentTimeParam(const CurrentTimeParam &param);
   CurrentTimeParam& operator= (const CurrentTimeParam &right); 
   virtual ~CurrentTimeParam();

   A1Mjd GetValue() const;
   Real  GetRealValue() const;

   void SetValue(const CurrentTimeParam &param);
   void SetValue(const A1Mjd &a1mjd);
   void SetValue(const Real &val);

   bool operator==(const CurrentTimeParam &right) const;
   bool operator!=(const CurrentTimeParam &right) const;

   // The inherited methods from GmatBase
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual std::string GetParameterText(const Integer id);
   virtual Integer GetParameterID(const std::string str);
   virtual Real GetRealParameter(const Integer id);
   virtual Real SetRealParameter(const Integer id, const Real value);

   // The inherited methods from Parameter
   virtual const std::string* GetParameterList() const;

protected:
   A1Mjd mA1Mjd;

   //loj:future build
   //loj: add YEAR, MONTH, DAY, HOUR, MINUTE, SECOND
   enum
   {
      CURRENT_TIME = 0,
      CurrentTimeParamCount
   };

   static const std::string PARAMETER_TEXT[CurrentTimeParamCount];
   static const Gmat::ParameterType PARAMETER_TYPE[CurrentTimeParamCount];

private:

};

#endif // CurrentTimeParam_hpp
