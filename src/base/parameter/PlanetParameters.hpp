//$Header$
//------------------------------------------------------------------------------
//                             File: PlanetParameters.hpp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/12/13
//
/**
 * Declares planet related parameter classes.
 *   GHA, Longitude, Latitude, LST, BetaAngle
 */
//------------------------------------------------------------------------------
#ifndef PlanetParameters_hpp
#define PlanetParameters_hpp

#include "PlanetReal.hpp"

//==============================================================================
//                                     GHA
//==============================================================================
/**
 * Declares Greenwich hour angle parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API GHA : public PlanetReal
{
public:

   GHA(const std::string &name = "", GmatBase *obj = NULL);
   GHA(const GHA &copy);
   const GHA& operator=(const GHA &right);
   virtual ~GHA();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//==============================================================================
//                                 Longitude
//==============================================================================
/**
 * Declares Longitude parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API Longitude : public PlanetReal
{
public:

   Longitude(const std::string &name = "", GmatBase *obj = NULL);
   Longitude(const Longitude &copy);
   const Longitude& operator=(const Longitude &right);
   virtual ~Longitude();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                                 Latitude
//==============================================================================
/**
 * Declares Latitude parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API Latitude : public PlanetReal
{
public:

   Latitude(const std::string &name = "", GmatBase *obj = NULL);
   Latitude(const Latitude &copy);
   const Latitude& operator=(const Latitude &right);
   virtual ~Latitude();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                                     LST
//==============================================================================
/**
 * Declares LST parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API LST : public PlanetReal
{
public:

   LST(const std::string &name = "", GmatBase *obj = NULL);
   LST(const LST &copy);
   const LST& operator=(const LST &right);
   virtual ~LST();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                                     BetaAngle
//==============================================================================
/**
 * Declares BetaAngle parameter class.. BetaAngle is the angle between the
 * satellites's orbital plane and the Sun.
 */
//------------------------------------------------------------------------------

class GMAT_API BetaAngle : public PlanetReal
{
public:

   BetaAngle(const std::string &name = "", GmatBase *obj = NULL);
   BetaAngle(const BetaAngle &copy);
   const BetaAngle& operator=(const BetaAngle &right);
   virtual ~BetaAngle();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

#endif //PlanetParameters_hpp
