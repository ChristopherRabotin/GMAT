//$Header$
//------------------------------------------------------------------------------
//                            File: EquinoctialParameters.hpp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Daniel Hunter
// Created: 2006/6/27
//
/**
 * Declares Equinoctial related parameter classes.
 *   EquiSma, EquiEy, EquiEx, EquiNy, EquiNx, EquiMlong, EquiState
 */
//------------------------------------------------------------------------------
#ifndef EQUINOCTIALPARAMETERS_HPP_
#define EQUINOCTIALPARAMETERS_HPP_

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"

class GMAT_API EquiSma : public OrbitReal
{
public:

   EquiSma(const std::string &name = "", GmatBase *obj = NULL);
   EquiSma(const EquiSma &copy);
   EquiSma& operator=(const EquiSma &right);
   virtual ~EquiSma();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquiEy : public OrbitReal
{
public:

   EquiEy(const std::string &name = "", GmatBase *obj = NULL);
   EquiEy(const EquiEy &copy);
   EquiEy& operator=(const EquiEy &right);
   virtual ~EquiEy();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquiEx : public OrbitReal
{
public:

   EquiEx(const std::string &name = "", GmatBase *obj = NULL);
   EquiEx(const EquiEx &copy);
   EquiEx& operator=(const EquiEx &right);
   virtual ~EquiEx();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquiNy : public OrbitReal
{
public:

   EquiNy(const std::string &name = "", GmatBase *obj = NULL);
   EquiNy(const EquiNy &copy);
   EquiNy& operator=(const EquiNy &right);
   virtual ~EquiNy();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquiNx : public OrbitReal
{
public:

   EquiNx(const std::string &name = "", GmatBase *obj = NULL);
   EquiNx(const EquiNx &copy);
   EquiNx& operator=(const EquiNx &right);
   virtual ~EquiNx();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquiMlong : public OrbitReal
{
public:

   EquiMlong(const std::string &name = "", GmatBase *obj = NULL);
   EquiMlong(const EquiMlong &copy);
   EquiMlong& operator=(const EquiMlong &right);
   virtual ~EquiMlong();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquiState : public OrbitRvec6
{
public:

   EquiState(const std::string &name = "", GmatBase *obj = NULL);
   EquiState(const EquiState &copy);
   EquiState& operator=(const EquiState &right);
   virtual ~EquiState();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
    
};

#endif /*EQUINOCTIALPARAMETERS_HPP_*/
