//------------------------------------------------------------------------------
//                           PointGroup
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Wendy Shoan, NASA/GSFC
// Created: 2016.05.06
//
/**
 * Definition of the PointGroup class.  This class stores latitudes, 
 * longitudes, and coordinates for points that are either set on input or
 * computed in the class based on an input number or angle.
 */
//------------------------------------------------------------------------------
#ifndef PointGroup_hpp
#define PointGroup_hpp

#include "gmatdefs.hpp"
#include "AbsoluteDate.hpp"
#include "Spacecraft.hpp"
#include "OrbitState.hpp"
#include "Rvector6.hpp"

class PointGroup
{
public:
   
   // class methods
   PointGroup();
   PointGroup(const PointGroup &copy);
   PointGroup& operator=(const PointGroup &copy);
   
   virtual ~PointGroup();
   
   virtual void      AddUserDefinedPoints(const RealArray& lats,
                                          const RealArray& lons);
   virtual void      AddHelicalPointsByNumPoints(Integer numGridPoints);
   virtual void      AddHelicalPointsByAngle(Real angleBetweenPoints);
   virtual Rvector3* GetPointPositionVector(Integer idx);
   virtual void      GetLatAndLon(Integer idx, Real &theLat, Real &theLon);
   virtual Integer   GetNumPoints();
   virtual void      GetLatLonVectors(RealArray &lats, RealArray &lons);
   virtual void      SetLatLonBounds(Real latUp, Real latLow,
                                     Real lonUp, Real lonLow);
   
   
protected:
   
   /// Latitude coordinates of grid points
   RealArray              lat;
   /// Longitude coordinates of grid points
   RealArray              lon;
   // Cartesian coordinates of grid points
   std::vector<Rvector3*> coords;
   /// num of points
   Integer                numPoints;
   /// Number of points requested in the point algorithm
   Integer                numRequestedPoints;  // WHERE/WHY is this used?
   /// Upper bound on allowable latitude -pi/2 <= latUpper <= pi/2
   Real                   latUpper;
   /// Upper bound on allowable latitude -pi/2 <= latLower <= pi/2
   Real                   latLower;
   /// Upper bound on allowable longitude
   Real                   lonUpper;
   /// Upper bound on allowable longitude
   Real                   lonLower;
   
   bool    CheckHasPoints();
   void    AccumulatePoints(Real lat1, Real lon1);
   void    ComputeTestPoints(const std::string &modelName, Integer numGridPts);
   void    ComputeHelicalPoints(Integer numReqPts);
   
};
#endif // PointGroup_hpp







