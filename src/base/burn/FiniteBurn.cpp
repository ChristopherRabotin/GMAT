//$Header$
//------------------------------------------------------------------------------
//                              FiniteBurn
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/12/20
//
/**
 * Implements the FiniteBurn class used for maneuvers. 
 */
//------------------------------------------------------------------------------


#include "FiniteBurn.hpp"


//---------------------------------
// static data
//---------------------------------

/// Labels used for the finite burn parameters.
const std::string
FiniteBurn::PARAMETER_TEXT[FiniteBurnParamCount - BurnParamCount] =
{
   "Thrusters",
   "BurnScaleFactor"
};

/// Types of the parameters used by finite burns.
const Gmat::ParameterType
FiniteBurn::PARAMETER_TYPE[FiniteBurnParamCount - BurnParamCount] =
{
   Gmat::STRINGARRAY_TYPE,
   Gmat::REAL_TYPE,
};


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
//  FiniteBurn(std::string nomme)
//------------------------------------------------------------------------------
/**
 * FiniteBurn constructor (default constructor).
 * 
 * @param nomme Name of the constructed object.
 */
//------------------------------------------------------------------------------
FiniteBurn::FiniteBurn(std::string nomme) :
   Burn              ("FiniteBurn", nomme),
   burnScaleFactor   (1.0)
{
   parameterCount = FiniteBurnParamCount;
}


//------------------------------------------------------------------------------
//  ~FiniteBurn()
//------------------------------------------------------------------------------
/**
 * FiniteBurn destructor.
 */
//------------------------------------------------------------------------------
FiniteBurn::~FiniteBurn()
{
}


//------------------------------------------------------------------------------
//  FiniteBurn(const FiniteBurn& fb)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param fb The FiniteBurn that is copied.
 */
//------------------------------------------------------------------------------
FiniteBurn::FiniteBurn(const FiniteBurn& fb) :
   Burn              (fb),
   thrusters         (fb.thrusters),
   burnScaleFactor   (fb.burnScaleFactor)
{
   parameterCount = fb.parameterCount;
}


//------------------------------------------------------------------------------
//  FiniteBurn& operator=(const FiniteBurn& fb)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param fb The FiniteBurn that is copied.
 * 
 * @return this instance, with parameters set to match fb.
 */
//------------------------------------------------------------------------------
FiniteBurn& FiniteBurn::operator=(const FiniteBurn& fb)
{
   if (&fb == this)
      return *this;
      
   Burn::operator=(fb);
   thrusters = fb.thrusters;
   burnScaleFactor = fb.burnScaleFactor;
      
   return *this;
}


//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the name of the parameter with the input id.
 * 
 * @param id Integer id for the parameter.
 * 
 * @return The string name of the parameter.
 */
//------------------------------------------------------------------------------
std::string FiniteBurn::GetParameterText(const Integer id) const
{
   if (id >= BurnParamCount && id < FiniteBurnParamCount)
      return PARAMETER_TEXT[id - BurnParamCount];
      
   return Burn::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Gets the id corresponding to a named parameter.
 * 
 * @param str Name of the parameter.
 * 
 * @return The ID.
 */
//------------------------------------------------------------------------------
Integer FiniteBurn::GetParameterID(const std::string &str) const
{
   for (Integer i = BurnParamCount; i < FiniteBurnParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - BurnParamCount])
         return i;
   }

   return Burn::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the type of a parameter.
 * 
 * @param id Integer ID of the parameter.
 * 
 * @return The type of the parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType FiniteBurn::GetParameterType(const Integer id) const
{
   if (id >= BurnParamCount && id < FiniteBurnParamCount)
      return PARAMETER_TYPE[id - BurnParamCount];
      
   return Burn::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the text description for the type of a parameter.
 * 
 * @param id Integer ID of the parameter.
 * 
 * @return The text description of the type of the parameter.
 */
//------------------------------------------------------------------------------
std::string FiniteBurn::GetParameterTypeString(const Integer id) const
{
   return Burn::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  Real SetStringParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param id Integer ID of the parameter.
 * @param value New value for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
bool FiniteBurn::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == THRUSTER) {
      if (find(thrusters.begin(), thrusters.end(), value) == thrusters.end())
         thrusters.push_back(value);
      return true;
   }
         
   return Burn::SetStringParameter(id, value);
}


//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 * 
 * For the Burn classes, calls to this method get passed to the maneuver frame
 * manager when the user requests the frames that are available for the system.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a 
 *         StringArray.
 */
//---------------------------------------------------------------------------
const StringArray& FiniteBurn::GetStringArrayParameter(const Integer id) const
{
   if (id == THRUSTER)
      return thrusters;

   return Burn::GetStringArrayParameter(id);
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access the Real data associated with this burn.
 * 
 * @param id The integer ID for the parameter.
 *
 * @return The requested Real.
 */
//---------------------------------------------------------------------------
Real FiniteBurn::GetRealParameter(const Integer id) const
{
   if (id == BURN_SCALE_FACTOR)
      return burnScaleFactor;

   return Burn::GetRealParameter(id);
}


//---------------------------------------------------------------------------
//  Real setRealParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Change the Real data associated with this burn.
 * 
 * @param id Integer ID of the parameter.
 * @param value New value for the parameter.
 *
 * @return The value of the parameter at the end of the call.
 */
//---------------------------------------------------------------------------
Real FiniteBurn::SetRealParameter(const Integer id, const Real value)
{
   if (id == BURN_SCALE_FACTOR) {
      if (value > 0.0)
         burnScaleFactor = value;
      return burnScaleFactor;
   }

   return Burn::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
//  bool Fire(Real *burnData)
//------------------------------------------------------------------------------
/**
 * Fire does not currently perform any action for FiniteBurn objects.  The 
 * BeginManeuver/EndManeuver commands replace the actions that fire would 
 * perform.
 * 
 * @param burnData Not currently used for finite burns.
 *
 * @return false, indicating that Fire() doesn't do anything.
 */
//------------------------------------------------------------------------------
bool FiniteBurn::Fire(Real *burnData)
{
   return false;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the ImpulsiveBurn.
 *
 * @return clone of the ImpulsiveBurn.
 */
//------------------------------------------------------------------------------
GmatBase* FiniteBurn::Clone(void) const
{
   return (new FiniteBurn(*this));
}

