//$Header$
//------------------------------------------------------------------------------
//                              SpacecraftStateParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/09/22
//
/**
 * Defines Cartesian parameter operations.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "SpacecraftStateParam.hpp"
#include "Cartesian.hpp"
#include "Rvector3.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
SpacecraftStateParam::PARAMETER_TEXT[SpacecraftStateParamCount] =
{
   "PosX",
   "PosY",
   "PosZ",
   "Velx",
   "Vely",
   "Velz",
   "MagOfPosition",
   "MagOfVelocity"
}; 

const Gmat::ParameterType
SpacecraftStateParam::PARAMETER_TYPE[SpacecraftStateParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE
};


//------------------------------------------------------------------------------
// SpacecraftStateParam(const std::string &name, const std::string &desc,
//                      const Cartesian &cartesian)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <desc> description of the parameter
 * @param <cartesian>  Cartesian value of spacecraft state
 */
//------------------------------------------------------------------------------
SpacecraftStateParam::SpacecraftStateParam(const std::string &name,
                                           const std::string &desc,
                                           const Cartesian &cartesian)
   : Parameter(name, "SpacecraftStateParam", SYSTEM_PARAM, desc)
{
   // GmatBase data
   parameterCount = SpacecraftStateParamCount;
   
   // SpacecraftStateParam data
   mCartesian = cartesian;
   UpdateParameters();
}

//------------------------------------------------------------------------------
// SpacecraftStateParam(const SpacecraftStateParam &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the parameter to make copy of
 */
//------------------------------------------------------------------------------
SpacecraftStateParam::SpacecraftStateParam(const SpacecraftStateParam &param)
   : Parameter(param)
{
   // SpacecraftStateParam data
   mCartesian = param.mCartesian;
   UpdateParameters();
}

