//$Header$
//------------------------------------------------------------------------------
//                              SpaceObject
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CI63P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/7/24
//
/**
 * Implements the base class used for spacecraft and formations. 
 */
//------------------------------------------------------------------------------


#include "SpaceObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_J2000_STATE 1


//---------------------------------
// static data
//---------------------------------

const std::string SpaceObject::PARAMETER_TEXT[SpaceObjectParamCount -
                                              SpacePointParamCount] =
   {
      "Epoch"
   };


const Gmat::ParameterType SpaceObject::PARAMETER_TYPE[SpaceObjectParamCount - 
                                                      SpacePointParamCount] =
   {
      Gmat::REAL_TYPE
   };


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// SpaceObject(Gmat::ObjectType typeId, const std::string &typeStr,
//             const std::string &instName)
//------------------------------------------------------------------------------
/**
 * Default constructor.
 *
 * @param <typeId>   Gmat::ObjectType of the constructed object.
 * @param <typeStr>  String describing the type of object created.
 * @param <instName> Name of the constructed instance.
 */
//------------------------------------------------------------------------------
SpaceObject::SpaceObject(Gmat::ObjectType typeId, const std::string &typeStr,
                         const std::string &instName) :
   SpacePoint       (typeId, typeStr, instName),
   isManeuvering    (false),
   originName       ("Earth"),
   origin           (NULL)
{
   objectTypes.push_back(Gmat::SPACEOBJECT);
   objectTypeNames.push_back("SpaceObject");
}


//------------------------------------------------------------------------------
// ~SpaceObject()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
SpaceObject::~SpaceObject()
{
}


//------------------------------------------------------------------------------
// SpaceObject(const SpaceObject& so)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <so> SpaceObject that is copied onto this one.
 */
//------------------------------------------------------------------------------
SpaceObject::SpaceObject(const SpaceObject& so) :
   SpacePoint     (so),
   state          (so.state),
   isManeuvering  (so.isManeuvering),
   originName     (so.originName),
   origin         (so.origin)
{
}


//------------------------------------------------------------------------------
// SpaceObject& operator=(const SpaceObject& so)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <so> SpaceObject that is copied onto this one.
 *
 * @return this instance, configured like the input instance.
 */
//------------------------------------------------------------------------------
SpaceObject& SpaceObject::operator=(const SpaceObject& so)
{
   if (this == &so)
      return *this;
      
   SpacePoint::operator=(so);
   state         = so.state;
   isManeuvering = so.isManeuvering;
   originName    = so.originName;
   origin        = so.origin;

   return *this;
}


//------------------------------------------------------------------------------
// PropState& GetState()
//------------------------------------------------------------------------------
/**
 * Accessor for the PropState of the object.
 *
 * @return The embedded PropState.
 */
//------------------------------------------------------------------------------
PropState& SpaceObject::GetState()
{
   return state;
}


//------------------------------------------------------------------------------
// Real GetEpoch()
//------------------------------------------------------------------------------
/**
 * Accessor for the current epoch of the object, in A.1 Modified Julian format.
 *
 * @return The A.1 epoch.
 *
 * @todo The epoch probably should be TAI throughout GMAT.
 */
//------------------------------------------------------------------------------
Real SpaceObject::GetEpoch()
{
   return state.GetEpoch();
}


//------------------------------------------------------------------------------
// Real SetEpoch(const Real ep)
//------------------------------------------------------------------------------
/**
 * Accessor used to set epoch (in A.1 Modified Julian format) of the object.
 *
 * @param <ep> The new A.1 epoch.
 *
 * @return The updated A.1 epoch.
 *
 * @todo The epoch probably should be TAI throughout GMAT.
 */
//------------------------------------------------------------------------------
Real SpaceObject::SetEpoch(const Real ep)
{
   return state.SetEpoch(ep);
}


//------------------------------------------------------------------------------
// bool IsManeuvering()
//------------------------------------------------------------------------------
/**
 * Function that checks to see if a finite burn needs to be applied to this 
 * object.
 *
 * @return true if a finite burn is active, false otherwise.
 */
//------------------------------------------------------------------------------
bool SpaceObject::IsManeuvering()
{
   return isManeuvering;
}


//------------------------------------------------------------------------------
// void IsManeuvering(bool mnvrFlag)
//------------------------------------------------------------------------------
/**
 * Function that sets or clears the maneuvering flag.
 * 
 * Derived classes may override this method so that the flag is updated based on
 * the state of the hardware attached to the SpaceObjects.
 * 
 * @param <mnvrFlag> The new value for the flag.
 */
//------------------------------------------------------------------------------
void SpaceObject::IsManeuvering(bool mnvrFlag)
{
   isManeuvering = mnvrFlag;
}


/// @todo Waiting for CoordinateSystems in Spacecraft, then see if needed
void SpaceObject::SetOriginName(std::string cbName)
{
   originName = cbName;
}

const std::string SpaceObject::GetOriginName()
{
   return originName;
}


void SpaceObject::SetOrigin(SpacePoint *cb)
{
   origin = cb;
}



//------------------------------------------------------------------------------
// const Rvector6 GetMJ2000State(const A1Mjd &atTime)
//------------------------------------------------------------------------------
/**
 * Access the MJ2000 state for this SpaceObject.
 *
 * @param <atTime> Epoch for the state data.
 *
 * @return The Cartesian MJ2000 state.
 *
 * @todo Determine if this calculation should ber moved into the derived
 *       classes.
 */
