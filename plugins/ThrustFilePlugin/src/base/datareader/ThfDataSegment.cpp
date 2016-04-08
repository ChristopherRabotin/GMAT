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
 * 
 */
//------------------------------------------------------------------------------

#include "ThfDataSegment.hpp"



//------------------------------------------------------------------------------
// ThfDataSegment()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
ThfDataSegment::ThfDataSegment() :
   segmentName                   (""),
   startEpochString              (""),
   startEpoch                    (0.0),      // 0.0 so relative timing is simple
   endEpoch                      (0.0),
   csName                        ("EarthMJ2000Eq"),
   cs                            (NULL),
   interpolationMethod           ("None"),   // Default stairsteps the data
   accelIntType                  (NONE),
   massFlowInterpolationMethod   ("None"),
   massIntType                   (NONE),
   modelFlag                     ("ModelThrustOnly"),
   modelThrust                   (true),
   thrustScaleFactor             (1.0),
   massFlowScaleFactor           (1.0),
   includeThrustFactorInMassFlow (false)
{
}

//------------------------------------------------------------------------------
// ~ThfDataSegment()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ThfDataSegment::~ThfDataSegment()
{
}

ThfDataSegment::ThfDataSegment(const ThfDataSegment& ds) :
   segmentName                   (ds.segmentName),
   startEpochString              (ds.startEpochString),
   startEpoch                    (ds.startEpoch),
   endEpoch                      (ds.endEpoch),
   csName                        (ds.csName),
   cs                            (ds.cs),
   interpolationMethod           (ds.interpolationMethod),
   accelIntType                  (ds.accelIntType),
   massFlowInterpolationMethod   (ds.massFlowInterpolationMethod),
   massIntType                   (ds.massIntType),
   modelFlag                     (ds.modelFlag),
   modelThrust                   (ds.modelThrust),
   thrustScaleFactor             (ds.thrustScaleFactor),
   massFlowScaleFactor           (ds.massFlowScaleFactor),
   includeThrustFactorInMassFlow (ds.includeThrustFactorInMassFlow),
   tanks                         (ds.tanks)
{
   for (UnsignedInt i = 0; i < ds.profile.size(); ++i)
   {
      ThrustPoint tp = ds.profile[i];
      profile.push_back(tp);
   }
}

ThfDataSegment& ThfDataSegment::operator =(const ThfDataSegment& ds)
{
   if (this != &ds)
   {
      segmentName                 =  ds.segmentName;
      startEpochString            =  ds.startEpochString;
      startEpoch                  =  ds.startEpoch;
      endEpoch                    =  ds.endEpoch;
      csName                      =  ds.csName;
      cs                          =  ds.cs;
      interpolationMethod         =  ds.interpolationMethod;
      accelIntType                =  ds.accelIntType;
      massFlowInterpolationMethod =  ds.massFlowInterpolationMethod;
      massIntType                 =  ds.massIntType;
      modelFlag                   =  ds.modelFlag;
      modelThrust                 =  ds.modelThrust;
      thrustScaleFactor           =  ds.thrustScaleFactor;
      massFlowScaleFactor         =  ds.massFlowScaleFactor;
      includeThrustFactorInMassFlow
                                  =  ds.includeThrustFactorInMassFlow;
      tanks                       =  ds.tanks;

      profile.clear();
      for (UnsignedInt i = 0; i < ds.profile.size(); ++i)
      {
         ThrustPoint tp = ds.profile[i];
         profile.push_back(tp);
      }
   }

   return *this;
}


// Convenience methods for the thrust profile data structure

//------------------------------------------------------------------------------
// ThrustPoint()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
ThfDataSegment::ThrustPoint::ThrustPoint() :
   time           (0.0),
   magnitude      (0.0),
   mdot           (0.0)

{
   vector[0] = vector[1] = vector[2] = 0.0;
}

//------------------------------------------------------------------------------
// ~ThrustPoint()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ThfDataSegment::ThrustPoint::~ThrustPoint()
{
}

//------------------------------------------------------------------------------
// ThrustPoint(const ThrustPoint& tp)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param tp The point copied to this make one
 */
//------------------------------------------------------------------------------
ThfDataSegment::ThrustPoint::ThrustPoint(const ThrustPoint& tp) :
   time        (tp.time),
   magnitude   (tp.magnitude),
   mdot        (tp.mdot)
{
   vector[0] = tp.vector[0];
   vector[1] = tp.vector[1];
   vector[2] = tp.vector[2];
}

//------------------------------------------------------------------------------
// ThrustSegment::ThrustPoint& operator=(const ThrustPoint& tp)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param tp The point copied to this one
 *
 * @return This point, set to match tp
 */
//------------------------------------------------------------------------------
ThfDataSegment::ThrustPoint& ThfDataSegment::ThrustPoint::operator=(const ThrustPoint& tp)
{
   if (this != &tp)
   {
      time = tp.time;
      magnitude = tp.magnitude;
      vector[0] = tp.vector[0];
      vector[1] = tp.vector[1];
      vector[2] = tp.vector[2];
      mdot = tp.mdot;
   }
   return *this;
}

