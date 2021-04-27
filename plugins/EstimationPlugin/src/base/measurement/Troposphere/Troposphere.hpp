//$Id: Troposphere.hpp 65 2010-07-22 00:10:28Z  $
//------------------------------------------------------------------------------
//                         Troposphere Model
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
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
// Created: 2010/07/22
//
/**
 * Troposphere media correction model.
 */
//------------------------------------------------------------------------------
#ifndef Troposphere_hpp
#define Troposphere_hpp

#include "MediaCorrection.hpp"

#include "math.h"

class Troposphere : public MediaCorrection
{
public:
	Troposphere(const std::string& nomme);
	virtual ~Troposphere();
    Troposphere(const Troposphere& tps);
    Troposphere& operator=(const Troposphere& tps);
    virtual GmatBase*    Clone() const;

	bool SetTime(GmatEpoch ep);

	virtual RealArray Correction();		// specify the changes of range, angle, and time

private:

   RealArray CalculateHS(); // HopfieldSaastamoinen model
   RealArray CalculateMarini(); // Marini model

   void TROGET(Real FLATD, Real FLOND, Integer MON, Integer &NS, Real &HT);
   // 'FF1_Tropo' and 'FF2_Tropo' are necessary because FF1 is an OS X Macro and
   // the code will not compile with original argument name 'FF1'
   Real BendingIntegral(Real ALPHA, Real FF1_Tropo, Real FF2_Tropo,
                        Real FO, Real F1, Real P);

   void LoadMariniDataFile();

   struct GMAT_API MariniDataStruct
   {
      Real      latitude;
      Real      longitude;
      Integer   refractivity[12];
   };
   
	Integer month;						// unit: no unit. January = 1
	std::vector<MariniDataStruct> mariniData;

   static const Real term2;

};

#endif /* Troposphere_hpp */
