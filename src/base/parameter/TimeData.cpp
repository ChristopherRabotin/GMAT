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
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
TimeData::VALID_OBJECT_TYPE_LIST[TimeDataObjectCount] =
{
   "Spacecraft" //loj: use spacecraft to get current time?
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
   mIsInitialEpochSet = false;
   mSpacecraft = NULL;
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
// bool IsInitialEpochSet()
//------------------------------------------------------------------------------
/**
 * @return true if initial epoch is set
 */
//------------------------------------------------------------------------------
bool TimeData::IsInitialEpochSet()
{
   return mIsInitialEpochSet;
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
   mIsInitialEpochSet = true;
   
   //MessageInterface::ShowMessage("TimeData::SetInitialEpoch() mInitialEpoch = %f\n",
   //                              mInitialEpoch);
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
   //GmatBase *obj = FindFirstObject("Spacecraft");
   if (mSpacecraft == NULL)
      InitializeRefObjects();

   Real a1mjd = mSpacecraft->GetRealParameter("Epoch");
   //MessageInterface::ShowMessage("TimeData::GetCurrentTimeReal() time = %f\n",
   //                              a1mjd);
   if (str == "A1MJD")
      return a1mjd;
   else if (str == "JD")
      return a1mjd + MJD_OFFSET;
   else
      throw ParameterException("TimeData::GetCurrentTimeReal() Unknown parameter name: " +
                               str);
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
   Real a1mjd = GetCurrentTimeReal("A1MJD");
    
   //loj: future build
   //if (str == "Years")
   //if (str == "Months")
   if (str == "Days")
      return a1mjd - mInitialEpoch;
   //else if (str == "Hours")
   //else if (str == "Mins")
   else if (str == "Secs")
      return (a1mjd - mInitialEpoch)* 86400;
   else
      throw ParameterException("TimeData::GetElapsedTimeReal() Unknown parameter name: " +
                               str);
   
}


//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* TimeData::GetValidObjectList() const
{
   return VALID_OBJECT_TYPE_LIST;
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
   //loj: 3/23/04 removed checking for type
   bool status = false;
    
   if (HasObjectType(VALID_OBJECT_TYPE_LIST[SPACECRAFT]))
   {
      if (mIsInitialEpochSet)
      {
         status = true;
      }
      else
      {
         Spacecraft *sc = (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
         Real rval = sc->GetRealParameter("Epoch");

         if (rval != GmatBase::REAL_PARAMETER_UNDEFINED)
         {
            mInitialEpoch = sc->GetRealParameter("Epoch");
            mIsInitialEpochSet = true;
            status = true;
         }
      }
   }

   return status;
}

//loj: 3/31/04 added
//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void TimeData::InitializeRefObjects()
{
   mSpacecraft = (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
   if (mSpacecraft == NULL)
      throw ParameterException("OrbitData::InitializeRefObjects() Cannot find Spacecraft object");
   else
      if (!mIsInitialEpochSet)
      {
         mInitialEpoch = mSpacecraft->GetRealParameter("Epoch");
         mIsInitialEpochSet = true;
         //MessageInterface::ShowMessage
         //   ("TimeData::InitializeRefObjects() set mInitialEpoch to %f\n", mInitialEpoch);
      }
}

//------------------------------------------------------------------------------
// bool IsValidObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Checks reference object type.
 */
//------------------------------------------------------------------------------
bool TimeData::IsValidObject(GmatBase *obj)
{
   bool valid = false;
    
   // check for object type if not NULL
   if (obj != NULL)
   {
      for (int i=0; i<TimeDataObjectCount; i++)
      {
         if (obj->GetTypeName() == VALID_OBJECT_TYPE_LIST[i])
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

