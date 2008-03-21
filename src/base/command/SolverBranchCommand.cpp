//$Header$
//------------------------------------------------------------------------------
//                            SolverBranchCommand
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway
// Created: 2006/10/20
//
/**
 * Definition for the Solver loop command base class (Target, Optimize and 
 * Iterate).
 */
//------------------------------------------------------------------------------


#include "SolverBranchCommand.hpp"
#include "Spacecraft.hpp"
#include "Formation.hpp"

//------------------------------------------------------------------------------
//  SolverBranchCommand(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Creates a SolverBranchCommand command.  (default constructor)
 * 
 * @param <typeStr> The type name of the SolverBranchCommand.
 */
//------------------------------------------------------------------------------
SolverBranchCommand::SolverBranchCommand(const std::string &typeStr) :
   BranchCommand(typeStr)
{
   objectTypeNames.push_back("SolverBranchCommand");
}

//------------------------------------------------------------------------------
//  SolverBranchCommand(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Destroys a SolverBranchCommand command.  (destructor)
 */
//------------------------------------------------------------------------------
SolverBranchCommand::~SolverBranchCommand()
{
}


//------------------------------------------------------------------------------
//  SolverBranchCommand(const SolverBranchCommand &sbc)
//------------------------------------------------------------------------------
/**
 * Creates a SolverBranchCommand command based on another.  (Copy constructor)
 * 
 * @param <sbc> The SolverBranchCommand that is copied into this instance.
 */
//------------------------------------------------------------------------------
SolverBranchCommand::SolverBranchCommand(const SolverBranchCommand& sbc) :
   BranchCommand(sbc)
{
}

//------------------------------------------------------------------------------
//  SolverBranchCommand& operator=(const SolverBranchCommand& sbc)
//------------------------------------------------------------------------------
/**
 * Copies a SolverBranchCommand command.  (Assignment operator)
 * 
 * @param <sbc> The SolverBranchCommand that is copied into this instance.
 * 
 * @return This instance, configured to match sbc.
 */
//------------------------------------------------------------------------------
SolverBranchCommand& SolverBranchCommand::operator=(
   const SolverBranchCommand& sbc)
{
   if (&sbc != this)
   {
      BranchCommand::operator=(sbc);
   }
   
   return *this;
}

//------------------------------------------------------------------------------
//  GmatCommand* GetNext()
//------------------------------------------------------------------------------
/**
 * Access the next command in the mission sequence.
 *
 * For SolverBranchCommands, this method returns its own pointer while the child
 * commands are executingm, and it tells the Publisher about a state change after
 * the Solver has finished its work.
 *
 * @return The next command, or NULL if the sequence has finished executing.
 */
