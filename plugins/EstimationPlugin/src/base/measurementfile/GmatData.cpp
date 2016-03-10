//$Id: GmatData.cpp 1398 2013-07-02 20:39:37Z tdnguyen $
//------------------------------------------------------------------------------
//                         GmatData
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
// Author: Tuan Dang Nguyen. NASA/GSFC
// Created: 2013/07/02
//
/**
 * Class that contains GMAT data record
 *
 * This class is based class for all data record structure used by GMAT.
 */
//------------------------------------------------------------------------------


#include "GmatData.hpp"
#include "EstimationDefs.hpp"


//-----------------------------------------------------------------------------
// ObservationData()
//-----------------------------------------------------------------------------
/**
 * Default constructor
 */
//-----------------------------------------------------------------------------
GmatData::GmatData(const std::string &format):
   dataFormat   (format)
{
}


//-----------------------------------------------------------------------------
// ~ObservationData()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
GmatData::~GmatData()
{
}


//-----------------------------------------------------------------------------
// GmatData(const GmatData& gd):
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param gd The GmatData object that sets the data for the new one
 */
//-----------------------------------------------------------------------------
GmatData::GmatData(const GmatData& gd):
   dataFormat      (gd.dataFormat)
{
}


//-----------------------------------------------------------------------------
// GmatData& operator=(const GmatData& gd)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param gd The GmatData object that sets the data for the this one
 *
 * @return This object, configured to match gd
 */
//-----------------------------------------------------------------------------
GmatData& GmatData::operator=(const GmatData& gd)
{
   if (&gd != this)
   {
      dataFormat              = gd.dataFormat;
   }

   return *this;
}

