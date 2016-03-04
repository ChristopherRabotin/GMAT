//$Id$
//------------------------------------------------------------------------------
//                           ThfDataSegment
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Feb 17, 2016
/**
 * Segmant data read from a Thrust History File.
 * 
 * This class acts as s structure - member data is public - and is a helper
 * for the ThrustHistoryFile and ThrustSegment classes.
 */
//------------------------------------------------------------------------------

#ifndef ThfDataSegment_hpp
#define ThfDataSegment_hpp

#include "ThrustFileDefs.hpp"
#include "CoordinateSystem.hpp"

class ThfDataSegment
{
public:
   ThfDataSegment();
   virtual ~ThfDataSegment();
   ThfDataSegment(const ThfDataSegment &ds);
   ThfDataSegment& operator=(const ThfDataSegment &ds);

   /// Structure for the thrust profile data points
   struct ThrustPoint
   {
      Real time;
      Real magnitude;
      Real vector[3];
      Real mdot;

      // constructor, destructor, copy constructor, assignment operator
      ThrustPoint();
      ~ThrustPoint();
      ThrustPoint(const ThrustPoint& tp);
      ThrustPoint& operator=(const ThrustPoint& tp);
   };

   enum InterpolationType
   {
      NONE,
      LINEAR,
      SPLINE
   };

   //-------------------------------------------------
   // Data parsed from the file
   //-------------------------------------------------
   // Header fields
   /// Name of the segment
   std::string segmentName;
   /// Start epoch
   std::string startEpochString;
   /// a.1 start epoch
   GmatEpoch startEpoch;
   /// a.1 epoch of the last node
   GmatEpoch endEpoch;
   /// Name of the coordinate system for the thrust data
   std::string csName;
   /// The coordinate system
   CoordinateSystem *cs;
   /// Method of data interpolation
   std::string interpolationMethod;
   /// Interpolation method type for the force/accel
   InterpolationType accelIntType;
   /// Interpolation method for mass flow rate
   std::string massFlowInterpolationMethod;
   /// Interpolation method type for the mass flow
   InterpolationType massIntType;
   /// Segment modeling behavior
   std::string modelFlag;
   /// Thrust or acceleration?
   bool modelThrust;
   /// The thrust profile data, node by node
   std::vector<ThrustPoint> profile;

   //-------------------------------------------------
   // Data passed from the ThrustSegment Object
   //-------------------------------------------------
   /// Thrust scale factor
   Real thrustScaleFactor;
   /// Mass flow scale factor
   Real massFlowScaleFactor;
   /// Flag indicating is the mass flow includes the thrust scale factor
   bool includeThrustFactorInMassFlow;
   /// List of tanks that are used for mass flow
   StringArray tanks;

//   // Estimation settings
//   /// Standard dev. for the thrust scale factor
//   Real tsfSigma;
//   /// List of solve-for parameters
//   StringArray solveFors;
};

#endif /* ThfDataSegment_hpp */
