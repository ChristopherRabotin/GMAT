//$Id: GeometricMeasurement.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         GeometricMeasurement
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
// Created: 2009/06/29
//
/**
 * Definition of the geometric measurement base class.
 */
//------------------------------------------------------------------------------


#ifndef GeometricMeasurement_hpp
#define GeometricMeasurement_hpp


#include "estimation_defs.hpp"
#include "EstimationDefs.hpp"
#include "CoreMeasurement.hpp"
#include "MeasurementData.hpp"


#include "Rmatrix33.hpp"
#include "Rvector3.hpp"
#include "SpacePoint.hpp"


/**
 * GeometricMeasurement is the base class for measurements based on geometry
 *
 * The GeometricMeasurement class is a CoreMeasurement class that calculates
 * measurement values based on the position and velocity of several SpacePoints.
 * Typical GeometricMeasurements are the range between two points, the range
 * rate between the points, and angles from one point to another.
 *
 * GeometricMeasurement objects are CoreMeasurement objects.  As such, they
 * should never be encountered outside of a MeasurementModel container.  The
 * GeometricMeasurement class is derived from GmatBase so that GMAT's factory
 * subsystem can be used to create instances of the class.  These instances are
 * constructed and passed to a MeasurementModel when the MeasurementModel type
 * is set.
 */
class ESTIMATION_API GeometricMeasurement : public CoreMeasurement
{
public:
   GeometricMeasurement(const std::string &type, const std::string &nomme = "");
   // Abstract to prevent instantiation
   virtual ~GeometricMeasurement() = 0;
   GeometricMeasurement(const GeometricMeasurement& gm);
   GeometricMeasurement&      operator=(const GeometricMeasurement& gm);

   virtual bool         Initialize();

   // Here are the parameter access shells in case we need them later
//   virtual std::string        GetParameterText(const Integer id) const;
//   virtual Integer            GetParameterID(const std::string &str) const;
//   virtual Gmat::ParameterType
//                              GetParameterType(const Integer id) const;
//   virtual std::string        GetParameterTypeString(const Integer id) const;

   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);

//   // Move to CoreMeasurement when it is created
//   virtual MeasurementData*        GetMeasurementDataPointer();
//   virtual std::vector<RealArray>* GetDerivativePointer();
//
//   virtual const MeasurementData&
//                              CalculateMeasurement();
//   virtual const std::vector<RealArray>&
//                              CalculateMeasurementDerivatives(GmatBase *obj,
//                                                              Integer id) = 0;

protected:
   // These go on CoreMeasurement
//   std::vector<RealArray>     currentDerivatives;
//   Rmatrix                    stm;

   StringArray                participantNames;
   SpacePoint                 *anchorPoint;
//   std::vector<SpacePoint*>   participants;


   Integer                    GetParmIdFromEstID(Integer id, GmatBase *obj);

   /// Support members for the range vector calculation
//   Rvector3                   p1Loc;
//   Rvector3                   p1LocF1;
//   Rvector3                   p2Loc;
//   Rvector3                   p2LocF2;
////   Rvector3                   rangeVec;
//   Rvector3                   p1Vel;
//   Rvector3                   p1VelF1;
//   Rvector3                   p2Vel;
//   Rvector3                   p2VelF2;
//   Rvector3                   r12_j2k;
//   Rvector3                   r12_j2k_vel;
//   Rvector3                   rangeVecInertial;
//   Rvector3                   rangeVecObs;
//   Rvector3                   rangeRateVecObs;
   
//   Integer                    satEpochID;
   /// flag indicating whether or not the first participant is a GroundStation 
   /// (otherwise, there are two spacecraft as participants)
//   bool                       stationParticipant;
//   bool                       initialized;
   
   virtual bool               Evaluate(bool withEvents = false) = 0;
   virtual void               InitializeMeasurement();
   
   /// Enumerated parameter IDs
   enum
   {
//      PARTICIPANT = GmatBaseParamCount,
//      GeometricMeasurementParamCount
      GeometricMeasurementParamCount = GmatBaseParamCount,
   };

//   // In case they are needed
//   /// Array of supported parameters
//   static const std::string PARAMETER_TEXT[GeometricMeasurementParamCount -
//                                           GmatBaseParamCount];
//   /// Array of parameter types
//   static const Gmat::ParameterType PARAMETER_TYPE[GeometricMeasurementParamCount -
//                                                   GmatBaseParamCount];
};

#endif /* GeometricMeasurement_hpp */
