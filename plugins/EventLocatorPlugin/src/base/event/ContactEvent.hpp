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
 * Definition for the ContactEvent class.
 */
//------------------------------------------------------------------------------

#ifndef ContactEvent_hpp
#define ContactEvent_hpp

#include "gmatdefs.hpp"
#include "EventLocatorDefs.hpp"
#include "LocatedEvent.hpp"

/**
 * ContactEvent class
 */
class LOCATOR_API ContactEvent : public LocatedEvent
{
public:

   ContactEvent(Real startEpoch, Real endEpoch);
   virtual ~ContactEvent();
   ContactEvent(const ContactEvent& copy);
   ContactEvent& operator=(const ContactEvent& copy);

   /// Return computed quantities

   // Get the report string
   virtual std::string  GetReportString();

protected:
};

#endif // ContactEvent_hpp
