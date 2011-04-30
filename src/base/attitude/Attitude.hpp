//$Id$
//------------------------------------------------------------------------------
//                               Attitude
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan/GSFC
// Created: 2006.02.16
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Class definition for the Attitude base class.
 * 
 * @note The time passed in for the epoch or passed into the methods 
 *       should be an A1Mjd (though its type is currently Real).
 * @note Methods called to set/get parameter values will require/return angle
 *       values in degrees and rate values in degrees/sec.  All other methods 
 *       assume/expect radians and radians/sec.
 */
//------------------------------------------------------------------------------


#ifndef Attitude_hpp
#define Attitude_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CoordinateSystem.hpp"
#include "Rmatrix33.hpp"
#include "Rvector3.hpp"

//------------------------------------------------------------------------------
// namespace to define attitude state types
//------------------------------------------------------------------------------
 namespace GmatAttitude
 {
   enum AttitudeStateType
   {
      QUATERNION_TYPE = 0,
      DIRECTION_COSINE_MATRIX_TYPE,
      EULER_ANGLES_AND_SEQUENCE_TYPE,
      MODIFIED_RODRIGUES_PARAMETERS_TYPE
   };
   
   enum AttitudeRateStateType
   {
      ANGULAR_VELOCITY_TYPE = 0,
      EULER_ANGLE_RATES_TYPE
   };
 };
 
/**
 * Base class used for Attitude classes.
 * 
 * This class is the base class for classes repressenting spacecraft attitude 
 * for GMAT.
 *  
 * @note The current builds of GMAT only contain kinematic attitude modeling.
 */
class GMAT_API Attitude : public GmatBase 
{
public:

   // BEGIN static methods for conversion
   static Rmatrix33 ToCosineMatrix(const Rvector &quat1);
   static Rmatrix33 ToCosineMatrix(const Rvector3 &eulerAngles, 
                                   Integer seq1, Integer seq2, 
                                   Integer seq3);
   static Rmatrix33 ToCosineMatrix(const Real *eulerAngles, 
                                   Integer seq1, Integer seq2, 
                                   Integer seq3);
   static Rvector3  ToEulerAngles(const Rvector &quat1, Integer seq1,
                                  Integer seq2,         Integer seq3);
   static Rvector3  ToEulerAngles(const Rmatrix33 &cosMat, Integer seq1,
                                  Integer seq2,            Integer seq3);
   static Rvector   ToQuaternion(const Rvector3 &eulerAngles, 
                                 Integer seq1, Integer seq2, Integer seq3);
   static Rvector   ToQuaternion(const Rmatrix33 &cosMat);
   static Rvector   ToQuaternion(const Rvector3 &MRPs);
   static Rvector3  ToMRPs(const Rvector &quat1);
   
   static Rvector3  ToEulerAngleRates(const Rvector3 &angularVel, 
                                      const Rvector3 &eulerAngles,
                                      Integer seq1, Integer seq2, 
                                      Integer seq3);
   static Rvector3  ToAngularVelocity(const Rvector3 &eulerRates, 
                                      const Rvector3 &eulerAngles,
                                      Integer seq1, Integer seq2, 
                                      Integer seq3);
                                      
   static StringArray       GetEulerSequenceStrings();
   static UnsignedIntArray  ExtractEulerSequence(const std::string &seqStr);


   // method to convert an euler axis and angle to a cosine matrix
   static Rmatrix33 EulerAxisAndAngleToDCM(
                        const Rvector3 &eAxis, Real eAngle);
   // method to convert a cosine matrix to an euler axis and angle
   static void      DCMToEulerAxisAndAngle(const Rmatrix33 &cosMat,
                                            Rvector3 &eAxis, Real &eAngle);
// END static methods for conversion


   // Constructor
   Attitude(const std::string &typeStr, const std::string &itsName = "");
   // copy constructor
   Attitude(const Attitude& att);
   // operator =
   Attitude& operator=(const Attitude& att);
   // destructor
   virtual ~Attitude();
   // Initialize the Attitude
   virtual bool       Initialize();
   
   Real               GetEpoch() const;
   void               SetEpoch(Real toEpoch); // A1Mjd time
   virtual bool       SetReferenceCoordinateSystemName(
                         const std::string &refName);
   
