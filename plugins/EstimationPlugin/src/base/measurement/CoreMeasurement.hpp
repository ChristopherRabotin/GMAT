//$Id: CoreMeasurement.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         CoreMeasurement
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/12/01
//
/**
 * The base class for all measurement primitives in GMAT.
 */
//------------------------------------------------------------------------------


#ifndef CoreMeasurement_hpp
#define CoreMeasurement_hpp


#include "estimation_defs.hpp"
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "EstimationDefs.hpp"
#include "MeasurementData.hpp"
#include "SpacePoint.hpp"
#include "Hardware.hpp"
#include "Sensor.hpp"
#include "Event.hpp"
#include "CoordinateConverter.hpp"
#include "CoordinateSystem.hpp"
#include "SolarSystem.hpp"


/**
 * GMAT's estimation subsystem represents measurement models using a
 * MeasurementModel container class and measurement primitives that perform the
 * actual measurement calculations.  These measurement primitives are all
 * derived from this class.
 */
class ESTIMATION_API CoreMeasurement : public GmatBase
{
public:
   CoreMeasurement(const std::string &type, const std::string &nomme = "");
   virtual ~CoreMeasurement();
   CoreMeasurement(const CoreMeasurement& core);
   CoreMeasurement& operator=(const CoreMeasurement& core);

   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                              const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                              const std::string &name,
                              const Integer index);
   virtual bool         SetParticipantHardware(GmatBase *obj,
                              const std::string &hwName, Integer hwIndex);

   virtual MeasurementData*        GetMeasurementDataPointer();
   virtual std::vector<RealArray>* GetDerivativePointer();
   virtual const MeasurementData&  CalculateMeasurement(bool withEvents=false);
   virtual const std::vector<RealArray>&
                                   CalculateMeasurementDerivatives(
                                         GmatBase *obj, Integer id) = 0;

   void                       SetUniqueId(Integer id);
   Integer                    GetMeasurementTypeID();
   Integer                    GetMeasurementSize();
   void                       SetSolarSystem(SolarSystem *ss);
   virtual bool               Initialize();

   virtual Integer            GetEventCount();
   virtual Event*             GetEvent(UnsignedInt whichOne);
   virtual bool               SetEventData(Event *locatedEvent = NULL);

   virtual Real               GetDelay(UnsignedInt forParticipant,
                                    Integer whichOne);

   virtual Integer            GetMeasurementParameterCount() const;

   /// @todo: Check this
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS


