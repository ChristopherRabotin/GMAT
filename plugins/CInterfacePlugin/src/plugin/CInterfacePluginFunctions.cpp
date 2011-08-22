//$Id: GmatOdtbxFunctions.cpp 9544 2011-05-18 21:45:36Z djcinsb $
//------------------------------------------------------------------------------
//                          CInterfacePluginFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
// ODTBX: Orbit Determination Toolbox
//
// **Legal**
//
// Developed jointly by NASA/GSFC, Emergent Space Technologies, Inc.
// and Thinking Systems, Inc. under the FDSS contract, Task 28
//
// Author: Darrel J. Conway (Thinking Systems)
// Created: 2011/03/22
//
/**
 * Implementation of library code interfaces needed by GMAT.
 */
//------------------------------------------------------------------------------

#include "CInterfacePluginFunctions.hpp"
#include "MessageInterface.hpp"
#include "Moderator.hpp"
#include "ODEModel.hpp"
#include "Propagate.hpp"
#include "Factory.hpp"

#include "CCommandFactory.hpp"


// Library globals - lastMsg or internal statics are needed for messaging, the 
// others (ode, pSetup) are here for convenience but could be made internal to
// the functions.
ODEModel    *ode = NULL;
PropSetup   *pSetup = NULL;
std::string lastMsg = "";
std::string extraMsg = "";

