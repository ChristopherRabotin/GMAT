//$Id$
//------------------------------------------------------------------------------
//                            EndFiniteBurn
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS task
// order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2005/01/04
//
/**
 *  Implementation code for the EndFiniteBurn command.
 */
//------------------------------------------------------------------------------

#include "EndFiniteBurn.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_END_MANEUVER
//#define DEBUG_END_MANEUVER_EXE


//------------------------------------------------------------------------------
// EndFiniteBurn()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
EndFiniteBurn::EndFiniteBurn() :
   GmatCommand        ("EndFiniteBurn"),
   thrustName         (""),
   burnForce          (NULL),
   burnName           (""),
   maneuver           (NULL),
   transientForces    (NULL),
   firstTimeExecution (true)
{
   if (instanceName == "")
      instanceName = "EndFiniteBurn";
}


//------------------------------------------------------------------------------
// ~EndFiniteBurn()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
EndFiniteBurn::~EndFiniteBurn()
{
}


//------------------------------------------------------------------------------
// EndFiniteBurn(const BeginManeuver& endman)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param <endman> The command that gets copied.
 */
//------------------------------------------------------------------------------
EndFiniteBurn::EndFiniteBurn(const EndFiniteBurn& endman) :
   GmatCommand        (endman),
   thrustName         (endman.thrustName),
   burnForce          (NULL),
   burnName           (endman.burnName),
   maneuver           (NULL),
   transientForces    (NULL),
   satNames           (endman.satNames),
   firstTimeExecution (true)
{
   sats.clear();
   thrusters.clear();
}


//------------------------------------------------------------------------------
// EndFiniteBurn& operator=(const EndFiniteBurn& endman)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param <endman> The command that gets copied.
 * 
 * @return this instance, with internal data structures set to match the input
 *         instance.
 */
//------------------------------------------------------------------------------
EndFiniteBurn& EndFiniteBurn::operator=(const EndFiniteBurn& endman)
{
   if (&endman == this)
      return *this;
      
   GmatCommand::operator=(endman);
   thrustName = endman.thrustName;
   burnForce = NULL;
   burnName = endman.burnName;
   maneuver = NULL;
   transientForces = NULL;
   satNames = endman.satNames;
   firstTimeExecution = true;
   
   sats.clear();
   thrusters.clear();
   
   return *this;
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,  
//                         const std::string &actionData = "");
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action
 * @return true if action successfully performed
 *
 */
//------------------------------------------------------------------------------
bool EndFiniteBurn::TakeAction(const std::string &action,
                                 const std::string &actionData)
{
   #if DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("EndFiniteBurn::TakeAction() action=%s, actionData=%s\n",
       action.c_str(), actionData.c_str());
   #endif
   
   if (action == "Clear")
   {
      satNames.clear();
      return true;
   }

   return false;
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * Accesses names for referenced objects.
 * 
 * @param <type> Type of object requested.
 * 
 * @return the referenced object's name.
 */
//------------------------------------------------------------------------------
std::string EndFiniteBurn::GetRefObjectName(const Gmat::ObjectType type) const
{
   switch (type)
   {
      case Gmat::FINITE_BURN:
         #ifdef DEBUG_END_MANEUVER
            MessageInterface::ShowMessage
               ("Getting EndFiniteBurn reference burn names\n");
         #endif
         return burnName;
         
      default:
         ;
   }
   
   return GmatCommand::GetRefObjectName(type);
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by the BeginFiniteBurn.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& EndFiniteBurn::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::FINITE_BURN);
   refObjectTypes.push_back(Gmat::SPACECRAFT);
   return refObjectTypes;
}



//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Accesses arrays of names for referenced objects.
 * 
 * @param <type> Type of object requested.
 * 
 * @return the StringArray containing the referenced object names.
 */
//------------------------------------------------------------------------------
const StringArray& EndFiniteBurn::GetRefObjectNameArray(
                             const Gmat::ObjectType type)
{
   refObjectNames.clear();
   
   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::SPACECRAFT)
   {
      refObjectNames.insert(refObjectNames.end(), satNames.begin(), satNames.end());
   }
   
   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::FINITE_BURN)
   {
      refObjectNames.push_back(burnName);
   }

   return refObjectNames;
/*   switch (type)
   {
      case Gmat::SPACECRAFT:
         #ifdef DEBUG_END_MANEUVER
            MessageInterface::ShowMessage
               ("Getting EndFiniteBurn reference spacecraft list\n");
         #endif
         return satNames;
      
      default:
         ;
   }
   
   return GmatCommand::GetRefObjectNameArray(type);
   */
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets names for referenced objects.
 * 
 * @param <type> Type of the object.
 * @param <name> Name of the object.
 * 
 * @return true if the name was set, false if not.
 */
