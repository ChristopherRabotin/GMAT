//
//------------------------------------------------------------------------------
//                                PrinceDormand853
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// *** File Name : PrinceDormand853.hpp
// *** Created   : July 1, 2013
// **************************************************************************
// ***  Developed By  :  Hwayeong Kim, CBNU                               ***
// **************************************************************************

#ifndef PrinceDormand853_H
#define PrinceDormand853_H

#include "ProductionPropagatorDefs.hpp"
#include "gmatdefs.hpp"
#include "RungeKutta.hpp"

/**
 * This class implements a Runge-Kutta integrator using the coefficients derived 
 * by Prince and Dormand.  This particular set of coefficients implements the
 * eighth order integrator with seventh order error control.  
 */
class PRODUCTIONPROPAGATOR_API PrinceDormand853 :
    public RungeKutta
{
public:
//    PrinceDormand853();
    PrinceDormand853(const std::string &nomme = "");
    virtual ~PrinceDormand853();

    PrinceDormand853(const PrinceDormand853&);
    PrinceDormand853 & operator=(const PrinceDormand853&);

    virtual Propagator* Clone() const;
protected:
    void                        SetCoefficients();

};

#endif // PrinceDormand853_hpp
