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
 */
//------------------------------------------------------------------------------

#include "StopCondition.hpp"
#include "StopConditionException.hpp"
#include "MeanJ2000Equatorial.hpp"
#include "CubicSplineInterpolator.hpp"
#include "KeplerianParameters.hpp"     // for KepEcc()
#include "SphericalParameters.hpp"     // for SphRMag()
#include "MessageInterface.hpp"

#if !defined __UNIT_TEST__
#include "Moderator.hpp"
#endif

//#define DEBUG_BASE_STOPCOND 1
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
   "RefFrame",
};

const Gmat::ParameterType
BaseStopCondition::PARAMETER_TYPE[BaseStopConditionParamCount - GmatBaseParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// BaseStopCondition(const std::string &name, const std::string &desc,
//                   Parameter *epochParam, Parameter *stopParam, const Real &goal,
//                   const Real &tol, const Integer repeatCount, RefFrame *refFrame,
//                   Interpolator *interp)
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
BaseStopCondition::BaseStopCondition(const std::string &name, const std::string &desc,
                                     Parameter *epochParam, Parameter *stopParam,
                                     const Real &goal, const Real &tol,
                                     const Integer repeatCount, RefFrame *refFrame,
                                     Interpolator *interp)
   : GmatBase(Gmat::STOP_CONDITION, "StopCondition", name)
{
   objectTypes.push_back(Gmat::STOP_CONDITION);
   objectTypeNames.push_back("BaseStopCondition");

   mBaseEpoch = 0.0;
   mEpoch = 0.0;
   mGoal = goal;
   mTolerance = tol;
   mRepeatCount = repeatCount;
   mRefFrame = refFrame;
   mInterpolator = interp;
   mEpochParam = epochParam;
   mStopParam = stopParam;
   mEccParam = NULL;
   mRmagParam = NULL;
   mDescription = desc;
   mEccTol = 1.0e-6;  //loj: valid default?
   mRange = 100000;   //loj: valid default? 6/16/04 changed to 100000 from 10000
   
   mStopParamType = "";
   if (mStopParam != NULL)
      mStopParamType = mStopParam->GetTypeName();
   
   mNumValidPoints = 0;
   mBufferSize = 0;
   mStopEpoch = REAL_PARAMETER_UNDEFINED;
   
   mInitialized = false;
   mUseInternalEpoch = false;
   mNeedInterpolator = false;
   mNeedRefFrame = false;
   
   if (epochParam == NULL)
      mUseInternalEpoch = true;
   
   // Create default Interpolator
   if (mInterpolator == NULL)
   {
      mInterpolator = new CubicSplineInterpolator("InternalInterpolator");
   }
   
   // Create default RefFrame
   if (mRefFrame == NULL)
   {
      mRefFrame = new MeanJ2000Equatorial("InternalRefFrame");
   }
   
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
   : GmatBase(copy)
{
   mBaseEpoch = copy.mBaseEpoch;
   mEpoch = copy.mEpoch;
   mGoal = copy.mGoal;
   mTolerance = copy.mTolerance;
   mRepeatCount = copy.mRepeatCount;
   mRefFrame = copy.mRefFrame;
   mInterpolator = copy.mInterpolator;
   mSolarSystem = copy.mSolarSystem;
   mEpochParam = copy.mEpochParam;
   mStopEpoch = copy.mStopEpoch;
   mStopParam = copy.mStopParam;
   mEccParam = copy.mEccParam;
   mRmagParam = copy.mRmagParam;
   
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
   if (this != &right)
   {
      GmatBase::operator=(right);        
      mBaseEpoch = right.mBaseEpoch;
      mEpoch = right.mEpoch;
      mGoal = right.mGoal;
      mTolerance = right.mTolerance;
      mRepeatCount = right.mRepeatCount;
      mRefFrame = right.mRefFrame;
      mInterpolator = right.mInterpolator;
      mSolarSystem = right.mSolarSystem;
      mEpochParam = right.mEpochParam;
      mStopEpoch = right.mStopEpoch;
      mEccParam = right.mEccParam;
      mRmagParam = right.mRmagParam;

      CopyDynamicData(right);
   }

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
// RefFrame* GetRefFrame()
//------------------------------------------------------------------------------
RefFrame* BaseStopCondition::GetRefFrame()
{
   return mRefFrame;
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
      //Initialize();
      return true;
   }
   else
   {
      return false;
   }
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
// bool SetInterpolator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets Interpolator to interpolate stop condition epoch.
 *
 * @return true if Interplator is successfully set; false otherwise
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::SetInterpolator(const std::string &name)
{
   bool status = false;

   //@todo Do not use Moderator
    
#if !defined __UNIT_TEST__
   Moderator *theModerator = Moderator::Instance();
    
   if (name != "")
   {
      // get Interpolator pointer
      Interpolator *interp = theModerator->GetInterpolator(name);
      if (interp != NULL)
      {
         status = SetInterpolator(interp);
      }
   }

#endif
   return status;
}


//------------------------------------------------------------------------------
// bool SetRefFrame(RefFrame *refFrame)
//------------------------------------------------------------------------------
/**
 * Sets reference frame
 *
 * @return true if RefFrame is successfully set; false otherwise
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::SetRefFrame(RefFrame *refFrame)
{
   if (refFrame != NULL)
   {
      if (mRefFrame->GetName() == "InternalRefFrame")
         delete mRefFrame;
            
      mRefFrame = refFrame;
      return true;
   }
   else
   {
      return false;
   }
}


//------------------------------------------------------------------------------
// bool SetRefFrame(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets reference frame
 *
 * @return true if RefFrame is successfully set; false otherwise
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::SetRefFrame(const std::string &name)
{
   bool status = false;

#if !defined __UNIT_TEST__
   Moderator *theModerator = Moderator::Instance();
    
   if (name != "")
   {
      // get RefFrame pointer
      RefFrame *refFrame = theModerator->GetRefFrame(name);
      if (refFrame != NULL)
      {
         status = SetRefFrame(refFrame);
      }
   }
#endif
   return status;
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
// bool SetEpochParameter(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets epoch parameter which will be used to interpolate stop epoch
 *
 * @return true if EpochParam is successfully set; false otherwise
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::SetEpochParameter(const std::string &name)
{
   bool status = false;

   #if DEBUG_BASE_STOPCOND
   MessageInterface::ShowMessage("BaseStopCondition::SetEpochParameter() name = %s\n",
                                 name.c_str());
   #endif
   
#if !defined __UNIT_TEST__
   Moderator *theModerator = Moderator::Instance();
    
   if (name != "")
   {
      // get parameter pointer
      Parameter *param = theModerator->GetParameter(name);
      if (param != NULL)
      {
         //MessageInterface::ShowMessage("BaseStopCondition::SetEpochParameter() name = %s\n",
         //                               param->GetName().c_str());
         status = SetEpochParameter(param);
      }
   }
#endif
   return status;
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
// virtual bool SetStopParameter(const std::string &name)
//------------------------------------------------------------------------------
bool BaseStopCondition::SetStopParameter(const std::string &name)
{
   #if DEBUG_BASE_STOPCOND
   MessageInterface::ShowMessage("BaseStopCondition::SetStopParameter() name = %s\n",
                                 name.c_str());
   #endif
   
   bool status = false;
   
#if !defined __UNIT_TEST__
   Moderator *theModerator = Moderator::Instance();
   
   if (name != "")
   {
      // get parameter pointer
      Parameter *param = theModerator->GetParameter(name);
      if (param != NULL)
      {
         //MessageInterface::ShowMessage("BaseStopCondition::SetStopParameter() name = %s\n",
         //                               param->GetName().c_str());
         SetStopParameter(param);
         status = true;
      }
   }
#endif
   
   return status;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool BaseStopCondition::Initialize()
{
   mInitialized = false;
   
   if (Validate())
   {
      if (mStopParam->GetTypeName() == "Apoapsis" ||
          mStopParam->GetTypeName() == "Periapsis")
      {
         mGoal = 0.0;
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
      
      //loj: How about mRefFrame? - need for crossing plane
   }

   #if DEBUG_BASE_STOPCOND
   MessageInterface::ShowMessage
      ("BaseStopCondition::Initialize() mInitialized=%d\n",
       mInitialized);
   #endif
   
   return mInitialized;
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
   //return false; // base class doesn't know which object is used
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


//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
/**
 * @return true if all necessary objects have been set; false otherwise
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::Validate()
{
   bool valid = false;

   // check on epoch parameter
   if (mUseInternalEpoch || mEpochParam != NULL)
      valid = true;
   
   // check on stop parameter
   if (valid)
   {
      valid = false;
      if (mStopParam != NULL)
      {
         if (mStopParam->IsTimeParameter())
         {
            mNeedInterpolator = false;
            valid = true;
         }
         else if (mInterpolator != NULL)
         {
            mNeedInterpolator = true;
            valid = true;
         }
      }
   }

   //loj: 4/12/05 Added to set SpacePoint object for internal parameters.
   // Apoapsis and Periapsis need additional parameters
   if (valid)
   {
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
                                         mStopParam->GetRefObjectName(Gmat::SPACE_POINT)));
            
            mEccParam->SetInternalCoordSystem(mStopParam->GetInternalCoordSystem());            
            mEccParam->AddRefObject(mSolarSystem);
            mEccParam->Initialize();
         }
         
         // check on SphRMag parameter if "Periapsis"
         if (mStopParam->GetTypeName() == "Periapsis")
         {
            if (mRmagParam == NULL)
            {
               #if DEBUG_BASE_STOPCOND
               MessageInterface::ShowMessage
                  ("BaseStopCondition::Validate(): Creating SphRMag...\n");
               #endif
               
               mRmagParam = new SphRMag("");
            
               mRmagParam->AddRefObject
                  (mStopParam->GetRefObject(Gmat::SPACECRAFT,
                                            mStopParam->GetRefObjectName(Gmat::SPACECRAFT)));
               mRmagParam->AddRefObject
                  (mStopParam->GetRefObject(Gmat::COORDINATE_SYSTEM,
                                            mStopParam->GetRefObjectName(Gmat::COORDINATE_SYSTEM)));
               mRmagParam->AddRefObject
                  (mStopParam->GetRefObject(Gmat::SPACE_POINT,
                                            mStopParam->GetRefObjectName(Gmat::SPACE_POINT)));
            
               mRmagParam->SetInternalCoordSystem(mStopParam->GetInternalCoordSystem());
               mRmagParam->AddRefObject(mSolarSystem);
               mRmagParam->Initialize();
            }
         }
      }
   }

   #if DEBUG_BASE_STOPCOND
   if (!valid)
   {
      MessageInterface::ShowMessage
         ("BaseStopCondition::Validate() failed mUseInternalEpoch=%d, "
          "mEpochParam=%d, mStopParam=%d, mInterpolator=%d\n",
          mUseInternalEpoch, mEpochParam, mStopParam, mInterpolator);
   }

   MessageInterface::ShowMessage
      ("BaseStopCondition::Validate() Exiting valid=%d\n", valid);
   
   #endif
   
   return valid;
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
   }
   
   return true;
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
   //MessageInterface::ShowMessage("BaseStopCondition::GetParameterID() str = %s\n", str.c_str());
    
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
   case GOAL:
      return mGoal;
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
   case GOAL:
      mGoal = value;
      return mGoal;
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
   //MessageInterface::ShowMessage("BaseStopCondition::SetRealParameter() label=%s, "
   //                              "value=%d\n", label.c_str(), value);
    
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
      if (mEpochParam != NULL)
         return mEpochParam->GetTypeName();
      else
         return "InternalEpoch";
   case STOP_VAR:
      if (mStopParam != NULL)
         return mStopParam->GetTypeName();
      else
         return "UndefinedStopVar";
      //      case ECC_VAR:
      //          if (mEccParam != NULL)
      //              return mEccParam->GetTypeName();
      //          else
      //              return "UndefinedEccParam";
   case INTERPOLATOR:
      if (mInterpolator != NULL)
         return mInterpolator->GetTypeName();
      else
         return "UndefinedInterpolator";
   case REF_FRAME:
      if (mRefFrame != NULL)
         return mRefFrame->GetTypeName();
      else
         return "UndefinedRefFrame";
   default:
      return GmatBase::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string BaseStopCondition::GetStringParameter(const std::string &label) const
{
   //MessageInterface::ShowMessage("BaseStopCondition::GetStringParameter() label = %s\n",
   //                              label.c_str());

   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool BaseStopCondition::SetStringParameter(const Integer id, const std::string &value)
{
   //MessageInterface::ShowMessage("BaseStopCondition::SetStringParameter() id = %d, "
   //                              "value = %s \n", id, value.c_str());
    
   switch (id)
   {
   case EPOCH_VAR:
      return SetEpochParameter(value);
   case STOP_VAR:
      return SetStopParameter(value);
   case INTERPOLATOR:
      return SetInterpolator(value);
   case REF_FRAME:
      return SetRefFrame(value);
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
   //MessageInterface::ShowMessage("BaseStopCondition::SetStringParameter() label = %s, "
   //                              "value = %s \n", label.c_str(), value.c_str());

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
   for (int i=0; i<mBufferSize; i++)
   {
      mEpochBuffer[i] = stopCond.mEpochBuffer[i];
      mValueBuffer[i] = stopCond.mValueBuffer[i];
   }
}

