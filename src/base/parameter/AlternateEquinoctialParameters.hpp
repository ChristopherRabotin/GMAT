//$Id: AlternateEquinoctialParameters.hpp 2013-08-26 00:07:00 CBNU Hwayeong KIM $
//------------------------------------------------------------------------------
//                            File: AlternateEquinoctialParameters.hpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Hwayeong Kim
// Created: 2013/08/26
//
/**
 * Declares Equinoctial related parameter classes.
 *   AltEquinP, AltEquinQ, AltEquinState
 */
//------------------------------------------------------------------------------
#ifndef ALTERNATEEQUINOCTIALPARAMETERS_HPP_
#define ALTERNATEEQUINOCTIALPARAMETERS_HPP_

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"

class GMAT_API AltEquinP : public OrbitReal
{
public:

   AltEquinP(const std::string &name = "", GmatBase *obj = NULL);
   AltEquinP(const AltEquinP &copy);
   AltEquinP& operator=(const AltEquinP &right);
   virtual ~AltEquinP();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API AltEquinQ : public OrbitReal
{
public:

   AltEquinQ(const std::string &name = "", GmatBase *obj = NULL);
   AltEquinQ(const AltEquinQ &copy);
   AltEquinQ& operator=(const AltEquinQ &right);
   virtual ~AltEquinQ();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API AltEquinState : public OrbitRvec6
{
public:

   AltEquinState(const std::string &name = "", GmatBase *obj = NULL);
   AltEquinState(const AltEquinState &copy);
   AltEquinState& operator=(const AltEquinState &right);
   virtual ~AltEquinState();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
    
};

#endif /*ALTERNATEEQUINOCTIALPARAMETERS_HPP_*/
