//$Header$
//------------------------------------------------------------------------------
//                              BaseStopCondition
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/10/09
//
/**
 * Implements StopCondition class.
 * It stores last five points of integration epoch and values. When desired
 * stopping condition is fallen between forth and fifth value, it feeds all five
 * points to CubicSplineInterpolator to estimate the epoch of desired stopping
 * condition.
 */
//------------------------------------------------------------------------------

#include "StopCondition.hpp"
#include "StopConditionException.hpp"
#include "CubicSplineInterpolator.hpp"
#include "NotAKnotInterpolator.hpp"
#include "KeplerianParameters.hpp"     // for KepEcc()
#include "SphericalParameters.hpp"     // for SphRMag()
#include "MessageInterface.hpp"

//#define DEBUG_BASE_STOPCOND 1
//#define DEBUG_BASE_STOPCOND_INIT 1
//#define DEBUG_BASE_STOPCOND_GET 1
//#define DEBUG_BASE_STOPCOND_SET 1
//#define DEBUG_RENAME 1

//---------------------------------
// static data
//---------------------------------
const Real BaseStopCondition::STOP_COND_TOL = 1.0e-11;

const std::string
BaseStopCondition::PARAMETER_TEXT[BaseStopConditionParamCount - GmatBaseParamCount] =
{
   "BaseEpoch",
   "Epoch",
   "EpochVar",
   "StopVar",
   "Goal",
   "Tol",
   "EccTol",
   "Range",
   "Repeat",
   "Interpolator",
};