//------------------------------------------------------------------------------
const Rvector6 SpaceObject::GetMJ2000State(const A1Mjd &atTime)
{
   #ifdef DEBUG_J2000_STATE
      MessageInterface::ShowMessage(
         "SpaceObject::GetMJ2000State entered; epoch is %lf\n", atTime.Get());
   #endif
   if (j2000Body == NULL)
      throw SpaceObjectException("MJ2000 body not yet set for " +
         instanceName);
         
   PropState ps = GetState();
   
   Real *st = ps.GetState();

   #ifdef DEBUG_J2000_STATE
      MessageInterface::ShowMessage("   Accessing J2000 body state for %s\n",
         j2000Body->GetName().c_str());
   #endif
   Rvector6 bodyState = j2000Body->GetMJ2000State(atTime);

   #ifdef DEBUG_J2000_STATE
      MessageInterface::ShowMessage("   MJ2000: [%lf %lf %lf %lf %lf %lf]\n",
         bodyState[0], bodyState[1], bodyState[2], bodyState[3], bodyState[4], 
         bodyState[5]);
   #endif

   // If origin is NULL, assume it is set at the J2000 origin.
   if (origin)
   {
      #ifdef DEBUG_J2000_STATE
         MessageInterface::ShowMessage("   Accessing origin state for %s\n",
            origin->GetName().c_str());
      #endif
      
      Rvector6 offset = origin->GetMJ2000State(atTime);
      
      #ifdef DEBUG_J2000_STATE
         MessageInterface::ShowMessage("   origin: [%lf %lf %lf %lf %lf %lf]\n",
            offset[0], offset[1], offset[2], offset[3], offset[4], offset[5]);
      #endif
      
      bodyState -= offset;
      
      #ifdef DEBUG_J2000_STATE
         MessageInterface::ShowMessage("   Diff: [%lf %lf %lf %lf %lf %lf]\n",
            bodyState[0], bodyState[1], bodyState[2], bodyState[3], bodyState[4], 
            bodyState[5]);
      #endif
   }
   
   Rvector6 j2kState;
   
   j2kState[0] = st[0] - bodyState[0];
   j2kState[1] = st[1] - bodyState[1];
   j2kState[2] = st[2] - bodyState[2];

   j2kState[3] = st[3] - bodyState[3];
   j2kState[4] = st[4] - bodyState[4];
   j2kState[5] = st[5] - bodyState[5];

   return j2kState;
}


//------------------------------------------------------------------------------
// const Rvector3 GetMJ2000Position(const A1Mjd &atTime)
//------------------------------------------------------------------------------
/**
 * Access the MJ2000 position for this SpaceObject.
 *
 * @param <atTime> Epoch for the state data.
 *
 * @return The Cartesian MJ2000 position.
 *
 * @todo Implement GetMJ2000Position in the derived classes, and remove this
 *       implementation.
 */
//------------------------------------------------------------------------------
const Rvector3 SpaceObject::GetMJ2000Position(const A1Mjd &atTime)
{
   const Rvector6 rv6 = GetMJ2000State(atTime);
   return (rv6.GetR()); 
}


//------------------------------------------------------------------------------
// const Rvector3 GetMJ2000Velocity(const A1Mjd &atTime)
//------------------------------------------------------------------------------
/**
 * Access the MJ2000 velocity for this SpaceObject.
 *
 * @param <atTime> Epoch for the state data.
 *
 * @return The Cartesian MJ2000 velocity.
 *
 * @todo Implement GetMJ2000Velocity in the derived classes, and remove this
 *       implementation.
 */
//------------------------------------------------------------------------------
const Rvector3 SpaceObject::GetMJ2000Velocity(const A1Mjd &atTime)
{
   const Rvector6 rv6 = GetMJ2000State(atTime);
   return (rv6.GetV());
}


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer SpaceObject::GetParameterID(const std::string &str) const
{
   for (Integer i = SpacePointParamCount; i < SpaceObjectParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SpacePointParamCount])
         return i;
   }
   
   return SpacePoint::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string SpaceObject::GetParameterText(const Integer id) const
{
   if (id >= SpacePointParamCount && id < SpaceObjectParamCount)
      return PARAMETER_TEXT[id - SpacePointParamCount];
   return SpacePoint::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType SpaceObject::GetParameterType(const Integer id) const
{
   if (id >= SpacePointParamCount && id < SpaceObjectParamCount)
      return PARAMETER_TYPE[id - SpacePointParamCount];
   
   return SpacePoint::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string SpaceObject::GetParameterTypeString(const Integer id) const
{
   return SpacePoint::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter value.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real SpaceObject::GetRealParameter(const Integer id) const
{
   if (id == EPOCH_PARAM)
      return state.GetEpoch();
   return SpacePoint::GetRealParameter(id);
}


//------------------------------------------------------------------------------
//  Real  GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param <label> String description for the requested parameter value.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real SpaceObject::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param <id>    ID for the parameter whose value to change.
 * @param <value> value for the parameter.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real SpaceObject::SetRealParameter(const Integer id, const Real value)
{
   if (id == EPOCH_PARAM)
      return state.SetEpoch(value);
   return SpacePoint::GetRealParameter(id);
}


//------------------------------------------------------------------------------
//  Real  SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param <label> String description for the parameter value.
 * @param <value> value for the parameter.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real SpaceObject::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}
