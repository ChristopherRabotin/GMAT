//$Id$
//------------------------------------------------------------------------------
//                            SolarFluxReader
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

class SolarFluxReader
{

public:
   SolarFluxReader();
   ~SolarFluxReader(); 
   
private:
   typedef struct FluxDataCSSI
   {
      GmatEpoch epoch;
      Real kp[8];
      Real ap[8];
      Real adjF107;
      Real adjCtrF107a;
      Real obsF107;
      Real obsCtrF107a;
   };

   typedef struct FluxData : FluxDataCSSI
   {
      // the first three items is NOMINAL, the second three items
      // is EARLY, and the last three items is LATE TIMING.
      Real F107a[9];
      // the first is NOMINAL, the second is EARLY, and the last is LATE TIMING.
      Real apSchatten[3];
   };
  

   const char *beg_ObsTag;
   const char *end_ObsTag;
   /// offset required to start reading observation (BEGIN OBSERVED) tag
   std::streamoff begObs;
   /// offset required to stop reading observation (END OBSERVED) tag
   std::streamoff endObs;
   /// offset required to start reading Schatten data
   std::streamoff begData;

   char *line; 

   const char *obsFileName;
   const char *predictFileName;
   
   /// Obs File Stream
   std::ifstream inObs;
   /// Predict File Stream
   std::ifstream inPredict;
   /// CSSI data array 
   std::vector<FluxData> obsFluxData;
   /// Schatten data array
   std::vector<FluxData> predictFluxData;

   bool LoadObsData();
   bool LoadPredictData();

public:
   /// Open both CSSI and Schatten files
   bool Open();
   /// Close both CSSI and Schatten files
   bool Close();
   /// Load data from both opened files
   bool LoadFluxData(const char *obsFileName = NULL, const char *predictFileName = NULL);
   /// Get Flux data from either of two vectors filled in during LoadFluxData
   FluxData GetInputs(GmatEpoch epoch);
};


#endif