protected:
   /// The current or most recently calculated measurement
   MeasurementData            currentMeasurement;
   /// The derivatives of the current measurement
   std::vector<RealArray>     currentDerivatives;
   /// The state transition matrix for the measurement
   Rmatrix                    stm;
   /// A unique ID number used internally during estimation and simulation
   Integer                    uniqueId;
   /// The number of elements in the measurement
   Integer                    measurementSize;
   /// The number of events that need to be processed to refine the measurement
   Integer                    eventCount;

   /// The names of the measurement participants
   StringArray                participantNames;
   /// The participant that acts as the anchor point for the measurement
   SpacePoint                 *anchorPoint;
   /// Pointers to the participants
   std::vector<SpacePoint*>   participants;
   /// Pointers to the participant hardware to use
   std::vector<std::vector<Hardware*> >
                              participantHardware;

   // Coordinate System objects moved from GeometricMeasurement
   /// Flag used to specify if the coordinate systems have been built
   bool                       initialized;
   /// Flag indicating if there is a groundstation in the participant list
   bool                       stationParticipant;

   /// Coordinate systems used by participants; one per participant
   std::vector<CoordinateSystem*>
                              participantCoords;

   // The data structure above (participantCoords) will eventually replace some
   // of the following.  Schedule for this is TBD.

   /// Coordinate system in which state of participant 1 is expressed
   CoordinateSystem           *F1;
   /// Coordinate system in which state of participant 2 is expressed
   CoordinateSystem           *F2;
   /// Coordinate system in which observation is expressed
   CoordinateSystem           *Fo;
   /// J2K coordinate system
   CoordinateSystem           *j2k;

   /// Converter to convert between F1, F2, Fo frames
   CoordinateConverter        converter;
   /// the Solar System in use
   SolarSystem                *solarSystem;

   // Other properties moved in from GeometricMeasurement
   /// Parameter ID used to retrieve internal epoch data
   Integer                    satEpochID;
   /// Rotation matrix from F2 to J2K (Identity by default)
   Rmatrix33                  R_j2k_2;
   /// Rotation matrix from J2K to F2 (Identity by default)
   Rmatrix33                  R_2_j2k;
   /// Rotation matrix from F1 to J2K (Identity by default)
   Rmatrix33                  R_j2k_1;
   /// Rotation matrix from J2K to F1 (Identity by default)
   Rmatrix33                  R_1_j2k;
   /// Rotation matrix from F2 to Fo (Identity by default)
   Rmatrix33                  R_o_2;
   /// Rotation matrix from F1 to Fo (Identity by default)
   Rmatrix33                  R_o_1;
   /// Rotation matrix from J2K to Fo (Identity by default)
   Rmatrix33                  R_o_j2k;
   /// Rotation Dot matrix from J2K to Fo (Identity by default)
   Rmatrix33                  RDot_o_j2k;
   /// Rotation Dot matrix from F2 to Fo (Identity by default)
   Rmatrix33                  RDot_o_2;
   /// Rotation Dot matrix from F1 to Fo (Identity by default)
   Rmatrix33                  RDot_o_1;
   //   /// Rotation Dot matrix from F2 to J2K (Identity by default)
   //   Rmatrix33                  RDot_j2k_2;
   //   /// Rotation Dot matrix from F1 to J2K (Identity by default)
   //   Rmatrix33                  RDot_j2k_1;

   /// @todo Wendy needs to comment the code she added to core measurement.
   // Properties needed to calculate the range and range rate vectors
   Rvector3                   p1Loc;
   Rvector3                   p1LocF1;
   Rvector3                   p2Loc;
   Rvector3                   p2LocF2;
   Rvector3                   p1Vel;
   Rvector3                   p1VelF1;
   Rvector3                   p2Vel;
   Rvector3                   p2VelF2;
   Rvector3                   r12_j2k;
   Rvector3                   r12_j2k_vel;
   Rvector3                   rangeVecInertial;
   Rvector3                   rangeVecObs;
   Rvector3                   rangeRateVecObs;

   // Utility vectors and matrices
   static const Rmatrix33     I33;               // Identity matrix, 3x3
   static const Rmatrix33     zero33;
   static const Rvector3      zeroVector;        // zero-vector
   static const Rvector3      xUnit;             // [1,0,0]T
   static const Rvector3      yUnit;             // [0,1,0]T
   static const Rvector3      zUnit;             // [0,0,1]T


   // Methods
   Integer                    GetParmIdFromEstID(Integer id, GmatBase *obj);

   /// Abstract method that derived classes override to calculate a measurement
   virtual bool               Evaluate(bool withEvents = false) = 0;

   // Coordinate System methods based on methods in GeometricMeasurement
   virtual void               CalculateRangeVectorInertial(Integer p1index = 0,
                                    Integer p2index = 1);
   virtual void               CalculateRangeVectorObs(Integer p1index = 0,
                                    Integer p2index = 1);
   virtual void               CalculateRangeRateVectorObs(Integer p1index = 0,
                                    Integer p2index = 1);
   virtual void               InitializeMeasurement();
   virtual void               UpdateRotationMatrix(Real atEpoch,
                                          const std::string &whichOne = "All");

   // Line of sight checks
   bool                       CheckLOS(Integer p1index, Integer p2index, SpacePoint *cb);
   bool                       CheckSat2SatLOS(Rvector3 p1loc, Rvector3 p2loc, SpacePoint *cb);
   bool                       CheckStation2SatLOS(Real a1Epoch, Rvector3 sLoc, SpacePoint *cb);


   // Used for debug
   virtual void               DumpParticipantStates(const std::string& ref);


   /// Enumeration defining the CoreMeasurement's scriptable parameters
   enum
   {
       CoreMeasurementParamCount = GmatBaseParamCount
   };

};

#endif /* CoreMeasurement_hpp */
