//$Header$
//------------------------------------------------------------------------------
//                              RefFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/10/07
//
/**
 * Defines base class of Reference Frame
 */
//------------------------------------------------------------------------------
#include "RefFrame.hpp"
#include "CelestialBody.hpp"
#include "A1Mjd.hpp"
#include "SolarSystem.hpp"

#if !defined __UNIT_TEST__
#include "Moderator.hpp"
#endif

//---------------------------------
// static data
//---------------------------------

const std::string
RefFrame::PARAMETER_TEXT[RefFrameParamCount - GmatBaseParamCount] =
{
    "CB",
};

const Gmat::ParameterType
RefFrame::PARAMETER_TYPE[RefFrameParamCount - GmatBaseParamCount] =
{
    Gmat::STRING_TYPE,
};


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// CelestialBody* GetCentralBody() const
//------------------------------------------------------------------------------
/*
 * Retrieves the central body object
 *
 * @return the central body object
 */
//------------------------------------------------------------------------------
CelestialBody* RefFrame::GetCentralBody() const
{
    return mCentralBody;
}

//------------------------------------------------------------------------------
// void SetCentralBody(CelestialBody *centralBody) const
//------------------------------------------------------------------------------
/*
 * Sets the central body object
 */
//------------------------------------------------------------------------------
void RefFrame::SetCentralBody(CelestialBody *centralBody)
{
    mCentralBody = centralBody;
}

//------------------------------------------------------------------------------
// bool SetCentralBody(const std::string &name) const
//------------------------------------------------------------------------------
/*
 * Sets the central body object
 *
 * @return true if central body object successfully set, false otherwise
 */
//------------------------------------------------------------------------------
bool RefFrame::SetCentralBody(const std::string &name)
{
    bool status = false;

#if !defined __UNIT_TEST__
    
    Moderator *theModerator = Moderator::Instance();
    
    if (name != "")
    {
        SolarSystem *ss = theModerator->GetDefaultSolarSystem();
        CelestialBody *body = ss->GetBody(name);
        if (body != NULL)
        {
            SetCentralBody(body);
        }
    }
#endif
    
    return status;
}

//------------------------------------------------------------------------------
// std::string GetCentralBodyName() const
//------------------------------------------------------------------------------
/*
 * Retrieves the name of central body.
 *
 * @return the name of central body
 */
//------------------------------------------------------------------------------
std::string RefFrame::GetCentralBodyName() const
{
    return mCentralBody->GetName();
}

//------------------------------------------------------------------------------
// A1Mjd GetRefDate() const
//------------------------------------------------------------------------------
/*
 * Retrieves the reference date.
 *
 * @return the reference date
 */
//------------------------------------------------------------------------------
A1Mjd RefFrame::GetRefDate() const
{
    return mRefDate;
}

//------------------------------------------------------------------------------
// bool operator== (const RefFrame &right) const
//------------------------------------------------------------------------------
/*
 * Equal operator.
 *
 * @param <right> the object to compare for equality
 *
 * @return true if central body and ref. date are equal to right;
 *  false otherwise.
 *
 * @note There will be more data to check for == in the future build.
 */
//------------------------------------------------------------------------------
bool RefFrame::operator== (const RefFrame &right) const
{
    if (mCentralBody->GetName() != right.mCentralBody->GetName())
        return false;

    if (mRefDate != right.mRefDate)
        return false;
   
    return true;
}

//------------------------------------------------------------------------------
// bool operator!= (const RefFrame &right) const
//------------------------------------------------------------------------------
/*
 * Equal operator.
 *
 * @param <right> the object to compare for inequality
 *
 * @return true if central body and ref. date are not equal to right;
 *  false otherwise.
 *
 * @note There will be more data to check for != in the future build.
 */
//------------------------------------------------------------------------------
bool RefFrame::operator!= (const RefFrame &right) const
{
    return !(operator==(right));
}


//---------------------------------
// methods inherited from GmatBase
//---------------------------------

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string RefFrame::GetParameterText(const Integer id) const
{
    if (id >= GmatBaseParamCount && id < RefFrameParamCount)
        return PARAMETER_TEXT[id - GmatBaseParamCount];
    else
        return GmatBase::GetParameterText(id);
    
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer RefFrame::GetParameterID(const std::string &str) const
{
   for (int i=GmatBaseParamCount; i<RefFrameParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
          return i;
   }
   
   return GmatBase::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType RefFrame::GetParameterType(const Integer id) const
{
    if (id >= GmatBaseParamCount && id < RefFrameParamCount)
        return PARAMETER_TYPE[id - GmatBaseParamCount];
    else
        return GmatBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string RefFrame::GetParameterTypeString(const Integer id) const
{
    if (id >= GmatBaseParamCount && id < RefFrameParamCount)
        return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
    else
       return GmatBase::GetParameterTypeString(id);
    
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string RefFrame::GetStringParameter(const Integer id) const
{
    switch (id)
    {
    case CENTRAL_BODY:
        return mCentralBody->GetName();
    default:
        return GmatBase::GetStringParameter(id);
    }
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string RefFrame::GetStringParameter(const std::string &label) const
{
    //MessageInterface::ShowMessage("RefFrame::GetStringParameter() label = %s\n",
    //                              label.c_str());

    return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool RefFrame::SetStringParameter(const Integer id, const std::string &value)
{
    //MessageInterface::ShowMessage("RefFrame::SetStringParameter() id = %d, "
    //                              "value = %s \n", id, value.c_str());
    switch (id)
    {
    case CENTRAL_BODY:
        return SetCentralBody(value);
    default:
        return GmatBase::SetStringParameter(id, value);
    }
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool RefFrame::SetStringParameter(const std::string &label,
                                const std::string &value)
{
    //MessageInterface::ShowMessage("RefFrame::SetStringParameter() label = %s, "
    //                              "value = %s \n", label.c_str(), value.c_str());

    return SetStringParameter(GetParameterID(label), value);
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// RefFrame(const std::string &name, const std::string &typeStr,
//          const A1Mjd &refDate, CelestialBody *centralBody)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> reference frame name
 * @param <typeStr> reference frame type
 * @param <refDate> reference to reference date object.
 * @param <centralBody> pointer to central body object
 */
//------------------------------------------------------------------------------
RefFrame::RefFrame(const std::string &name, const std::string &typeStr,
                   const A1Mjd &refDate, CelestialBody *centralBody)
    : GmatBase(Gmat::REF_FRAME, typeStr, name)
{
    //loj: 3/22/04 mCentralBody = &centralBody;
    mCentralBody = centralBody;
    mRefDate = refDate;
}

//------------------------------------------------------------------------------
// RefFrame(const RefFrame &copy)
//------------------------------------------------------------------------------
RefFrame::RefFrame(const RefFrame &copy)
    : GmatBase(copy)
{
    mCentralBody = copy.mCentralBody;
    mRefDate = copy.mRefDate;
}

//------------------------------------------------------------------------------
// RefFrame& operator= (const RefFrame &right)
//------------------------------------------------------------------------------
RefFrame& RefFrame::operator= (const RefFrame &right)
{
    if (this != &right)
    {
        GmatBase::operator=(right);
        mCentralBody = right.mCentralBody;
        mRefDate = right.mRefDate;
    }

    return *this;
}

//------------------------------------------------------------------------------
// ~RefFrame()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
RefFrame::~RefFrame()
{
}

