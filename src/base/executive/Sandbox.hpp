//$Header$
//------------------------------------------------------------------------------
//                                 Sandbox
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/10/08
//
/**
 * Definition for the GMAT Sandbox class
 */
//------------------------------------------------------------------------------


#ifndef Sandbox_hpp
#define Sandbox_hpp

#include "gmatdefs.hpp"

// Core classes
#include "GmatBase.hpp"
#include "Command.hpp"
#include "Publisher.hpp"

#include "Spacecraft.hpp"
#include "PropSetup.hpp"
#include "Subscriber.hpp"
#include "SolarSystem.hpp"
#include "Solver.hpp"
#include "Burn.hpp"

class Moderator;        // Forward reference for the moderator pointer

class GMAT_API Sandbox
{
public:
   Sandbox();
   ~Sandbox();

   // Setup methods
   bool AddObject(GmatBase *obj);   
   bool AddCommand(GmatCommand *cmd);
   bool AddSolarSystem(SolarSystem *ss);
   bool AddSubscriber(Subscriber *sub);
   bool SetPublisher(Publisher *pub = NULL);
    
   GmatBase* GetInternalObject(std::string name, Gmat::ObjectType type);
   Spacecraft* GetSpacecraft(std::string name);
    
   // Execution methods
   bool Initialize();
   bool Execute();
   bool Interrupt();
   void Clear();

protected:
    
private:
   /// Enumerated values for the current Sandbox state
   enum runMode
   {
      IDLE = 7001,
      INITIALIZED,
      RUNNING,
      PAUSED,
      STOPPED,
      RESET
   };
    
   /// Object store for this run
   std::map<std::string, GmatBase *> objectMap;
   /// Solar System model for this Sandbox
   SolarSystem   *solarSys;
   /// GMAT Publisher singleton
   Publisher     *publisher;
   /// Command sequence for the run
   GmatCommand   *sequence;
   /// The executing Command
   GmatCommand   *current;
   /// Pointer to the moderator, used to check Pause/Stop status
   Moderator     *moderator;
   /// Execution mode for the sandbox
   runMode       state;

   Sandbox(const Sandbox&);
   Sandbox& operator=(const Sandbox&);
};

#endif
