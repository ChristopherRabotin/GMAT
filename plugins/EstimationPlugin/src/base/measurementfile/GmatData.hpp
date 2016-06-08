//$Id: GmatData.hpp 1398 2013-07-02 20:39:37Z tdnguyen $
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
 * Class that contains GMAT data
 *
 * This class is essentially a struct designed to contain GMAT data
 * retrieved from a data stream.
 */
//------------------------------------------------------------------------------


#ifndef GmatData_hpp
#define GmatData_hpp

#include "estimation_defs.hpp"
#include "EstimationDefs.hpp"


/**
 * Class used to set and retrieve GMAT data record.
 */
class ESTIMATION_API GmatData
{
public:
   GmatData(const std::string &format = "");
   virtual ~GmatData();
   GmatData(const GmatData& od);
   GmatData&  operator=(const GmatData& od);

   /// declare abstract functions:
   virtual void   Clear() = 0;

public:
   /// Data format
   std::string dataFormat;			// Flag indicating which data format is used. Its value is either "GMATInternal", "GMAT_OD", "GMAT_ODDoppler" or "GMAT_RampTable"

};

#endif /* GmatData_hpp */
