//$Id$
//------------------------------------------------------------------------------
//                                PrinceDormand78
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// *** File Name : PrinceDormand78.cpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
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
//                           : 6/1/2004 - A. Greene, Missions Applications Branch
// **************************************************************************

#ifndef PrinceDormand78_H
#define PrinceDormand78_H

#include "gmatdefs.hpp"
#include "RungeKutta.hpp"

/**
 * This class implements a Runge-Kutta integrator using the coefficients derived 
 * by Prince and Dormand.  This particular set of coefficients implements the
 * eighth order integrator with seventh order error control.  
 */
class GMAT_API PrinceDormand78 :
    public RungeKutta
{
public:
//    PrinceDormand78();
    PrinceDormand78(const std::string &nomme = "");
    virtual ~PrinceDormand78();

    PrinceDormand78(const PrinceDormand78&);
    PrinceDormand78 & operator=(const PrinceDormand78&);

    virtual Propagator* Clone() const;
protected:
    void                        SetCoefficients();

};

#endif // PrinceDormand78_hpp
