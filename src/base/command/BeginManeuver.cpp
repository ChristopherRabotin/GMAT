//$Header$
//------------------------------------------------------------------------------
//                            BeginManeuver
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS task
// order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2005/01/04
//
/**
 *  Implementation code for the BeginManeuver command.
 */
//------------------------------------------------------------------------------

#include "BeginManeuver.hpp"


//#define DEBUG_BEGIN_MANEUVER
//#define DEBUG_BEGIN_MANEUVER_EXE


#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// BeginManeuver()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
BeginManeuver::BeginManeuver() :
   GmatCommand    ("BeginManeuver"),
   burnName       (""),
   maneuver       (NULL),
   burnForce      (NULL),
   transientForces(NULL)
{
   if (instanceName == "")
      instanceName = "BeginManeuver";
}


//------------------------------------------------------------------------------
// ~BeginManeuver()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BeginManeuver::~BeginManeuver()
{
   if (burnForce)
      delete burnForce;
}


//------------------------------------------------------------------------------
// BeginManeuver(const BeginManeuver& begman)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param begman The command that gets copied.
 */
//------------------------------------------------------------------------------
BeginManeuver::BeginManeuver(const BeginManeuver& begman) :
   GmatCommand       (begman),
   burnName          (begman.burnName),
   maneuver          (NULL),
   burnForce         (NULL),
   transientForces   (NULL),
   satNames          (begman.satNames)
{
}


//------------------------------------------------------------------------------
// BeginManeuver& operator=(const BeginManeuver& begman)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param begman The command that gets copied.
 * 
 * @return this instance, with internal data structures set to match the input
 *         instance.
 */
//------------------------------------------------------------------------------
BeginManeuver& BeginManeuver::operator=(const BeginManeuver& begman)
{
   if (&begman == this)
      return *this;
      
   GmatCommand::operator=(begman);
   burnName = begman.burnName;
   satNames = begman.satNames;
   transientForces = NULL;
   
   return *this;
}


std::string BeginManeuver::GetRefObjectName(const Gmat::ObjectType type) const
{
   switch (type) {
      case Gmat::BURN:
         #ifdef DEBUG_BEGIN_MANEUVER
            MessageInterface::ShowMessage
               ("Getting BeginManeuver reference burn names\n");
         #endif
         return burnName;
         
      default:
         ;
   }
   
   return GmatCommand::GetRefObjectName(type);
}


const StringArray& BeginManeuver::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   switch (type) {
      case Gmat::SPACECRAFT:
         #ifdef DEBUG_BEGIN_MANEUVER
            MessageInterface::ShowMessage
               ("Getting BeginManeuver reference spacecraft list\n");
         #endif
         return satNames;
      
      default:
         ;
   }
   
   return GmatCommand::GetRefObjectNameArray(type);
}


bool BeginManeuver::SetRefObjectName(const Gmat::ObjectType type, 
                                     const std::string &name)
{
   switch (type) {
      case Gmat::SPACECRAFT:
         #ifdef DEBUG_BEGIN_MANEUVER
            MessageInterface::ShowMessage
               ("Setting BeginManeuver reference spacecraft \"%s\"\n", 
                name.c_str());
         #endif
         satNames.push_back(name);
         return true;
      
      case Gmat::BURN:
         #ifdef DEBUG_BEGIN_MANEUVER
            MessageInterface::ShowMessage
               ("Setting BeginManeuver reference burn \"%s\"\n", name.c_str());
         #endif
         burnName = name;
         return true;
         
      default:
         #ifdef DEBUG_BEGIN_MANEUVER
            MessageInterface::ShowMessage
               ("BeginManeuver reference object \"%s\" not set!\n", 
                name.c_str());
         #endif
         ;
   }
   
   return GmatCommand::SetRefObjectName(type, name);
}


GmatBase* BeginManeuver::GetObject(const Gmat::ObjectType type, 
                                   const std::string objName)
{
   if (type == Gmat::TRANSIENT_FORCE)
      return burnForce;
   return GmatCommand::GetObject(type, objName);
}
                                  
//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the BeginManeuver command.
 *
 * @return clone of the Propagate.
 */
//------------------------------------------------------------------------------
GmatBase* BeginManeuver::Clone() const
{
   return new BeginManeuver(*this);
}


