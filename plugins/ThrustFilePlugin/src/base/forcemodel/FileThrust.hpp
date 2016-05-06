//------------------------------------------------------------------------------
//                           FileThrust
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2015 United States Government as represented by the
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

class FileThrust: public PhysicalModel
{
public:
   FileThrust(const std::string &name = "");
   virtual ~FileThrust();
   FileThrust(const FileThrust& ft);
   FileThrust&             operator=(const FileThrust& ft);
   bool                    operator==(const FileThrust& ft) const;

   virtual GmatBase*       Clone() const;

   virtual void            Clear(const Gmat::ObjectType
                                 type = Gmat::UNKNOWN_OBJECT);
   virtual const ObjectTypeArray&
                           GetRefObjectTypeArray();
   virtual bool            SetRefObjectName(const Gmat::ObjectType type,
                                            const std::string &name);
   virtual const StringArray&
                           GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool            SetRefObject(GmatBase *obj,
                              const Gmat::ObjectType type,
                              const std::string &name = "");
   virtual bool            SetRefObject(GmatBase *obj,
                              const Gmat::ObjectType type,
                              const std::string &name, const Integer index);
   virtual bool            RenameRefObject(const Gmat::ObjectType type,
                                           const std::string &oldName,
                                           const std::string &newName);
   virtual GmatBase*       GetRefObject(const Gmat::ObjectType type,
                                        const std::string &name);
   virtual GmatBase*       GetRefObject(const Gmat::ObjectType type,
                                        const std::string &name,
                                        const Integer index);

   virtual bool            IsTransient();
   virtual bool            DepletesMass();
   virtual void            SetSegmentList(std::vector<ThrustSegment>* segs);
   virtual void            SetPropList(ObjectArray *soList);
   virtual bool            Initialize();

   virtual bool            GetDerivatives(Real * state, Real dt, Integer order,
                                          const Integer id = -1);
   virtual Rvector6        GetDerivativesForSpacecraft(Spacecraft *sc);


   // Methods used by the ODEModel to set the state indexes, etc
   virtual bool SupportsDerivative(Gmat::StateElementId id);
   virtual bool SetStart(Gmat::StateElementId id, Integer index,
                         Integer quantity, Integer sizeOfType);

   DEFAULT_TO_NO_CLONES

protected:
   // Pieces needed for bookkeeping
//   /// The finite burn objects this model uses
//   std::vector <FiniteBurn *>    burns;
//   /// Names for the FiniteBurn objects used for this force
//   StringArray                   burnNames;
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

   void ComputeAccelerationMassFlow(const GmatEpoch atEpoch, Real burnData[4]);
   void GetSegmentData(Integer atIndex, GmatEpoch atEpoch);
   void LinearInterpolate(Integer atIndex, GmatEpoch atEpoch);
   void SplineInterpolate(Integer atIndex, GmatEpoch atEpoch);

   void ConvertDirectionToInertial(Real *dir, Real *dirInertial, Real epoch);
};

#endif /* FileThrust_hpp */
