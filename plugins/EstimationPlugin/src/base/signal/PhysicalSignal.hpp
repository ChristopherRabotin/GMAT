//$Id$
//------------------------------------------------------------------------------
//                           PhysicalSignal
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 9, 2014
/**
 * Class used to model instantaneous signals
 */
//------------------------------------------------------------------------------

#ifndef PhysicalSignal_hpp
#define PhysicalSignal_hpp

#include "SignalBase.hpp"

/**
 * Signal class used for instantaneous measurements
 *
 * This class might be going away based on the evolving measurement design
 */
class ESTIMATION_API PhysicalSignal: public SignalBase
{
public:
   PhysicalSignal(const std::string &typeStr, const std::string &name = "");
   virtual ~PhysicalSignal();
   PhysicalSignal(const PhysicalSignal& gs);
   PhysicalSignal& operator=(const PhysicalSignal& gs);

   virtual GmatBase* Clone() const;

   virtual bool ModelSignal(bool EpochAtReceive = true);
   virtual const std::vector<RealArray>&
            ModelSignalDerivative(GmatBase *obj, Integer forId);

protected:
};

#endif /* PhysicalSignal_hpp */
