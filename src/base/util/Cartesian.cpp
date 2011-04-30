//$Id$
//------------------------------------------------------------------------------
//                               Cartesian
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
// Author: Rodger Abel
// Created: 1995/07/18 for GSS project
// Modified: 2003/09/16 Linda Jun - See Cartesian.hpp
//
/**
 * Implements Cartesian class.
 */
//------------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include "gmatdefs.hpp"
//#include "EphemerisUtil.hpp"
#include "Keplerian.hpp"
#include "Cartesian.hpp"
#include "Rvector3.hpp"
#include "Rvector.hpp"

//---------------------------------
//  static data
//---------------------------------
const std::string Cartesian::DATA_DESCRIPTIONS[NUM_DATA] =
{
   "Position X",
   "Position Y",
   "Position Z",
   "Velocity X",
   "Velocity Y",
   "Velocity Z"
};

const Cartesian Cartesian::ZeroCartesian = Cartesian(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

//---------------------------------
//  public
//---------------------------------

//------------------------------------------------------------------------------
//  Cartesian::Cartesian() 
//------------------------------------------------------------------------------
Cartesian::Cartesian() 
{
}

//------------------------------------------------------------------------------
//  Cartesian::Cartesian(Real posX, Real posY, Real posZ,
//                       Real velX, Real velY, Real velZ)
//------------------------------------------------------------------------------
Cartesian::Cartesian(Real posX, Real posY, Real posZ,
                     Real velX, Real velY, Real velZ)
{
   positionD = Rvector3(posX, posY, posZ);
   velocityD = Rvector3(velX, velY, velZ);
}

//loj: 4/19/04 added
//------------------------------------------------------------------------------
//  Cartesian::Cartesian(const Rvector6 &state)
//------------------------------------------------------------------------------
Cartesian::Cartesian(const Rvector6 &state)
{
   positionD = Rvector3(state[0], state[1], state[2]);
   velocityD = Rvector3(state[3], state[4], state[5]);
}

//------------------------------------------------------------------------------
//  Cartesian::Cartesian(const Rvector3 &p, const Rvector3 &v)
//------------------------------------------------------------------------------
Cartesian::Cartesian(const Rvector3 &p, const Rvector3 &v)
   : positionD(p), velocityD(v)
{
}

//------------------------------------------------------------------------------
//  Cartesian::Cartesian(const Cartesian &cartesian)
//------------------------------------------------------------------------------
Cartesian::Cartesian(const Cartesian &cartesian)
  : positionD(cartesian.positionD), velocityD(cartesian.velocityD)
{
}

//------------------------------------------------------------------------------
//  Cartesian& Cartesian::operator=(const Cartesian &cartesian)
//------------------------------------------------------------------------------
Cartesian& Cartesian::operator=(const Cartesian &cartesian)
{
   if (this != &cartesian)
   {
      positionD = cartesian.positionD;
      velocityD = cartesian.velocityD;
   }
   return *this;
}

//------------------------------------------------------------------------------
//  Cartesian::~Cartesian() 
//------------------------------------------------------------------------------
Cartesian::~Cartesian() 
{
}

//------------------------------------------------------------------------------
//  Rvector3 Cartesian::GetPosition() const
//------------------------------------------------------------------------------
Rvector3 Cartesian::GetPosition() const
{
    return positionD;
}

//------------------------------------------------------------------------------
//  Rvector3 Cartesian::GetVelocity() const
//------------------------------------------------------------------------------
Rvector3 Cartesian::GetVelocity() const
{
    return velocityD;
}

//------------------------------------------------------------------------------
//  Real Cartesian::GetPosition(Integer id) const
//------------------------------------------------------------------------------
Real Cartesian::GetPosition(Integer id) const
{
    return positionD[id];
}

//------------------------------------------------------------------------------
//  Real Cartesian::GetVelocity(Integer id) const
//------------------------------------------------------------------------------
Real Cartesian::GetVelocity(Integer id) const
{
    return velocityD[id];
}

//------------------------------------------------------------------------------
//  void Cartesian::SetPosition(const Rvector3 &pos)
//------------------------------------------------------------------------------
void Cartesian::SetPosition(const Rvector3 &pos)
{
    positionD = pos;
}

//------------------------------------------------------------------------------
//  void Cartesian::SetVelocity(const Rvector3 &vel)
//------------------------------------------------------------------------------
void Cartesian::SetVelocity(const Rvector3 &vel)
{
    velocityD = vel;
}

//------------------------------------------------------------------------------
//  void Cartesian::SetPosition(Integer id, const Real &val)
//------------------------------------------------------------------------------
void Cartesian::SetPosition(Integer id, const Real &val)
{
    positionD[id] = val;
}

//------------------------------------------------------------------------------
//  void Cartesian::SetVelocity(Integer id, const Real &val)
//------------------------------------------------------------------------------
void Cartesian::SetVelocity(Integer id, const Real &val)
{
    velocityD[id] = val;
}

//------------------------------------------------------------------------------
// void Cartesian::SetAll(const Real &posX, const Real &posY, const Real &posZ,
//                        const Real &velX, const Real &velY, const Real &velZ)
//------------------------------------------------------------------------------
void Cartesian::SetAll(const Real &posX, const Real &posY, const Real &posZ,
                       const Real &velX, const Real &velY, const Real &velZ)
{
   positionD.Set(posX, posY, posZ);
   velocityD.Set(velX, velY, velZ);
}

// //------------------------------------------------------------------------------
// // Cartesian ToCartesian(const Keplerian &k,  const Rvector3 &pVector,
// //                       const Rvector3 &qVector, Real mu)
// //------------------------------------------------------------------------------
// Cartesian ToCartesian(const Keplerian &k,  const Rvector3 &pVector,
//                       const Rvector3 &qVector, Real mu)
// {
//     Cartesian c;

//     ToCartesian(k.GetSemimajorAxis(), k.GetEccentricity(),
//                                k.GetInclination(), k.GetRAAscendingNode(),
//                                k.GetArgumentOfPeriapsis(), k.GetMeanAnomaly(),
//                                mu, pVector, qVector, c.positionD, c.velocityD);

//     return c;
// }

// //------------------------------------------------------------------------------
// //  Cartesian ToCartesian(const Keplerian &k, Real mu)
// //------------------------------------------------------------------------------
// Cartesian ToCartesian(const Keplerian &k, Real mu)
// {
//     Cartesian c;

//     ToCartesian(k.GetSemimajorAxis(), k.GetEccentricity(), k.GetInclination(),
//                 k.GetRAAscendingNode(), k.GetArgumentOfPeriapsis(), k.GetMeanAnomaly(),
//                 mu, c.positionD, c.velocityD);

//     return c;
// }

//------------------------------------------------------------------------------
//  std::ostream& operator<<(std::ostream& output, Cartesian& c)
//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& output, Cartesian& c)
{
    Rvector v(6, c.positionD[0], c.positionD[1], c.positionD[2],
              c.velocityD[0], c.velocityD[1], c.velocityD[2]);

    output << v;
    return output;
}

//------------------------------------------------------------------------------
//  <friend>
//  std::istream& operator>>(std::istream& input, Cartesian& c)
//------------------------------------------------------------------------------
std::istream& operator>>(std::istream& input, Cartesian& c)
{
   input >> c.positionD >> c.velocityD;
   return input;
}

//------------------------------------------------------------------------------
// Integer GetNumData() const
//------------------------------------------------------------------------------
Integer Cartesian::GetNumData() const
{
   return NUM_DATA;
}

//------------------------------------------------------------------------------
// const std::string* GetDataDescriptions() const
//------------------------------------------------------------------------------
const std::string* Cartesian::GetDataDescriptions() const
{
   return DATA_DESCRIPTIONS;
}

//------------------------------------------------------------------------------
//  std::string* ToValueStrings(void)
//------------------------------------------------------------------------------
std::string* Cartesian::ToValueStrings(void)
{
   std::stringstream ss("");

   ss << positionD[0];
   stringValues[0] = ss.str();
   
   ss.str("");
   ss << positionD[1];
   stringValues[1] = ss.str();
   
   ss.str("");
   ss << positionD[2];
   stringValues[2] = ss.str();
   
   ss.str("");
   ss << velocityD[0];
   stringValues[3] = ss.str();
   
   ss.str("");
   ss << velocityD[1];
   stringValues[4] = ss.str();
   
   ss.str("");
   ss << velocityD[2];
   stringValues[5] = ss.str();
   
   return stringValues;
}