extern "C"
{
   //---------------------------------------------------------------------------
   // Integer GetFactoryCount()
   //---------------------------------------------------------------------------
   /**
    * Returns the number of plug-in factories in this module
    *
    * @return The number of factories
    */
   //---------------------------------------------------------------------------
   Integer GetFactoryCount()
   {
      return 1;
   }
   
   //---------------------------------------------------------------------------
   // Factory* GetFactoryPointer(Integer index)
   //---------------------------------------------------------------------------
   /**
    * Retrieves a pointer to a specific factory
    *
    * @param index The index to the Factory
    *
    * @return The Factory pointer
    */
   //---------------------------------------------------------------------------
   Factory* GetFactoryPointer(Integer index)
   {
      Factory* factory = NULL;

      switch (index)
      {
         case 0:
            factory = new CCommandFactory;
            break;
            
         default:
            break;
      }

      return factory;
   }
   
   //---------------------------------------------------------------------------
   // void SetMessageReceiver(MessageReceiver* mr)
   //---------------------------------------------------------------------------
   /**
    * Sets the messaging interface used for GMAT messages
    *
    * @param mr The message receiver
    */
   //---------------------------------------------------------------------------
   void SetMessageReceiver(MessageReceiver* mr)
   {
      MessageInterface::SetMessageReceiver(mr);
   }


   //---------------------------------------------------------------------------
   // Client Interface functions
   //---------------------------------------------------------------------------
   
   //---------------------------------------------------------------------------
   // const char* getLastMessage()
   //---------------------------------------------------------------------------
   /**
    * Returns a status message
    */
   //---------------------------------------------------------------------------
   const char* getLastMessage()
   {
      if (lastMsg == "")
         lastMsg = "getLastMessage() called; message is empty\n";
      return lastMsg.c_str();
   }

   //---------------------------------------------------------------------------
   // unsigned int StartGmat()
   //---------------------------------------------------------------------------
   /**
    * Starts GMAT running!
    *
    * @return A status flag indicating the status upon return; 0 means success
    */
   //---------------------------------------------------------------------------
   int StartGmat()
   {
      Moderator *theModerator = Moderator::Instance();
      if (theModerator == NULL)
         return -1;

      if (theModerator->Initialize() == false)
         return -2;

      lastMsg = "The Moderator has been initialized";
      ode     = NULL;
      pSetup  = NULL;

      return 0;
   }

   //---------------------------------------------------------------------------
   // int LoadScript(const char* scriptName)
   //---------------------------------------------------------------------------
   /**
    * Loads a scripted configuration into GMAT
    *
    * @param scriptName The file name of the script containing the configuration
    *
    * @return A status flag indicating the status upon return; 0 means success
    */
   //---------------------------------------------------------------------------
   int LoadScript(const char* scriptName)
   {
      int retval = -1;

      Moderator *theModerator = Moderator::Instance();
      if (theModerator == NULL)
      {
         lastMsg = "Cannot find the Moderator";
         return retval;
      }

      std::string script = scriptName;

      if (theModerator->InterpretScript(scriptName))
      {
         lastMsg = "Interpreted the script " + script + " successfully.";
         retval = 0;
      }
      else
      {
         lastMsg = "The script " + script + " failed to load.";
         retval = -2;
      }

      return retval;
   }

   //---------------------------------------------------------------------------
   // int RunScript()
   //---------------------------------------------------------------------------
   /**
    * Runs a GMAT script.  This is needed to fully establish the connections 
    * between objects in the GMAT Sandbox
    *
    * @return A status flag indicating the status upon return; 0 means success
    */
   //---------------------------------------------------------------------------
   int RunScript()
   {
      int retval = -1;

      Moderator *theModerator = Moderator::Instance();
      if (theModerator == NULL)
      {
         lastMsg = "Cannot find the Moderator";
         return retval;
      }

      retval = theModerator->RunMission();
      switch (retval)
      {
      case 1:
         lastMsg = "Mission run succeeded!";
         retval = 0;
         break;

      case -1:
         lastMsg = "Sandbox number is invalid";
         break;

      case -2:
         lastMsg = "Execution interrupted by user";
         break;

      case -3:
         lastMsg = "Exception thrown during the run";
         break;

      case -4:
      default:
         lastMsg = "unknown error occurred";
         break;
      }

      return retval;
   }

   //---------------------------------------------------------------------------
   // int LoadAndRunScript(const char* scriptName)
   //---------------------------------------------------------------------------
   /**
    * Loads a script and runs it.
    *
    * @note: This method is not yet ready, and should not be used
    *
    * @param scriptName The file name of the script containing the configuration
    *
    * @return A status flag indicating the status upon return; 0 means success
    */
   //---------------------------------------------------------------------------
   int LoadAndRunScript(const char* scriptName)
   {
      lastMsg = "LoadAndRunScript is not yet ready for use.";
      return -1;

      int retval = LoadScript(scriptName);

      if (retval == 0)
      {
         retval = RunScript();
      }

      return retval;
   }

   //---------------------------------------------------------------------------
   // int FindOdeModel(const char* modelName)
   //---------------------------------------------------------------------------
   /**
    * Finds an ODE model in the GMAT Sandbox
    *
    * @param modelName The name of the model (not yet used)
    *
    * @return A status flag indicating the status upon return; 0 means success
    *
    * @note The current implementation finds the first ODE model and sets the 
    *       libary pointer to it.  A later implementation will find the instance 
    *       by name.
    */
   //---------------------------------------------------------------------------
   int FindOdeModel(const char* modelName)
   {
      int retval = -1;
      ode = NULL;
      pSetup = NULL;

      Moderator *theModerator = Moderator::Instance();
      if (theModerator == NULL)
      {
         lastMsg = "Cannot find the Moderator";
         return retval;
      }

      GmatCommand *current = theModerator->GetFirstCommand(1);
      GetODEModel(current, modelName);

      if (ode != NULL)
         retval = 0;
      else
         retval = -2;
   
      return retval;
   }

   //---------------------------------------------------------------------------
   // int GetStateSize()
   //---------------------------------------------------------------------------
   /**
    * Retrieves the size of GMAT's propagation state vector for the ODEModel
    *
    * @return The state vector size, or 0 if the state vector is not available.
    */
   //---------------------------------------------------------------------------
   int GetStateSize()
   {
      int retval = 0;

      if (ode != NULL)
         retval = ode->GetDimension();

      return retval;
   }

   //---------------------------------------------------------------------------
   // const char *GetStateDescription()
   //---------------------------------------------------------------------------
   /**
    * Retrieves a test description of GMAT's propagation state vector, element 
    * by element.
    *
    * @return The state vector description
    */
   //---------------------------------------------------------------------------
   const char *GetStateDescription()
   {
      lastMsg = "";

      if (ode != NULL)
      {
         GmatState *theState = pSetup->GetPropStateManager()->GetState();
         StringArray desc = theState->GetElementDescriptions();
         for (unsigned int i = 0; i < desc.size(); ++i)
            lastMsg += "   " + desc[i] + "\n";
      }

      return lastMsg.c_str();
   }

   //---------------------------------------------------------------------------
   // int SetState(double epoch, double state[], int stateDim)
   //---------------------------------------------------------------------------
   /**
    * Sets the data in GMAT's propagation state vector
    *
    * @param epoch The epoch of the state used in the calculation
    * @param state The input state vector; it must be sized at or below the size
    *              of the output vector.
    * @param stateDim The size of the input state vector
    *
    * @return A status flag indicating the status upon return; 0 means success
    */
   //---------------------------------------------------------------------------
   int SetState(double epoch, double state[], int stateDim)
   {
      int retval = -1;

      if (pSetup != NULL)
      {
         GmatState *theState = pSetup->GetPropStateManager()->GetState();

         if (stateDim <= theState->GetSize())
         {
            theState->SetEpoch(epoch);
            theState->SetState(state, stateDim);

            retval = 0;
         }
         else
         {
            char msg[128];
            sprintf(msg, "ERROR: Incoming state size (%d) is larger than the "
                  "propagation state vector size (%d)!\n   Epoch: %lf\n   "
                  "State = [%lf %lf %lf %lf %lf %lf]\n", stateDim,
                  theState->GetSize(), epoch, state[0], state[1], state[2],
                  state[3], state[4], state[5]);
            lastMsg = msg;
            retval = -2;
         }
      }
      else
         lastMsg = "ERROR: The propagation setup is not yet set.";


      return retval;
   }

   //---------------------------------------------------------------------------
   // double *GetState()
   //---------------------------------------------------------------------------
   /**
    * Retrieves the propagation state vector
    *
    * @return A pointer to the state vector, or NULL if it is not set
    */
   //---------------------------------------------------------------------------
   double *GetState()
   {
      double *retval = NULL;

      if (pSetup != NULL)
      {
         retval = pSetup->GetPropStateManager()->GetState()->GetState();
      }
      else
         lastMsg = "ERROR: The propagation setup is not yet set.";

      return retval; 
   }

   //---------------------------------------------------------------------------
   // double *GetDerivativesForState(double epoch, double state[], int stateDim, 
   //      double dt, int order, int *pdim)
   //---------------------------------------------------------------------------
   /**
    * Calculates and returns the derivative of the input state vector
    *
    * One side effect of this call is that the internal state vector is set to
    * the input data.  This is necessary to ensure that full dimensionality is
    * preserved.
    *
    * @param epoch The epoch of the state used in the calculation
    * @param state The input state vector; it must be sized at or below the size
    *              of the output vector.
    * @param stateDim The size of the input state vector
    * @param dt Time offset (in sec) off of the input epoch
    * @param order Order of the derivative data returned -- 1 or 2 for first or
    *              second derivative data
    * @param pdim Output variable that contains the size of the derivative
    *             vector being returned
    *
    * @return The derivative data in a real array, or NULL if the derivatives 
    *         cannot be calculated
    */
   //---------------------------------------------------------------------------
   double *GetDerivativesForState(double epoch, double state[], 
         int stateDim, double dt, int order, int *pdim)
   {
      double *retval = NULL;
      static double *deriv = NULL;
      if (ode != NULL)
      {
         if (SetState(epoch, state, stateDim) == 0)
         {
            double *dvState = GetState();
            *pdim = GetStateSize();
            if (deriv == NULL)
               deriv = new double[*pdim];
            ode->GetDerivatives(dvState, dt, order);
            const double *ddt = ode->GetDerivativeArray();
            memcpy(deriv, ddt, *pdim * sizeof(double));
            retval = deriv;
         }
      }
      return retval;
   }

   //---------------------------------------------------------------------------
   // double *GetDerivatives(double dt, int order, int *pdim)
   //---------------------------------------------------------------------------
   /**
    * Calculates and returns the derivative of GMAT's internal state vector
    *
    * @param dt Time offset (in sec) off of the input epoch
    * @param order Order of the derivative data returned -- 1 or 2 for first or
    *              second derivative data
    * @param pdim Output variable that contains the size of the derivative
    *             vector being returned
    *
    * @return The derivative data in a real array, or NULL if the derivatives 
    *         cannot be calculated
    */
   //---------------------------------------------------------------------------
   double *GetDerivatives(double dt, int order, int *pdim)
   {
      double *retval = NULL;
      static double *deriv = NULL;
      if (ode != NULL)
      {
         double *state = GetState();
         int dim = GetStateSize();
         *pdim = dim;
         if (deriv == NULL)
            deriv = new double[dim];
         ode->GetDerivatives(state, dt, order);
         const double *ddt = ode->GetDerivativeArray();
         memcpy(deriv, ddt, dim * sizeof(double));
         retval = deriv;
      }
      return retval;
   }

   //---------------------------------------------------------------------------
   // int CountObjects()
   //---------------------------------------------------------------------------
   /**
    * Determins how many objects exist in GMAT's configuration manager
    *
    * @return The count of the objects
    */
   //---------------------------------------------------------------------------
   int CountObjects()
   {
      int retval = -1;

      Moderator *theModerator = Moderator::Instance();
      if (theModerator == NULL)
      {
         lastMsg = "Cannot find the Moderator";
         return retval;
      }

      StringArray objects = theModerator->GetListOfObjects(
            Gmat::UNKNOWN_OBJECT);
      retval = objects.size();

      return retval;
   }

   //---------------------------------------------------------------------------
   // const char *GetObjectName(int which)
   //---------------------------------------------------------------------------
   /**
    * Retrieves the name of the object at the input index 
    *
    * @param which The index of the object being queried
    *
    * @return The name of the object at index = which
    */
   //---------------------------------------------------------------------------
   const char *GetObjectName(int which)
   {
      Moderator *theModerator = Moderator::Instance();
      if ((theModerator == NULL) || (which < 0))
      {
         lastMsg = "Cannot find the Moderator";
         return "Error accessing the Moderator";
      }

      StringArray objects = theModerator->GetListOfObjects(
            Gmat::UNKNOWN_OBJECT);
      if ((int)objects.size() > which)
      {
         lastMsg = objects[which];
      }

      return lastMsg.c_str();
   }

   //---------------------------------------------------------------------------
   // const char *GetRunSummary()
   //---------------------------------------------------------------------------
   /**
    * Retrieves command summary data for a GMAT run
    *
    * @return The summary data for each command in the mission control sequence
    */
   //---------------------------------------------------------------------------
   const char *GetRunSummary()
   {
      Moderator *theModerator = Moderator::Instance();
      if (theModerator == NULL)
      {
         lastMsg = "Cannot find the Moderator";
         return "Error accessing the Moderator";
      }

      GmatCommand *current = theModerator->GetFirstCommand(1);
      lastMsg = "";
      while (current != NULL)
      {
         if (current->GetTypeName() != "NoOp")
         {
            lastMsg += current->GetStringParameter("Summary");
            lastMsg += "\n-----------------------------------\n";
         }
         current = current->GetNext();
      }

      return lastMsg.c_str();
   }

};  // End of extern "C"


