//$Id$
//------------------------------------------------------------------------------
//                            File: EquinoctialParameters.hpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Daniel Hunter
// Created: 2006/6/27
//
/**
 * Declares Equinoctial related parameter classes.
 *   EquinSma, EquinEy, EquinEx, EquinNy, EquinNx, EquinMlong, EquinState
 */
//------------------------------------------------------------------------------
#ifndef EQUINOCTIALPARAMETERS_HPP_
#define EQUINOCTIALPARAMETERS_HPP_

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"

class GMAT_API EquinSma : public OrbitReal
{
public:

   EquinSma(const std::string &name = "", GmatBase *obj = NULL);
   EquinSma(const EquinSma &copy);
   EquinSma& operator=(const EquinSma &right);
   virtual ~EquinSma();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquinEy : public OrbitReal
{
public:

   EquinEy(const std::string &name = "", GmatBase *obj = NULL);
   EquinEy(const EquinEy &copy);
   EquinEy& operator=(const EquinEy &right);
   virtual ~EquinEy();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquinEx : public OrbitReal
{
public:

   EquinEx(const std::string &name = "", GmatBase *obj = NULL);
   EquinEx(const EquinEx &copy);
   EquinEx& operator=(const EquinEx &right);
   virtual ~EquinEx();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquinNy : public OrbitReal
{
public:

   EquinNy(const std::string &name = "", GmatBase *obj = NULL);
   EquinNy(const EquinNy &copy);
   EquinNy& operator=(const EquinNy &right);
   virtual ~EquinNy();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquinNx : public OrbitReal
{
public:

   EquinNx(const std::string &name = "", GmatBase *obj = NULL);
   EquinNx(const EquinNx &copy);
   EquinNx& operator=(const EquinNx &right);
   virtual ~EquinNx();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquinMlong : public OrbitReal
{
public:

   EquinMlong(const std::string &name = "", GmatBase *obj = NULL);
   EquinMlong(const EquinMlong &copy);
   EquinMlong& operator=(const EquinMlong &right);
   virtual ~EquinMlong();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquinState : public OrbitRvec6
{
public:

   EquinState(const std::string &name = "", GmatBase *obj = NULL);
   EquinState(const EquinState &copy);
   EquinState& operator=(const EquinState &right);
   virtual ~EquinState();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
    
};

#endif /*EQUINOCTIALPARAMETERS_HPP_*/
