//$Header$
//------------------------------------------------------------------------------
//                             File: TimeParameters.cpp
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
 * Implements Time related parameter classes.
 *   CurrA1MJD, ElapsedDays, ElapsedSecs
 */
//------------------------------------------------------------------------------
#include "TimeParameters.hpp"

//==============================================================================
//                              CurrA1MJD
//==============================================================================

//------------------------------------------------------------------------------
// CurrA1MJD(const std::string &name, GmatBase *obj,
//           const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
CurrA1MJD::CurrA1MJD(const std::string &name, GmatBase *obj,
                     const std::string &desc, const std::string &unit)
   : TimeReal(name, "CurrA1MJD", SYSTEM_PARAM, obj, desc, unit, true)
{
   AddRefObject(obj);
}

//------------------------------------------------------------------------------
// CurrA1MJD::CurrA1MJD(const CurrA1MJD &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CurrA1MJD::CurrA1MJD(const CurrA1MJD &param)
   : TimeReal(param)
{
}

//------------------------------------------------------------------------------
// CurrA1MJD& CurrA1MJD::operator=(const CurrA1MJD &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
CurrA1MJD& CurrA1MJD::operator=(const CurrA1MJD &right)
{
   if (this != &right)
      TimeReal::operator=(right);
      
   return *this;
}

