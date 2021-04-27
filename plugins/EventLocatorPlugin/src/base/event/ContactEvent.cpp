//$Id$
//------------------------------------------------------------------------------
//                               ContactEvent
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
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
// Author: Wendy C. Shoan
// Created: 2015.01.08
//
/**
 * Implementation for the ContactEvent base class.
 */
//------------------------------------------------------------------------------

#include <sstream>
#include "ContactEvent.hpp"
#include "TimeSystemConverter.hpp"
#include "StringUtil.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

ContactEvent::ContactEvent(Real startEpoch, Real endEpoch) :
   LocatedEvent(startEpoch, endEpoch)
{
}

ContactEvent::~ContactEvent()
{
   // nothing to do here
}

ContactEvent::ContactEvent(const ContactEvent& copy) :
   LocatedEvent(copy)
{
}

ContactEvent& ContactEvent::operator=(const ContactEvent& copy)
{
   if (&copy != this)
   {
      LocatedEvent::operator=(copy);
   }

   return *this;
}


std::string ContactEvent::GetReportString()
{
   std::stringstream eventString("");
   std::string outputFormat = "UTCGregorian";  // will use epochFormat in the future?
   std::string startGregorian, endGregorian;
   Real        resultMjd;
   std::string blanks       = "    ";

   TimeSystemConverter::Instance()->Convert("A1ModJulian", start, "",
                              outputFormat, resultMjd, startGregorian);
   TimeSystemConverter::Instance()->Convert("A1ModJulian", end, "",
                              outputFormat, resultMjd, endGregorian);

   Real currentDuration = GetDuration();

   eventString << startGregorian << blanks << endGregorian         << blanks <<
                  GmatStringUtil::BuildNumber(currentDuration, false, 14);

   return eventString.str();
}
