//$Header$
//------------------------------------------------------------------------------
//                                 Maneuver
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/20
//
/**
 * Definition for the Maneuver command class
 */
//------------------------------------------------------------------------------


#ifndef Maneuver_hpp
#define Maneuver_hpp

#include "GmatCommand.hpp"
#include "Burn.hpp"
#include "Spacecraft.hpp"


/**
 * This class implements the Maneuver command.  Maneuvers are performed using
 * pre-configured Burn objects.  A typical segment of script that implements
 * a maneuver for a spacecraft named Sat1 looks like this:
 *
 *     Create ImpulsiveBurn burn;
 *     burn.Type = VNB;
 *     burn.Element1 = 0.125;         % km/s
 *     ...
 *     Maneuver burn(Sat1);
 */
class GMAT_API Maneuver : public GmatCommand
{
public:
   Maneuver(void);
   virtual ~Maneuver(void);
    
   Maneuver(const Maneuver& m);
   Maneuver&               operator=(const Maneuver& m);
    
   // Methods used for configuration
   virtual bool            SetObject(const std::string &name,
                                     const Gmat::ObjectType type,
                                     const std::string &associate = "",
                                     const Gmat::ObjectType associateType =
                                     Gmat::UNKNOWN_OBJECT);
    
   virtual bool            SetObject(GmatBase *obj,
                                     const Gmat::ObjectType type);


   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   virtual const std::string&
                           GetGeneratingString(Gmat::WriteMode mode,
                                               const std::string &prefix,
                                               const std::string &useName);

   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);
   
   virtual const ObjectTypeArray&
                          GetRefObjectTypeArray();
   virtual const StringArray&
                          GetRefObjectNameArray(const Gmat::ObjectType type);
   
   // Parameter accessor methods -- overridden from GmatBase
   virtual std::string     GetParameterText(const Integer id) const;
   virtual Integer         GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                           GetParameterType(const Integer id) const;
   virtual std::string     GetParameterTypeString(const Integer id) const;
    
   virtual std::string     GetStringParameter(const Integer id) const;
   virtual bool            SetStringParameter(const Integer id, 
                                              const std::string &value);
    
   // Methods used to run the command
   virtual bool            InterpretAction(void);
    
   virtual bool            Initialize(void);
   virtual bool            Execute(void);
    
protected:
   /// The burn object used for the maneuver
   std::string              burnName;
   /// The burn object used for the maneuver
   Burn                    *burn;
   /// The name of the spacecraft that gets maneuvered
   std::string             satName;
   /// The spacecraft
   Spacecraft              *sat;
   
   // Parameter IDs 
   /// ID for the burn object
   Integer                 burnNameID;
   /// ID for the spacecraft name
   Integer                 satNameID;
};

#endif // Maneuver_hpp
