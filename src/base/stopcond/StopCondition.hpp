//$Header$
//------------------------------------------------------------------------------
//                              StopCondition
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
 * Declares propagator stopping condition operations.
 */
//------------------------------------------------------------------------------
#ifndef StopCondition_hpp
#define StopCondition_hpp

#include "gmatdefs.hpp"
#include "RealTypes.hpp"
#include "paramdefs.hpp"
#include "GmatBase.hpp"
#include "Interpolator.hpp"
#include "RefFrame.hpp"

class GMAT_API StopCondition : public GmatBase
{
public:

StopCondition(const std::string &name,
              const std::string typeStr = "StopCondition",
              const Real &goal = GmatBase::REAL_PARAMETER_UNDEFINED,
              const Real &tol = GmatRealConst::REAL_TOL,
              Parameter *parameter = NULL,
              const Integer repeatCount = 1,
              RefFrame *refFrame = NULL,
              Interpolator *interp = NULL);
   StopCondition(const StopCondition &sc);
   StopCondition& operator= (const StopCondition &right); 
   virtual ~StopCondition();

   bool IsInitialized();
   Integer GetNumParameters();
   Integer GetBufferSize();

   Real GetGoal();
   Real GetTolerance();
   Parameter* GetFirstParameter();
   RefFrame* GetRefFrame();
   Interpolator* GetInterpolator();

   void SetGoal(const Real &goal);
   void SetTolerance(const Real &tol);
   void SetFirstParameter(Parameter *param);
   void SetInterpolator(Interpolator *interp);
   void SetRefFrame(RefFrame *refFrame);

   void AddParameter(Parameter *param);
   Real GetStopEpoch();

  /**
   * Evaluates stopping condition.
   * All derived classes must implement this method.
   */
   virtual bool Evaluate() = 0;

protected:

   void Initialize();

   Real mGoal;
   Real mTolerance;
   Integer mRepeatCount;
   RefFrame *mRefFrame;
   Interpolator *mInterpolator;

   ParameterPtrArray mParameters;
   Integer mNumParams;

   /// ring buffer for epoches
   RealArray mEpochBuffer;
   /// ring buffer for associated values
   RealArray mValueBuffer;
   Integer mNumValidPoints;
   Integer mBufferSize;

   bool mInitialized;

private:
   StopCondition();
   void CopyDynamicData(const StopCondition &sc);
};

#endif // StopCondition_hpp
