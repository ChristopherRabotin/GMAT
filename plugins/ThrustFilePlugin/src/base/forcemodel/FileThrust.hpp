//------------------------------------------------------------------------------
//                           FileThrust
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 13, 2016
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef FileThrust_hpp
#define FileThrust_hpp

#include "ThrustFileDefs.hpp"
#include "PhysicalModel.hpp"
#include "ThrustSegment.hpp"
#include "LinearInterpolator.hpp"
#include "NotAKnotInterpolator.hpp"

/**
 * Physical model used to apply derivative data from a thrust history file
 */
class FileThrust: public PhysicalModel
{
public:
   FileThrust(const std::string &name = "");
   virtual ~FileThrust();
   FileThrust(const FileThrust& ft);
   FileThrust&             operator=(const FileThrust& ft);
   bool                    operator==(const FileThrust& ft) const;

   virtual GmatBase*       Clone() const;

   virtual void            Clear(const UnsignedInt
                                 type = Gmat::UNKNOWN_OBJECT);

   virtual Integer         GetParameterID(const std::string &str) const;
   virtual bool            IsParameterReadOnly(const Integer id) const;
   virtual bool            IsParameterReadOnly(const std::string &label) const;
   virtual Real            GetRealParameter(const Integer id) const;
   virtual Real            SetRealParameter(const Integer id,
                                         const Real value);



   virtual const ObjectTypeArray&
                           GetRefObjectTypeArray();
   virtual bool            SetRefObjectName(const UnsignedInt type,
                                            const std::string &name);
   virtual const StringArray&
                           GetRefObjectNameArray(const UnsignedInt type);
   virtual bool            SetRefObject(GmatBase *obj,
                              const UnsignedInt type,
                              const std::string &name = "");
   virtual bool            SetRefObject(GmatBase *obj,
                              const UnsignedInt type,
                              const std::string &name, const Integer index);
   virtual bool            RenameRefObject(const UnsignedInt type,
                                           const std::string &oldName,
                                           const std::string &newName);
   virtual GmatBase*       GetRefObject(const UnsignedInt type,
                                        const std::string &name);
   virtual GmatBase*       GetRefObject(const UnsignedInt type,
                                        const std::string &name,
                                        const Integer index);

   virtual bool            IsTransient();
   virtual bool            DepletesMass();
   virtual void            SetSegmentList(std::vector<ThrustSegment>* segs);

   const std::vector<ThrustSegment> GetAllThrustSegments();                   // Thrust Scale Factor Solve For

   virtual void            SetPropList(ObjectArray *soList);
   virtual bool            Initialize();

   virtual bool            GetDerivatives(Real * state, Real dt, Integer order,
                                          const Integer id = -1);
   virtual Rvector6        GetDerivativesForSpacecraft(Spacecraft *sc);


   // Methods used by the ODEModel to set the state indexes, etc
   virtual bool            SupportsDerivative(Gmat::StateElementId id);
   virtual bool            SetStart(Gmat::StateElementId id, Integer index,
                                 Integer quantity, Integer sizeOfType);
   virtual StringArray     GetSolveForList();
   virtual void            SetStmIndex(Integer id, Integer paramID);

   // Methods we may need in the future are commented out
   // virtual Integer         GetEstimationParameterID(const std::string &param);
   // virtual std::string     GetParameterNameForEstimationParameter(const std::string &parmName);
   // virtual std::string     GetParameterNameFromEstimationParameter(const std::string &parmName);
   // virtual Integer         SetEstimationParameter(const std::string &param);
   virtual bool            IsEstimationParameterValid(const Integer id);
   // virtual Integer         GetEstimationParameterSize(const Integer id);
   // virtual Real*           GetEstimationParameterValue(const Integer id);

   // Covariance handling code
   virtual Integer         HasParameterCovariances(Integer parameterId);
   virtual Rmatrix*        GetParameterCovariances(Integer parameterId = -1);
   virtual Covariance*     GetCovariance();

   virtual bool            SetPrecisionTimeFlag(bool onOff);

   // Methods for getting stop epochs for force models
   virtual Real                     GetForceMaxStep(bool forward = true);
   virtual Real                     GetForceMaxStep(Real theEpoch, bool forward = true);
   virtual Real                     GetForceMaxStep(const GmatTime& theEpochGT, bool forward = true);

