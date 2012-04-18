//$Id$
//------------------------------------------------------------------------------
//                                  TimeData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include "TimeSystemConverter.hpp"
#include "TimeTypes.hpp"
#include "Linear.hpp"               // for GmatRealUtil::ToString()
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_TIMEDATA

//---------------------------------
// static data
//---------------------------------

//const Real TimeData::MJD_OFFSET = GmatTimeConstants::JD_JAN_5_1941;
const Real TimeData::TIME_REAL_UNDEFINED = GmatRealConstants::REAL_UNDEFINED_LARGE;
const std::string TimeData::TIME_STRING_UNDEFINED = "INVALID_TIME";

const std::string
TimeData::VALID_OBJECT_TYPE_LIST[TimeDataObjectCount] =
{
   "Spacecraft" //loj: use spacecraft to get current time?
}; 


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// TimeData(const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
TimeData::TimeData(const std::string &name)
   : RefData(name)
{
   mInitialEpoch = 0.0;
   mIsInitialEpochSet = false;
   mSpacecraft = NULL;
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
   #ifdef DEBUG_TIMEDATA
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
   
   #ifdef DEBUG_TIMEDATA
   MessageInterface::ShowMessage
      ("TimeData::SetInitialEpoch() mInitialEpoch = %f\n",
       mInitialEpoch);
   #endif
}


//------------------------------------------------------------------------------
// Real GetTimeReal(Integer id)
//------------------------------------------------------------------------------
/**
 * Retrives current time.
 */
