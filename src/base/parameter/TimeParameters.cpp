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
    : RealVar(name, "CurrA1MJD", SYSTEM_PARAM, obj, desc, unit, true)
{
    AddObject(obj);
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
    : RealVar(param)
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
        RealVar::operator=(right);
      
    return *this;
}

//------------------------------------------------------------------------------
// CurrA1MJD::~CurrA1MJD()
//------------------------------------------------------------------------------
CurrA1MJD::~CurrA1MJD()
{
}

//-------------------------------------
// Inherited methods from RealVar
//-------------------------------------

//------------------------------------------------------------------------------
// virtual Real EvaluateReal()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
Real CurrA1MJD::EvaluateReal()
{
    Evaluate();
    return mValue;
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual Integer GetNumObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer CurrA1MJD::GetNumObjects() const
{
    return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
GmatBase* CurrA1MJD::GetObject(const std::string &objTypeName)
{
    return GetRefObject(objTypeName);
}

//------------------------------------------------------------------------------
// virtual bool SetObject(Gmat::ObjectType objType, const std::string &objName,
//                        GmatBase *obj
//------------------------------------------------------------------------------
/**
 * Sets reference object.
 *
 * @return true if the object has been set.
 */
//------------------------------------------------------------------------------
bool CurrA1MJD::SetObject(Gmat::ObjectType objType,
                                  const std::string &objName,
                                  GmatBase *obj)
{
    if (obj != NULL)
        return SetRefObject(objType, objName, obj);
    else
        return false;
}

//------------------------------------------------------------------------------
// virtual bool AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Adds reference objects.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool CurrA1MJD::AddObject(GmatBase *obj)
{
    if (obj != NULL)
    {
        if (AddRefObject(obj))
            ManageObject(obj);
        
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
/**
 * Validates reference objects.
 *
 * @return true if all objects are set; false otherwise
 */
//------------------------------------------------------------------------------
bool CurrA1MJD::Validate()
{
    return ValidateRefObjects(this);
}

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
    mValue = GetCurrentTimeReal("A1MJD");
    
    if (mValue == TIME_REAL_UNDEFINED)
        return false;
    else
        return true;
}

//==============================================================================
//                              ElapsedDays
//==============================================================================

//---------------------------------
// static data
//---------------------------------
const std::string
ElapsedDays::PARAMETER_TEXT[ElapsedDaysParamCount] =
{
    "InitialEpoch"
};

const Gmat::ParameterType
ElapsedDays::PARAMETER_TYPE[ElapsedDaysParamCount] =
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
    : RealVar(name, "ElapsedDays", SYSTEM_PARAM, obj, desc, unit, true)
{
    // GmatBase data
    parameterCount = ElapsedDaysParamCount;

    AddObject(obj);
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
    : RealVar(copy)
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
        RealVar::operator=(right);
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
// Inherited methods from RealVar
//-------------------------------------

//------------------------------------------------------------------------------
// virtual Real EvaluateReal()
//------------------------------------------------------------------------------
/**
 * Computes elapsed time from epoch and current time.
 *
 * @return the elapsed days
 */
//------------------------------------------------------------------------------
Real ElapsedDays::EvaluateReal()
{
    Evaluate();
    return mValue;
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual Integer GetNumObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer ElapsedDays::GetNumObjects() const
{
    return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
GmatBase* ElapsedDays::GetObject(const std::string &objTypeName)
{
    return GetRefObject(objTypeName);
}

//------------------------------------------------------------------------------
// virtual bool SetObject(Gmat::ObjectType objType, const std::string &objName,
//                        GmatBase *obj
//------------------------------------------------------------------------------
/**
 * Sets reference object.
 *
 * @return true if the object has been set.
 */
//------------------------------------------------------------------------------
bool ElapsedDays::SetObject(Gmat::ObjectType objType,
                                 const std::string &objName,
                                 GmatBase *obj)
{
    if (obj != NULL)
        return SetRefObject(objType, objName, obj);
    else
        return false;
}

//------------------------------------------------------------------------------
// virtual bool AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Adds reference objects.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool ElapsedDays::AddObject(GmatBase *obj)
{
    if (obj != NULL)
    {
        if (AddRefObject(obj))
            ManageObject(obj);
        
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
/**
 * Validates reference objects.
 *
 * @return true if all objects are set; false otherwise
 */
//------------------------------------------------------------------------------
bool ElapsedDays::Validate()
{
    return ValidateRefObjects(this);
}

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
    mValue = GetElapsedTimeReal("Days");
    
    if (mValue == TIME_REAL_UNDEFINED)
        return false;
    else
        return true;
}

//--------------------------------------
// Inherited methods from GmatBase
//--------------------------------------

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ElapsedDays::GetParameterType(const Integer id) const
{
   switch (id)
   {
   case INITIAL_EPOCH:
      return ElapsedDays::PARAMETER_TYPE[id - RealVarParamCount];
   default:
      return RealVar::GetParameterType(id);
   }
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
   switch (id)
   {
   case INITIAL_EPOCH:
      return RealVar::PARAM_TYPE_STRING[GetParameterType(id)];
   default:
      return RealVar::GetParameterTypeString(id);
   }
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
   switch (id)
   {
   case INITIAL_EPOCH:
      return ElapsedDays::PARAMETER_TEXT[id - RealVarParamCount];
   default:
      return RealVar::GetParameterText(id);
   }
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
   for (int i=0; i<ElapsedDaysParamCount; i++)
   {
      if (str == ElapsedDays::PARAMETER_TEXT[i])
          return i + RealVarParamCount;
   }
   
   return RealVar::GetParameterID(str);
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
       return RealVar::GetRealParameter(id);
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
       return RealVar::GetRealParameter(label);
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
       return RealVar::SetRealParameter(id, value);
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
       return RealVar::SetRealParameter(label, value);
    }
}

//==============================================================================
//                              ElapsedSecs
//==============================================================================

//---------------------------------
// static data
//---------------------------------
const std::string
ElapsedSecs::PARAMETER_TEXT[ElapsedSecsParamCount] =
{
    "InitialEpoch"
};

const Gmat::ParameterType
ElapsedSecs::PARAMETER_TYPE[ElapsedSecsParamCount] =
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
    : RealVar(name, "ElapsedSecs", SYSTEM_PARAM, obj, desc, unit, true)
{
    // GmatBase data
    parameterCount = ElapsedSecsParamCount;

    AddObject(obj);
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
    : RealVar(copy)
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
        RealVar::operator=(right);
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
// Inherited methods from RealVar
//-------------------------------------

//------------------------------------------------------------------------------
// Real EvaluateReal()
//------------------------------------------------------------------------------
/**
 * Computes elapsed time from epoch and current time.
 *
 * @return the elapsed seconds
 */
//------------------------------------------------------------------------------
Real ElapsedSecs::EvaluateReal()
{
    Evaluate();
    return mValue;
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// Integer GetNumObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer ElapsedSecs::GetNumObjects() const
{
    return GetNumRefObjects();
}

//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
GmatBase* ElapsedSecs::GetObject(const std::string &objTypeName)
{
    return GetRefObject(objTypeName);
}

//------------------------------------------------------------------------------
// virtual bool SetObject(Gmat::ObjectType objType, const std::string &objName,
//                        GmatBase *obj
//------------------------------------------------------------------------------
/**
 * Sets reference object.
 *
 * @return true if the object has been set.
 */
//------------------------------------------------------------------------------
bool ElapsedSecs::SetObject(Gmat::ObjectType objType,
                                 const std::string &objName,
                                 GmatBase *obj)
{
    if (obj != NULL)
        return SetRefObject(objType, objName, obj);
    else
        return false;
}

//------------------------------------------------------------------------------
// virtual bool AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Adds reference objects.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool ElapsedSecs::AddObject(GmatBase *obj)
{
    if (obj != NULL)
    {
        if (AddRefObject(obj))
            ManageObject(obj);
        
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
/**
 * Validates reference objects.
 *
 * @return true if all objects are set; false otherwise
 */
//------------------------------------------------------------------------------
bool ElapsedSecs::Validate()
{
    return ValidateRefObjects(this);
}

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
    mValue = GetElapsedTimeReal("Secs");
    
    if (mValue == TIME_REAL_UNDEFINED)
        return false;
    else
        return true;
}


//--------------------------------------
// Inherited methods from GmatBase
//--------------------------------------

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ElapsedSecs::GetParameterType(const Integer id) const
{
   switch (id)
   {
   case INITIAL_EPOCH:
      return ElapsedSecs::PARAMETER_TYPE[id - RealVarParamCount];
   default:
      return RealVar::GetParameterType(id);
   }
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
   switch (id)
   {
   case INITIAL_EPOCH:
      return RealVar::PARAM_TYPE_STRING[GetParameterType(id)];
   default:
      return RealVar::GetParameterTypeString(id);
   }
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
   switch (id)
   {
   case INITIAL_EPOCH:
      return ElapsedSecs::PARAMETER_TEXT[id - RealVarParamCount];
   default:
      return RealVar::GetParameterText(id);
   }
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
   for (int i=0; i<ElapsedSecsParamCount; i++)
   {
      if (str == ElapsedSecs::PARAMETER_TEXT[i])
          return i + RealVarParamCount;
   }
   
   return RealVar::GetParameterID(str);
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
       return RealVar::GetRealParameter(id);
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
       return RealVar::GetRealParameter(label);
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
       return RealVar::SetRealParameter(id, value);
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
       return RealVar::SetRealParameter(label, value);
    }
}
