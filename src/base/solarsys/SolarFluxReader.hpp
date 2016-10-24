//$Id$
//------------------------------------------------------------------------------
//                            SolarFluxReader
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
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
// Author: Farideh Farahnak
// Created: 2014/11/14
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS II.
//
/**
 * SolarFluxReader class will be used by the AtomsphereModel class.
 */
//------------------------------------------------------------------------------


#ifndef SOLARFLUXREADER_HPP
#define SOLARFLUXREADER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include "DateUtil.hpp"
#include "GmatConstants.hpp"
#include "GmatBase.hpp"

class GMAT_API SolarFluxReader
{

public:
   SolarFluxReader();
   SolarFluxReader(const SolarFluxReader &sfr);
   SolarFluxReader& operator=(const SolarFluxReader &sfr);
   ~SolarFluxReader(); 
   
   struct FluxDataCSSI
   {
      GmatEpoch epoch;
      Real kp[8];
      Real ap[8];
      Real apAvg;
      Real adjF107;
      Real adjCtrF107a;
      Real obsF107;
      Real obsCtrF107a;

      bool isObsData;
   };

   struct FluxData : FluxDataCSSI
   {
      // the first three items is NOMINAL, the second three items
      // is EARLY, and the last three items is LATE TIMING.
      Real F107a[9];
      // the first is NOMINAL, the second is EARLY, and the last is LATE TIMING.
      Real apSchatten[3];

      FluxData();
      FluxData(const FluxData &fD);
      FluxData &operator=(const FluxData &fD);
      
      // Used in Schatten file indexing
      Integer index;
      Integer id;
   };
  
private:
   const char * beg_ObsTag;
   const char * end_ObsTag;
   /// offset required to start reading observation (BEGIN OBSERVED) tag
   std::streamoff begObs;
   /// offset required to stop reading observation (END OBSERVED) tag
   std::streamoff endObs;
   /// offset required to start reading Schatten data
   std::streamoff begData;
   /// each line in the file
   const char *line;

   std::string obsFileName;
   std::string predictFileName;
   
   /// Obs File Stream
   std::ifstream inObs;
   /// Predict File Stream
   std::ifstream inPredict;
   /// CSSI data array 
   std::vector<FluxData> obsFluxData;
   /// Schatten data array
   std::vector<FluxData> predictFluxData;

   GmatEpoch historicStart;
   GmatEpoch historicEnd;
   GmatEpoch predictStart;
   GmatEpoch predictEnd;

   /// Index for Schatten flux setting
   Integer schattenFluxIndex;
   /// Index for Schatten Ap value
   Integer schattenApIndex;
      
   /// Flag used to indicate that the "Too early" warning not yet issued
   bool warnEpochBefore;
   /// Flag used to indicate that the "Too late" warning not yet issued
   bool warnEpochAfter;

   /// Epoch reference point for when the F10.7 observations changed location
   GmatEpoch f107RefEpoch;
   /// Flag used to toggle interpolation of the f10.7 values
   bool interpolateFlux;
   /// Flag used to toggle interpolation for the geomagnetic index (predict only)
   bool interpolateGeo;

   bool LoadObsData();
   bool LoadPredictData();
   Real ConvertApToKp(Real ap);

public:
   /// Open CSSI and Schatten files if opened
   bool Open();
   /// Close CSSI and Schatten files if opened
   bool Close();
   /// Load data from both opened files
   bool LoadFluxData(const std::string &obsFile = "", const std::string &predictFile = "");
   /// Get Flux data from either of two vectors filled in during LoadFluxData
   FluxData GetInputs(GmatEpoch epoch);

   void GetEpochs(GmatEpoch &hStart, GmatEpoch &hEnd, GmatEpoch &pStart,
                  GmatEpoch &pEnd);

   /// Change Ap data for MSISE model
   void PrepareApData(FluxData &fD, GmatEpoch epoch);
   void PrepareKpData(SolarFluxReader::FluxData &fD, GmatEpoch epoch);
   void SetSchattenFlags(Integer timingSet, Integer magnitudeSet);
};


#endif

