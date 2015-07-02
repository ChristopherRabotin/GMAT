//$Header: /cygdrive/p/dev/cvs/test/TestParam/MyEtParam.hpp,v 1.1 2003/10/24 15:09:47 ljun Exp $
//------------------------------------------------------------------------------
//                              MyEtParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Author: Linda Jun
// Created: 2003/09/22
//
/**
 * Declares my elapsed time parameter operations.
 */
//------------------------------------------------------------------------------
#ifndef MyEtParam_hpp
#define MyEtParam_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"
#include "ElapsedTimeParam.hpp"
#include "A1Mjd.hpp"

class GMAT_API MyEtParam : public ElapsedTimeParam
{
public:

   MyEtParam(const std::string &name, const std::string &desc,
             const ElapsedTime &elapsedTime, const Real anotherTime);
   MyEtParam(const std::string &name, const std::string &desc,
             const Real val, const Real anotherTime);
   MyEtParam(const MyEtParam &param);
   const MyEtParam& operator= (const MyEtParam &param); 
   virtual ~MyEtParam();

   Real GetAnotherTimeValue() const;

   // The inherited methods from GmatBase
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual std::string GetParameterText(const Integer id);
   virtual Integer GetParameterID(const std::string str);
   virtual Real GetRealParameter(const Integer id);
   virtual Real SetRealParameter(const Integer id, const Real value);

protected:

   Real mAnotherTime;

   enum
   {
      ANOTHER_ELAPSED_TIME = ElapsedTimeParamCount,
      MyEtParamCount
   };

   static const std::string PARAMETER_TEXT[MyEtParamCount];
   static const Gmat::ParameterType PARAMETER_TYPE[MyEtParamCount];

private:
};

#endif // MyEtParam_hpp
