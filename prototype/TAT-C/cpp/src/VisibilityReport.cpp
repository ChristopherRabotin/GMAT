//------------------------------------------------------------------------------
//                           VisibilityReport
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
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
// Author: Wendy Shoan, NASA/GSFC
// Created: 2016.04.29
//
/**
 * Implementation of the the visibility report base class
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "VisibilityReport.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
VisibilityReport::VisibilityReport()
{
   // by default, startDate and endDate are the default values
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
VisibilityReport::VisibilityReport( const VisibilityReport &copy) :
   startDate   (copy.startDate),
   endDate     (copy.endDate)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
VisibilityReport& VisibilityReport::operator=(const VisibilityReport &copy)
{
   if (&copy == this)
      return *this;
   
   startDate  = copy.startDate;
   endDate    = copy.endDate;
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------

VisibilityReport::~VisibilityReport()
{
}

//------------------------------------------------------------------------------
// void SetStartDate(const AbsoluteDate &toDate)
//------------------------------------------------------------------------------
void VisibilityReport::SetStartDate(const AbsoluteDate &toDate)
{
   startDate = toDate;
}

//------------------------------------------------------------------------------
// void SetEndDate(const AbsoluteDate &toDate)
//------------------------------------------------------------------------------
void VisibilityReport::SetEndDate(const AbsoluteDate &toDate)
{
   endDate = toDate;
}



//------------------------------------------------------------------------------
// const AbsoluteDate& VisibilityReport::GetStartDate()
//------------------------------------------------------------------------------
const AbsoluteDate& VisibilityReport::GetStartDate()
{
   return startDate;
}

//------------------------------------------------------------------------------
// const AbsoluteDate& VisibilityReport::GetEndDate()
//------------------------------------------------------------------------------
const AbsoluteDate& VisibilityReport::GetEndDate()
{
   return endDate;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// None
