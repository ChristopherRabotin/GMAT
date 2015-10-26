//$Id$
//------------------------------------------------------------------------------
//                             PlotReceiver
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Thomas Grubb
// Created: 2008/08/21
//
/**
 * Declares ListenerManager class.
 */
//------------------------------------------------------------------------------
#ifndef ListenerManager_hpp
#define ListenerManager_hpp

#include "gmatdefs.hpp"
#include "ISolverListener.hpp"

/**
 * ListenerManager defines the interfaces used for creating listener classes (like solver window)
 */

class GMAT_API ListenerManager
{
public:

   virtual ISolverListener * CreateSolverListener(const std::string &tableName,
                                         const std::string &oldName,
                                         Real positionX, Real positionY,
                                         Real width, Real height, bool isMaximized) = 0;
protected:
   ListenerManager();
   virtual ~ListenerManager();
};

#endif
