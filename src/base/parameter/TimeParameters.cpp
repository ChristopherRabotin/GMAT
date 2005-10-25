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
// CurrA1MJD(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
CurrA1MJD::CurrA1MJD(const std::string &name, GmatBase *obj)
   : TimeReal(name, "CurrA1MJD", obj, "A1 Mod. Julian Days", "day")
{
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
   //mRealValue = GetCurrentTimeReal("A1MJD");
   mRealValue = GetCurrentTimeReal(GmatParam::A1MJD);
   
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
ElapsedDays::PARAMETER_TEXT[ElapsedDaysParamCount - ParameterParamCount] =
{
   "InitialEpoch"
};

const Gmat::ParameterType
ElapsedDays::PARAMETER_TYPE[ElapsedDaysParamCount - ParameterParamCount] =
{
   Gmat::REAL_TYPE
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// ElapsedDays(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
ElapsedDays::ElapsedDays(const std::string &name, GmatBase *obj)
   : TimeReal(name, "ElapsedDays", obj, "Elapsed Days", "Day")
{
   // GmatBase data
   parameterCount = ElapsedDaysParamCount;
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
   //mRealValue = GetElapsedTimeReal("Days");
   mRealValue = GetElapsedTimeReal(GmatParam::DAYS);
   
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
   if (id >= ParameterParamCount && id < ElapsedDaysParamCount)
      return PARAMETER_TYPE[id - ParameterParamCount];
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
   if (id >= ParameterParamCount && id < ElapsedDaysParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - ParameterParamCount)];
   else
      return TimeReal::GetParameterTypeString(id);

}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedDays::GetParameterText(const Integer id) const
{
   if (id >= ParameterParamCount && id < ElapsedDaysParamCount)
      return PARAMETER_TEXT[id - ParameterParamCount];
   else
      return TimeReal::GetParameterText(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> parameter index.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool ElapsedDays::IsParameterReadOnly(const Integer id) const
{
   if (id == INITIAL_EPOCH)
      return true;
   
   return TimeReal::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer ElapsedDays::GetParameterID(const std::string &str) const
{
   for (int i = ParameterParamCount; i < ElapsedDaysParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ParameterParamCount])
         return i;
   }
   
   return TimeReal::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDays::GetRealParameter(const Integer id) const
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
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedDays::GetRealParameter(const std::string &label) const
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
ElapsedSecs::PARAMETER_TEXT[ElapsedSecsParamCount - ParameterParamCount] =
{
   "InitialEpoch"
};

const Gmat::ParameterType
ElapsedSecs::PARAMETER_TYPE[ElapsedSecsParamCount - ParameterParamCount] =
{
   Gmat::REAL_TYPE
};


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// ElapsedSecs(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
ElapsedSecs::ElapsedSecs(const std::string &name, GmatBase *obj)
   : TimeReal(name, "ElapsedSecs", obj, "Elapsed Seconds", "s")
{
   // GmatBase data
   parameterCount = ElapsedSecsParamCount;
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
   //mRealValue = GetElapsedTimeReal("Secs");
   mRealValue = GetElapsedTimeReal(GmatParam::SECS);
   
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
   if (id >= ParameterParamCount && id < ElapsedSecsParamCount)
      return PARAMETER_TYPE[id - ParameterParamCount];
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
   if (id >= ParameterParamCount && id < ElapsedSecsParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - ParameterParamCount)];
   else
      return TimeReal::GetParameterTypeString(id);

}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ElapsedSecs::GetParameterText(const Integer id) const
{
   if (id >= ParameterParamCount && id < ElapsedSecsParamCount)
      return PARAMETER_TEXT[id - ParameterParamCount];
   else
      return TimeReal::GetParameterText(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> parameter index.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool ElapsedSecs::IsParameterReadOnly(const Integer id) const
{
   if (id == INITIAL_EPOCH)
      return true;
   
   return TimeReal::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer ElapsedSecs::GetParameterID(const std::string &str) const
{
   for (int i = ParameterParamCount; i < ElapsedSecsParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ParameterParamCount])
         return i;
   }
   
   return TimeReal::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedSecs::GetRealParameter(const Integer id) const
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
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ElapsedSecs::GetRealParameter(const std::string &label) const
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
