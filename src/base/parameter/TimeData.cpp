//$Header$
//------------------------------------------------------------------------------
//                                  TimeData
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/12
//
/**
 * Implements Time related data class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "TimeData.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
TimeData::VALID_OBJECT_LIST[TimeDataObjectCount] =
{
    "Spacecraft"
}; 

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// TimeData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
TimeData::TimeData()
    : RefData()
{
    mInitialEpoch = 0.0;
}

//------------------------------------------------------------------------------
// TimeData(const TimeData &td)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <td> the object being copied.
 */
//------------------------------------------------------------------------------
TimeData::TimeData(const TimeData &td)
    : RefData(td)
{
}

//------------------------------------------------------------------------------
// TimeData& operator= (const TimeData& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
TimeData& TimeData::operator= (const TimeData& right)
{
    if (this != &right)
        RefData::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~TimeData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TimeData::~TimeData()
{
}

//------------------------------------------------------------------------------
// Real GetInitialEpoch()
//------------------------------------------------------------------------------
/**
 * Retrives initial epoch.
 */
//------------------------------------------------------------------------------
Real TimeData::GetInitialEpoch()
{
    return mInitialEpoch;
}

//------------------------------------------------------------------------------
// void SetInitialEpoch(const Real &initialEpoch)
//------------------------------------------------------------------------------
/**
 * Sets initial epoch.
 */
//------------------------------------------------------------------------------
void TimeData::SetInitialEpoch(const Real &initialEpoch)
{
    mInitialEpoch = initialEpoch;
}

//------------------------------------------------------------------------------
// Real GetCurrentTimeReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives current time.
 */
//------------------------------------------------------------------------------
Real TimeData::GetCurrentTimeReal(const std::string &str)
{
    //loj: where do I get current time? - from Spacecraft (loj: for now)
    GmatBase *obj = FindObject("Spacecraft");
    
    if (obj == NULL)
    {
        return TIME_REAL_UNDEFINED;
    }
    else
    {
        Real a1mjd = obj->GetRealParameter(0); //("Epoch"); -- just to compile
        
        if (str == "A1Mjd")
            return a1mjd;
        if (str == "Jd")
            return a1mjd + MJD_OFFSET;
    }
}

//------------------------------------------------------------------------------
// Real GetElapsedTimeReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives elapsed time from Spacecraft (loj: for now)
 */
//------------------------------------------------------------------------------
Real TimeData::GetElapsedTimeReal(const std::string &str)
{
    Real a1mjd = GetCurrentTimeReal("A1Mjd");
    
    //loj: future build
    //if (str == "Years")
    //if (str == "Months")
    if (str == "Days")
        return a1mjd - mInitialEpoch;
    //if (str == "Hours")
    //if (str == "Mins")
    if (str == "Secs")
        return (a1mjd - mInitialEpoch)* 86400;
}


//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* TimeData::GetValidObjectList() const
{
    return VALID_OBJECT_LIST;
}


//------------------------------------------------------------------------------
// bool ValidateRefObjects(GmatBase *param)
//------------------------------------------------------------------------------
/**
 * Validates reference objects for given parameter
 */
//------------------------------------------------------------------------------
bool TimeData::ValidateRefObjects(GmatBase *param)
{
    std::string paramType = param->GetTypeName();
    
    if (paramType == "ElapsedDaysParam" ||
        paramType == "CurrentA1MjdParam")
    {
        if (HasObject(VALID_OBJECT_LIST[SPACECRAFT]))
            return true;
        else
            return false;
    }
}


//------------------------------------------------------------------------------
// bool CheckRefObjectType(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Checks reference object type.
 */
//------------------------------------------------------------------------------
bool TimeData::CheckRefObjectType(GmatBase *obj)
{
    bool valid = false;
    
    // check for object type if not NULL
    if (obj != NULL)
    {
        for (int i=0; i<TimeDataObjectCount; i++)
        {
            if (obj->GetTypeName() == VALID_OBJECT_LIST[i])
            {
                valid = true;
                break;
            }
        }
    }
    else
    {
        valid = false;
    }

    return valid;
}