const Gmat::ParameterType
BaseStopCondition::PARAMETER_TYPE[BaseStopConditionParamCount - GmatBaseParamCount] =
{
   Gmat::REAL_TYPE,    //"BaseEpoch",
   Gmat::REAL_TYPE,    //"Epoch",
   Gmat::STRING_TYPE,  //"EpochVar",
   Gmat::STRING_TYPE,  //"StopVar",
   Gmat::STRING_TYPE,  //"Goal",
   Gmat::REAL_TYPE,    //"Tol",
   Gmat::REAL_TYPE,    //"EccTol",
   Gmat::REAL_TYPE,    //"Range",
   Gmat::INTEGER_TYPE, //"Repeat",
   Gmat::STRING_TYPE,  //"Interpolator",
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// BaseStopCondition(const std::string &name, const std::string &desc,
//                   Parameter *epochParam, Parameter *stopParam, const Real &goal,
//                   const Real &tol, const Integer repeatCount,
//                   Interpolator *interp)
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
BaseStopCondition::BaseStopCondition(const std::string &name, const std::string &desc,
                                     Parameter *epochParam, Parameter *stopParam,
                                     const Real &goal, const Real &tol,
                                     const Integer repeatCount, //RefFrame *refFrame,
                                     Interpolator *interp)
   : GmatBase(Gmat::STOP_CONDITION, "StopCondition", name),
     previousEpoch      (-999999.999999),
     previousValue      (-999999.999999),
     isAngleParameter   (false),
     isPeriapse         (false),
     isApoapse          (false)
{
   objectTypes.push_back(Gmat::STOP_CONDITION);
   objectTypeNames.push_back("BaseStopCondition");
   
   mBaseEpoch = 0.0;
   mEpoch = 0.0;
   mGoal = goal;
   mTolerance = tol;
   mRepeatCount = repeatCount;
   mGoalStr = "0.0";
   mInterpolator = interp;
   mStopParam = stopParam;
   mGoalParam = NULL;
   mEpochParam = epochParam;
   mEccParam = NULL;
   mRmagParam = NULL;
   mDescription = desc;
   mEccTol = 1.0e-6;  //loj: valid default?
   mRange = 100000;   //loj: valid default? 6/16/04 changed to 100000 from 10000
   
   mStopParamType = "";
   mStopParamName = "";
   if (mStopParam != NULL)
   {
      mStopParamType = mStopParam->GetTypeName();
      mStopParamName = mStopParam->GetName();
   }
   
   mNumValidPoints = 0;
   mBufferSize = 0;
   mStopEpoch = REAL_PARAMETER_UNDEFINED;
   mStopInterval = 0.0;
   
   mInitialized = false;
   mUseInternalEpoch = false;
   mNeedInterpolator = false;
   mAllowGoalParam = false;
   mBackwardsProp = false;
   
   if (epochParam == NULL)
      mUseInternalEpoch = true;
   
   // Create default Interpolator
   if (mInterpolator == NULL)
      mInterpolator = new NotAKnotInterpolator("InternalInterpolator");
   
   mSolarSystem = NULL;
}


//------------------------------------------------------------------------------
// BaseStopCondition(const BaseStopCondition &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 */
//------------------------------------------------------------------------------
BaseStopCondition::BaseStopCondition(const BaseStopCondition &copy)
   : GmatBase(copy),
     previousEpoch      (-999999.999999),
     previousValue      (-999999.999999),
     isAngleParameter   (copy.isAngleParameter),
     isPeriapse         (copy.isPeriapse),
     isApoapse          (copy.isApoapse)
{
   mAllRefObjectNames = copy.mAllRefObjectNames;  // Is this correct?
   
   mBaseEpoch = copy.mBaseEpoch;
   mEpoch = copy.mEpoch;
   mGoal = copy.mGoal;
   mTolerance = copy.mTolerance;
   mEccTol = copy.mEccTol;
   mRange = copy.mRange;
   mRepeatCount = copy.mRepeatCount;
   //mInterpolator = copy.mInterpolator;
   
   if (copy.mInterpolator != NULL)
      if (copy.mInterpolator->GetName() == "InternalInterpolator")
         mInterpolator = (Interpolator*)copy.mInterpolator->Clone();

   mSolarSystem = copy.mSolarSystem;
   mDescription = copy.mDescription;
   mStopParamType = copy.mStopParamType;
   mStopParamName = copy.mStopParamName;
   mGoalStr = copy.mGoalStr;
   
   mStopEpoch = copy.mStopEpoch;
   mStopInterval = 0.0;
   mStopParam = copy.mStopParam;
   mEpochParam = copy.mEpochParam;
   mGoalParam = copy.mGoalParam;
   mEccParam = copy.mEccParam;
   mRmagParam = copy.mRmagParam;
   
   mInitialized = copy.mInitialized;
   mUseInternalEpoch = copy.mUseInternalEpoch;
   mNeedInterpolator = copy.mNeedInterpolator;
   mAllowGoalParam = copy.mAllowGoalParam;
   mBackwardsProp = copy.mBackwardsProp;
   
   CopyDynamicData(copy);
}


//------------------------------------------------------------------------------
// BaseStopCondition& operator= (const BaseStopCondition &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 */
//------------------------------------------------------------------------------
BaseStopCondition& BaseStopCondition::operator= (const BaseStopCondition &right)
{
   if (this == &right)
      return *this;
   
   GmatBase::operator=(right);
   
   mAllRefObjectNames = right.mAllRefObjectNames; // Is this correct?
   
   mBaseEpoch = right.mBaseEpoch;
   mEpoch = right.mEpoch;
   mGoal = right.mGoal;
   mTolerance = right.mTolerance;
   mEccTol = right.mEccTol;
   mRange = right.mRange;
   mRepeatCount = right.mRepeatCount;
   //mInterpolator = right.mInterpolator;

   if (right.mInterpolator != NULL)
      if (right.mInterpolator->GetName() == "InternalInterpolator")
         mInterpolator = (Interpolator*)right.mInterpolator->Clone();

   mSolarSystem = right.mSolarSystem;
   mDescription = right.mDescription;
   mStopParamType = right.mStopParamType;
   mStopParamName = right.mStopParamName;
   mGoalStr = right.mGoalStr;
   
   mStopEpoch = right.mStopEpoch;
   mStopInterval = right.mStopInterval;
   mStopParam = right.mStopParam;
   mEpochParam = right.mEpochParam;
   mGoalParam = right.mGoalParam;
   mEccParam = right.mEccParam;
   mRmagParam = right.mRmagParam;
   
   mInitialized = right.mInitialized;
   mUseInternalEpoch = right.mUseInternalEpoch;
   mNeedInterpolator = right.mNeedInterpolator;
   mAllowGoalParam = right.mAllowGoalParam;
   mBackwardsProp = right.mBackwardsProp;
   
   previousEpoch = -999999.999999;
   previousValue = -999999.999999;
   
   isAngleParameter = right.isAngleParameter;
   isPeriapse       = right.isPeriapse;
   isApoapse        = right.isApoapse;
   
   CopyDynamicData(right);

   return *this;
}


//------------------------------------------------------------------------------
// virtual ~BaseStopCondition()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BaseStopCondition::~BaseStopCondition()
{
   if (mEccParam != NULL)
      delete mEccParam;

   if (mRmagParam != NULL)
      delete mRmagParam;

   // loj:04/26/06 Set to NULL
   mEccParam = NULL;
   mRmagParam = NULL;

   if (mInterpolator != NULL)
   {
      if (mInterpolator->GetName() == "InternalInterpolator")
      {
         delete mInterpolator;
         mInterpolator = NULL;
      }
   }
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool BaseStopCondition::Initialize()
{
   mInitialized = false;

   mStopInterval = 0.0; ///loj
   
   // clear local parameters
   if (mEccParam != NULL)
      delete mEccParam;

   if (mRmagParam != NULL)
      delete mRmagParam;

   mEccParam = NULL;
   mRmagParam = NULL;

   if (Validate())
   {
      if (mStopParam->GetTypeName() == "Apoapsis" ||
          mStopParam->GetTypeName() == "Periapsis")
      {
         mGoal = 0.0;
         mAllowGoalParam = false;
         if (mStopParam->GetTypeName() == "Apoapsis")
            isApoapse = true;
         if (mStopParam->GetTypeName() == "Periapsis")
            isPeriapse = true;
      }
      
      if (mStopParamType == "TA" || mStopParamType == "MA" ||
          mStopParamType == "EA")
      {
         isAngleParameter = true;
      }
      
      if (mNeedInterpolator)
      {
         mBufferSize = mInterpolator->GetBufferSize();
         mEpochBuffer.reserve(mBufferSize);
         mValueBuffer.reserve(mBufferSize);
         
         for (int i=0; i<mBufferSize; i++)
         {
            mEpochBuffer[i] = 0.0;
            mValueBuffer[i] = 0.0;
         }
         
         mNumValidPoints = 0;
         mInitialized = true;
      }
      else
      {
         mInitialized = true;
      }
   }
   
   #if DEBUG_BASE_STOPCOND_INIT
   MessageInterface::ShowMessage
      ("BaseStopCondition::Initialize() mInitialized=%d\n",
       mInitialized);
   #endif
   
   return mInitialized;
}


//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
/**
 * @return true if all necessary objects have been set; false otherwise
 *
 * @exception thrown if necessary object pointer is NULL
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::Validate()
{   
   // check on epoch parameter
   if (!mUseInternalEpoch && mEpochParam == NULL)
   {
      throw StopConditionException
         ("BaseStopCondition::Validate() epoch parameter: " + mEpochParamName +
          " has NULL pointer.\n");
   }
   
   // check on stop parameter
   if (mStopParam == NULL)
   {
      throw StopConditionException
         ("BaseStopCondition::Validate() stop parameter: " + mStopParamName +
          " has NULL pointer.\n");
   }
   
   // check on interpolator
   if (mStopParam->IsTimeParameter())
   {
      mNeedInterpolator = false;
   }
   else
   {
      if (mInterpolator == NULL)
      {
         throw StopConditionException
            ("BaseStopCondition::Validate() Interpolator: " + mInterpolatorName +
             " has NULL pointer.\n");
      }
         
      mNeedInterpolator = true;
   }
   
   // check on goal parameter
   if (mAllowGoalParam && mGoalParam == NULL)
      throw StopConditionException
         ("BaseStopCondition::Validate() goal parameter: " + mGoalStr +
          " has NULL pointer.\n");
   
   // Apoapsis and Periapsis need additional parameters
   if (mStopParam->GetTypeName() == "Apoapsis" ||
       mStopParam->GetTypeName() == "Periapsis")
   {
      // check on Ecc parameter
      if (mEccParam  == NULL)
      {
         #if DEBUG_BASE_STOPCOND
         MessageInterface::ShowMessage
            ("BaseStopCondition::Validate(): Creating KepEcc...\n");
         #endif
         
         mEccParam = new KepEcc("");
         
         mEccParam->AddRefObject
            (mStopParam->GetRefObject(Gmat::SPACECRAFT, 
                                      mStopParam->GetRefObjectName(Gmat::SPACECRAFT)));
         mEccParam->AddRefObject
            (mStopParam->GetRefObject(Gmat::COORDINATE_SYSTEM, 
                                      mStopParam->GetRefObjectName(Gmat::COORDINATE_SYSTEM)));
         
         mEccParam->AddRefObject
            (mStopParam->GetRefObject(Gmat::SPACE_POINT, 
                                      mStopParam->GetRefObjectName(Gmat::SPACE_POINT)),
             true);
         
         mEccParam->SetInternalCoordSystem(mStopParam->GetInternalCoordSystem());            
         mEccParam->AddRefObject(mSolarSystem);
         mEccParam->Initialize();
      }
      
      // check on SphRMag parameter if "Periapsis"
      if (mStopParam->GetTypeName() == "Periapsis")
      {
         if (mRmagParam == NULL)
         {
            #if DEBUG_BASE_STOPCOND_INIT
            MessageInterface::ShowMessage
               ("BaseStopCondition::Validate(): Creating SphRMag...\n");
            #endif
            
            std::string depObjName = mStopParam->GetStringParameter("DepObject");
            
            #if DEBUG_BASE_STOPCOND_INIT
            MessageInterface::ShowMessage
               ("BaseStopCondition::Validate() depObjName of mStopParam=%s\n",
                depObjName.c_str());
            #endif
            
            mRmagParam = new SphRMag("");
            mRmagParam->SetStringParameter("DepObject", depObjName);
            
            mRmagParam->AddRefObject
               (mStopParam->GetRefObject(Gmat::SPACECRAFT,
                                         mStopParam->GetRefObjectName(Gmat::SPACECRAFT)));
            mRmagParam->AddRefObject
               (mStopParam->GetRefObject(Gmat::COORDINATE_SYSTEM,
                                         mStopParam->GetRefObjectName(Gmat::COORDINATE_SYSTEM)));
            mRmagParam->AddRefObject
               (mStopParam->GetRefObject(Gmat::SPACE_POINT,
                                         mStopParam->GetRefObjectName(Gmat::SPACE_POINT)),
                true);
            
            mRmagParam->SetInternalCoordSystem(mStopParam->GetInternalCoordSystem());
            mRmagParam->AddRefObject(mSolarSystem);
            mRmagParam->Initialize();
            
            // set mRange
            if (depObjName == "Earth")
               mRange = 5.0e5;
            else if (depObjName == "Luna")
               //mRange = 2.0e5; // Swingby has this.
               mRange = 5.0e5;
            else
               mRange = 1.0e10;
         }
      }
   }
   
   #if DEBUG_BASE_STOPCOND_INIT
   MessageInterface::ShowMessage
      ("BaseStopCondition::Validate() mUseInternalEpoch=%d, "
       "mEpochParam=%d, mStopParam=%d\n   mInterpolator=%d, mRange=%f\n",
       mUseInternalEpoch, mEpochParam, mStopParam, mInterpolator, mRange);
   #endif
   
   return true;
}

void BaseStopCondition::Reset()
{
   mNumValidPoints = 0;
}

Real BaseStopCondition::GetStopInterval()
{
   return mStopInterval;
}


//------------------------------------------------------------------------------
// bool IsInitialized()
//------------------------------------------------------------------------------
bool BaseStopCondition::IsInitialized()
{
   return mInitialized;
}


//------------------------------------------------------------------------------
// Integer GetBufferSize()
//------------------------------------------------------------------------------
Integer BaseStopCondition::GetBufferSize()
{
   return mBufferSize;
}


//------------------------------------------------------------------------------
// std::string& GetDescription()
//------------------------------------------------------------------------------
std::string& BaseStopCondition::GetDescription()
{
   return mDescription;
}


//------------------------------------------------------------------------------
// Parameter* GetEpochParameter()
//------------------------------------------------------------------------------
Parameter* BaseStopCondition::GetEpochParameter()
{
   return mEpochParam;
}


//------------------------------------------------------------------------------
// Parameter* GetStopParameter()
//------------------------------------------------------------------------------
Parameter* BaseStopCondition::GetStopParameter()
{
   return mStopParam;
}


//------------------------------------------------------------------------------
// Interpolator* GetInterpolator()
//------------------------------------------------------------------------------
Interpolator* BaseStopCondition::GetInterpolator()
{
   return mInterpolator;
}


//------------------------------------------------------------------------------
// void SetDescription(const std::string &desc)
//------------------------------------------------------------------------------
void BaseStopCondition::SetDescription(const std::string &desc)
{
   mDescription = desc;
}


//------------------------------------------------------------------------------
// void SetPropDirection(Real dir)
//------------------------------------------------------------------------------
void BaseStopCondition::SetPropDirection(Real dir)
{   
   if (dir >= 1.0)
      mBackwardsProp = false;
   else
      mBackwardsProp = true;
   
   #if DEBUG_BASE_STOPCOND
   MessageInterface::ShowMessage
      ("BaseStopCondition::SetPropDirection() dir=%f, mBackwardsProp=%d\n",
       dir, mBackwardsProp);
   #endif
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *solarSystem)
//------------------------------------------------------------------------------
/**
 * Sets SolarSystem
 */
//------------------------------------------------------------------------------
void BaseStopCondition::SetSolarSystem(SolarSystem *solarSystem)
{
   if (solarSystem != NULL)
   {
      mSolarSystem = solarSystem;
   }
   else
   {
      throw StopConditionException(
         "Attempting to set solar system with NULL pointer.");
   }
}


//------------------------------------------------------------------------------
// bool SetInterpolator(Interpolator *interp)
//------------------------------------------------------------------------------
/**
 * Sets Interpolator to interpolate stop condition epoch.
 *
 * @return true if Interplator is successfully set; false otherwise
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::SetInterpolator(Interpolator *interp)
{
   if (interp != NULL)
   {
      if (mInterpolator->GetName() == "InternalInterpolator")
         delete mInterpolator;
      
      mInterpolator = interp;
      return true;
   }
   else
   {
      return false;
   }
}


//------------------------------------------------------------------------------
// bool SetEpochParameter(Parameter *param)
//------------------------------------------------------------------------------
/**
 * Sets epoch parameter which will be used to interpolate stop epoch
 *
 * @return true if EpochParam is successfully set; false otherwise
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::SetEpochParameter(Parameter *param)
{
   if (param != NULL)
   {
      mEpochParam = param;
      mUseInternalEpoch = false;
      return true;
   }
   else
   {
      return false;
   }
}


//------------------------------------------------------------------------------
// virtual bool SetStopParameter(Parameter *param)
//------------------------------------------------------------------------------
/**
 * Add parameter to stop condition.
 *
 * @return true if parameter has added to array.
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::SetStopParameter(Parameter *param)
{
   if (param != NULL)
   {
      mStopParam = param;
      mStopParamType = mStopParam->GetTypeName();
      
      if (param->IsTimeParameter())
         mInitialized = true;
      
      return true;
   }
   
   return false;
}


//------------------------------------------------------------------------------
// virtual bool SetGoalParameter(Parameter *param)
//------------------------------------------------------------------------------
/**
 * Sets goal parameter to stop condition.
 *
 * @return true if parameter has added to array.
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::SetGoalParameter(Parameter *param)
{
   mGoalParam = param;
   
   return true;
}


//------------------------------------------------------------------------------
// void SetGoalString(const std::string &str)
//------------------------------------------------------------------------------
void BaseStopCondition::SetGoalString(const std::string &str)
{
   mGoalStr = str;
   
   // remove leading blanks
   std::string::size_type pos = mGoalStr.find_first_not_of(' ');
   mGoalStr.erase(0, pos);
   
   // if str is just a number
   if (isdigit(mGoalStr[0]) || mGoalStr[0] == '.' || mGoalStr[0] == '-')
   {
      mGoal = atof(mGoalStr.c_str());
      mAllowGoalParam = false;
   }
   else
   {
      mAllowGoalParam = true;
   }
   
   #if DEBUG_BASE_STOPCOND
   MessageInterface::ShowMessage
      ("BaseStopCondition::SetGoalString() mAllowGoalParam=%d, mGoalStr=<%s>, "
       "mGoal=%le\n", mAllowGoalParam, mGoalStr.c_str(), mGoal);
   #endif
   
}


//------------------------------------------------------------------------------
// virtual bool SetSpacecraft(Spacecraft *sc)
//------------------------------------------------------------------------------
/**
 * Sets spacecraft pointer to internal parameter used in stop condition.
 *
 * @return true if spacecraft has been set.
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::SetSpacecraft(SpaceObject *sc)
{
   if (mEccParam != NULL)
      mEccParam->SetRefObject(sc, Gmat::SPACECRAFT, sc->GetName());
   
   if (mRmagParam != NULL)
      mRmagParam->SetRefObject(sc, Gmat::SPACECRAFT, sc->GetName());
   
   return true;
}


//------------------------------------------------------------------------------
// virtual Real BaseStopCondition::GetStopEpoch()
//------------------------------------------------------------------------------
/**
 * @return epoch at stop condition met.
 */
//------------------------------------------------------------------------------
Real BaseStopCondition::GetStopEpoch()
{
   return mStopEpoch;
}


//---------------------------------
// methods inherited from GmatBase
//---------------------------------

//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool BaseStopCondition::RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("BaseStopCondition::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type == Gmat::SPACECRAFT)
   {
      // set new StopCondition name
      std::string name = GetName();
      std::string::size_type pos = name.find(oldName);
      
      if (pos != name.npos)
      {
         name.replace(pos, oldName.size(), newName);
         SetName(name);
      }
      
      // epoch parameter name
      pos = mEpochParamName.find(oldName);
      if (pos != mEpochParamName.npos)
         mEpochParamName.replace(pos, oldName.size(), newName);
      
      // stop parameter name
      pos = mStopParamName.find(oldName);
      if (pos != mStopParamName.npos)
         mStopParamName.replace(pos, oldName.size(), newName);
   }
   
   return true;
}


//---------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//---------------------------------------------------------------------------
/**
 * Retrieves reference object name array for given type. It will return all
 * object names if type is Gmat::UNKNOWN_NAME.
 *
 * @param <type> object type
 * @return reference object name.
 */
//------------------------------------------------------------------------------
const StringArray&
BaseStopCondition::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   mAllRefObjectNames.clear();
   
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::PARAMETER)
   {
      mAllRefObjectNames.push_back(mStopParamName);
      if (mAllowGoalParam)
         mAllRefObjectNames.push_back(mGoalStr);
   }
   
   return mAllRefObjectNames;
}


