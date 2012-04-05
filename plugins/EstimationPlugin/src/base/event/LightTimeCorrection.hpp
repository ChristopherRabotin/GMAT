//$Id: LightTimeCorrection.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         LightTimeCorrection
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

   virtual void CalculateTimestepEstimate();
   virtual Real CalculateRange();
};

#endif /* LightTimeCorrection_hpp */
