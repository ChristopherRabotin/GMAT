//$Header$
//------------------------------------------------------------------------------
//                              ElapsedTimeParam
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
 * Declares elapsed time parameter operations.
 */
//------------------------------------------------------------------------------
#ifndef ElapsedTimeParam_hpp
#define ElapsedTimeParam_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"
#include "ElapsedTime.hpp"
#include "A1Mjd.hpp"

class GMAT_API ElapsedTimeParam : public Parameter
{
public:

   ElapsedTimeParam(const std::string &name, const std::string &desc,
                    const ElapsedTime &elapsedTime);
   ElapsedTimeParam(const std::string &name, const std::string &desc,
                    const Real val);
   ElapsedTimeParam(const ElapsedTimeParam &param);
   const ElapsedTimeParam& operator= (const ElapsedTimeParam &right); 
   virtual ~ElapsedTimeParam();

   ElapsedTime GetValue() const;
   Real GetRealValue() const;

   void SetValue(const ElapsedTimeParam &param);
   void SetValue(const ElapsedTime &elapsedTime);
   void SetValue(const A1Mjd &epoch, const A1Mjd &currentTime);
   void SetValue(const Real &val);

   ElapsedTime Evaluate(const A1Mjd &epoch, const A1Mjd &currentTime);
   Real Evaluate(const Real &epoch, const Real &currentTime);

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

   ElapsedTime mElapsedTime;

   //loj:future build
   //loj: add ELAPSED_YEARS, ELAPSED_DAYS, ELAPSED_HOURS, ELAPSED_MINUTES, ELAPSED_SECONDS
   enum
   {
      ELAPSED_TIME = 0,
      ElapsedTimeParamCount
   };

   static const std::string PARAMETER_TEXT[ElapsedTimeParamCount];
   static const Gmat::ParameterType PARAMETER_TYPE[ElapsedTimeParamCount];

private:
};

#endif // ElapsedTimeParam_hpp
