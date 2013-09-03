//$Id: DelaunaylParameters.hpp 2013-05-24 Minhee Kim $
//------------------------------------------------------------------------------
//                            File: DelaunayParameters.hpp
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
// Author: Minhee Kim 
// Created: 2013/5/24
//
/**
 * Declares Delaunay related parameter classes.
 *   Delaunayl, Delaunayg, Delaunayh, DelaunayL, DelaunayG, DelaunayH, DelaState
 */
//------------------------------------------------------------------------------
#ifndef DELAUNAYPARAMETERS_HPP_
#define DELAUNAYPARAMETERS_HPP_

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"

class GMAT_API Delal : public OrbitReal
{
public:

   Delal(const std::string &name = "", GmatBase *obj = NULL);
   Delal(const Delal &copy);
   Delal& operator=(const Delal &right);
   virtual ~Delal();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};


class GMAT_API Delag : public OrbitReal
{
public:

   Delag(const std::string &name = "", GmatBase *obj = NULL);
   Delag(const Delag &copy);
   Delag& operator=(const Delag &right);
   virtual ~Delag();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API Delah : public OrbitReal
{
public:

   Delah(const std::string &name = "", GmatBase *obj = NULL);
   Delah(const Delah &copy);
   Delah& operator=(const Delah &right);
   virtual ~Delah();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API DelaL : public OrbitReal
{
public:

   DelaL(const std::string &name = "", GmatBase *obj = NULL);
   DelaL(const DelaL &copy);
   DelaL& operator=(const DelaL &right);
   virtual ~DelaL();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API DelaG : public OrbitReal
{
public:

   DelaG(const std::string &name = "", GmatBase *obj = NULL);
   DelaG(const DelaG &copy);
   DelaG& operator=(const DelaG &right);
   virtual ~DelaG();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API DelaH : public OrbitReal
{
public:

   DelaH(const std::string &name = "", GmatBase *obj = NULL);
   DelaH(const DelaH &copy);
   DelaH& operator=(const DelaH &right);
   virtual ~DelaH();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API DelaState : public OrbitRvec6
{
public:

   DelaState(const std::string &name = "", GmatBase *obj = NULL);
   DelaState(const DelaState &copy);
   DelaState& operator=(const DelaState &right);
   virtual ~DelaState();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
    
};

#endif /*DELAUNAYPARAMETERS_HPP_*/