void BeginManeuver::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   transientForces = tf;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the BeginManeuver structures at the start of a run.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool BeginManeuver::Initialize()
{
   bool retval = GmatCommand::Initialize();

   #ifdef DEBUG_BEGIN_MANEUVER
      MessageInterface::ShowMessage("BeginManeuver::Initialize() entered\n");
   #endif
   
   if (retval) {
      // Look up the maneuver object
      if (objectMap->find(burnName) == objectMap->end()) 
         throw CommandException("Unknown finite burn \"" + burnName + "\"");
      if ((*objectMap)[burnName]->GetTypeName() != "FiniteBurn")
         throw CommandException((burnName) + " is not a FiniteBurn");

      #ifdef DEBUG_BEGIN_MANEUVER
         MessageInterface::ShowMessage("BeginManeuver::Initialize() found %s\n", 
            burnName.c_str());
      #endif      

      maneuver = (FiniteBurn*)((*objectMap)[burnName]);

      // find all of the spacecraft
      StringArray::iterator scName;
      Spacecraft *sc;
      for (scName = satNames.begin(); scName != satNames.end(); ++scName) {
         if (objectMap->find(*scName) == objectMap->end()) 
            throw CommandException("Unknown SpaceObject \"" + (*scName) + "\"");

         #ifdef DEBUG_BEGIN_MANEUVER
            MessageInterface::ShowMessage(
               "BeginManeuver::Initialize() found %s\n", scName->c_str());
         #endif
         
         if ((*objectMap)[*scName]->GetType() != Gmat::SPACECRAFT)
            throw CommandException((*scName) + " is not a Spacecraft");
         sc = (Spacecraft*)(*objectMap)[*scName];
         sats.push_back(sc);
      }
      
      // Validate that the spacecraft have the thrusters they need
      thrusters.clear();
      for (std::vector<Spacecraft*>::iterator current = sats.begin(); 
           current != sats.end(); ++current) {
         StringArray thrusterNames = (*current)->GetStringArrayParameter(
                                     (*current)->GetParameterID("Thrusters"));
         StringArray engines = (maneuver)->GetStringArrayParameter(
                               (maneuver)->GetParameterID("Thrusters"));
         for (StringArray::iterator i = engines.begin(); 
              i != engines.end(); ++i) {
            if (find(thrusterNames.begin(), thrusterNames.end(), *i) == 
                     thrusterNames.end()) {
               thrusters.clear();
               throw CommandException("Spacecraft " + (*current)->GetName() +
                                      " does not have a thruster named \"" +
                                      (*i) + "\"");
            }
      
            Thruster* th = 
               (Thruster*)((*current)->GetRefObject(Gmat::THRUSTER, *i));
            if (th)
               thrusters.push_back(th);
            else {
               thrusters.clear();
               throw CommandException("Thruster object \"" + (*i) +
                                      "\" was not set on Spacecraft \"" 
                                      + (*current)->GetName() +
                                      "\"");
            }
         }
      }
      
      // If all is okay, create the FiniteThrust object and configure it.
      std::string thrustName = burnName + "_FiniteThrust";
      burnForce = new FiniteThrust(thrustName);
      
      Gmat::ObjectType type = Gmat::SPACECRAFT;
      StringArray::iterator iter;
      
      // load up the spacecraft name list
      for (iter = satNames.begin(); iter != satNames.end(); ++iter) {
         #ifdef DEBUG_BEGIN_MANEUVER
            MessageInterface::ShowMessage(
               "BeginManeuver::Initialize() setting %s on %s\n", 
               iter->c_str(), thrustName.c_str());
         #endif
         burnForce->SetRefObjectName(type, *iter);
      }
      
   }
   
   
   return initialized;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/** 
 * The method that is fired to turn on thrusters.
 *
 * @return true if the GmatCommand runs to completion, false if an error 
 *         occurs. 
 */
//------------------------------------------------------------------------------
bool BeginManeuver::Execute()
{
   // Turn on all of the referenced thrusters
   for (std::vector<Thruster*>::iterator i = thrusters.begin(); 
        i != thrusters.end(); ++i) {
      #ifdef DEBUG_BEGIN_MANEUVER_EXE
         MessageInterface::ShowMessage
            ("Activating engine %s\n", 
             (*i)->GetName().c_str());
      #endif
      (*i)->SetBooleanParameter((*i)->GetParameterID("IsFiring"), true);

      #ifdef DEBUG_BEGIN_MANEUVER_EXE
         MessageInterface::ShowMessage
            ("Checking to see if engine is active: returned %s\n", 
             ((*i)->GetBooleanParameter((*i)->GetParameterID("IsFiring")) ? 
              "true" : "false"));
      #endif      
   }
   
   // Tell active spacecraft that they are now firing
   for (std::vector<Spacecraft*>::iterator s=sats.begin(); s!=sats.end(); ++s)
      (*s)->IsManeuvering(true);
      
   // Insert the force into the list of transient forces
   transientForces->push_back(burnForce);

   #ifdef DEBUG_BEGIN_MANEUVER_EXE
      MessageInterface::ShowMessage("Current TransientForces list:\n");
      for (std::vector<PhysicalModel*>::iterator j = transientForces->begin();
           j != transientForces->end(); ++j)
         MessageInterface::ShowMessage("   %s\n", (*j)->GetName().c_str());
   #endif
   
   return true;
}
