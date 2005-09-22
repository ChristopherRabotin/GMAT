//$Header$
//------------------------------------------------------------------------------
//                             File: PlanetParameters.cpp
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
//   2005/6/10 Linda Jun - Moved BetaAngle to AngularParamters.cpp
//
/**
 * Implements planet related parameter classes.
 *   MHA, Longitude, Latitude, LST
 */
//------------------------------------------------------------------------------

#include "PlanetParameters.hpp"


//==============================================================================
//                              MHA
//==============================================================================
/**
 * Implements Greenwich Hour Angle parameter class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// MHA(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
MHA::MHA(const std::string &name, GmatBase *obj)
   : PlanetReal(name, "MHA", obj, "Greenwich Hour Angle", "deg",
                Gmat::SPACECRAFT, GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   PlanetData::mCentralBodyName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
}


//------------------------------------------------------------------------------
// MHA(const MHA &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
MHA::MHA(const MHA &copy)
   : PlanetReal(copy)
{
}


//------------------------------------------------------------------------------
// const MHA& operator=(const MHA &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const MHA&
MHA::operator=(const MHA &right)
{
   if (this != &right)
      PlanetReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~MHA()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
MHA::~MHA()
{
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool MHA::Evaluate()
{
   mRealValue = PlanetData::GetReal("MHA");    
   
   if (mRealValue == PlanetData::PLANET_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* MHA::Clone(void) const
{
   return new MHA(*this);
}


//==============================================================================
//                              Longitude
//==============================================================================
/**
 * Implements Longitude parameter class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Longitude(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
Longitude::Longitude(const std::string &name, GmatBase *obj)
   : PlanetReal(name, "Longitude", obj, "Longitude", "deg",
                Gmat::SPACECRAFT, GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   PlanetData::mCentralBodyName = "Earth";
}


//------------------------------------------------------------------------------
// Longitude(const Longitude &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
Longitude::Longitude(const Longitude &copy)
   : PlanetReal(copy)
{
}


//------------------------------------------------------------------------------
// const Longitude& operator=(const Longitude &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const Longitude&
Longitude::operator=(const Longitude &right)
{
   if (this != &right)
      PlanetReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~Longitude()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Longitude::~Longitude()
{
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool Longitude::Evaluate()
{
   mRealValue = PlanetData::GetReal("Longitude");    
   
   if (mRealValue == PlanetData::PLANET_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* Longitude::Clone(void) const
{
   return new Longitude(*this);
}


//==============================================================================
//                              Latitude
//==============================================================================
/**
 * Implements Latitude parameter class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Latitude(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
Latitude::Latitude(const std::string &name, GmatBase *obj)
   : PlanetReal(name, "Latitude", obj, "Latitude", "deg",
                Gmat::SPACECRAFT, GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   PlanetData::mCentralBodyName = "Earth";
}


//------------------------------------------------------------------------------
// Latitude(const Latitude &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
Latitude::Latitude(const Latitude &copy)
   : PlanetReal(copy)
{
}


//------------------------------------------------------------------------------
// const Latitude& operator=(const Latitude &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const Latitude&
Latitude::operator=(const Latitude &right)
{
   if (this != &right)
      PlanetReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~Latitude()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Latitude::~Latitude()
{
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool Latitude::Evaluate()
{
   mRealValue = PlanetData::GetReal("Latitude");    
   
   if (mRealValue == PlanetData::PLANET_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* Latitude::Clone(void) const
{
   return new Latitude(*this);
}


//==============================================================================
//                              LST
//==============================================================================
/**
 * Implements LST parameter class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// LST(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
LST::LST(const std::string &name, GmatBase *obj)
   : PlanetReal(name, "LST", obj, "Local Sidereal Time", "deg",
                Gmat::SPACECRAFT, GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   PlanetData::mCentralBodyName = "Earth";
}


//------------------------------------------------------------------------------
// LST(const LST &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
LST::LST(const LST &copy)
   : PlanetReal(copy)
{
}


//------------------------------------------------------------------------------
// const LST& operator=(const LST &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const LST&
LST::operator=(const LST &right)
{
   if (this != &right)
      PlanetReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~LST()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
LST::~LST()
{
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool LST::Evaluate()
{
   mRealValue = PlanetData::GetReal("LST");    
   
   if (mRealValue == PlanetData::PLANET_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* LST::Clone(void) const
{
   return new LST(*this);
}