//---------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name)
//---------------------------------------------------------------------------
bool BaseStopCondition::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name)
{
   #if DEBUG_BASE_STOPCOND_GET
   MessageInterface::ShowMessage
      ("BaseStopCondition::SetRefObject() Name=%s, mStopParamName=%s, mGoalStr=%s\n",
       this->GetName().c_str(), mStopParamName.c_str(), mGoalStr.c_str());
   #endif
   
   if (type == Gmat::PARAMETER)
   {
      if (name == mStopParamName)
         SetStopParameter((Parameter*)obj);
      else if (name == mGoalStr)
         SetGoalParameter((Parameter*)obj);
      else if (name == mEpochParamName)
         SetEpochParameter((Parameter*)obj);
      
      return true;
   }
   else if (type == Gmat::INTERPOLATOR)
   {
      SetInterpolator((Interpolator*)obj);
      return true;
   }
   
   return GmatBase::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string BaseStopCondition::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < BaseStopConditionParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterText(id);
    
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer BaseStopCondition::GetParameterID(const std::string &str) const
{
   #if DEBUG_BASE_STOPCOND_GET > 1
   MessageInterface::ShowMessage
      ("BaseStopCondition::GetParameterID() str = %s\n", str.c_str());
   #endif
   
   for (int i=GmatBaseParamCount; i<BaseStopConditionParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType BaseStopCondition::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < BaseStopConditionParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string BaseStopCondition::GetParameterTypeString(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < BaseStopConditionParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return GmatBase::GetParameterTypeString(id);
    
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer BaseStopCondition::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
   case REPEAT_COUNT:
      return mRepeatCount;
   default:
      return GmatBase::GetIntegerParameter(id);
   }
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
Integer BaseStopCondition::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer BaseStopCondition::SetIntegerParameter(const Integer id,
                                               const Integer value)
{
   switch (id)
   {
   case REPEAT_COUNT:
      mRepeatCount = value;
      return mRepeatCount;
   default:
      return GmatBase::SetIntegerParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
Integer BaseStopCondition::SetIntegerParameter(const std::string &label,
                                               const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
Real BaseStopCondition::GetRealParameter(const Integer id) const
{
   switch (id)
   {
   case TOLERANCE:
      return mTolerance;
   case ECC_TOL:
      return mEccTol;
   case RANGE:
      return mRange;
   case BASE_EPOCH:
      return mBaseEpoch;
   case EPOCH:
      return mEpoch;
   default:
      return GmatBase::GetRealParameter(id);
   }
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
Real BaseStopCondition::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real BaseStopCondition::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case TOLERANCE:
      mTolerance = value;
      return mTolerance;
   case ECC_TOL:
      mEccTol = value;
      return mEccTol;
   case RANGE:
      mRange = value;
      return mRange;
   case BASE_EPOCH:
      mBaseEpoch = value;
      return mBaseEpoch;
   case EPOCH:
      mEpoch = value;
      return mEpoch;
   default:
      return GmatBase::GetRealParameter(id);
   }
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
Real BaseStopCondition::SetRealParameter(const std::string &label, const Real value)
{
   #if DEBUG_BASE_STOPCOND_SET
   MessageInterface::ShowMessage
      ("BaseStopCondition::SetRealParameter() label=%s, "
       "value=%le\n", label.c_str(), value);
   #endif
   
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string BaseStopCondition::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case EPOCH_VAR:
      return mEpochParamName;
   case STOP_VAR:
      return mStopParamName;
   case GOAL:
      return mGoalStr;
   case INTERPOLATOR:
      return mInterpolatorName;
   default:
      return GmatBase::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string BaseStopCondition::GetStringParameter(const std::string &label) const
{
   #if DEBUG_BASE_STOPCOND_GET
   MessageInterface::ShowMessage
      ("BaseStopCondition::GetStringParameter() label = %s\n", label.c_str());
   #endif
   
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool BaseStopCondition::SetStringParameter(const Integer id, const std::string &value)
{
   #if DEBUG_BASE_STOPCOND_SET
   MessageInterface::ShowMessage
      ("BaseStopCondition::SetStringParameter() id = %d, value = %s \n",
       id, value.c_str());
   #endif
   
   switch (id)
   {
   case EPOCH_VAR:
      mEpochParamName = value;
      return true;
   case STOP_VAR:
      mStopParamName = value;
      return true;
   case GOAL:
      SetGoalString(value);
      return true;
   case INTERPOLATOR:
      mInterpolatorName = value;
      return true;
   default:
      return GmatBase::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool BaseStopCondition::SetStringParameter(const std::string &label,
                                           const std::string &value)
{
   #if DEBUG_BASE_STOPCOND_SET
   MessageInterface::ShowMessage
      ("BaseStopCondition::SetStringParameter() label = %s, "
       "value = %s \n", label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}


//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void CopyDynamicData()
//------------------------------------------------------------------------------
void BaseStopCondition::CopyDynamicData(const BaseStopCondition &stopCond)
{
   mNumValidPoints = stopCond.mNumValidPoints;
   mBufferSize = stopCond.mBufferSize;

   if ((Integer)mEpochBuffer.size() < mBufferSize)
      mEpochBuffer.reserve(mBufferSize);

   if ((Integer)mValueBuffer.size() < mBufferSize)
      mValueBuffer.reserve(mBufferSize);
      
   for (int i=0; i<mBufferSize; i++)
   {
      mEpochBuffer[i] = stopCond.mEpochBuffer[i];
      mValueBuffer[i] = stopCond.mValueBuffer[i];
   }
}

