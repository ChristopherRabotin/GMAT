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
 * Defines the base class used for spacecraft and formations. 
 */
//------------------------------------------------------------------------------


#ifndef SPACEOBJECT_HPP
#define SPACEOBJECT_HPP

#include "GmatBase.hpp"
#include "SpacePoint.hpp"
#include "PropState.hpp"
#include "SpaceObjectException.hpp"

class GMAT_API SpaceObject : public SpacePoint
{
public:
	SpaceObject(Gmat::ObjectType typeId, const std::string &typeStr, 
               const std::string &nomme);
	virtual ~SpaceObject();
   SpaceObject(const SpaceObject& so);
   SpaceObject&         operator=(const SpaceObject& so);
   
   virtual PropState&   GetState();
   virtual Real         GetEpoch();
   virtual Real         SetEpoch(const Real ep);
   
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Real GetRealParameter(const Integer id) const;
   virtual Real GetRealParameter(const std::string &label) const;
   virtual Real SetRealParameter(const Integer id, const Real value);
   virtual Real SetRealParameter(const std::string &label, const Real value);
//   virtual std::string GetStringParameter(const Integer id) const;
//   virtual bool SetStringParameter(const Integer id, const std::string &value);
   
   // temporarily here *************************************************
   virtual const Rvector6 GetMJ2000State(const A1Mjd &atTime);
   virtual const Rvector3 GetMJ2000Position(const A1Mjd &atTime);
   virtual const Rvector3 GetMJ2000Velocity(const A1Mjd &atTime);
   // temporarily here *************************************************
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

protected:
   PropState            state;
   
   /// Enumerated parameter IDs   
   enum
   {
       EPOCH_PARAM= SpacePointParamCount,
       SpaceObjectParamCount
   };
   /// Array of supported parameters
   static const std::string PARAMETER_TEXT[SpaceObjectParamCount - 
                                           SpacePointParamCount];
   /// Array of parameter types
   static const Gmat::ParameterType PARAMETER_TYPE[SpaceObjectParamCount - 
                                                   SpacePointParamCount];
   
};

#endif // SPACEOBJECT_HPP
