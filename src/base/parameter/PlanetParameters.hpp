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
// Modified:
//   2005/6/10 Linda Jun - Moved BetaAngle to AngularParamters.hpp
//
/**
 * Declares planet related parameter classes.
 *   MHA, Longitude, Altitude(Geodetic), Latitude(Geodetic), LST
 */
//------------------------------------------------------------------------------
#ifndef PlanetParameters_hpp
#define PlanetParameters_hpp

#include "PlanetReal.hpp"


//==============================================================================
//                                     MHA
//==============================================================================
/**
 * Declares Greenwich hour angle parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API MHA : public PlanetReal
{
public:

   MHA(const std::string &name = "", GmatBase *obj = NULL);
   MHA(const MHA &copy);
   const MHA& operator=(const MHA &right);
   virtual ~MHA();

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
//                              Altitude
//==============================================================================
/**
 * Declares Altitude class.
 */
//------------------------------------------------------------------------------

class GMAT_API Altitude : public PlanetReal
{
public:
   
   Altitude(const std::string &name = "", GmatBase *obj = NULL);
   Altitude(const Altitude &copy);
   const Altitude& operator=(const Altitude &right);
   virtual ~Altitude();
   
   // The inherited methods from Parameter
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

#endif //PlanetParameters_hpp
