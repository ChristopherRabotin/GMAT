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

#define DEBUG_HELICAL_POINTS
#define DEBUG_POINTS

using namespace GmatMathConstants;
using namespace GmatMathUtil;

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
   latUpper           (PI_OVER_TWO),
   latLower           (-PI_OVER_TWO),
   lonUpper           (TWO_PI),
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
   Integer numGridPoints = Floor(4.0*PI/
                           (angleBetweenPoints*angleBetweenPoints));
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

   if (latUp < -PI_OVER_TWO ||
       latUp > PI_OVER_TWO)
      throw TATCException("latUpper value is invalid\n");

   latUpper = latUp;

   if (latLow < -PI_OVER_TWO ||
       latLow > PI_OVER_TWO)
       throw TATCException("latLower value is invalid\n");

   latLower = latLow;

   lonUpper = Mod(lonUp, TWO_PI);
   lonLower = Mod(lonLow,TWO_PI);
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
// void AccumulatePoints(Real lat1, Real lon1)
//------------------------------------------------------------------------------
void PointGroup::AccumulatePoints(Real lat1, Real lon1)
{
   #ifdef DEBUG_POINTS
      MessageInterface::ShowMessage(
       "Entering PG::AccumulatePoints with lat1 = %12.10f and lon1 = %12.10f\n",
        lat1, lon1);
   #endif
   // Accumulates points, only adding them if they pass constraint
   // checks
   if ((lat1 >= latLower) && (lat1 <= latUpper) &&
       (lon1 >= lonLower) && (lon1 <= lonUpper))
   {
      lon.push_back(lon1);

      lat.push_back(lat1);
      // TODO:  Use geodetic to Cartesian conversion and don't
      // hard code the Earth radius.
      Rvector3 *newCoord = new Rvector3(
                     Cos(lon1) * Cos(lat1),
                     Sin(lon1) * Cos(lat1),
                     Sin(lat1) * 6378.1363);
      #ifdef DEBUG_POINTS
         MessageInterface::ShowMessage(
                 "PG::AccumulatePoints, pushing back %12.10f  %12.10f  %12.10f\n",
                 newCoord->GetElement(0), newCoord->GetElement(1), newCoord->GetElement(2));
      #endif
      coords.push_back(newCoord);
      numPoints++;
   }
   else
   {
      throw TATCException("lat or lon too big or small!!!!\n");
   }
}

//------------------------------------------------------------------------------
// void ComputeTestPoints(const std::string &modelName, Integer numGridPts)
//------------------------------------------------------------------------------
void PointGroup::ComputeTestPoints(const std::string &modelName,
                                   Integer numGridPts)
{
   #ifdef DEBUG_POINTS
      MessageInterface::ShowMessage(
            "Entering PG::ComputeTestPoints with modelName = %s and numGridPoints = %d\n",
                                    modelName.c_str(), numGridPts);
   #endif
   // Computes surface grid points
   // Inputs: string modelName
   
   // Place first point at north pole
   if (numGridPts > 0)
   {
      // One Point at North Pole
      AccumulatePoints(PI_OVER_TWO,0.0);
   }
   // Place second point at south pole
   if (numGridPts > 1)
   {
      // One Point at South Pole
      AccumulatePoints(-PI_OVER_TWO,0.0);
   }
   // Place remaining points according to requested algorithm
   if (numGridPts > 2)
   {
      if (modelName == "Helical")
         ComputeHelicalPoints(numGridPts-2);
   }
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
   Real    q                    = ((Real)(numReqPts+1)) * (PI/4.0);
   Integer numDiscreteLatitudes = (Integer) Floor(Sqrt(q));
   
   Rvector discreteLatitudes(numDiscreteLatitudes);  // all zeros by default
   #ifdef DEBUG_HELICAL_POINTS
      MessageInterface::ShowMessage("In PG::ComputeHelicalPoints, numDiscreteLatitudes = %d\n",
                                    numDiscreteLatitudes);
   #endif
   
   for (Integer latIdx = 0; latIdx < numDiscreteLatitudes; latIdx += 2)
   {
      Real latOverNum = (Real) (latIdx + 1) / (Real)(numDiscreteLatitudes + 1);
      Real latToUse   = PI_OVER_TWO * (1.0 - latOverNum);
      // EVEN numbers
      discreteLatitudes(latIdx)   = latToUse;
      // ODD numbers
      #ifdef DEBUG_HELICAL_POINTS
            MessageInterface::ShowMessage("  latIdx               = %d\n", latIdx);
            MessageInterface::ShowMessage("  numDiscreteLatitudes = %d\n", numDiscreteLatitudes);
            MessageInterface::ShowMessage("  latOverNum           = %12.10f\n", latOverNum);
            MessageInterface::ShowMessage("  latToUse             = %12.10f\n", latToUse);
            MessageInterface::ShowMessage("  Setting discreteLatitude(%d) = %12.10f\n",
                                          latIdx, discreteLatitudes(latIdx));
      #endif
      // Only do this if it fits in the array (if numDiscreteLatitudes
      // is odd, latIdx + 1 is too big for the array)
      if (latIdx < (numDiscreteLatitudes - 1))
      {
         discreteLatitudes(latIdx+1) = -latToUse;
         #ifdef DEBUG_HELICAL_POINTS
            MessageInterface::ShowMessage("  Setting discreteLatitude(%d) = %12.10f\n",
                                          latIdx+1, discreteLatitudes(latIdx+1));
         #endif
      }
   }
   
   // Now compute the longitude points for each latitude band
   Real alpha = 0.0;  // was Real array in MATLAB
   for (Integer latIdx = 0; latIdx < numDiscreteLatitudes; latIdx++)
      alpha += Cos(discreteLatitudes(latIdx));

   #ifdef DEBUG_HELICAL_POINTS
      MessageInterface::ShowMessage("  alpha = %12.10f\n", alpha);
   #endif
   Integer pointIdx = 2; // this is two because we already added in points at the poles
   // QUESTION : what is pointIdx used for in this method???
   
   Integer      numPtsByBand       = 0;          // this was an int array in MATLAB
   Integer      numRemainingPoints = numReqPts;  // this was an int array in MATLAB
   
   for (Integer latIdx = 0; latIdx < numDiscreteLatitudes; latIdx++)
   {
      Real currentLat      = discreteLatitudes(latIdx);
      Real cosLat          = Cos(currentLat);
      Real numToRound      = ((Real) numRemainingPoints) * cosLat / alpha;
      numPtsByBand         = (Integer) Round(numToRound);

      #ifdef DEBUG_HELICAL_POINTS
            MessageInterface::ShowMessage("  cosLat(for latIdx = %d) = %12.10f\n",
                                          latIdx, cosLat);
            MessageInterface::ShowMessage("  numPtsByBand = %d\n", numPtsByBand);
      #endif
      // decrement numRemainingPoints and alpha
      numRemainingPoints -= numPtsByBand;
      alpha              -= cosLat;
      
      for (Integer pt = 0; pt < numPtsByBand; pt++)
      {
         // Compute the latitude for the next point
         // WAS (pt-1) but this causes a lat or lon out-of-range!!!
         Real currentLongitude = TWO_PI * ((Real) (pt)) / ((Real) numPtsByBand);
         // Insert the point into the cartesian and spherical arrays.
         pointIdx++;  // used WHERE?
         //  @TODO:  Use geodetic to Cartesian conversion.
         AccumulatePoints(currentLat,currentLongitude);
      }
   }
}