//------------------------------------------------------------------------------
// Internal functions
//------------------------------------------------------------------------------
   
//------------------------------------------------------------------------------
// ODEModel *GetODEModel(GmatCommand *cmd, std::string modelName)
//------------------------------------------------------------------------------
/**
   * Retrieves a PropSetup from the mission control sequence
   *
   * Retrieves a PropSetup from the mission control sequence and sets the ode 
   * pointer to it, along with the pSetup pointer to its owning PropSetup.
   *
   * @param cmd The starting command in teh mission control sequence
   * @param modelName The name of the model that is wanted.  Currently not used.
   *
   * @return The ODEModel pointer, or NULL if it was not found
   *
   * @note The current implementation returns the first ODEModel found.
   */
//------------------------------------------------------------------------------
void GetODEModel(GmatCommand *cmd, const char *modelName)
{
   ODEModel* model = NULL;
   ode = NULL;
   pSetup = NULL;

   char extraMsg[256] = "";

   PropSetup *prop = GetFirstPropagator(cmd);
   if (prop != NULL)
   {
      model = prop->GetODEModel();
      lastMsg = "In GetODEModel; found prop";
      if (model != NULL)
      {
         #ifdef DEBUG_ODE_SEARCH
            lastMsg += ", model != NULL\n";
            lastMsg += ::extraMsg;
         #else
            lastMsg += " ";
         #endif
         lastMsg += model->GetName();
         lastMsg += "\n";
         pSetup = prop;
         ode = model;
         return;
         if (ode != NULL)
            sprintf(extraMsg, "%s", ode->GetName().c_str());
      }
      else
         lastMsg += ", model == NULL\n";
   }
   else
   {
      lastMsg = "In GetODEModel; did not find prop\n";
      lastMsg += ::extraMsg;
   }

   if (ode != NULL)
   {
      lastMsg = "ODE model is set to ";
      lastMsg += extraMsg;
   }
   else
   {
      lastMsg += "No ODE model found\n";
   }
}

