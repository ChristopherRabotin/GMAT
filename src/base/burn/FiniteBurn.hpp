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
 * Defines the FiniteBurn class used for maneuvers. 
 */
//------------------------------------------------------------------------------


#ifndef FiniteBurn_hpp
#define FiniteBurn_hpp

#include "Burn.hpp"


/**
 * Class used to configure finite burns.
 */
 class GMAT_API FiniteBurn : public Burn
{
public:
   FiniteBurn(std::string nomme = "");
   virtual ~FiniteBurn();
   FiniteBurn(const FiniteBurn& fb);
   FiniteBurn&             operator=(const FiniteBurn& fb);
   
   // Inherited (GmatBase) methods
   virtual std::string     GetParameterText(const Integer id) const;
   virtual Integer         GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                           GetParameterType(const Integer id) const;
   virtual std::string     GetParameterTypeString(const Integer id) const;

   virtual bool            SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual const StringArray&    
                           GetStringArrayParameter(const Integer id) const;

   virtual Real            GetRealParameter(const Integer id) const;
   virtual Real            SetRealParameter(const Integer id, const Real value);

   void                    Initialize();
   virtual bool            Fire(Real *burnData);
   
   GmatBase*               Clone() const;
   
protected:
   /// List of thrusters used in the maneuver
   StringArray             thrusters;
   /// List of fuel tanks used in the maneuver
   StringArray             tanks;
   /// Overall thrust scale factor for this burn
   Real                    burnScaleFactor;
   /// Flag used to determine if the configuration needs updating
   bool                    initialized;
   
   /// Published parameters for thrusters
   enum
   {
      THRUSTER = BurnParamCount,
      FUEL_TANK, 
      BURN_SCALE_FACTOR,
      FiniteBurnParamCount
   };
   
   /// Thruster parameter labels
   static const std::string 
                        PARAMETER_TEXT[FiniteBurnParamCount - BurnParamCount];
   /// Thruster parameter types
   static const Gmat::ParameterType 
                        PARAMETER_TYPE[FiniteBurnParamCount - BurnParamCount];
};

#endif // FiniteBurn_hpp
