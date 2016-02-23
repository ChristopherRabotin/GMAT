//------------------------------------------------------------------------------
//                           EndFileThrust
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 13, 2016
/**
 * Toggle command used to tell GMAT to stop using a thrust data file
 */
//------------------------------------------------------------------------------


#include "EndFileThrust.hpp"
#include "MessageInterface.hpp"


EndFileThrust::EndFileThrust(const std::string &name) :
   GmatCommand             ("EndFileThrust"),
   thrustFileReader        (""),
   burnForce               (NULL),
   transientForces         (NULL)
{
   objectTypeNames.push_back("BurnCommand");
   physicsBasedCommand = true;
}

EndFileThrust::~EndFileThrust()
{
}

EndFileThrust::EndFileThrust(const EndFileThrust& eft) :
   GmatCommand             (eft),
   thrustFileReader        (eft.thrustFileReader),
   burnForce               (NULL),
   transientForces         (NULL)
{
}

EndFileThrust& EndFileThrust::operator =(const EndFileThrust& eft)
{
   if (this != &eft)
   {
      GmatCommand::operator=(eft);

      thrustFileReader  = eft.thrustFileReader;
      burnForce         = NULL;
      transientForces   = NULL;
      satNames          = eft.satNames;

      sats.clear();
   }

   return *this;
}

GmatBase* EndFileThrust::Clone() const
{
   return new EndFileThrust(*this);
}

bool EndFileThrust::RenameRefObject(const Gmat::ObjectType type,
      const std::string& oldName, const std::string& newName)
{
   bool retval = false;

   // EndFiniteBurn needs to know about Burn and Spacecraft only
   if ((type != Gmat::SPACECRAFT) && (type != Gmat::INTERFACE))
      return true;

   if (thrustFileReader == oldName)
   {
      thrustFileReader = newName;
      retval = true;
   }

   for (UnsignedInt i=0; i<satNames.size(); i++)
      if (satNames[i] == oldName)
      {
         satNames[i] = newName;
         retval = true;
      }

   return retval;
}


void EndFileThrust::SetTransientForces(std::vector<PhysicalModel*>* tf)
{
   transientForces = tf;
}

bool EndFileThrust::TakeAction(const std::string& action,
      const std::string& actionData)
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

std::string EndFileThrust::GetRefObjectName(const Gmat::ObjectType type) const
{
   switch (type)
   {
      case Gmat::INTERFACE:
         #ifdef DEBUG_END_MANEUVER
            MessageInterface::ShowMessage
               ("Getting EndFileThrust history file name\n");
         #endif
         return thrustFileReader;

      default:
         break;
   }

   return GmatCommand::GetRefObjectName(type);
}

const ObjectTypeArray& EndFileThrust::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::INTERFACE);
   refObjectTypes.push_back(Gmat::SPACECRAFT);

   return refObjectTypes;
}

const StringArray& EndFileThrust::GetRefObjectNameArray(
      const Gmat::ObjectType type)
{
   refObjectNames.clear();

   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::SPACECRAFT)
   {
      refObjectNames.insert(refObjectNames.end(), satNames.begin(),
            satNames.end());
   }

   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::INTERFACE)
   {
      refObjectNames.push_back(thrustFileReader);
   }

   return refObjectNames;
}

bool EndFileThrust::SetRefObjectName(const Gmat::ObjectType type,
      const std::string& name)
{
   switch (type)
   {
      case Gmat::SPACECRAFT:
         #ifdef DEBUG_END_MANEUVER
            MessageInterface::ShowMessage
               ("Setting EndFileThrust reference spacecraft \"%s\"\n",
                name.c_str());
         #endif
         if (find(satNames.begin(), satNames.end(), name) != satNames.end())
         {
            MessageInterface::ShowMessage("In the EndFileThrust command %s, "
                  "the spacecraft %s is set more than once.  Only one instance "
                  "will be used.\n",
                  GetGeneratingString(Gmat::NO_COMMENTS).c_str(), name.c_str());
            return true;
         }
         satNames.push_back(name);
         if (satNames.size() > 1)
            throw CommandException("EndFileThrust commands do not currently "
                  "support multiple Spacecraft; please toggle finite burns off "
                  "one spacecraft at a time.");
         return true;

         // Note:  ThrustHistoryFile looks like FINITE_BURN to the Interpreter
      case Gmat::FINITE_BURN:
      case Gmat::INTERFACE:
         #ifdef DEBUG_END_MANEUVER
            MessageInterface::ShowMessage
               ("Setting EndFileThrust reference burn \"%s\"\n", name.c_str());
         #endif
         thrustFileReader = name;
         return true;

      default:
         #ifdef DEBUG_END_MANEUVER
            MessageInterface::ShowMessage
               ("EndFileThrust reference object \"%s\" not set!\n",
                name.c_str());
         #endif
         break;
   }

   return GmatCommand::SetRefObjectName(type, name);
}

const std::string& EndFileThrust::GetGeneratingString(Gmat::WriteMode mode,
      const std::string& prefix, const std::string& useName)
{
   generatingString = prefix + "EndFileThrust " + thrustFileReader + "(";
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

bool EndFileThrust::Initialize()
{
   bool retval = false;
   return retval;
}

bool EndFileThrust::Execute()
{
   bool retval = false;
   return retval;
}