   DEFAULT_TO_NO_CLONES

protected:

   // Pieces needed for bookkeeping
   /// Names of the spacecraft accessed by this force
   StringArray                   spacecraftNames;
   /// Propagated objects used in the ODEModel
   ObjectArray                   spacecraft;
   /// Indexes (in the spacecraft vector) for the Spacecraft used by this force
   std::vector<Integer>          scIndices;
   /// Number of spacecraft in the state vector that use CartesianState
   Integer                       satCount;
   /// Start index for the Cartesian state
   Integer                       cartIndex;
   /// Flag indicating if the Cartesian state should be populated
   bool                          fillCartesian;
   /// Flag to toggle thrust vs accel
   bool                          dataIsThrust;
   /// Flag used to warn once that  then go silent if mass flow is missing tank
   bool                          massFlowWarningNeeded;

   /// Names of the segments accessed by this force
   StringArray                   segmentNames;
   /// The segment data from the thrust history file
   std::vector<ThrustSegment>    *segments;

   /// Start index for the dm/dt data
   Integer                       mDotIndex;
   /// Flag indicating if any thrusters are set to deplete mass
   bool                          depleteMass;
   /// Name of the tank that is supplying fuel (just 1 for now)
   std::string                   activeTankName;
   /// List of coordinate systems used in the segments
   StringArray                   csNames;
   /// Current coordinate system, used when coversion is needed
   CoordinateSystem              *coordSystem;

   /// 5 raw data elements: 3 thrust/accel components, mdot, interpolation method
   Real                          dataBlock[7];
   /// dataSet is (up to) 5 dataBlock sets, with the last element set to time
   Real                          dataSet[5][5];

   /// Linear interpolator object (currently not used
   LinearInterpolator            *liner;
   /// Not a knot interpolator, used for spline interpolation
   NotAKnotInterpolator          *spliner;
   /// Flag used to mark when the "too few points" warning has been written
   bool                          warnTooFewPoints;
   /// Indices into the profile data that is loaded into the interpolator
   Integer                       interpolatorData[5];
   /// Last used index pair
   Integer                       indexPair[2];
  
   // Thrust Scale Factor Solve For data
   /// Spacecraft thrust scale factor
   Real   thrustSF;
   /// Starting value for the Spacecraft thrust scale factor
   Real   thrustSFinitial;
   /// Initial value of thrust scale factor
   std::vector<Real> tsfInitial;
   /// Flag indicating if ThrustSF is being estimated
   bool estimatingTSF;
   /// ID for the tsfEpsilon parameter
   Integer tsfEpsilonID;
   /// Row/Column for the TSF entries in the A-matrix and STM
   Integer tsfEpsilonRow;
   /// Current value(s) of tsf
   //std::vector<Real> tsfEpsilon;

   Rvector3 Accelerate(GmatEpoch &theEpoch, Real mass);
   Rvector3 Accelerate(GmatTime &theEpoch, Real mass);

   void ComputeAccelerationMassFlow(const GmatEpoch segEpoch, const GmatEpoch atEpoch, Real burnData[4]);
   void ComputeAccelerationMassFlow(const GmatTime &segEpoch, const GmatTime &atEpoch, Real burnData[4]);
   Integer GetSegmentData(Integer atIndex, Real offset);
   void Interpolate(Integer atIndex, Integer profileIndex, Real offset);
   void LinearInterpolate(Integer atIndex, Integer profileIndex, Real offset);
   void SplineInterpolate(Integer atIndex, Integer profileIndex, Real offset);

   void ConvertDirectionToInertial(Real *dir, Real *dirInertial, Real epoch);
   void ConvertDirectionToInertial(Real *dir, Real *dirInertial, const GmatTime &epochGT);

   // Methods for checking if an epoch is in an interval, when direction matters
   virtual bool            InSegmentInterval(GmatEpoch begin, GmatEpoch end, GmatEpoch epoch);
   virtual bool            InSegmentInterval(const GmatTime& begin, const GmatTime& end, const GmatTime& epoch);
};

#endif /* FileThrust_hpp */
