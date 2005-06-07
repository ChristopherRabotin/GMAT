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
//
/**
 * Implements planet related parameter classes.
 *   GHA, Longitude, Latitude, LST, BetaAngle
 */
//------------------------------------------------------------------------------

#include "PlanetParameters.hpp"


//==============================================================================
//                              GHA
//==============================================================================
/**
 * Implements Greenwich Hour Angle parameter class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// GHA(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
GHA::GHA(const std::string &name, GmatBase *obj)
   : PlanetReal(name, "GHA", obj, "Greenwich Hour Angle", "deg",
                Gmat::SPACECRAFT, GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   PlanetData::mCentralBodyName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   //loj: 5/27/05 Commented out
   //SetRefObjectName(Gmat::PARAMETER, "CurrA1MJD"); //if parameter, use type name
}


//------------------------------------------------------------------------------
// GHA(const GHA &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
GHA::GHA(const GHA &copy)
   : PlanetReal(copy)
{
}


//------------------------------------------------------------------------------
// const GHA& operator=(const GHA &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const GHA&
GHA::operator=(const GHA &right)
{
   if (this != &right)
      PlanetReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~GHA()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
GHA::~GHA()
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
bool GHA::Evaluate()
{
   mRealValue = PlanetData::GetReal("GHA");    
   
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
GmatBase* GHA::Clone(void) const
{
   return new GHA(*this);
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
   SetRefObjectName(Gmat::SPACE_POINT, "Earth"); //loj: 4/7/05 Added
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
   SetRefObjectName(Gmat::SPACE_POINT, "Earth"); //loj: 4/7/05 Added
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
   SetRefObjectName(Gmat::SPACE_POINT, "Earth"); //loj: 4/7/05 Added
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


//==============================================================================
//                              BetaAngle
//==============================================================================
/**
 * Implements BetaAngle parameter class. BetaAngle is the angle between the
 * satellites's orbital plane and the Sun.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// BetaAngle(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
BetaAngle::BetaAngle(const std::string &name, GmatBase *obj)
   : PlanetReal(name, "BetaAngle", obj, "Beta Angle", "deg",
                Gmat::SPACECRAFT, GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth"); //loj: 4/7/05 Added
   PlanetData::mCentralBodyName = "Earth";
}


//------------------------------------------------------------------------------
// BetaAngle(const BetaAngle &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BetaAngle::BetaAngle(const BetaAngle &copy)
   : PlanetReal(copy)
{
}


//------------------------------------------------------------------------------
// const BetaAngle& operator=(const BetaAngle &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const BetaAngle&
BetaAngle::operator=(const BetaAngle &right)
{
   if (this != &right)
      PlanetReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~BetaAngle()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BetaAngle::~BetaAngle()
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
bool BetaAngle::Evaluate()
{
   mRealValue = PlanetData::GetReal("BetaAngle");
   
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
GmatBase* BetaAngle::Clone(void) const
{
   return new BetaAngle(*this);
}
