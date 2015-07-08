//$Id: Ionosphere.hpp 65 2010-06-21 00:10:28Z tdnguye2@NDC $
//------------------------------------------------------------------------------
//                         Ionosphere Model
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

//#ifdef __linux__
//
//#ifndef integer
//typedef int integer;
//#endif
//
//#ifndef logical
//typedef int logical;
//#endif
//
//#ifndef ftnlen
//typedef int ftnlen;
//#endif
//
//#else
typedef long int integer;
typedef long int logical;
typedef long int ftnlen;
//#endif

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

   virtual bool Initialize();                         // made changes by TUAN NGUYEN

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
   Integer yyyymmddMin;                              // made changes by TUAN NGUYEN
   Integer yyyymmddMax;                              // made changes by TUAN NGUYEN


private:
   void GetTimeRange();                         // made changes by TUAN NGUYEN

   float ElectronDensity(Rvector3 pos2, Rvector3 pos1);
//   float ElectronDensity1(Rvector3 pos2, Rvector3 pos1);

   Real waveLength;          // wave length of the signal
   GmatEpoch epoch;          // time
   Rvector3 stationLoc;      // station location
   Rvector3 spacecraftLoc;   // spacecraft location

   Integer yyyy;
   Integer mmdd;
   Real hours;

   Real earthRadius;

   static const Real NUM_OF_INTERVALS;
   static const Real IONOSPHERE_MAX_ATTITUDE;             // made changes by TUAN NGUYEN
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