   const StringArray&         GetEulerSequenceList() const;
   // get the attitude as a Quaternion
   virtual const Rvector&     GetQuaternion(Real atTime);
   // get the attitude as a set of Euler angles, using the Euler 
   // sequence provided by the user (an exception will be thrown if the
   // sequence has not been set
   virtual const Rvector3&    GetEulerAngles(Real atTime);
   // get the attitude as a set of Euler angles, using the input Euler sequence
   virtual const Rvector3&    GetEulerAngles(Real atTime,  Integer seq1, 
                                     Integer seq2, Integer seq3);
   // get the attitude as a Direction Cosine Matrix
   virtual const Rmatrix33&   GetCosineMatrix(Real atTime);
   
   virtual const Rvector3&    GetAngularVelocity(Real atTime);
   virtual const Rvector3&    GetEulerAngleRates(Real atTime);
   
   // return the type of attitude model it is
   std::string         GetAttitudeModelName() const;
   
   // methods to access object parameters
   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;
   virtual bool        SetRefObjectName(const Gmat::ObjectType type,
                                        const std::string &name);
   virtual bool        RenameRefObject(const Gmat::ObjectType type,
                                       const std::string &oldName,
                                       const std::string &newName);
   virtual GmatBase*   GetRefObject(const Gmat::ObjectType type,
                                    const std::string &name);
   virtual bool        SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                    const std::string &name = "");
                                    
   // methods to get/set parameter values
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual bool        IsParameterReadOnly(const Integer id) const;
   virtual bool        IsParameterReadOnly(const std::string &label) const;
   
   virtual Real        GetRealParameter(const Integer id) const;
   virtual Real        GetRealParameter(const std::string &label) const;
   
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);
   virtual Real        SetRealParameter(const std::string &label,
                                        const Real value);
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value,
                                        const Integer index);
   virtual const UnsignedIntArray& 
                       GetUnsignedIntArrayParameter(
                       const Integer id) const; 
   virtual const UnsignedIntArray& 
                       GetUnsignedIntArrayParameter(
                       const std::string &label) const;
                                               
   virtual const Rvector&    GetRvectorParameter(const Integer id) const;
   virtual const Rvector&    GetRvectorParameter(const std::string &label) const;
   virtual const Rvector&    SetRvectorParameter(const Integer id,
                                                 const Rvector &value);
   virtual const Rvector&    SetRvectorParameter(const std::string &label,
                                                 const Rvector &value);
                                    
   virtual const Rmatrix&    GetRmatrixParameter(const Integer id) const;
   virtual const Rmatrix&    GetRmatrixParameter(const std::string &label) const;
   virtual const Rmatrix&    SetRmatrixParameter(const Integer id,
                                                 const Rmatrix &value);
   virtual const Rmatrix&    SetRmatrixParameter(const std::string &label,
                                                 const Rmatrix &value);
                                                 
   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value);
   virtual bool        SetStringParameter(const std::string &label, 
                                          const std::string &value);
   virtual std::string GetStringParameter(const Integer id,
                                          const Integer index) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value,
                                          const Integer index);
   virtual const StringArray& 
                       GetStringArrayParameter(const Integer id) const; 
   virtual const StringArray& 
                       GetStringArrayParameter(const std::string &label) const;
   virtual const std::string&  
                       GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                           const std::string &prefix = "",
                                           const std::string &useName = "");