//------------------------------------------------------------------------------
GmatCommand* SolverBranchCommand::GetNext()
{
   // Return the next pointer in the command sequence if this command -- 
   // includng its branches -- has finished executing.
   if ((commandExecuting) && (!commandComplete))
      return this;
   
   // Set state back to RUNNING
   if (publisher)
      publisher->SetRunState(Gmat::RUNNING);
   
   return next;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void StoreLoopData()
//------------------------------------------------------------------------------
/**
 * Makes local copies of the data so that a solver loop can recover initial
 * data while iterating.
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::StoreLoopData()
{
   // Make local copies of all of the objects that may be affected by optimize
   // loop iterations
   // Check the Local Object Store first
   std::map<std::string, GmatBase *>::iterator pair = objectMap->begin();
   GmatBase *obj;
    
   // Loop through the object map, looking for objects we'll need to restore.
   while (pair != objectMap->end()) 
   {
      obj = (*pair).second;
      // Save copies of all of the spacecraft
      if (obj->GetType() == Gmat::SPACECRAFT)
      {
         Spacecraft *orig = (Spacecraft*)(obj);
         Spacecraft *sc = new Spacecraft(*orig);
         // Handle CoordinateSystems
         if (orig->GetInternalCoordSystem() == NULL)
            MessageInterface::ShowMessage(
               "Internal CS is NULL on spacecraft %s prior to optimizer cloning\n",
               orig->GetName().c_str());
         if (orig->GetRefObject(Gmat::COORDINATE_SYSTEM, "") == NULL)
            MessageInterface::ShowMessage(
               "Coordinate system is NULL on spacecraft %s prior to optimizer cloning\n",
               orig->GetName().c_str());
         sc->SetInternalCoordSystem(orig->GetInternalCoordSystem());
         sc->SetRefObject(orig->GetRefObject(Gmat::COORDINATE_SYSTEM, ""),
            Gmat::COORDINATE_SYSTEM, "");
         
         localStore.push_back(sc);
      }
      if (obj->GetType() == Gmat::FORMATION)
      {
         Formation *orig = (Formation*)(obj);
         Formation *form  = new Formation(*orig);
         localStore.push_back(form);
      }
      ++pair;
   }
   // Check the Global Object Store next
   std::map<std::string, GmatBase *>::iterator globalPair = globalObjectMap->begin();
    
   // Loop through the object map, looking for objects we'll need to restore.
   while (globalPair != globalObjectMap->end()) 
   {
      obj = (*globalPair).second;
      // Save copies of all of the spacecraft
      if (obj->GetType() == Gmat::SPACECRAFT)
      {
         Spacecraft *orig = (Spacecraft*)(obj);
         Spacecraft *sc = new Spacecraft(*orig);
         // Handle CoordinateSystems
         if (orig->GetInternalCoordSystem() == NULL)
            MessageInterface::ShowMessage(
               "Internal CS is NULL on spacecraft %s prior to optimizer cloning\n",
               orig->GetName().c_str());
         if (orig->GetRefObject(Gmat::COORDINATE_SYSTEM, "") == NULL)
            MessageInterface::ShowMessage(
               "Coordinate system is NULL on spacecraft %s prior to optimizer cloning\n",
               orig->GetName().c_str());
         sc->SetInternalCoordSystem(orig->GetInternalCoordSystem());
         sc->SetRefObject(orig->GetRefObject(Gmat::COORDINATE_SYSTEM, ""),
            Gmat::COORDINATE_SYSTEM, "");
         
         localStore.push_back(sc);
      }
      if (obj->GetType() == Gmat::FORMATION)
      {
         Formation *orig = (Formation*)(obj);
         Formation *form  = new Formation(*orig);
         localStore.push_back(form);
      }
      ++globalPair;
   }
}


//------------------------------------------------------------------------------
// void ResetLoopData()
//------------------------------------------------------------------------------
/**
 * Resets starting data from local copies so that a solver loop can iterate.
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::ResetLoopData()
{
   Spacecraft *sc;
   Formation  *fm;
   std::string name;
    
   for (std::vector<GmatBase *>::iterator i = localStore.begin();
        i != localStore.end(); ++i) {
      name = (*i)->GetName();
      //GmatBase *gb = (*objectMap)[name];
      GmatBase *gb = FindObject(name);
      if (gb != NULL) {
         if (gb->GetType() == Gmat::SPACECRAFT)
         {
            sc = (Spacecraft*)gb;
            *sc = *((Spacecraft*)(*i));
         }
         else if (gb->GetType() == Gmat::FORMATION)
         {
            fm = (Formation*)gb;
            *fm = *((Formation*)(*i));
         }
      }
   }
   // Reset the propagators so that propagations run identically loop to loop
   BranchCommand::TakeAction("ResetLoopData");
//   GmatCommand *cmd = branch[0];
//   while (cmd != this)
//   {
//      if (cmd->GetTypeName() == "Propagate")
//         cmd->TakeAction("ResetLoopData");
//      cmd = cmd->GetNext();
//   }
}


//------------------------------------------------------------------------------
// void FreeLoopData()
//------------------------------------------------------------------------------
/**
 * Cleans up starting data store after solver has completed.
 */
//------------------------------------------------------------------------------
void SolverBranchCommand::FreeLoopData()
{
   GmatBase *obj;
   while (!localStore.empty()) {
      obj = *(--localStore.end());
      localStore.pop_back();
      delete obj;
   }
}
