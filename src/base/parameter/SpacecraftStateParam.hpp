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
// Created: 2003/09/10
//
/**
 * Declares Cartesian parameter operations.
 */
//------------------------------------------------------------------------------
#ifndef SpacecraftStateParam_hpp
#define SpacecraftStateParam_hpp

#include "gmatdefs.hpp"
#include "Cartesian.hpp"
#include "Parameter.hpp"
#include "Rvector3.hpp"

class GMAT_API SpacecraftStateParam : public Parameter
{
public:

   SpacecraftStateParam(const std::string &name, const std::string &desc,
                        const Cartesian &cartesian);
   SpacecraftStateParam(const SpacecraftStateParam &param);
   const SpacecraftStateParam& operator=(const SpacecraftStateParam &right);
   virtual ~SpacecraftStateParam();

   Cartesian GetValue() const;
   Rvector3 GetPosition() const;
   Rvector3 GetVelocity() const;
   Real GetMagnitudeOfPosition() const;
   Real GetMagnitudeOfVelocity() const;

   void SetValue(const SpacecraftStateParam &param);
   void SetValue(const Cartesian &cartesian);
   void SetValue(const Rvector3 &pos, const Rvector3 &vel);
   void SetValue(const Real &posX, const Real &posY, const Real &posZ,
                 const Real &velX, const Real &velY, const Real &velZ);
   void SetPosition(const Rvector3 &pos);
   void SetVelocity (const Rvector3 &vel);

   // The inherited methods from GmatBase
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual std::string GetParameterText(const Integer id);
   virtual Integer GetParameterID(const std::string str);
   virtual Real GetRealParameter(const Integer id);
   virtual Real SetRealParameter(const Integer id, const Real value);

   // The inherited methods from Parameter
   virtual const std::string* GetParameterList() const;

protected:
   SpacecraftStateParam();

   Cartesian mCartesian;
   Real mMagOfPos;
   Real mMagOfVel;

   enum
   {
      POSITION_X = 0,
      POSITION_Y,
      POSITION_Z,
      VELOCITY_X,
      VELOCITY_Y,
      VELOCITY_Z,
      MAG_OF_POSITION,
      MAG_OF_VELOCITY,
      SpacecraftStateParamCount
   };

   static const Gmat::ParameterType PARAMETER_TYPE[SpacecraftStateParamCount];
   static const std::string PARAMETER_TEXT[SpacecraftStateParamCount];

private:
   void UpdateParameters();
};

#endif // SpacecraftStateParam_hpp