protected:
   enum
   {
       ATTITUDE_DISPLAY_STATE_TYPE = GmatBaseParamCount,   
       ATTITUDE_RATE_DISPLAY_STATE_TYPE,   
       REFERENCE_COORDINATE_SYSTEM,
       EPOCH,                      // A1Mjd (Real)
       Q_1,
       Q_2,
       Q_3,
       Q_4,
       EULER_ANGLE_SEQUENCE,
       EULER_ANGLE_1,
       EULER_ANGLE_2,
       EULER_ANGLE_3,
       DCM_11,
       DCM_12,
       DCM_13,
       DCM_21,
       DCM_22,
       DCM_23,
       DCM_31,
       DCM_32,
       DCM_33,
       MRP_1,    // Dunn Added
       MRP_2,
       MRP_3,
       EULER_ANGLE_RATE_1,
       EULER_ANGLE_RATE_2,
       EULER_ANGLE_RATE_3,
       ANGULAR_VELOCITY_X,
       ANGULAR_VELOCITY_Y,
       ANGULAR_VELOCITY_Z, 
       AttitudeParamCount
   };
   
   enum OtherReps
   {
       EULER_SEQUENCE_LIST = 7000,
       EULER_SEQUENCE,
       EULER_ANGLES,               // degrees
       EULER_ANGLE_RATES,          // degrees/second
       QUATERNION,
       MRPS,					   // Dunn Added
       DIRECTION_COSINE_MATRIX,
       ANGULAR_VELOCITY,           // degrees/second
       EndOtherReps
   };
   
   static const std::string PARAMETER_TEXT[AttitudeParamCount - GmatBaseParamCount];
   
   static const Gmat::ParameterType PARAMETER_TYPE[AttitudeParamCount - GmatBaseParamCount];
 
   static const std::string OTHER_REP_TEXT[EndOtherReps - 7000]; // OTHER_REPS_OFFSET
   
   static const Gmat::ParameterType OTHER_REP_TYPE[EndOtherReps - 7000]; // OTHER_REPS_OFFSET
 
   static const std::string EULER_SEQ_LIST[12];
   static const Real TESTACCURACY;
   static const Real QUAT_MIN_MAG;
   static const Real ATTITUDE_TIME_TOLERANCE;
   static const Real EULER_ANGLE_TOLERANCE;
   static const Real DCM_ORTHONORMALITY_TOLERANCE;
   static const Integer OTHER_REPS_OFFSET;


   
   GmatAttitude::AttitudeStateType
                         inputAttitudeType;
   GmatAttitude::AttitudeRateStateType
                         inputAttitudeRateType;
                         
   std::string           attitudeDisplayType; 
   std::string           attitudeRateDisplayType;
                         
   bool                  isInitialized;
   bool                  needsReinit;
   /// the list of possible euler sequences
   StringArray eulerSequenceList;
   
   /// initial user-supplied epoch as an A1Mjd time (as Real)
   Real                  epoch;
    
   /// the reference coordinate system name
   std::string           refCSName;
   /// pointer to the reference coordinate system
   CoordinateSystem      *refCS;
   std::string           eulerSequence;
   /// initial user-supplied euler sequence
   UnsignedIntArray      eulerSequenceArray;
   
   /// the input rotation matrix (from Fi to Fb) computed, on
   /// initialization (from quaternion or euler angles/sequence 
   /// if necessary)
   Rmatrix33 RBi;  // Identity Matrix by default
   /// the input angular velocity computed, on initialization 
   /// (from euler angle rates if necessary)
   Rvector3  wIBi;   
   
   /// the current rotation matrix (from inertial to body)
   Rmatrix33 cosMat;
   /// the current angular velocity, with respect to the inertial frame
   /// (radians/second)
   Rvector3  angVel;
   /// last time that the CosineMatrix and angular velocity
   /// were computed                     
   Real      attitudeTime;
   /// the last computed quaternion
   Rvector   quaternion;
   /// the last computed MRPs - Dunn Added
   Rvector3  mrps;
   /// the last computed euler angles (radians)
   Rvector3  eulerAngles;
   /// the last computed euler angle rates  (radians/second)
   Rvector3  eulerAngleRates;
   
   std::string attitudeModelName;
 

   
   //------------------------------------------------------------------------------
   //  virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime)
   //------------------------------------------------------------------------------
   /**
    * This mothod computes the current CosineMatrix at the input time atTime.
    *
    * @param atTime the A1Mjd time at which to compute the attitude.
    *
    * @note This method will update the currentRBI and currentwIBB members
    *       if the class.
    * @note This method is pure virtual and will need to be implemented in
    *       the 'leaf' classes.
    */
   //------------------------------------------------------------------------------
   virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime) = 0;
   
private:
   // default constructor - not implemented
   Attitude(); 
   bool      ValidateCosineMatrix(const Rmatrix33 &mat);
   bool      ValidateEulerSequence(const std::string &seq);
   bool      ValidateEulerSequence(const UnsignedIntArray &eulAng);
   bool      ValidateQuaternion(const Rvector &quat);
   bool      ValidateMRPs(const Rvector &mrps);		// Dunn Added
   void      UpdateState(const std::string &rep);
   void      SetRealArrayFromString(Integer id, const std::string &sval);
};
#endif /*Attitude_hpp*/
