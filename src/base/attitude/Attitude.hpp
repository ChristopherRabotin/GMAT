//$Header$
//------------------------------------------------------------------------------
//                               Attitude
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
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
 namespace GmatAttitude   // do I even need this?  for now, I'll leave it here
 {
   enum AttitudeStateType
   {
      QUATERNION_TYPE = 0,
      DIRECTION_COSINE_MATRIX_TYPE,
      EULER_ANGLES_AND_SEQUENCE_TYPE
   };
   
   enum AttitudeRateStateType
   {
      ANGULAR_VELOCITY_TYPE = 0,
      EULER_ANGLE_RATES_TYPE
   };
   
   enum EulerSequenceType
   {
      EULER_1_2_3 = 0,
      EULER_2_3_1,
      EULER_3_1_2,
      EULER_1_3_2,
      EULER_3_2_1,
      EULER_2_1_3,
      EULER_1_2_1,
      EULER_2_3_2,
      EULER_3_1_3,
      EULER_1_3_1,
      EULER_3_2_3,
      EULER_2_1_2,
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

   // static methods for conversion
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
   
   static Rvector3  ToEulerAngleRates(const Rvector3 &angularVel, 
                                      const Rvector3 &eulerAngles,
                                      Integer seq1, Integer seq2, 
                                      Integer seq3);
   static Rvector3  ToAngularVelocity(const Rvector3 &eulerRates, 
                                      const Rvector3 &eulerAngles,
                                      Integer seq1, Integer seq2, 
                                      Integer seq3);
   

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
   // gt the attitude as a Direction Cosine Matrix
   virtual const Rmatrix33&   GetCosineMatrix(Real atTime);
   
   virtual const Rvector3&    GetAngularVelocity(Real atTime);
   virtual const Rvector3&    GetEulerAngleRates(Real atTime);
   
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
   virtual const UnsignedIntArray& 
                       GetUnsignedIntArrayParameter(
                       const Integer id) const; 
   virtual const UnsignedIntArray& 
                       GetUnsignedIntArrayParameter(
                       const std::string &label) const;
   virtual UnsignedInt SetUnsignedIntParameter(const Integer id,
                                               const UnsignedInt value,
                                               const Integer index);
                                               
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
   virtual const StringArray& 
                       GetStringArrayParameter(const Integer id) const; 
   virtual const StringArray& 
                       GetStringArrayParameter(const std::string &label) const;
protected:
   enum 
   {
       REFERENCE_COORDINATE_SYSTEM_NAME = GmatBaseParamCount,
       INITIAL_EPOCH,                      // A1Mjd (Real)
       EULER_SEQUENCE_LIST,
       EULER_SEQUENCE_STRING,
       INITIAL_EULER_SEQUENCE,
       INITIAL_EULER_ANGLES,               // degrees
       INITIAL_EULER_ANGLE_RATES,          // degrees/second
       INITIAL_QUATERNION,
       INITIAL_DIRECTION_COSINE_MATRIX,
       INITIAL_ANGULAR_VELOCITY,           // degrees/second
       AttitudeParamCount
   };
   
   static const std::string PARAMETER_TEXT[AttitudeParamCount - GmatBaseParamCount];
   
   static const Gmat::ParameterType PARAMETER_TYPE[AttitudeParamCount - GmatBaseParamCount];
 
   static const std::string EULER_SEQ_LIST[12];
   
   static const Real TESTACCURACY;
   
   static const Real ATTITUDE_TIME_TOLERANCE;

   
   GmatAttitude::AttitudeStateType
                         inputAttType;
   GmatAttitude::AttitudeRateStateType
                         inputAttRateType;
                         
   bool                  isInitialized;
   /// the list of possible euler sequences
   StringArray eulerSequenceList;
   
   /// initial user-supplied epoch as an A1Mjd time (as Real)
   Real                  initialEpoch; 
    
   /// the reference coordinate system name
   std::string           refCSName;
   /// pointer to the reference coordinate system
   CoordinateSystem      *refCS;
   std::string           eulerSeqString;
   /// initial user-supplied euler sequence
   UnsignedIntArray      initialEulerSeq;
   /// initial user-supplied euler angles (degrees) [defaults to zeros]
   Rvector3              initialEulerAng; 
   /// initial user-supplied cosince matrix [defaults to identity matrix]
   Rmatrix33             initialDcm; 
   /// initial user-supplied euler angle rates (deg/sec) [defaults to zeros]
   Rvector3              initialEulerAngRates; 
   /// initial user-supplied angular velocity (deg/sec)  [defaults to zeros]
   Rvector3              initialAngVel;          // deg.sec 
   /// initial user-supplied quaternion
   Rvector               initialQuaternion;
   
   /// the input rotation matrix (from Fi to Fb) computed, on
   /// initialization (from quaternion or euler angles/sequence 
   /// if necessary)
   Rmatrix33 RBi;  // Identity Matrix by default
   /// the input angular velocity computed, on initialization 
   /// (from euler angle rates if necessary)
   Rvector3  wIBi;   
   
   /// the current rotation matrix (from inertial to body)
   Rmatrix33 currentRBI;
   /// the current angular velocity, with respect to the inertial frame
   /// (radians/second)
   Rvector3  currentwIBB;
   /// last time that the CosineMatrix and angular velocity
   /// were computed                     
   Real      currentAttitudeTime; 
   /// last time that the quaternion was computed
   Real      lastQuaternionTime;
   /// the last computed quaternion
   Rvector   lastQuaternion;
   /// last time that the euler angles were computed
   Real      lastEulerAngleTime;
   /// the last computed euler angles (radians)
   Rvector3  lastEulerAngles;
   /// last time that the euler angle rates were computed
   Real      lastEulerAngleRatesTime;
   /// the last computed euler angle rates  (radians/second)
   Rvector3  lastEulerAngleRates;
 
   // method to convert an euler axis and angle to a cosine matrix                     
   virtual Rmatrix33 EulerAxisAndAngleToDCM(
                        const Rvector3 &eAxis, Real eAngle);
   // method to convert a cosine matrix to an euler axis and angle
   virtual void      DCMToEulerAxisAndAngle(const Rmatrix33 &cosMat, 
                                            Rvector3 &eAxis, Real &eAngle);
                                            
   // method to set the euler sequence string to match the input
   // euler sequence array                        
   bool  SetEulerSequenceString(const UnsignedIntArray &eulerArray);
   // method to set the euler sequence array to match the input
   // euler sequence string; assumes string is of the form
   // "N M P" where N, M, and P are Integers
   bool  SetEulerSequence(const std::string &seqStr);
   
   
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
   
};
#endif /*Attitude_hpp*/