//------------------------------------------------------------------------------
// const SpacecraftStateParam& operator=(const SpacecraftStateParam &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const SpacecraftStateParam&
SpacecraftStateParam::operator=(const SpacecraftStateParam &right)
{
   if (this != &right)
   {
      Parameter::operator=(right);
      mCartesian = right.mCartesian;
      UpdateParameters();
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~SpacecraftStateParam()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
SpacecraftStateParam::~SpacecraftStateParam()
{
}

//------------------------------------------------------------------------------
// Cartesian GetValue() const
//------------------------------------------------------------------------------
/**
 * @return the Cartesian object.
 */
//------------------------------------------------------------------------------
Cartesian SpacecraftStateParam::GetValue() const
{
   return mCartesian;
}

//------------------------------------------------------------------------------
// Rvector3 GetPosition() const
//------------------------------------------------------------------------------
/**
 * @return the position part of the cartesian state.
 */
//------------------------------------------------------------------------------
Rvector3 SpacecraftStateParam::GetPosition() const
{
   return mCartesian.GetPosition();
}

//------------------------------------------------------------------------------
// Rvector3 GetVelocity() const
//------------------------------------------------------------------------------
/**
 * @return the velocity part of the cartesian state.
 */
//------------------------------------------------------------------------------
Rvector3 SpacecraftStateParam::GetVelocity() const
{
   return mCartesian.GetVelocity();
}

//------------------------------------------------------------------------------
// Real GetMagnitudeOfPosition() const
//------------------------------------------------------------------------------
/**
 * @return the magnitude of position.
 */
//------------------------------------------------------------------------------
Real SpacecraftStateParam::GetMagnitudeOfPosition() const
{
   return mMagOfPos;
}

//------------------------------------------------------------------------------
// Real GetMagnitudeOfVelocity() const
//------------------------------------------------------------------------------
/**
 * @return the magnitude of velocity.
 */
//------------------------------------------------------------------------------
Real SpacecraftStateParam::GetMagnitudeOfVelocity() const
{
   return mMagOfVel;
}

//------------------------------------------------------------------------------
// void SetValue(const SpacecraftStateParam &param)
//------------------------------------------------------------------------------
/**
 * Sets value of param to Cartesian object.
 *
 * @param <param> the parameter to set Cartesian object
 */
//------------------------------------------------------------------------------
void SpacecraftStateParam::SetValue(const SpacecraftStateParam &param)
{
   mCartesian = param.mCartesian;
   UpdateParameters();
}

//------------------------------------------------------------------------------
// void SetValue(const Cartesian &cartesian)
//------------------------------------------------------------------------------
/**
 * Sets cartesian value to Cartesian object.
 *
 * @param <cartesian> the value to set Cartesian object
 */
//------------------------------------------------------------------------------
void SpacecraftStateParam::SetValue(const Cartesian &cartesian)
{
   mCartesian = cartesian;
   UpdateParameters();
}

//------------------------------------------------------------------------------
// void SetValue(const Rvector3 &pos, const Rvector3 &vel)
//------------------------------------------------------------------------------
/**
 * Sets Cartesian object from position and velocity.
 *
 * @param <pos> the position
 * @param <vel> the velocity
 */
//------------------------------------------------------------------------------
void SpacecraftStateParam::SetValue(const Rvector3 &pos, const Rvector3 &vel)
{
   mCartesian.SetPosition(pos);
   mCartesian.SetVelocity(vel);
   UpdateParameters();
}

//------------------------------------------------------------------------------
// void SetValue(const Real &posX, const Real &posY, const Real &posZ,
//               const Real &velX, const Real &velY, const Real &velZ)
//------------------------------------------------------------------------------
/**
 * Sets Cartesian object from six elements
 *
 * @param <posX> the position X
 * @param <posY> the position Y
 * @param <posZ> the position Z
 * @param <velX> the velocity X
 * @param <velY> the velocity Y
 * @param <velZ> the velocity Z
 */
//------------------------------------------------------------------------------
void SpacecraftStateParam::SetValue(const Real &posX, const Real &posY,
                                    const Real &posZ, const Real &velX,
                                    const Real &velY, const Real &velZ)
{
   mCartesian.SetAll(posX, posY, posZ, velX, velY, velZ);
   UpdateParameters();
}

//------------------------------------------------------------------------------
// void SetPosition(const Rvector3 &pos)
//------------------------------------------------------------------------------
/**
 * Sets position of Cartesian object from pos.
 *
 * @param <pos> the position
 */
//------------------------------------------------------------------------------
void SpacecraftStateParam::SetPosition(const Rvector3 &pos)
{
   mCartesian.SetPosition(pos);
   mMagOfPos = mCartesian.GetPosition().GetMagnitude();
}

//------------------------------------------------------------------------------
// void SetVelocity (const Rvector3 &vel)
//------------------------------------------------------------------------------
/**
 * Sets velocity of Cartesian object from vel.
 *
 * @param <vel> the velocity
 */
//------------------------------------------------------------------------------
void SpacecraftStateParam::SetVelocity (const Rvector3 &vel)
{
   mCartesian.SetVelocity(vel);
   mMagOfVel = mCartesian.GetVelocity().GetMagnitude();
}

//------------------------------------
// Inherited methods from GmatBase
//------------------------------------

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType SpacecraftStateParam::GetParameterType(const Integer id) const
{
   switch (id)
   {
   case POSITION_X:
   case POSITION_Y:
   case POSITION_Z:
   case VELOCITY_X:
   case VELOCITY_Y:
   case VELOCITY_Z:
   case MAG_OF_POSITION:
   case MAG_OF_VELOCITY:
      return SpacecraftStateParam::PARAMETER_TYPE[id];
   default:
      return GmatBase::GetParameterType(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string SpacecraftStateParam::GetParameterTypeString(const Integer id) const
{
   switch (id)
   {
   case POSITION_X:
   case POSITION_Y:
   case POSITION_Z:
   case VELOCITY_X:
   case VELOCITY_Y:
   case VELOCITY_Z:
   case MAG_OF_POSITION:
   case MAG_OF_VELOCITY:
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   default:
      return GmatBase::GetParameterTypeString(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
std::string SpacecraftStateParam::GetParameterText(const Integer id)
{
   switch (id)
   {
   case POSITION_X:
   case POSITION_Y:
   case POSITION_Z:
   case VELOCITY_X:
   case VELOCITY_Y:
   case VELOCITY_Z:
   case MAG_OF_POSITION:
   case MAG_OF_VELOCITY:
      return PARAMETER_TEXT[id];
   default:
      return GmatBase::GetParameterText(id);
   }
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str)
//------------------------------------------------------------------------------
Integer SpacecraftStateParam::GetParameterID(const std::string str)
{
   for (int i=0; i<SpacecraftStateParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i])
          return i;
   }
   
   return GmatBase::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id)
//------------------------------------------------------------------------------
Real SpacecraftStateParam::GetRealParameter(const Integer id)
{
   switch (id)
   {
   case POSITION_X:
      return mCartesian.GetPosition(Cartesian::X);
   case POSITION_Y:
      return mCartesian.GetPosition(Cartesian::Y);
   case POSITION_Z:
      return mCartesian.GetPosition(Cartesian::Z);
   case VELOCITY_X:
      return mCartesian.GetVelocity(Cartesian::X);
   case VELOCITY_Y:
      return mCartesian.GetVelocity(Cartesian::Y);
   case VELOCITY_Z:
      return mCartesian.GetVelocity(Cartesian::Z);
   case MAG_OF_POSITION:
      return mMagOfPos;
   case MAG_OF_VELOCITY:
      return mMagOfVel;
   default:
      return GmatBase::GetRealParameter(id);
   }
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real SpacecraftStateParam::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case POSITION_X:
      mCartesian.SetPosition(Cartesian::X, value);
      return mCartesian.GetPosition(Cartesian::X);
   case POSITION_Y:
      mCartesian.SetPosition(Cartesian::Y, value);
      return mCartesian.GetPosition(Cartesian::Y);
   case POSITION_Z:
      mCartesian.SetPosition(Cartesian::Z, value);
      return mCartesian.GetPosition(Cartesian::Z);
   case VELOCITY_X:
      mCartesian.SetVelocity(Cartesian::X, value);
      return mCartesian.GetVelocity(Cartesian::X);
   case VELOCITY_Y:
      mCartesian.SetVelocity(Cartesian::Y, value);
      return mCartesian.GetVelocity(Cartesian::Y);
   case VELOCITY_Z:
      mCartesian.SetVelocity(Cartesian::Z, value);
      return mCartesian.GetVelocity(Cartesian::Z);
   case MAG_OF_POSITION:
      mMagOfPos = value;
      return mMagOfPos;
   case MAG_OF_VELOCITY:
      mMagOfVel = value;
      return mMagOfVel;
   default:
      return GmatBase::SetRealParameter(id, value);
   }
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void UpdateParameters()
//------------------------------------------------------------------------------
void SpacecraftStateParam::UpdateParameters()
{
   mMagOfPos = mCartesian.GetPosition().GetMagnitude();
   mMagOfVel = mCartesian.GetVelocity().GetMagnitude();
}

//------------------------------------
// Inherited methods from Parameter
//------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetParameterList() const
//------------------------------------------------------------------------------
const std::string* SpacecraftStateParam::GetParameterList() const
{
   return PARAMETER_TEXT;
}
