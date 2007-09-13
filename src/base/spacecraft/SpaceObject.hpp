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


#ifndef SpaceObject_hpp
#define SpaceObject_hpp

#include "GmatBase.hpp"
#include "SpacePoint.hpp"
#include "PropState.hpp"
#include "SpaceObjectException.hpp"

class GMAT_API SpaceObject : public SpacePoint
{
public:
   SpaceObject(Gmat::ObjectType typeId, const std::string &typeStr,
               const std::string &instName);
   virtual ~SpaceObject();
   SpaceObject(const SpaceObject& so);
   SpaceObject&         operator=(const SpaceObject& so);
   
   virtual PropState&   GetState();
   virtual Real         GetEpoch();
   virtual Real         SetEpoch(const Real ep);
   virtual bool         IsManeuvering();
   virtual void         IsManeuvering(bool mnvrFlag);
   virtual bool         ParametersHaveChanged();
   virtual void         ParametersHaveChanged(bool flag);
   
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Real GetRealParameter(const Integer id) const;
   virtual Real GetRealParameter(const std::string &label) const;
   virtual Real SetRealParameter(const Integer id, const Real value);
   virtual Real SetRealParameter(const std::string &label, const Real value);

   /// @todo Waiting for CoordinateSystems in Spacecraft, then see if needed
   virtual void SetOriginName(std::string cbName);
   virtual const std::string GetOriginName();
   virtual void SetOrigin(SpacePoint *cb);
   
   virtual const Rvector6 GetMJ2000State(const A1Mjd &atTime);
   virtual const Rvector3 GetMJ2000Position(const A1Mjd &atTime);
   virtual const Rvector3 GetMJ2000Velocity(const A1Mjd &atTime);
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual void ClearLastStopTriggered();
   virtual void SetLastStopTriggered(const std::string &stopCondName);
   virtual bool WasLastStopTriggered(const std::string &stopCondName);
   
protected:
   /// The spacecraft state
   PropState         state;
   /// true when a finite burn needs to be applied to this SpaceObject
   bool              isManeuvering;
   /// Reference SpacePoint for the data
   std::string       originName;
   /// Reference SpacePoint for the data
   SpacePoint        *origin;
   /// Flag indicating if the force model parms have changed
   bool              parmsChanged;
   /// The names of the last set of stopping conditions met
   StringArray       lastStopTriggered;

   /// Enumerated parameter IDs   
   enum
   {
       EPOCH_PARAM = SpacePointParamCount,
       SpaceObjectParamCount
   };
   /// Array of supported parameters
   static const std::string PARAMETER_TEXT[SpaceObjectParamCount - 
                                           SpacePointParamCount];
   /// Array of parameter types
   static const Gmat::ParameterType PARAMETER_TYPE[SpaceObjectParamCount - 
                                                   SpacePointParamCount];
   
};

#endif // SpaceObject_hpp
