//$Header$
//------------------------------------------------------------------------------
//                                  AttitudeParameters
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Daniel Hunter
// Created: 2006/6/26
//
/**
 * Implements Attitude related parameter classes.
 *    Quat1, Quat2, Quat3, Quat4
 */
//------------------------------------------------------------------------------
#ifndef ATTITUDEPARAMETERS_HPP_
#define ATTITUDEPARAMETERS_HPP_

#include "gmatdefs.hpp"
#include "AttitudeReal.hpp"


class GMAT_API Quat1 : public AttitudeReal
{
public:

   Quat1(const std::string &name = "", GmatBase *obj = NULL);
   Quat1(const Quat1 &copy);
   Quat1& operator=(const Quat1 &right);
   virtual ~Quat1();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


class GMAT_API Quat2 : public AttitudeReal
{
public:

   Quat2(const std::string &name = "", GmatBase *obj = NULL);
   Quat2(const Quat2 &copy);
   Quat2& operator=(const Quat2 &right);
   virtual ~Quat2();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};



class GMAT_API Quat3 : public AttitudeReal
{
public:

   Quat3(const std::string &name = "", GmatBase *obj = NULL);
   Quat3(const Quat3 &copy);
   Quat3& operator=(const Quat3 &right);
   virtual ~Quat3();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API Quat4 : public AttitudeReal
{
public:

   Quat4(const std::string &name = "", GmatBase *obj = NULL);
   Quat4(const Quat4 &copy);
   Quat4& operator=(const Quat4 &right);
   virtual ~Quat4();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


#endif /*ATTITUDEPARAMETERS_HPP_*/
