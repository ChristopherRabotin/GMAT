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

#ifdef DEBUG_BEGIN_MANEUVER
  #include "MessageInterface.hpp"
#endif


//------------------------------------------------------------------------------
// BeginManeuver()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
BeginManeuver::BeginManeuver() :
   GmatCommand    ("BeginManeuver")
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
   
   if (retval) {
      // Look up the maneuver object
      if (objectMap->find(burnName) == objectMap->end()) 
         throw CommandException("Unknown finite burn \"" + burnName + "\"");
      if ((*objectMap)[burnName]->GetTypeName() != "FiniteBurn")
         throw CommandException((burnName) + " is not a FiniteBurn");
      maneuver = (FiniteBurn*)((*objectMap)[burnName]);
      
      // find all of the spacecraft
      StringArray::iterator scName;
      Spacecraft *sc;
      for (scName = satNames.begin(); scName != satNames.end(); ++scName) {
         if (objectMap->find(*scName) == objectMap->end()) 
            throw CommandException("Unknown SpaceObject \"" + (*scName) + "\"");
         
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
      #ifdef DEBUG_BEGIN_MANEUVER
         MessageInterface::ShowMessage
            ("Activating engine %s\n", 
             (*i)->GetName().c_str());
      #endif
      (*i)->SetBooleanParameter((*i)->GetParameterID("IsFiring"), true);

      #ifdef DEBUG_BEGIN_MANEUVER
         MessageInterface::ShowMessage
            ("Checking to see if engine is active: returned %s\n", 
             ((*i)->GetBooleanParameter((*i)->GetParameterID("IsFiring")) ? 
              "true" : "false"));
      #endif
      
   }
   
   return true;
}