//------------------------------------------------------------------------------
// PropSetup *GetFirstPropagator(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
   * Finds the first PropSetup in teh mission control sequence
   * 
   * @param cmd The first command in the mission control sequence
   * 
   * @return The first PropSetup found, or NULL if none were located
   */
//------------------------------------------------------------------------------
PropSetup *GetFirstPropagator(GmatCommand *cmd)
{
   static PropSetup *retval = NULL;
   GmatCommand *current = cmd;

   #ifdef DEBUG_ODE_SEARCH
      extraMsg = "Commands checked:\n";
   #endif
   while (current != NULL)
   {
      #ifdef DEBUG_ODE_SEARCH
            extraMsg += "   '" + current->GetTypeName() + "'\n";
      #endif
      if (current->GetTypeName() == "Propagate")
      {
         try
         {
            // Set all of the internal connections
//               current->TakeAction("PrepareToPropagate");
            current->Execute();
         }
         catch (BaseException &ex)
         {
            lastMsg = ex.GetFullMessage();
         }
         #ifdef DEBUG_ODE_SEARCH
            extraMsg += "      Checking in this command\n";
         #endif
         GmatBase *obj = current->GetRefObject(Gmat::PROP_SETUP, "", 0);

         #ifdef DEBUG_ODE_SEARCH
            if (obj != NULL)
               extraMsg += "      Found an object of type PROPSETUP\n";
            else
               extraMsg += "      Propagate command returns NULL PROPSETUP\n";
         #endif

         if (obj->IsOfType("PropSetup"))
         {
            retval = (PropSetup*)(obj);
            break;
         }
      }
      current = current->GetNext();
   }

   return retval;
}
