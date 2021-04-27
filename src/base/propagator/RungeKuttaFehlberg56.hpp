//$Id$
//------------------------------------------------------------------------------
//                              RungeKuttaFehlberg56
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
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
// *** File Name : rungekuttafehlberg56.h
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 1/15/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//                                    Changes
//                           : 02/26/2004 - L. Ruley, Missions Applications Branch
//                                            - Updated style using GMAT cpp style guide
//                                            - All Real types to Real types
//                            - All int types to Integer types
// **************************************************************************


#ifndef RungeKuttaFehlberg56_hpp
#define RungeKuttaFehlberg56_hpp

#include "gmatdefs.hpp"
#include "RungeKutta.hpp"


/** \brief Implementation of the Runge-Kutta-Fehlberg 5(6) integrator

  This class implements a Runge-Kutta integrator using the coefficients derived 
  by Fehlberg.  This particular set of coefficients implements the
  sixth order integrator with fifth order error control.  
 */
class GMAT_API RungeKuttaFehlberg56 :
    public RungeKutta
{
public:
//    RungeKuttaFehlberg56();
    RungeKuttaFehlberg56(const std::string &nomme = "");
    virtual ~RungeKuttaFehlberg56();

    RungeKuttaFehlberg56(const RungeKuttaFehlberg56&);
    RungeKuttaFehlberg56 &  operator=(const RungeKuttaFehlberg56&);

    virtual GmatBase*       Clone() const; // wcs 2004.06.04 

protected:
    void                    SetCoefficients();

};

#endif