//------------------------------------------------------------------------------
// CurrA1MJD::~CurrA1MJD()
//------------------------------------------------------------------------------
CurrA1MJD::~CurrA1MJD()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool CurrA1MJD::Evaluate()
{
   mRealValue = GetCurrentTimeReal("A1MJD");
    
   if (mRealValue == TIME_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* CurrA1MJD::Clone(void) const
{
   return new CurrA1MJD(*this);
}

//==============================================================================
//                              ElapsedDays
//==============================================================================

//---------------------------------
// static data
//---------------------------------
const std::string
ElapsedDays::PARAMETER_TEXT[ElapsedDaysParamCount - RealVarParamCount] =
{
   "InitialEpoch"
};

const Gmat::ParameterType
ElapsedDays::PARAMETER_TYPE[ElapsedDaysParamCount - RealVarParamCount] =
{
   Gmat::REAL_TYPE
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// ElapsedDays(const std::string &name, GmatBase *obj,
//             const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
ElapsedDays::ElapsedDays(const std::string &name, GmatBase *obj,
                         const std::string &desc, const std::string &unit)
   : TimeReal(name, "ElapsedDays", SYSTEM_PARAM, obj, desc, unit, true)
{
   // GmatBase data
   parameterCount = ElapsedDaysParamCount;

   AddRefObject(obj);
}

//------------------------------------------------------------------------------
// ElapsedDays(const ElapsedDays &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ElapsedDays::ElapsedDays(const ElapsedDays &copy)
    : TimeReal(copy)
{
}

//------------------------------------------------------------------------------
// const ElapsedDays& operator=(const ElapsedDays &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const ElapsedDays& ElapsedDays::operator=(const ElapsedDays &right)
{
   if (this != &right)
   {
      TimeReal::operator=(right);
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~ElapsedDays()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ElapsedDays::~ElapsedDays()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool ElapsedDays::Evaluate()
{
   mRealValue = GetElapsedTimeReal("Days");
    
   if (mRealValue == TIME_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//--------------------------------------
// Inherited methods from GmatBase
//--------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* ElapsedDays::Clone(void) const
{
   return new ElapsedDays(*this);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ElapsedDays::GetParameterType(const Integer id) const
{
   if (id >= RealVarParamCount && id < ElapsedDaysParamCount)
      return PARAMETER_TYPE[id - RealVarParamCount];
   else
      return TimeReal::GetParameterType(id);

}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedDays::GetParameterTypeString(const Integer id) const
{
   if (id >= RealVarParamCount && id < ElapsedDaysParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - RealVarParamCount)];
   else
      return TimeReal::GetParameterTypeString(id);

}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedDays::GetParameterText(const Integer id)
{
   if (id >= RealVarParamCount && id < ElapsedDaysParamCount)
      return PARAMETER_TEXT[id - RealVarParamCount];
   else
      return TimeReal::GetParameterText(id);

}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer ElapsedDays::GetParameterID(const std::string &str)
{
   for (int i = RealVarParamCount; i < ElapsedDaysParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - RealVarParamCount])
         return i;
   }
   
   return TimeReal::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDays::GetRealParameter(const Integer id)
{
   switch (id)
   {
   case INITIAL_EPOCH:
       return GetInitialEpoch();
   default:
       return TimeReal::GetRealParameter(id);
   }
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDays::GetRealParameter(const std::string &label)
{
   if (label == "InitialEpoch")
      return GetInitialEpoch();
   else
      return TimeReal::GetRealParameter(label);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDays::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case INITIAL_EPOCH:
      SetInitialEpoch(value);
      return value;
   default:
      return TimeReal::SetRealParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std:string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDays::SetRealParameter(const std::string &label, const Real value)
{
   if (label == "InitialEpoch")
   {
      SetInitialEpoch(value);
      return value;
   }
   else
   {
      return TimeReal::SetRealParameter(label, value);
   }
}

//==============================================================================
//                              ElapsedSecs
//==============================================================================

//---------------------------------
// static data
//---------------------------------
const std::string
ElapsedSecs::PARAMETER_TEXT[ElapsedSecsParamCount - RealVarParamCount] =
{
   "InitialEpoch"
};

const Gmat::ParameterType
ElapsedSecs::PARAMETER_TYPE[ElapsedSecsParamCount - RealVarParamCount] =
{
   Gmat::REAL_TYPE
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// ElapsedSecs(const std::string &name, GmatBase *obj,
//             const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
ElapsedSecs::ElapsedSecs(const std::string &name, GmatBase *obj,
                         const std::string &desc, const std::string &unit)
   : TimeReal(name, "ElapsedSecs", SYSTEM_PARAM, obj, desc, unit, true)
{
   // GmatBase data
   parameterCount = ElapsedSecsParamCount;

   AddRefObject(obj);
}

//------------------------------------------------------------------------------
// ElapsedSecs(const ElapsedSecs &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
ElapsedSecs::ElapsedSecs(const ElapsedSecs &copy)
   : TimeReal(copy)
{
}

//------------------------------------------------------------------------------
// const ElapsedSecs& operator=(const ElapsedSecs &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const ElapsedSecs& ElapsedSecs::operator=(const ElapsedSecs &right)
{
   if (this != &right)
   {
      TimeReal::operator=(right);
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~ElapsedSecs()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ElapsedSecs::~ElapsedSecs()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool ElapsedSecs::Evaluate()
{
   mRealValue = GetElapsedTimeReal("Secs");
    
   if (mRealValue == TIME_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//--------------------------------------
// Inherited methods from GmatBase
//--------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* ElapsedSecs::Clone(void) const
{
   return new ElapsedSecs(*this);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ElapsedSecs::GetParameterType(const Integer id) const
{
   if (id >= RealVarParamCount && id < ElapsedSecsParamCount)
      return PARAMETER_TYPE[id - RealVarParamCount];
   else
      return TimeReal::GetParameterType(id);

}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedSecs::GetParameterTypeString(const Integer id) const
{
   if (id >= RealVarParamCount && id < ElapsedSecsParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - RealVarParamCount)];
   else
      return TimeReal::GetParameterTypeString(id);

}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedSecs::GetParameterText(const Integer id)
{
   if (id >= RealVarParamCount && id < ElapsedSecsParamCount)
      return PARAMETER_TEXT[id - RealVarParamCount];
   else
      return TimeReal::GetParameterText(id);

}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer ElapsedSecs::GetParameterID(const std::string &str)
{
   for (int i = RealVarParamCount; i < ElapsedSecsParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - RealVarParamCount])
         return i;
   }
   
   return TimeReal::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedSecs::GetRealParameter(const Integer id)
{
   switch (id)
   {
   case INITIAL_EPOCH:
      return GetInitialEpoch();
   default:
      return TimeReal::GetRealParameter(id);
   }
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedSecs::GetRealParameter(const std::string &label)
{
   if (label == "InitialEpoch")
      return GetInitialEpoch();
   else
      return TimeReal::GetRealParameter(label);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedSecs::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case INITIAL_EPOCH:
      SetInitialEpoch(value);
      return value;
   default:
      return TimeReal::SetRealParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std:string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedSecs::SetRealParameter(const std::string &label, const Real value)
{
   if (label == "InitialEpoch")
   {
      SetInitialEpoch(value);
      return value;
   }
   else
   {
      return TimeReal::SetRealParameter(label, value);
   }
}
