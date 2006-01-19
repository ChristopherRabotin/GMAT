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

//#define DEBUG_TIMEDATA 1

//---------------------------------
// static data
//---------------------------------

const Real TimeData::MJD_OFFSET = 2430000.0; 
const Real TimeData::TIME_REAL_UNDEFINED = -9876543210.1234;

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
   mEpochId = -1;
}


//------------------------------------------------------------------------------
// TimeData(const TimeData &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the object being copied.
 */
//------------------------------------------------------------------------------
TimeData::TimeData(const TimeData &copy)
   : RefData(copy)
{
   mInitialEpoch = copy.mInitialEpoch;
   mIsInitialEpochSet = copy.mIsInitialEpochSet;
   mSpacecraft = copy.mSpacecraft;
}


//------------------------------------------------------------------------------
// TimeData& operator= (const TimeData &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
TimeData& TimeData::operator= (const TimeData &right)
{
   if (this != &right)
   {
      RefData::operator=(right);
      
      mInitialEpoch = right.mInitialEpoch;
      mIsInitialEpochSet = right.mIsInitialEpochSet;
      mSpacecraft = right.mSpacecraft;
   }
   
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
// Real GetInitialEpoch() const
//------------------------------------------------------------------------------
/**
 * Retrives initial epoch.
 */
//------------------------------------------------------------------------------
Real TimeData::GetInitialEpoch() const
{
   #if DEBUG_TIMEDATA
   MessageInterface::ShowMessage
      ("TimeData::GetInitialEpoch() returning %f\n", mInitialEpoch);
   #endif
   
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
   
   #if DEBUG_TIMEDATA
   MessageInterface::ShowMessage
      ("TimeData::SetInitialEpoch() mInitialEpoch = %f\n",
       mInitialEpoch);
   #endif
}


//------------------------------------------------------------------------------
// Real GetCurrentTimeReal(Integer id)
//------------------------------------------------------------------------------
/**
 * Retrives current time.
 */
//------------------------------------------------------------------------------
Real TimeData::GetCurrentTimeReal(GmatParam::TimeDataID id)
{
   //loj: where do I get current time? - from Spacecraft (loj: for now)
   //GmatBase *obj = FindFirstObject("Spacecraft");
   if (mSpacecraft == NULL)
      InitializeRefObjects();

   //Real a1mjd = mSpacecraft->GetRealParameter("A1Epoch");
   Real a1mjd = mSpacecraft->GetRealParameter(mEpochId);

   #if DEBUG_TIMEDATA
   MessageInterface::ShowMessage("TimeData::GetCurrentTimeReal() time = %f\n",
                                 a1mjd);
   #endif

   switch (id)
   {
   case GmatParam::A1MJD:
      return a1mjd;
   case GmatParam::JD:
      return a1mjd + MJD_OFFSET;
   default:
      throw ParameterException("TimeData::GetCurrentTimeReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(id));
   }
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

   //Real a1mjd = mSpacecraft->GetRealParameter("A1Epoch");
   Real a1mjd = mSpacecraft->GetRealParameter(mEpochId);

   #if DEBUG_TIMEDATA
   MessageInterface::ShowMessage("TimeData::GetCurrentTimeReal() time = %f\n",
                                 a1mjd);
   #endif
   
   if (str == "A1MJD")
      return a1mjd;
   else if (str == "JD")
      return a1mjd + MJD_OFFSET;
   else
      throw ParameterException("TimeData::GetCurrentTimeReal() Unknown parameter name: " +
                               str);
}


//------------------------------------------------------------------------------
// Real GetElapsedTimeReal(GmatParam::TimeDataID id)
//------------------------------------------------------------------------------
/**
 * Retrives elapsed time from Spacecraft (loj: for now)
 */
//------------------------------------------------------------------------------
Real TimeData::GetElapsedTimeReal(GmatParam::TimeDataID id)
{
   //Real a1mjd = GetCurrentTimeReal("A1MJD");
   Real a1mjd = GetCurrentTimeReal(GmatParam::A1MJD);
   
   switch (id)
   {
   //case GmatParam::YEARS:
   //case GmatParam::MONTHS:
   case GmatParam::DAYS:
      return a1mjd - mInitialEpoch;
   //case GmatParam::HOURS:
   //case GmatParam::MINS:
   case GmatParam::SECS:
      return (a1mjd - mInitialEpoch)* 86400;
   default:
      throw ParameterException("TimeData::GetElapsedTimeReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(id));
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
   //Real a1mjd = GetCurrentTimeReal("A1MJD");
   Real a1mjd = GetCurrentTimeReal(GmatParam::A1MJD);

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
         Real rval = sc->GetRealParameter("A1Epoch");

         if (rval != GmatBase::REAL_PARAMETER_UNDEFINED)
         {
            mInitialEpoch = sc->GetRealParameter("A1Epoch");
            mIsInitialEpochSet = true;
            status = true;
         }
      }
   }

   return status;
}


//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void TimeData::InitializeRefObjects()
{
   mSpacecraft = (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
   if (mSpacecraft == NULL)
      throw ParameterException("TimeData::InitializeRefObjects() Cannot find "
                               "Spacecraft object");
   else
      if (!mIsInitialEpochSet)
      {
         mEpochId = mSpacecraft->GetParameterID("A1Epoch");
         //mInitialEpoch = mSpacecraft->GetRealParameter("A1Epoch");
         mInitialEpoch = mSpacecraft->GetRealParameter(mEpochId);
         mIsInitialEpochSet = true;
         //MessageInterface::ShowMessage
         //   ("TimeData::InitializeRefObjects() set mInitialEpoch to %f\n", mInitialEpoch);
      }
}


//------------------------------------------------------------------------------
// virtual bool IsValidObjectType(Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Checks reference object type.
 *
 * @return return true if object is valid object, false otherwise
 */
//------------------------------------------------------------------------------
bool TimeData::IsValidObjectType(Gmat::ObjectType type)
{
   for (int i=0; i<TimeDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   return false;

}



