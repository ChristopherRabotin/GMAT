//$Id: LightTimeCorrection.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         LightTimeCorrection
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/11/16
//
/**
 * Event code used to find the epoch of one endpoint on a light signal path,
 * given the other endpoint.
 */
//------------------------------------------------------------------------------


#ifndef LightTimeCorrection_hpp
#define LightTimeCorrection_hpp

#include "Event.hpp"
#include "GmatState.hpp"
#include "EventData.hpp"


class ESTIMATION_API LightTimeCorrection : public Event
{
public:
   LightTimeCorrection(const std::string &name = "");
   virtual ~LightTimeCorrection();
   LightTimeCorrection(const LightTimeCorrection& ltc);
   LightTimeCorrection&    operator=(const LightTimeCorrection& ltc);

   virtual GmatBase* Clone() const;
   virtual bool Initialize();
   virtual Real Evaluate();
   virtual void FixState(GmatBase* obj, bool LockState = false);
   virtual void FixState();

   Real GetRelativityCorrection();     // Get relativity coorection (in Km)
   Real GetLightTripRange();				// Get light trip range	(in Km)

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// Storage element used to manage calculated position data
   Rvector3             positionBuffer;
   /**
    * Speed of light (Set from GMAT's GmatConstants header file, but set
    * here as an attribute in case a user needs to override it in later
    * implementations)
    */
   Real                 lightSpeed;
   /// Index of the fixed participant in this Event
   Integer				   fixedParticipant;
   /// Current range measurement
   Real                 range;
   /// Variable used for iteration
   Real                 oldRange;

   /// Number of Iteration. It is used to count number of iteration in order to exit loop
   UnsignedInt numIter;

   virtual void CalculateTimestepEstimate();
   virtual Real CalculateRange();

   Real RelativityCorrection(Rvector3 r1, Rvector3 r2, Real t1, Real t2);

public:
   Real ETminusTAI(Real tA1MJD, GmatBase* participant);

private:
   Real relativityCorrection;				// relativity correction (unit: km)
   Real precisionRange;						// precision light time range (unit: km)

};

#endif /* LightTimeCorrection_hpp */
