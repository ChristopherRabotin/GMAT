//$Header$
//------------------------------------------------------------------------------
//                              File: TimeParameters.hpp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/03/12
//
/**
 * Declares Time related parameter classes.
 *   CurrA1MJD, ElapsedDays, ElapsedSecs
 */
//------------------------------------------------------------------------------
#ifndef TimeParameters_hpp
#define TimeParameters_hpp

#include "RealVar.hpp"
#include "TimeReal.hpp"

//==============================================================================
//                              CurrA1MJD
//==============================================================================

class GMAT_API CurrA1MJD : public TimeReal
{
public:

   CurrA1MJD(const std::string &name = "",
             GmatBase *obj = NULL,
             const std::string &desc = "",
             const std::string &unit = "A1 Mod. Julian days");
   CurrA1MJD(const CurrA1MJD &copy);
   CurrA1MJD& operator= (const CurrA1MJD &right); 
   virtual ~CurrA1MJD();

   // The inherited methods from Parameter
   virtual bool Evaluate();

protected:
    
private:

};

//==============================================================================
//                              ElapsedDays
//==============================================================================

class GMAT_API ElapsedDays : public TimeReal
{
public:

   ElapsedDays(const std::string &name = "",
               GmatBase *obj = NULL,
               const std::string &desc = "",
               const std::string &unit = "Days");
   ElapsedDays(const ElapsedDays &copy);
   const ElapsedDays& operator= (const ElapsedDays &right); 
   virtual ~ElapsedDays();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // The inherited methods from GmatBase
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual std::string GetParameterText(const Integer id);
   virtual Integer GetParameterID(const std::string &str);
   virtual Real GetRealParameter(const Integer id);
   virtual Real GetRealParameter(const std::string &label);
   virtual Real SetRealParameter(const Integer id, const Real value);
   virtual Real SetRealParameter(const std::string &label, const Real value);
    
protected:
    
   enum
   {
      INITIAL_EPOCH = RealVarParamCount,
      ElapsedDaysParamCount
   };

   static const Gmat::ParameterType PARAMETER_TYPE[ElapsedDaysParamCount - RealVarParamCount];
   static const std::string PARAMETER_TEXT[ElapsedDaysParamCount - RealVarParamCount];
    
private:
};

//==============================================================================
//                              ElapsedSecs
//==============================================================================

class GMAT_API ElapsedSecs : public TimeReal
{
public:

   ElapsedSecs(const std::string &name = "",
               GmatBase *obj = NULL,
               const std::string &desc = "",
               const std::string &unit = "Sec");
   ElapsedSecs(const ElapsedSecs &copy);
   const ElapsedSecs& operator= (const ElapsedSecs &right); 
   virtual ~ElapsedSecs();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // The inherited methods from GmatBase
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual std::string GetParameterText(const Integer id);
   virtual Integer GetParameterID(const std::string &str);
   virtual Real GetRealParameter(const Integer id);
   virtual Real GetRealParameter(const std::string &label);
   virtual Real SetRealParameter(const Integer id, const Real value);
   virtual Real SetRealParameter(const std::string &label, const Real value);
    
protected:
    
   enum
   {
      INITIAL_EPOCH = RealVarParamCount,
      ElapsedSecsParamCount
   };

   static const Gmat::ParameterType PARAMETER_TYPE[ElapsedSecsParamCount - RealVarParamCount];
   static const std::string PARAMETER_TEXT[ElapsedSecsParamCount - RealVarParamCount];
    
private:
};


#endif // TimeParameters_hpp
