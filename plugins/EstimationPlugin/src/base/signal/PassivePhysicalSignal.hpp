//$Id$
//------------------------------------------------------------------------------
//                           PassivePhysicalSignal
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author:
// Created: 
/**
 * Class used to model physical signals that do not have hardware delay
 */
 //------------------------------------------------------------------------------

#ifndef PassivePhysicalSignal_hpp
#define PassivePhysicalSignal_hpp

#include "PhysicalSignal.hpp"

class ESTIMATION_API PassivePhysicalSignal : public PhysicalSignal
{
public:
   PassivePhysicalSignal(const std::string &typeStr, const std::string &name = "");
   virtual ~PassivePhysicalSignal();

   PassivePhysicalSignal(const PassivePhysicalSignal& pps);
   PassivePhysicalSignal& operator=(const PassivePhysicalSignal& pps);

   virtual GmatBase* Clone() const;

protected:

   /// This function is used to calculate total hardware delay
   virtual bool   HardwareDelayCalculation();

};

#endif /* PassivePhysicalSignal_hpp */

