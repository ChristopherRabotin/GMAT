//$Id: SnTwoWayRange.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         SnTwoWayRange
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
// Created: 2009/12/18
//
/**
 * The space network 2-way range measurement model
 */
//------------------------------------------------------------------------------


#ifndef SnTwoWayRange_hpp
#define SnTwoWayRange_hpp

#include "estimation_defs.hpp"
#include "TwoWayRange.hpp"

/**
 * This class implements the Space Network range measurement calculations.
 *
 * The class is not yet implemented.
 */
class ESTIMATION_API SnTwoWayRange: public TwoWayRange
{
public:
   SnTwoWayRange(const std::string &nomme = "");
   virtual ~SnTwoWayRange();
   SnTwoWayRange(const SnTwoWayRange& sn);
   SnTwoWayRange& operator=(const SnTwoWayRange& sn);

   virtual GmatBase* Clone() const;

   virtual const std::vector<RealArray>& CalculateMeasurementDerivatives(
         GmatBase *obj, Integer id);


protected:
   virtual bool                    Evaluate(bool withEvents = false);

};

#endif /* SnTwoWayRange_hpp */
