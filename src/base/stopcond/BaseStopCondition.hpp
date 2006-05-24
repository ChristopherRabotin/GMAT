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
 * Declares BaseStopCondition class.
 */
//------------------------------------------------------------------------------
#ifndef BaseStopCondition_hpp
#define BaseStopCondition_hpp

#include "gmatdefs.hpp"
#include "RealTypes.hpp"
#include "paramdefs.hpp"
#include "GmatBase.hpp"
#include "Interpolator.hpp"
//#include "RefFrame.hpp"
#include "SolarSystem.hpp"
#include "Spacecraft.hpp"
#include "Variable.hpp"

class GMAT_API BaseStopCondition : public GmatBase
{
public:
   
   static const Real STOP_COND_TOL;
   
   BaseStopCondition(const std::string &name = "",
                     const std::string &desc = "",
                     Parameter *epochParam = NULL,
                     Parameter *stopParam = NULL,
                     const Real &goal = GmatBase::REAL_PARAMETER_UNDEFINED,
                     const Real &tol = STOP_COND_TOL,
                     const Integer repeatCount = 1,
                     Interpolator *interp = NULL);
   BaseStopCondition(const BaseStopCondition &copy);
   BaseStopCondition& operator= (const BaseStopCondition &right); 
   virtual ~BaseStopCondition();
   
   bool Initialize();
   virtual bool Evaluate() = 0;
   virtual bool IsTimeCondition() = 0;
   virtual bool AddToBuffer(bool isInitialPoint) = 0;
   virtual bool Validate();
   void Reset();
   virtual Real GetStopInterval();
   
   bool IsInitialized();
   Integer GetBufferSize();
   std::string& GetDescription();
   Parameter* GetEpochParameter();
   Parameter* GetStopParameter();
   Interpolator* GetInterpolator();
   virtual Real GetStopEpoch();
   
   void SetDescription(const std::string &desc);
   void SetPropDirection(Real dir);
   void SetSolarSystem(SolarSystem *solarSystem);
   bool SetInterpolator(Interpolator *interp);
   bool SetEpochParameter(Parameter *param);
   bool SetStopParameter(Parameter *param);
   bool SetGoalParameter(Parameter *param);
   void SetGoalString(const std::string &str);
   
   virtual bool SetSpacecraft(SpaceObject *sc);
   
   // methods inherited from GmatBase
   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);
   
   virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name = "");
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   
   virtual Integer GetIntegerParameter(const Integer id) const;
   virtual Integer GetIntegerParameter(const std::string &label) const;
   virtual Integer SetIntegerParameter(const Integer id,
                                       const Integer value);
   virtual Integer SetIntegerParameter(const std::string &label,
                                       const Integer value);
   
   virtual Real GetRealParameter(const Integer id) const;
   virtual Real GetRealParameter(const std::string &label) const;
   virtual Real SetRealParameter(const Integer id, const Real value);
   virtual Real SetRealParameter(const std::string &label, const Real value);
   
   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value);
protected:
   
   StringArray mAllRefObjectNames;
   
   Real mBaseEpoch;
   Real mEpoch;
   Real mGoal;
   Real mTolerance;
   Real mEccTol;
   Real mRange;
   Integer mRepeatCount;
   SolarSystem *mSolarSystem;
   
   std::string mInterpolatorName;
   Interpolator *mInterpolator;
   std::string mDescription;
   std::string mStopParamType;
   std::string mEpochParamName;
   std::string mStopParamName;
   std::string mGoalStr;
   
   Parameter *mStopParam;
   Parameter *mGoalParam;
   Parameter *mEpochParam;
   Parameter *mEccParam;
   Parameter *mRmagParam;
   
   /// ring buffer for epochs
   RealArray mEpochBuffer;
   /// ring buffer for associated values
   RealArray mValueBuffer;
   Integer mNumValidPoints;
   Integer mBufferSize;
   Real mStopEpoch;
   Real mStopInterval;

   // History data used instead of ring buffer for general propagation (before
   // a stopping condition triggers
   Real previousEpoch;
   Real previousValue;
   
   bool mUseInternalEpoch;
   bool mInitialized;
   bool mNeedInterpolator;
   bool mAllowGoalParam;
   bool mBackwardsProp;
   
   // Flags used to mark special cases
   bool isAngleParameter;
   bool isPeriapse;
   bool isApoapse;
   
   enum
   {
      BASE_EPOCH = GmatBaseParamCount,
      EPOCH,
      EPOCH_VAR,
      STOP_VAR,
      GOAL,
      TOLERANCE,
      ECC_TOL,
      RANGE,
      REPEAT_COUNT,
      INTERPOLATOR,
      BaseStopConditionParamCount,
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[BaseStopConditionParamCount - GmatBaseParamCount];
   static const std::string
      PARAMETER_TEXT[BaseStopConditionParamCount - GmatBaseParamCount];
   
private:
   void CopyDynamicData(const BaseStopCondition &stopCond);
};

#endif // BaseStopCondition_hpp
