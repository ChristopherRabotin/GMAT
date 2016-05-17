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
 * Implementation of the the visibility report base class
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "PointGroup.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "Rmatrix33.hpp"
#include "TATCException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_HELICAL_POINTS

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
PointGroup::PointGroup(Spacecraft *sat) :
   numPoints          (0),
   numRequestedPoints (0),
   latUpper           (GmatMathConstants::PI_OVER_TWO),
   latLower           (-GmatMathConstants::PI_OVER_TWO),
   lonUpper           (GmatMathConstants::TWO_PI),
   lonLower           (0.0)
{
   // lat, lon, and coords are all empty at the start
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
PointGroup::PointGroup(const PointGroup &copy) :
   numPoints          (copy.numPoints),
   numRequestedPoints (copy.numRequestedPoints),
   latUpper           (copy.latUpper),
   latLower           (copy.latLower),
   lonUpper           (copy.lonUpper),
   lonLower           (copy.lonLower)
{
   lat.clear();
   lon.clear();
   coords.clear();
   lat   = copy.lat;
   lon   = copy.lon;
   for (Integer ii = 0; ii < copy.numPoints; ii++)
   {
      Rvector3 *copyCoord = copy.coords.at(ii);
      Rvector3 *newCoord  = new Rvector3(*copyCoord);
      coords.push_back(newCoord);
   }
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
PointGroup& PointGroup::operator=(const PointGroup &copy)
{
   if (&copy == this)
      return *this;
   
   numPoints          = copy.numPoints;
   numRequestedPoints = copy.numRequestedPoints;
   latUpper           = copy.latUpper;
   latLower           = copy.latLower;
   lonUpper           = copy.lonUpper;
   lonLower           = copy.lonLower;
   
   lat.clear();
   lon.clear();
   coords.clear();
   lat   = copy.lat;
   lon   = copy.lon;
   for (Integer ii = 0; ii < copy.numPoints; ii++)
   {
      Rvector3 *copyCoord = copy.coords.at(ii);
      Rvector3 *newCoord  = new Rvector3(*copyCoord);
      coords.push_back(newCoord);
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
PointGroup::~PointGroup()
{
   lat.clear();
   lon.clear();
   for (Integer ii = 0; ii < numPoints; ii++)
      delete coords.at(ii);
   coords.clear();
}

      
//------------------------------------------------------------------------------
// void AddUserDefinedPoints(const RealArray& lats,
//                           const RealArray& lons)
//------------------------------------------------------------------------------
void PointGroup::AddUserDefinedPoints(const RealArray& lats,
                                      const RealArray& lons)
{
   // Add user defined latitude and longitude points
   // Inputs are real arrays of longitude and latitude in radians
   if (lats.size() != lons.size())
      throw TATCException("latitude and longitude arrays must have the same length\n");

   Integer sz = (Integer) lats.size();
   for (Integer ptIdx = 0; ptIdx < sz; ptIdx++)
      AccumulatePoints(lats.at(ptIdx),lons.at(ptIdx));
}

//------------------------------------------------------------------------------
// void AddHelicalPointsByNumPoints(Integer numGridPoints)
//------------------------------------------------------------------------------
void PointGroup::AddHelicalPointsByNumPoints(Integer numGridPoints)
{
   ComputeTestPoints("Helical",numGridPoints);
}

//------------------------------------------------------------------------------
// void AddHelicalPointsByAngle(Real angleBetweenPoints)
//------------------------------------------------------------------------------
void PointGroup::AddHelicalPointsByAngle(Real angleBetweenPoints)
{
   Integer numGridPoints = GmatMathUtil::Floor(4.0*GmatMathConstants::PI/
                           angleBetweenPoints*angleBetweenPoints);
   ComputeTestPoints("Helical",numGridPoints);
}

//------------------------------------------------------------------------------
// RealArray GetPointPositionVector(Integer idx)
//------------------------------------------------------------------------------
Rvector3* PointGroup::GetPointPositionVector(Integer idx)
{
   // Returns body fixed location of point given point index
   // Inputs. int poiIndex
   // Outputs. Rvector 3x1 containing the position.
   
   // Make sure there are points
   CheckHasPoints();
   
  return coords.at(idx);
}

//------------------------------------------------------------------------------
// void GetLatAndLon(Integer idx, Real &theLat, Real &theLon)
//------------------------------------------------------------------------------
void PointGroup::GetLatAndLon(Integer idx, Real &theLat, Real &theLon)
{
   // Returns body fixed location of point given point index
   // Inputs. int poiIndex
   // Outputs. double lat, double lon
   
   // Make sure there are points
   CheckHasPoints();
   
   theLat = lat.at(idx);
   theLon = lon.at(idx);
}

//------------------------------------------------------------------------------
// Integer GetNumPoints()
//------------------------------------------------------------------------------
Integer PointGroup::GetNumPoints()
{
   return numPoints;
}

//------------------------------------------------------------------------------
// void GetLatLonVectors(RealArray &lats, RealArray &lons)
//------------------------------------------------------------------------------
void PointGroup::GetLatLonVectors(RealArray &lats, RealArray &lons)
{
   // Returns the latitude and longitude vectors
   
   //Make sure there are points
   CheckHasPoints();
   
   lats = lat;
   lons = lon;
}

//------------------------------------------------------------------------------
// void SetLatLonBounds(Integer latUp, Integer latLow,
//                      Integer lonUp, Integer lonLow)
//------------------------------------------------------------------------------
void  PointGroup::SetLatLonBounds(Integer latUp, Integer latLow,
                                  Integer lonUp, Integer lonLow)
{
   if (numPoints >0)
      throw TATCException("You must set Lat/Lon Bounds Before adding points\n");

      
   // Sets bounds on latitude and longitude for grid points
   // angle inputs are in radians
   if (latLow >= latUp)
      throw TATCException("latLower > latUpper or they are equal\n");
   if (lonLow >= lonUp)
      throw TATCException("lonLower > lonUpper or they are equal\n");

   if (latUp < -GmatMathConstants::PI_OVER_TWO ||
       latUp > GmatMathConstants::PI_OVER_TWO)
      throw TATCException("latUpper value is invalid\n");

   latUpper = latUp;

   if (latLow < -GmatMathConstants::PI_OVER_TWO ||
       latLow > GmatMathConstants::PI_OVER_TWO)
       throw TATCException("latLower value is invalid\n");

   latLower = latLow;

   lonUpper = GmatMathUtil::Mod(lonUpper,GmatMathConstants::TWO_PI);
   lonLower = GmatMathUtil::Mod(lonLower,GmatMathConstants::TWO_PI);
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// bool CheckHasPoints()
//------------------------------------------------------------------------------
bool PointGroup::CheckHasPoints()
{
   if (numPoints <= 0)
      throw TATCException("The point group does not have any points\n");
   return true;
}

//------------------------------------------------------------------------------
// void AccumulatePoints(Real lat1, Integer lon1)
//------------------------------------------------------------------------------
void PointGroup::AccumulatePoints(Real lat1, Integer lon1)
{
   // Accumlates points, only adding them if they pass constraint
   // checks
   if ((lat1 >= latLower) && (lat1 <= latUpper) &&
       (lon1 >= lonLower) && (lon1 <= lonUpper))
   {
      numPoints++;
      lon.push_back(lon1);

      lat.push_back(lat1);
      // TODO:  Use geodetic to Cartesian conversion and don't
      // hard code the Earth radius.
      Rvector3 *newCoord = new Rvector3(
                     GmatMathUtil::Cos(lon1) * GmatMathUtil::Cos(lat1),
                     GmatMathUtil::Sin(lon1) * GmatMathUtil::Cos(lat1),
                     GmatMathUtil::Sin(lat1) * 6378.1363);
      coords.push_back(newCoord);
   }
}

//------------------------------------------------------------------------------
// void ComputeTestPoints(const std::string &modelName, Integer numGridPts)
//------------------------------------------------------------------------------
void PointGroup::ComputeTestPoints(const std::string &modelName,
                                   Integer numGridPts)
{
   // Computes surface grid points
   // Inputs: string modelName
   
   // Place first point at north pole
   if (numGridPts >= 1)
   {
      // One Point at North Pole
      AccumulatePoints(GmatMathConstants::PI_OVER_TWO,0.0);
   }
   // Place second point at south pole
   if (numGridPts >= 2)
   {
      // One Point at South Pole
      AccumulatePoints(-GmatMathConstants::PI_OVER_TWO,0.0);
   }
   // Place remaining points according to requested algorithm
   if (numGridPts >= 3)
      if (modelName == "Helical")
         ComputeHelicalPoints(numGridPts-2);
}

//------------------------------------------------------------------------------
// void ComputeHelicalPoints(Integer numReqPts))
//------------------------------------------------------------------------------
void PointGroup::ComputeHelicalPoints(Integer numReqPts)
{
   #ifdef DEBUG_HELICAL_POINTS
      MessageInterface::ShowMessage("Entering PG::ComputeHelicalPoints with numReqPts = %d\n",
            numReqPts);
   #endif
   // Build a set of evenly spaced points using Helical algorithm
   // Inputs: int, numPoints
   
   // Determine how many longitude "bands" and fill them in
   Integer numDiscreteLatitudes = GmatMathUtil::Floor(
                                  GmatMathUtil::Sqrt((numReqPts+1) *
                                  GmatMathConstants::PI/4.0));
   Rvector discreteLatitudes(numDiscreteLatitudes);  // all zeros by default
   
   for (Integer latIdx = 0; latIdx < numDiscreteLatitudes-1; latIdx = latIdx + 2)
   {
      // EVEN numbers
      discreteLatitudes(latIdx) = (GmatMathConstants::PI/2.0)*
                                  (1-(latIdx+1)/(numDiscreteLatitudes+1));
      // ODD NUMBERS
      discreteLatitudes(latIdx+1) = -discreteLatitudes(latIdx);
   }
//   for (Integer latIdx = 1:2:numDiscreteLatitudes
//      % Odd Numbers
//      discreteLatitudes(latIdx) = (pi/2)*(1-(latIdx+1)/(numDiscreteLatitudes+1));
//   % Even Numbers
//   discreteLatitudes(latIdx+1) = -discreteLatitudes(latIdx);
//   end
   
   // Now compute the longitude points for each latitude band
   Rvector alpha(numDiscreteLatitudes+1); // zeros by default
   alpha(0) = 0.0;
   for (Integer latIdx = 0; latIdx < numDiscreteLatitudes; latIdx++)
      alpha(0) = alpha(0) + GmatMathUtil::Cos(discreteLatitudes(latIdx));

   Rvector numPointsByLatBand(numDiscreteLatitudes); // zeros by default
   Integer pointIdx = 2; // this is two because we already added in points at the poles
   Rvector numRemainingPoints(numDiscreteLatitudes+1); // Array of ints in C++
//   numRemainingPoints(0) = numRequestedPoints;
   numRemainingPoints(0) = numReqPts;
   for (Integer latIdx = 0; latIdx < numDiscreteLatitudes-1; latIdx++) // added -1 here
   {
      numPointsByLatBand(latIdx) = GmatMathUtil::Round(
                                   numRemainingPoints(latIdx)*
                                   GmatMathUtil::Cos(discreteLatitudes(latIdx))/
                                   alpha(latIdx));
      numRemainingPoints(latIdx+1) = numRemainingPoints(latIdx) -
                                     numPointsByLatBand(latIdx);
      alpha(latIdx+1) = alpha(latIdx) -
                        GmatMathUtil::Cos(discreteLatitudes(latIdx));
      for (Integer j = 0; j < numPointsByLatBand(latIdx); j++)
      {
         // Compute the latitude for the next point
         Real currentLongitude = 2 * GmatMathConstants::PI *
                                 (j-1)/numPointsByLatBand(latIdx);
         // Insert the point into the cartesian and spherical arrays.
         pointIdx++;
         //  @TODO:  Use geodetic to Cartesian conversion.
         AccumulatePoints(discreteLatitudes(latIdx),currentLongitude);
      }
   }
}