//------------------------------------------------------------------------------
Real TimeData::GetTimeReal(Integer id)
{
   // Get current time from Spacecraft
   if (mSpacecraft == NULL)
      InitializeRefObjects();
   
   Real a1Mjd = mSpacecraft->GetEpoch();

   #ifdef DEBUG_TIMEDATA_GET
   MessageInterface::ShowMessage
      ("TimeData::GetTimeReal() mSpacecraft=<%p>'%s', a1mjd=%f\n",
       mSpacecraft, mSpacecraft->GetName().c_str(), a1mjd);
   #endif
   
   Real time = -999.999;
   
   switch (id)
   {
   case A1:
      time = a1Mjd;
      break;
   case TAI:
      time = TimeConverterUtil::Convert(a1Mjd, TimeConverterUtil::A1MJD,
                                        TimeConverterUtil::TAIMJD,
                                        GmatTimeConstants::JD_JAN_5_1941);
      break;
   case TT:
      time = TimeConverterUtil::Convert(a1Mjd, TimeConverterUtil::A1MJD,
                                        TimeConverterUtil::TTMJD,
                                        GmatTimeConstants::JD_JAN_5_1941);
      break;
   case TDB:
      time = TimeConverterUtil::Convert(a1Mjd, TimeConverterUtil::A1MJD,
                                        TimeConverterUtil::TDBMJD,
                                        GmatTimeConstants::JD_JAN_5_1941);
      break;
   case UTC:
      time = TimeConverterUtil::Convert(a1Mjd, TimeConverterUtil::A1MJD,
                                        TimeConverterUtil::UTCMJD,
                                        GmatTimeConstants::JD_JAN_5_1941);
      break;
   default:
      throw ParameterException("TimeData::GetTimeReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(id));
   }
   
   #ifdef DEBUG_TIMEDATA_GET
   MessageInterface::ShowMessage
      ("TimeData::GetTimeReal() id=%d, a1Mjd=%.10f, return time=%.10f\n",
       id, a1Mjd, time);
   #endif

   return time;
}


//------------------------------------------------------------------------------
// void SetTimeReal(Integer id, Real value)
//------------------------------------------------------------------------------
/**
 * Sets time of real value to Spacecraft
 */
//------------------------------------------------------------------------------
void TimeData::SetTimeReal(Integer id, Real value)
{
   // Set input time to Spacecraft
   if (mSpacecraft == NULL)
      InitializeRefObjects();
   
   #ifdef DEBUG_TIMEDATA_SET
   MessageInterface::ShowMessage
      ("TimeData::SetTimeReal() entered, mSpacecraft=<%p>'%s', value=%f\n",
       mSpacecraft, mSpacecraft->GetName().c_str(), value);
   #endif
   
   Real a1Mjd = -9999.999;
   Integer epochId = mSpacecraft->GetParameterID("A1Epoch");
   
   switch (id)
   {
   case A1:
      mSpacecraft->SetRealParameter(epochId, value);
      break;
   case TAI:
      a1Mjd = TimeConverterUtil::Convert(value, TimeConverterUtil::TAIMJD,
                                         TimeConverterUtil::A1MJD,
                                         GmatTimeConstants::JD_JAN_5_1941);
      mSpacecraft->SetRealParameter(epochId, a1Mjd);
      break;
   case TT:
      a1Mjd = TimeConverterUtil::Convert(value, TimeConverterUtil::TTMJD,
                                         TimeConverterUtil::A1MJD,
                                         GmatTimeConstants::JD_JAN_5_1941);
      mSpacecraft->SetRealParameter(epochId, a1Mjd);
      break;
   case TDB:
      a1Mjd = TimeConverterUtil::Convert(value, TimeConverterUtil::TDBMJD,
                                         TimeConverterUtil::A1MJD,
                                         GmatTimeConstants::JD_JAN_5_1941);
      mSpacecraft->SetRealParameter(epochId, a1Mjd);
      break;
   case UTC:
      a1Mjd = TimeConverterUtil::Convert(value, TimeConverterUtil::UTCMJD,
                                         TimeConverterUtil::A1MJD,
                                         GmatTimeConstants::JD_JAN_5_1941);
      mSpacecraft->SetRealParameter(epochId, a1Mjd);
     break;
   default:
      throw ParameterException("TimeData::GetTimeReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(id));
   }
   
   #ifdef DEBUG_TIMEDATA_GET
   MessageInterface::ShowMessage
      ("TimeData::SetTimeReal() leaving, id=%d, a1Mjd=%f.10\n", id, a1Mjd);
   #endif
}


//------------------------------------------------------------------------------
// std::string GetTimeString(Integer id)
//------------------------------------------------------------------------------
/**
 * Retrives current time string.
 */
//------------------------------------------------------------------------------
std::string TimeData::GetTimeString(Integer id)
{
   // Get current time from Spacecraft
   if (mSpacecraft == NULL)
      InitializeRefObjects();
   
   Real time = GetTimeReal(id);
   
   switch (id)
   {
   case A1:
   case TAI:
   case TT:
   case TDB:
   case UTC:
      #ifdef DEBUG_TIMEDATA
      MessageInterface::ShowMessage
         ("TimeData::GetTimeString() id=%d, timeStr = %s\n", id,
          TimeConverterUtil::ConvertMjdToGregorian(time).c_str());
      #endif
      return TimeConverterUtil::ConvertMjdToGregorian(time);
   default:
      throw ParameterException("TimeData::GetTimeString() Unknown parameter id: " +
                               GmatRealUtil::ToString(id));
   }
}


//------------------------------------------------------------------------------
// void SetTimeString(Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets time of string value in gregorian to Spacecraft
 */
//------------------------------------------------------------------------------
void TimeData::SetTimeString(Integer id, const std::string &value)
{
   // Set input time to Spacecraft
   if (mSpacecraft == NULL)
      InitializeRefObjects();
   
   #ifdef DEBUG_TIMEDATA_SET
   MessageInterface::ShowMessage
      ("TimeData::SetTimeString() entered, mSpacecraft=<%p>'%s', value=%s\n",
       mSpacecraft, mSpacecraft->GetName().c_str(), value.c_str());
   #endif
   
   Integer epochId = mSpacecraft->GetParameterID("A1Epoch");
   Real fromMjd = -999.999;
   Real a1Mjd = -999.999;
   std::string a1MjdString;
   switch (id)
   {
   case A1:
      TimeConverterUtil::Convert("A1Gregorian", fromMjd, value, 
                                 "A1ModJulian", a1Mjd, a1MjdString);
      mSpacecraft->SetRealParameter(epochId, a1Mjd);
      break;
   case TAI:
      TimeConverterUtil::Convert("TAIGregorian", fromMjd, value, 
                                 "A1ModJulian", a1Mjd, a1MjdString);
      mSpacecraft->SetRealParameter(epochId, a1Mjd);
      break;
   case TT:
      TimeConverterUtil::Convert("TTGregorian", fromMjd, value, 
                                 "A1ModJulian", a1Mjd, a1MjdString);
      mSpacecraft->SetRealParameter(epochId, a1Mjd);
      break;
   case TDB:
      TimeConverterUtil::Convert("TDBGregorian", fromMjd, value, 
                                 "A1ModJulian", a1Mjd, a1MjdString);
      mSpacecraft->SetRealParameter(epochId, a1Mjd);
      break;
   case UTC:
      TimeConverterUtil::Convert("UTCGregorian", fromMjd, value, 
                                 "A1ModJulian", a1Mjd, a1MjdString);
      mSpacecraft->SetRealParameter(epochId, a1Mjd);
     break;
   default:
      throw ParameterException("TimeData::GetTimeReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(id));
   }
   
   #ifdef DEBUG_TIMEDATA_GET
   MessageInterface::ShowMessage
      ("TimeData::SetTimeString() leaving, id=%d, a1Mjd=%f.10\n", id, a1Mjd);
   #endif
}


//------------------------------------------------------------------------------
// Real GetElapsedTimeReal(Integer id)
//------------------------------------------------------------------------------
/**
 * Retrives elapsed time from Spacecraft (loj: for now)
 */
//------------------------------------------------------------------------------
Real TimeData::GetElapsedTimeReal(Integer id)
{
   Real a1mjd = GetTimeReal(A1);
   
   switch (id)
   {
   //case YEARS:
   //case MONTHS:
   case DAYS:
      return a1mjd - mInitialEpoch;
   //case HOURS:
   //case MINS:
   case SECS:
      return (a1mjd - mInitialEpoch)* GmatTimeConstants::SECS_PER_DAY;
   default:
      throw ParameterException("TimeData::GetElapsedTimeReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(id));
   }
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
   {
      std::string scName = GetRefObjectName(Gmat::SPACECRAFT);
      #ifdef DEBUG_TIMEDATA
      MessageInterface::ShowMessage
         ("TimeData::InitializeRefObjects() Cannot find Spacecraft object named "
          "\"%s\"\n", scName.c_str());
      #endif
      
      throw ParameterException
         ("TimeData::InitializeRefObjects() Cannot find Spacecraft object named \"" + 
          scName + "\"\n");
   }
   else
   {
      if (!mIsInitialEpochSet)
      {
         mInitialEpoch = mSpacecraft->GetEpoch();
         mIsInitialEpochSet = true;
         
         #ifdef DEBUG_TIMEDATA
         MessageInterface::ShowMessage
            ("TimeData::InitializeRefObjects() set mInitialEpoch to %f\n", mInitialEpoch);
         #endif
      }
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



