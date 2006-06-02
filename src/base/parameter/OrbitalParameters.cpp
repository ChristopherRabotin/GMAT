//$Header$
//------------------------------------------------------------------------------
//                             File: OrbitalParameters.cpp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/03/25
//
/**
 * Implements other orbit related parameter classes.
 *   VelApoapsis, VelPeriapsis, Apoapsis, Periapsis, OrbitPeriod,
 *   RadApoapsis, RadPeriapais, C3Energy, Energy
 */
//------------------------------------------------------------------------------
#include "OrbitalParameters.hpp"
#include "MessageInterface.hpp"

//==============================================================================
//                              VelApoapsis
//==============================================================================
/**
 * Implements Velocity at Apoapsis class.
 */
//------------------------------------------------------------------------------

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// VelApoapsis(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
VelApoapsis::VelApoapsis(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "VelApoapsis", obj, "Velocity at Apoapsis", "Km/s",
               GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
}


//------------------------------------------------------------------------------
// VelApoapsis(const VelApoapsis &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
VelApoapsis::VelApoapsis(const VelApoapsis &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const VelApoapsis& operator=(const VelApoapsis &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const VelApoapsis&
VelApoapsis::operator=(const VelApoapsis &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~VelApoapsis()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
VelApoapsis::~VelApoapsis()
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
bool VelApoapsis::Evaluate()
{
   mRealValue = OrbitData::GetOtherKepReal(VEL_APOAPSIS);    
   
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
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
GmatBase* VelApoapsis::Clone(void) const
{
   return new VelApoapsis(*this);
}


//==============================================================================
//                              VelPeriapsis
//==============================================================================
/**
 * Implements Velocity at Periapsis class.
 */
//------------------------------------------------------------------------------

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// VelPeriapsis(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
VelPeriapsis::VelPeriapsis(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "VelPeriapsis", obj, "Velocity at Periapsis", "Km/s",
               GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
}


//------------------------------------------------------------------------------
// VelPeriapsis(const VelPeriapsis &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
VelPeriapsis::VelPeriapsis(const VelPeriapsis &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const VelPeriapsis& operator=(const VelPeriapsis &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const VelPeriapsis&
VelPeriapsis::operator=(const VelPeriapsis &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~VelPeriapsis()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
VelPeriapsis::~VelPeriapsis()
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
bool VelPeriapsis::Evaluate()
{
   //mRealValue = OrbitData::GetOtherKepReal("VelPeriapsis");    
   mRealValue = OrbitData::GetOtherKepReal(VEL_PERIAPSIS);    
    
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
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
GmatBase* VelPeriapsis::Clone(void) const
{
   return new VelPeriapsis(*this);
}


//==============================================================================
//                              Apoapsis
//==============================================================================
/**
 * Implements Apoapsis class.
 */
//------------------------------------------------------------------------------

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Apoapsis(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
Apoapsis::Apoapsis(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Apoapsis", obj, "Apoapsis", " ", GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   mIsPlottable = false; //loj: 6/9/05 Added
}


//------------------------------------------------------------------------------
// Apoapsis(const Apoapsis &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
Apoapsis::Apoapsis(const Apoapsis &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const Apoapsis& operator=(const Apoapsis &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const Apoapsis&
Apoapsis::operator=(const Apoapsis &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~Apoapsis()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Apoapsis::~Apoapsis()
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
bool Apoapsis::Evaluate()
{
   //loj: 6/8/05 call GetRelativeCartState()
   //Rvector6 cartState = OrbitData::GetCartState();
   Rvector6 cartState = OrbitData::GetRelativeCartState(mOrigin);
   
   if (cartState == Rvector6::RVECTOR6_UNDEFINED)
      return false;
   
   // compute position and velocity unit vectors
   Rvector3 pos = Rvector3(cartState[0], cartState[1], cartState[2]);
   Rvector3 vel = Rvector3(cartState[3], cartState[4], cartState[5]);
   Rvector3 R = pos.GetUnitVector();
   Rvector3 V = vel.GetUnitVector();
   
   // compute cos(90 - beta) as the dot product of the R and V vectors
   Real rdotv = R*V;
   mRealValue = rdotv;
   
   //MessageInterface::ShowMessage("Apoapsis::Evaluate() r=%f,%f,%f, v=%f,%f,%f, r.v=%f\n",
   //                              R[0], R[1], R[2], V[0], V[1], V[2], rdotv);
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
GmatBase* Apoapsis::Clone(void) const
{
   return new Apoapsis(*this);
}


//==============================================================================
//                              Periapsis
//==============================================================================
/**
* Implements Periapsis class.
*/
//------------------------------------------------------------------------------

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Periapsis(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
* Constructor.
*
* @param <name> name of the parameter
* @param <obj> reference object pointer
*/
//------------------------------------------------------------------------------
Periapsis::Periapsis(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Periapsis", obj, "Periapsis", " ", GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   mIsPlottable = false; //loj: 6/9/05 Added
}


//------------------------------------------------------------------------------
// Periapsis(const Periapsis &copy)
//------------------------------------------------------------------------------
/**
* Copy constructor.
*
* @param <copy> the parameter to make copy of
*/
//------------------------------------------------------------------------------
Periapsis::Periapsis(const Periapsis &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const Periapsis& operator=(const Periapsis &right)
//------------------------------------------------------------------------------
/**
* Assignment operator.
*
* @param <right> the parameter to make copy of
*/
//------------------------------------------------------------------------------
const Periapsis&
Periapsis::operator=(const Periapsis &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~Periapsis()
//------------------------------------------------------------------------------
/**
* Destructor.
*/
//------------------------------------------------------------------------------
Periapsis::~Periapsis()
{
   //MessageInterface::ShowMessage("==> Periapsis::~Periapsis()\n");
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
bool Periapsis::Evaluate()
{
   //loj: 6/8/05 call GetRelativeCartState()
   //Rvector6 cartState = OrbitData::GetCartState();
   Rvector6 cartState = OrbitData::GetRelativeCartState(mOrigin);
   //MessageInterface::ShowMessage
   //   ("===> Periapsis::Evaluate() mOrigin=%s, cartState=%s\n",
   //    mOrigin->GetName().c_str(), cartState.ToString().c_str());
   
   if (cartState == Rvector6::RVECTOR6_UNDEFINED)
      return false;
   
   // compute position and velocity unit vectors
   Rvector3 pos = Rvector3(cartState[0], cartState[1], cartState[2]);
   Rvector3 vel = Rvector3(cartState[3], cartState[4], cartState[5]);
   Rvector3 R = pos.GetUnitVector();
   Rvector3 V = vel.GetUnitVector();

   // compute cos(90 - beta) as the dot product of the R and V vectors
//    Real rdotv = R*V;
//    mRealValue = rdotv;
   mRealValue = R*V;

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
GmatBase* Periapsis::Clone(void) const
{
   return new Periapsis(*this);
}


//==============================================================================
//                              OrbitPeriod
//==============================================================================
/**
* Implements OrbitPeriod class.
*/
//------------------------------------------------------------------------------

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// OrbitPeriod(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
* Constructor.
*
* @param <name> name of the parameter
* @param <obj> reference object pointer
*/
//------------------------------------------------------------------------------
OrbitPeriod::OrbitPeriod(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "OrbitPeriod", obj, "Orbit Period", "s", GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
}


//------------------------------------------------------------------------------
// OrbitPeriod(const OrbitPeriod &copy)
//------------------------------------------------------------------------------
/**
* Copy constructor.
*
* @param <copy> the parameter to make copy of
*/
//------------------------------------------------------------------------------
OrbitPeriod::OrbitPeriod(const OrbitPeriod &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const OrbitPeriod& operator=(const OrbitPeriod &right)
//------------------------------------------------------------------------------
/**
* Assignment operator.
*
* @param <right> the parameter to make copy of
*/
//------------------------------------------------------------------------------
const OrbitPeriod&
OrbitPeriod::operator=(const OrbitPeriod &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~OrbitPeriod()
//------------------------------------------------------------------------------
/**
* Destructor.
*/
//------------------------------------------------------------------------------
OrbitPeriod::~OrbitPeriod()
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
bool OrbitPeriod::Evaluate()
{
   //mRealValue = OrbitData::GetOtherKepReal("OrbitPeriod");    
   mRealValue = OrbitData::GetOtherKepReal(ORBIT_PERIOD);    

   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
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
GmatBase* OrbitPeriod::Clone(void) const
{
   return new OrbitPeriod(*this);
}


//==============================================================================
//                              RadApoapsis
//==============================================================================
/**
 * Implements radius of apoapsis.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// RadApoapsis(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
RadApoapsis::RadApoapsis(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "RadApo", obj, "Radius at Apoapsis", "Km", GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
}


//------------------------------------------------------------------------------
// RadApoapsis(const RadApoapsis &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
RadApoapsis::RadApoapsis(const RadApoapsis &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const RadApoapsis& operator=(const RadApoapsis &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const RadApoapsis&
RadApoapsis::operator=(const RadApoapsis &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~RadApoapsis()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
RadApoapsis::~RadApoapsis()
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
bool RadApoapsis::Evaluate()
{
   //mRealValue = OrbitData::GetOtherKepReal("RadApoapsis");    
   mRealValue = OrbitData::GetOtherKepReal(RAD_APOAPSIS);    
   
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
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
GmatBase* RadApoapsis::Clone(void) const
{
   return new RadApoapsis(*this);
}


//==============================================================================
//                              RadPeriapsis
//==============================================================================
/**
 * Implements radius of periapsis.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// RadPeriapsis(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
RadPeriapsis::RadPeriapsis(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "RadPer", obj, "Radius at Periapsis", "Km", GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
}


//------------------------------------------------------------------------------
// RadPeriapsis(const RadPeriapsis &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
RadPeriapsis::RadPeriapsis(const RadPeriapsis &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const RadPeriapsis& operator=(const RadPeriapsis &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const RadPeriapsis&
RadPeriapsis::operator=(const RadPeriapsis &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~RadPeriapsis()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
RadPeriapsis::~RadPeriapsis()
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
bool RadPeriapsis::Evaluate()
{
   //mRealValue = OrbitData::GetOtherKepReal("RadPeriapsis");    
   mRealValue = OrbitData::GetOtherKepReal(RAD_PERIAPSIS);    
   
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
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
GmatBase* RadPeriapsis::Clone(void) const
{
   return new RadPeriapsis(*this);
}


//==============================================================================
//                              C3Energy
//==============================================================================
/**
 * Implements orbit c3 energy.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// C3Energy(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
C3Energy::C3Energy(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "C3Energy", obj, "C-3 Energy", "Km^2/s^2", GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
}


//------------------------------------------------------------------------------
// C3Energy(const C3Energy &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
C3Energy::C3Energy(const C3Energy &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const C3Energy& operator=(const C3Energy &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const C3Energy&
C3Energy::operator=(const C3Energy &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~C3Energy()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
C3Energy::~C3Energy()
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
bool C3Energy::Evaluate()
{
   //mRealValue = OrbitData::GetOtherKepReal("C3Energy");    
   mRealValue = OrbitData::GetOtherKepReal(C3_ENERGY);    
   
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
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
GmatBase* C3Energy::Clone(void) const
{
   return new C3Energy(*this);
}


//==============================================================================
//                              Energy
//==============================================================================
/**
 * Implements orbit energy.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Energy(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 */
//------------------------------------------------------------------------------
Energy::Energy(const std::string &name, GmatBase *obj)
   : OrbitReal(name, "Energy", obj, "Energy", "Km^2/s^2", GmatParam::ORIGIN)
{
   mDepObjectName = "Earth";
   SetRefObjectName(Gmat::SPACE_POINT, "Earth");
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
}


//------------------------------------------------------------------------------
// Energy(const Energy &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
Energy::Energy(const Energy &copy)
   : OrbitReal(copy)
{
}


//------------------------------------------------------------------------------
// const Energy& operator=(const Energy &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const Energy&
Energy::operator=(const Energy &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~Energy()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Energy::~Energy()
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
bool Energy::Evaluate()
{
   //mRealValue = OrbitData::GetOtherKepReal("Energy");    
   mRealValue = OrbitData::GetOtherKepReal(ENERGY);    
   
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
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
GmatBase* Energy::Clone(void) const
{
   return new Energy(*this);
}