//------------------------------------------------------------------------------
bool EndFiniteBurn::SetRefObjectName(const Gmat::ObjectType type, 
                                     const std::string &name)
{
   switch (type)
   {
      case Gmat::SPACECRAFT:
         #ifdef DEBUG_END_MANEUVER
            MessageInterface::ShowMessage
               ("Setting EndFiniteBurn reference spacecraft \"%s\"\n", 
                name.c_str());
         #endif
         satNames.push_back(name);
         return true;
      
      case Gmat::FINITE_BURN:
         #ifdef DEBUG_END_MANEUVER
            MessageInterface::ShowMessage
               ("Setting EndFiniteBurn reference burn \"%s\"\n", name.c_str());
         #endif
         burnName = name;
         return true;
         
      default:
         #ifdef DEBUG_END_MANEUVER
            MessageInterface::ShowMessage
               ("EndFiniteBurn reference object \"%s\" not set!\n", 
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
 * This method returns a clone of the EndFiniteBurn command.
 *
 * @return clone of the Propagate.
 */
//------------------------------------------------------------------------------
GmatBase* EndFiniteBurn::Clone() const
{
   return new EndFiniteBurn(*this);
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param type Type of the object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool EndFiniteBurn::RenameRefObject(const Gmat::ObjectType type,
                                    const std::string &oldName,
                                    const std::string &newName)
{
   // EndFiniteBurn needs to know about Burn and Spacecraft only
   if (type != Gmat::FINITE_BURN && type != Gmat::SPACECRAFT)
      return true;
   
   if (burnName == oldName)
      burnName = newName;
   
   for (UnsignedInt i=0; i<satNames.size(); i++)
      if (satNames[i] == oldName)
         satNames[i] = newName;
   
   return true;
}


//------------------------------------------------------------------------------
//  const std::string GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param <mode>    Specifies the type of serialization requested.
 * @param <prefix>  Optional prefix appended to the object's name. (Used for
 *                  indentation)
 * @param <useName> Name that replaces the object's name (Not yet used
 *                  in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& EndFiniteBurn::GetGeneratingString(Gmat::WriteMode mode,
                                                    const std::string &prefix,
                                                    const std::string &useName)
{
   generatingString = prefix + "EndFiniteBurn " + burnName + "(";
   for (StringArray::iterator satIt = satNames.begin();
        satIt != satNames.end(); ++satIt)
   {
      if (satIt != satNames.begin())
         generatingString += ", ";
      generatingString += *satIt;
   }

   generatingString += ");";

   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
//  void SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
/**
 * Sets the array of transient forces for the command.
 *
 * @param <tf> The transient force vector.
 */
//------------------------------------------------------------------------------
void EndFiniteBurn::SetTransientForces(std::vector<PhysicalModel*> *tf)
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
bool EndFiniteBurn::Initialize()
{
   bool retval = GmatCommand::Initialize();
   firstTimeExecution = true;
   
   GmatBase *mapObj;
   if (retval)
   {
      // Look up the maneuver object
      if ((mapObj = FindObject(burnName)) == NULL) 
         throw CommandException("EndFiniteBurn: Unknown finite burn \"" + burnName + "\"");
      if (mapObj->GetTypeName() != "FiniteBurn")
         throw CommandException("EndFiniteBurn: " + (burnName) + " is not a FiniteBurn");
      maneuver = (FiniteBurn*)mapObj;
      
      // find all of the spacecraft
      StringArray::iterator scName;
      Spacecraft *sc;
      
      sats.clear();
      for (scName = satNames.begin(); scName != satNames.end(); ++scName)
      {
         if ((mapObj = FindObject(*scName)) == NULL) 
            throw CommandException("EndFiniteBurn: Unknown SpaceObject \"" + (*scName) + "\"");
         
         if (mapObj->GetType() != Gmat::SPACECRAFT)
            throw CommandException("EndFiniteBurn: " + (*scName) + " is not a Spacecraft");
         sc = (Spacecraft*)mapObj;
         sats.push_back(sc);
      }
      
      // We cannot validate thrusters until execution time(LOJ: 2010.08.13)
      // A script can have sc2 = sc1 before running BeginFiniteBurn
      //ValidateThrusters();
   }
   
   thrustName = burnName + "_FiniteThrust";
   #ifdef DEBUG_END_MANEUVER
      MessageInterface::ShowMessage
         ("EndFiniteBurn initialized with thrust force named \"%s\"\n", 
          thrustName.c_str());
   #endif
   
   return initialized;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/** 
 * The method that is fired to turn off thrusters.
 *
 * @return true if the GmatCommand runs to completion, false if an error 
 *         occurs. 
 */
//------------------------------------------------------------------------------
bool EndFiniteBurn::Execute()
{
   if (firstTimeExecution)
   {
      // Get updated thruster pointers from the spacecraft since spacecraft
      // clones them
      //ValidateThrusters(); // commented out (LOJ: 2010.01.25)
      firstTimeExecution = false;
   }
   
   // Let's validate thrusters all the time (LOJ: 2010.01.25)
   ValidateThrusters();
   
   // Turn off all of the referenced thrusters
   for (std::vector<Thruster*>::iterator i = thrusters.begin(); 
        i != thrusters.end(); ++i)
   {
      Thruster *th = *i;
      #ifdef DEBUG_END_MANEUVER_EXE
         MessageInterface::ShowMessage
            ("EndFiniteBurn::Execute() Deactivating engine <%p>'%s'\n", th,
             th->GetName().c_str());
      #endif
      th->SetBooleanParameter(th->GetParameterID("IsFiring"), false);

      #ifdef DEBUG_END_MANEUVER_EXE
         MessageInterface::ShowMessage
            ("Checking to see if engine is inactive: returned %s\n", 
             (th->GetBooleanParameter(th->GetParameterID("IsFiring")) ? 
              "true" : "false"));
      #endif      
   }
   
   
   // Tell active spacecraft that they are no longer firing
   for (std::vector<Spacecraft*>::iterator s=sats.begin(); s!=sats.end(); ++s)
   {
      /// todo: Be sure that no other maneuver has the spacecraft maneuvering
      (*s)->IsManeuvering(false);
//      (*current)->TakeAction("ReleaseCartesianStateDynamics");
   }
   
   // Remove the force from the list of transient forces
   for (std::vector<PhysicalModel*>::iterator j = transientForces->begin();
        j != transientForces->end(); ++j)
   {
      if (((*j)->GetName()) == thrustName)
      {
         #ifdef DEBUG_TRANSIENT_FORCES
         MessageInterface::ShowMessage
            ("EndFiniteBurn::Execute() Removing burnForce<%p>'%s' from transientForces\n",
             burnForce, burnForce->GetName().c_str());
         #endif
         transientForces->erase(j);
         break;
      }
   }
   
   // Reset maneuvering to Publisher so that any subscriber can do its own action
   if (!sats.empty())
   {
      Real epoch = sats[0]->GetEpoch();
      publisher->SetManeuvering(this, false, epoch, satNames, "end of finite maneuver");
   }
   
   #ifdef DEBUG_END_MANEUVER_EXE
      MessageInterface::ShowMessage("EndFiniteBurn::Execute() Current TransientForces list:\n");
      for (std::vector<PhysicalModel*>::iterator j = transientForces->begin();
           j != transientForces->end(); ++j)
         MessageInterface::ShowMessage("   %s\n", (*j)->GetName().c_str());
   #endif
   
   BuildCommandSummary(true);
   return true;
}


//------------------------------------------------------------------------------
// void ValidateThrusters()
//------------------------------------------------------------------------------
/**
 * Validate that the spacecrafts have the thrusters they need.
 */
//------------------------------------------------------------------------------
void EndFiniteBurn::ValidateThrusters()
{
   thrusters.clear();
   for (std::vector<Spacecraft*>::iterator current = sats.begin(); 
        current != sats.end(); ++current)
   {
      #ifdef DEBUG_EFB_THRUSTER
      MessageInterface::ShowMessage
         ("EndFiniteBurn::ValidateThrusters() entered, checking Spacecraft "
          "<%p>'%s' for Thrusters\n", *current, (*current)->GetName().c_str());
      #endif
      
      StringArray thrusterNames = (*current)->GetStringArrayParameter(
                                  (*current)->GetParameterID("Thrusters"));
      StringArray engines = (maneuver)->GetStringArrayParameter(
                            (maneuver)->GetParameterID("Thrusters"));
      
      #ifdef DEBUG_EFB_THRUSTER
      MessageInterface::ShowMessage
         ("   Spacecraft has %d Thrusters and FiniteBurn has %d thrusters\n",
          thrusterNames.size(), engines.size());
      #endif
      
      for (StringArray::iterator i = engines.begin(); i != engines.end(); ++i)
      {
         if (find(thrusterNames.begin(), thrusterNames.end(), *i) == 
             thrusterNames.end())
         {
            thrusters.clear();
            throw CommandException("EndFiniteBurn: Spacecraft " + (*current)->GetName() +
                                   " does not have a thruster named \"" +
                                   (*i) + "\"");
         }
         
         Thruster* th = 
            (Thruster*)((*current)->GetRefObject(Gmat::THRUSTER, *i));
         
         if (th)
         {
            #ifdef DEBUG_EFB_THRUSTER
            MessageInterface::ShowMessage
               ("EndFiniteBurn::ValidateThrusters() addding the Thruster <%p>'%s' "
                "to thrusters\n", th, th->GetName().c_str());
            #endif
            thrusters.push_back(th);
         }
         else
         {
            thrusters.clear();
            throw CommandException("EndFiniteBurn: Thruster object \"" + (*i) +
                                   "\" was not set on Spacecraft \"" 
                                   + (*current)->GetName() + "\"");
         }
      }
   }
   
   #ifdef DEBUG_EFB_THRUSTER
   MessageInterface::ShowMessage
      ("EndFiniteBurn::ValidateThrusters() leaving\n");
   #endif
}

