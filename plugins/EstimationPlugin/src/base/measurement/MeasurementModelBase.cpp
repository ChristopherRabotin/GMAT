//$Id$
//------------------------------------------------------------------------------
//                          MeasurementModelBase
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/06/24
//
/**
 * MeasurementModelBase declaration used in GMAT's estimator and simulator
 * factory code
 */
//------------------------------------------------------------------------------

#include "MeasurementModelBase.hpp"


MeasurementModelBase::MeasurementModelBase(const std::string& name,
      const std::string& typeName) :
   GmatBase             (Gmat::MEASUREMENT_MODEL, typeName, name)
{
   theTimeConverter = TimeSystemConverter::Instance();
}

MeasurementModelBase::~MeasurementModelBase()
{
}

MeasurementModelBase::MeasurementModelBase(const MeasurementModelBase& mm) :
   GmatBase             (mm)
{
   theTimeConverter = TimeSystemConverter::Instance();
}

MeasurementModelBase& MeasurementModelBase::operator=(
      const MeasurementModelBase& mm)
{
   if (this != &mm)
      GmatBase::operator=(mm);
   return *this;
}

Integer MeasurementModelBase::GetParmIdFromEstID(Integer id, GmatBase* obj)
{
   return id - obj->GetType() * 250;
}
