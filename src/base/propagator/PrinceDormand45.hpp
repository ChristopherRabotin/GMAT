//$Id$
//------------------------------------------------------------------------------
//                        PrinceDormand45
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// *** File Name : PrinceDormand45.hpp
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
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//            - All double types to Real types
//
//                           : 7/14/2004 - A. Greene, Missions Applications Branch
// **************************************************************************


#ifndef PrinceDormand45_H
#define PrinceDormand45_H

#include "gmatdefs.hpp"
#include "RungeKutta.hpp"

/**
 * This class implements a Runge-Kutta integrator using the coefficients derived 
 * by Prince and Dormand.  This particular set of coefficients implements the
 * fifth order integrator with fourth order error control.  
 */
class GMAT_API PrinceDormand45 :
    public RungeKutta
{
public:
//    PrinceDormand45();
    PrinceDormand45(const std::string &nomme = "");
    virtual ~PrinceDormand45();

    PrinceDormand45(const PrinceDormand45&);
    PrinceDormand45 &       operator=(const PrinceDormand45&);

    virtual Propagator*     Clone() const;

protected:
    void                        SetCoefficients();

};

#endif // PrinceDormand45_hpp
