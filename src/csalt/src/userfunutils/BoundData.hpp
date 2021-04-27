//------------------------------------------------------------------------------
//                              BoundData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan / NASA
// Created: 2016.01.05
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 *
 * NOTE: This class contains state, control, and time lower and upper bounds
 *
 */
//------------------------------------------------------------------------------

#ifndef BoundData_hpp
#define BoundData_hpp

#include "csaltdefs.hpp"
#include "Rvector.hpp"

class BoundData
{
public:
   
   BoundData();
   BoundData(const BoundData &copy);
   BoundData& operator=(const BoundData &copy);
   virtual ~BoundData();
   
   virtual void              SetStateLowerBound(const Rvector &bound);
   virtual void              SetStateUpperBound(const Rvector &bound);
   virtual void              SetControlLowerBound(const Rvector &bound);
   virtual void              SetControlUpperBound(const Rvector &bound);
   virtual void              SetTimeLowerBound(const Rvector &bound);
   virtual void              SetTimeUpperBound(const Rvector &bound);
   virtual void              SetStaticLowerBound(const Rvector &bound);
   virtual void              SetStaticUpperBound(const Rvector &bound);
   
   virtual const Rvector&    GetStateLowerBound();
   virtual const Rvector&    GetStateUpperBound();
   virtual const Rvector&    GetControlLowerBound();
   virtual const Rvector&    GetControlUpperBound();
   virtual const Rvector&    GetTimeLowerBound();
   virtual const Rvector&    GetTimeUpperBound();
   virtual const Rvector&    GetStaticLowerBound();
   virtual const Rvector&    GetStaticUpperBound();
   
   
protected:
   
   /// State Lower Bound
   Rvector stateLowerBound;
   /// State Upper Bound
   Rvector stateUpperBound;
   /// Control Lower Bound
   Rvector controlLowerBound;
   /// Control Upper Bound
   Rvector controlUpperBound;
   /// Time Lower Bound
   Rvector timeLowerBound;
   /// Time Upper Bound
   Rvector timeUpperBound;
   /// Static Lower Bound
   Rvector staticLowerBound;
   /// Static Upper Bound
   Rvector staticUpperBound;
   /// Vector used in place of bounds that don't exist
   Rvector emptyBoundVector;

};
#endif // BoundData_hpp
