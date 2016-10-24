//$Id: Ionosphere.hpp 65 2010-06-21 00:10:28Z  $
//------------------------------------------------------------------------------
//                         Ionosphere Model
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Tuan Dang Nguyen
// Created: 2010/06/21
//
/**
 * IRI 2007 ionosphere media correction model.
 */
//------------------------------------------------------------------------------
#ifndef Ionosphere_hpp
#define Ionosphere_hpp

#include "MediaCorrection.hpp"
#include "gmatdefs.hpp"
#include "Rvector3.hpp"

#include "f2c.h"

#ifdef __linux__

#ifndef integer
typedef int integer;
#endif

#ifndef logical
typedef int logical;
#endif

#ifndef ftnlen
typedef int ftnlen;
#endif

#endif

#ifdef __APPLE__

//#undef abs  // to work on Mac - the macro for abs confuses the Mac C++ code

#ifndef integer
typedef int integer;
#endif

#ifndef logical
typedef int logical;
#endif

#ifndef ftnlen
typedef int ftnlen;
#endif

#endif

#ifdef _MSC_VER
//#else
typedef long int integer;
typedef long int logical;
typedef long int ftnlen;
#endif

typedef float real;
typedef double doublereal;

typedef doublereal (*D_fp)(...), (*E_fp)(...);

class Ionosphere: public MediaCorrection
{
public:
   Ionosphere(const std::string &nomme);
   virtual ~Ionosphere();
   Ionosphere(const Ionosphere& ions);
   Ionosphere& operator=(const Ionosphere& ions);
   virtual GmatBase*    Clone() const;

   virtual bool Initialize();

   bool SetWaveLength(Real lambda);
   bool SetTime(GmatEpoch ep);
   bool SetStationPosition(Rvector3 p);
   bool SetSpacecraftPosition(Rvector3 p);
   bool SetEarthRadius(Real r);

   Real TEC();
   Real BendingAngle();            // specify the change of elevation angle
   virtual RealArray Correction();

protected:
   /// epoch range specified by ap.dat file
   Integer yyyymmddMin;
   Integer yyyymmddMax;


private:
   void GetTimeRange();

   float ElectronDensity(Rvector3 pos1);

   Real waveLength;          // wave length of the signal
   GmatEpoch epoch;          // time
   Rvector3 stationLoc;      // station location
   Rvector3 spacecraftLoc;   // spacecraft location

   Integer yyyy;
   Integer mmdd;
   Real hours;

   Real earthRadius;

   static const Real NUM_OF_INTERVALS;
   static const Real IONOSPHERE_MAX_ATTITUDE;
};


class IonosphereCorrectionModel
{
public:
   static IonosphereCorrectionModel* Instance(); 
   Ionosphere* GetIonosphereInstance();

private:
   IonosphereCorrectionModel();
   virtual ~IonosphereCorrectionModel();

   static IonosphereCorrectionModel* instance;
   Ionosphere* ionosphereObj;
};


#endif //Ionosphere_hpp_

