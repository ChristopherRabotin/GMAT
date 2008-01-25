//$Id$
//------------------------------------------------------------------------------
//                              ForceModel
//------------------------------------------------------------------------------
// *** File Name : ForceModel.cpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  Copyright U.S. Government 2002                                    ***
// ***  Copyright United States Government as represented by the          ***
// ***  Administrator of the National Aeronautics and Space               ***
// ***  Administration                                                    ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 2/28/2003 - D. Conway, Thinking Systems, Inc.
//                             Filled in some code to model cases with 
//                             discontinuous forces (e.g. SRP)
//
//                           : 10/01/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//                                - Changed FlightDynamicsForces class to ForceModel class
//
//                           : 10/20/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//
//                           : 11/9/2003 - D. Conway, Thinking Systems, Inc.
//                             Overrode GetParameterCount so the count 
//                             increases based on the member forces
// **************************************************************************

#include "ForceModel.hpp"
#include "PointMassForce.hpp"
#include "MessageInterface.hpp"
#include "Formation.hpp"      // for BuildState()

#include <string.h> 


//#define DEBUG_FORCEMODEL_INIT
//#define DEBUG_FORCEMODEL_EXE
//#define FORCE_REFERENCE_OBJECTS
//#define DEBUG_FORCEMODEL_EPOCHS
//#define DEBUG_SATELLITE_PARAMETERS
//#define DEBUG_FIRST_CALL
//#define DEBUG_GEN_STRING
//#define DEBUG_OWNED_OBJECT_STRINGS

//---------------------------------
// static data
//---------------------------------

#ifdef DEBUG_FIRST_CALL
static bool firstCallFired = false;
#endif

const std::string
ForceModel::PARAMETER_TEXT[ForceModelParamCount - PhysicalModelParamCount] =
{
   "CentralBody",
   "PrimaryBodies",
   "PointMasses",
   "Drag",
   "SRP",
   "ErrorControl",
   "CoordinateSystemList"
};


const Gmat::ParameterType
ForceModel::PARAMETER_TYPE[ForceModelParamCount - PhysicalModelParamCount] =
{
   //Gmat::STRING_TYPE,       // "CentralBody",
   Gmat::OBJECT_TYPE,       // "CentralBody",
   Gmat::STRINGARRAY_TYPE,  // "PrimaryBodies",
   Gmat::STRINGARRAY_TYPE,  // "PointMasses",
   Gmat::STRING_TYPE,       // "Drag",
   //Gmat::STRING_TYPE,       // "SRP",
   Gmat::ON_OFF_TYPE,       // "SRP",
   Gmat::STRING_TYPE,       // "ErrorControl",
   Gmat::STRINGARRAY_TYPE   // "CoordinateSystemList"
};


// Table of alternative words used in force model scripting
std::map<std::string, std::string> ForceModel::scriptAliases;


//---------------------------------
// public
//---------------------------------


//------------------------------------------------------------------------------
// ForceModel::ForceModel(Gmat::ObjectType id, const std::string &typeStr,
//                        const std::string &nomme))
//------------------------------------------------------------------------------
/**
 * The constructor
 */
//------------------------------------------------------------------------------
ForceModel::ForceModel(const std::string &nomme) :
   PhysicalModel     (Gmat::FORCE_MODEL, "ForceModel", nomme),
   previousState     (NULL),
   estimationMethod  (ESTIMATE_LOCALLY),
   normType          (L2_DIFFERENCES),
   parametersSetOnce (false),
   centralBodyName   ("Earth"),
   forceMembersNotInitialized (true),
   modelEpochId      (-1),
   j2kBodyName       ("Earth"),
   j2kBody           (NULL),
   earthEq           (NULL),
   earthFixed        (NULL)
{
   satIds[0] = satIds[1] = satIds[2] = satIds[3] = satIds[4] = 
   satIds[5] = satIds[6] = -1;
   
   objectTypes.push_back(Gmat::FORCE_MODEL);
   objectTypeNames.push_back("ForceModel");

   numForces = 0;
   stateSize = 6;
   dimension = 6;
   currentForce = 0;
   parameterCount = ForceModelParamCount;
}


//------------------------------------------------------------------------------
// ForceModel::~ForceModel()
//------------------------------------------------------------------------------
/**
 * The destructor
 * The destructor deletes the list of PhysicalModel instances.
 */
//------------------------------------------------------------------------------
ForceModel::~ForceModel()
{
   if (previousState)
      delete [] previousState;
//   if (rawState)
//      delete [] rawState;   
        
   // Delete the owned forces
   std::vector<PhysicalModel *>::iterator ppm = forceList.begin();
   PhysicalModel *pm;
   while (ppm != forceList.end()) {
      pm = *ppm;
      forceList.erase(ppm);
      // Transient forces are managed in the Sandbox.
      if (!pm->IsTransient())
         delete pm;
      ppm = forceList.begin();
   }
   
   ClearInternalCoordinateSystems();

   #ifdef DEBUG_FORCE_EPOCHS
      if (epochFile.is_open())
         epochFile.close();
   #endif
}

//------------------------------------------------------------------------------
// ForceModel::ForceModel(const ForceModel& fdf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * NOTE: The ForceModel copy constructor is not yet implemented.  This
 * method should be completed before the class is used in external code.
 *
 * @param fdf   The original of the ForceModel that is copied
 * 
 * @todo Check the PhysicalModel copy constructors and assignment operators.
 */
//------------------------------------------------------------------------------
ForceModel::ForceModel(const ForceModel& fdf) :
   PhysicalModel              (fdf),
   previousState              (fdf.previousState),
   estimationMethod           (fdf.estimationMethod),
   normType                   (fdf.normType),
   parametersSetOnce          (false),
   centralBodyName            (fdf.centralBodyName),
   forceMembersNotInitialized (true),
   modelEpochId               (-1),
   j2kBodyName                (fdf.j2kBodyName),
   /// @note: Since the next three are global objects or reset by the Sandbox, 
   ///assignment works
   j2kBody                    (fdf.j2kBody),
   earthEq                    (fdf.earthEq),
   earthFixed                 (fdf.earthFixed)
{
   satIds[0] = satIds[1] = satIds[2] = satIds[3] = satIds[4] = 
   satIds[5] = satIds[6] = -1;

   numForces           = fdf.numForces;
   stateSize           = fdf.stateSize;
   dimension           = fdf.dimension;
   currentForce        = fdf.currentForce;
   forceTypeNames      = fdf.forceTypeNames;
   previousTime        = fdf.previousTime;
   transientForceNames = fdf.transientForceNames;
   forceReferenceNames = fdf.forceReferenceNames;
   
   parameterCount = ForceModelParamCount;
   
   spacecraft.clear();
   forceList.clear();
   InternalCoordinateSystems.clear();
   
   // Copy the forces.  May not work -- the copy constructors need to be checked
   for (std::vector<PhysicalModel *>::const_iterator pm = fdf.forceList.begin();
        pm != fdf.forceList.end(); ++pm)
   {
      forceList.push_back((PhysicalModel*)(*pm)->Clone());
   }
}


//------------------------------------------------------------------------------
// ForceModel& ForceModel::operator=(const ForceModel& fdf)
//------------------------------------------------------------------------------
/**
 * The assignment operator.
 * 
 * NOTE: The ForceModel assignment operator is not yet tested.  This method 
 *       should be validated before the class is used in external code.
 * 
 * @param fdf   The original of the ForceModel that are copied
 */
//------------------------------------------------------------------------------
ForceModel& ForceModel::operator=(const ForceModel& fdf)
{
   if (&fdf == this)
        return *this;

   satIds[0] = satIds[1] = satIds[2] = satIds[3] = satIds[4] = 
   satIds[5] = satIds[6] = -1;

   numForces           = fdf.numForces;
   stateSize           = fdf.stateSize;
   dimension           = fdf.dimension;
   currentForce        = fdf.currentForce;
   forceTypeNames      = fdf.forceTypeNames;
   previousTime        = fdf.previousTime;
   previousState       = fdf.previousState;
   estimationMethod    = fdf.estimationMethod;
   normType            = fdf.normType;
   transientForceNames = fdf.transientForceNames;
   forceReferenceNames = fdf.forceReferenceNames;
   parametersSetOnce   = false;
   modelEpochId        = -1;
   
   parameterCount      = ForceModelParamCount;
   centralBodyName     = fdf.centralBodyName;
   j2kBodyName         = fdf.j2kBodyName;
   
   /// @note: Since the next three are global objects or reset by the Sandbox, 
   ///assignment works
   j2kBody             = fdf.j2kBody;
   earthEq             = fdf.earthEq;
   earthFixed          = fdf.earthFixed; 
   forceMembersNotInitialized = fdf.forceMembersNotInitialized;
   
   spacecraft.clear();
   forceList.clear();
   InternalCoordinateSystems.clear();
   
   // Copy the forces.  May not work -- the copy constructors need to be checked
   for (std::vector<PhysicalModel *>::const_iterator pm = fdf.forceList.begin();
        pm != fdf.forceList.end(); ++pm)
      forceList.push_back((PhysicalModel*)(*pm)->Clone());

   return *this;
}

//------------------------------------------------------------------------------
// void ForceModel::AddForce(PhysicalModel *pPhysicalModel)
//------------------------------------------------------------------------------
/**
 * Method used to add a new force to the force model
 *
 * This method takes the pointer to the new force and adds it to the force model
 * list for later use.  Each force should supply either first derivative 
 * information for elements 4 through 6 of a state vector and zeros for the 
 * first three elements, or second derivative information in elements 1 through 
 * 3 and zeroes in 4 through 6 for second order integrators.  The forces should 
 * have the ability to act on state vectors for formations as well, by filling 
 * in elements (6*n+4) through (6*n+6) for larger state vectors.  Some forces 
 * also affect the mass properties of the spacecraft; these elements are updated 
 * using a TBD data structure.
 * 
 * The force that is passed in is owned by this class, and should not be 
 * destructed externally.  In addition, every force that is passed to this class 
 * needs to have a copy constructor and an assignment operator so that it can be 
 * cloned for distribution to multiple sandboxes.
 * 
 * @param pPhysicalModel        The force that is being added to the force model
 * 
 * @todo Document the mass depletion approach.
 */
//------------------------------------------------------------------------------
void ForceModel::AddForce(PhysicalModel *pPhysicalModel)
{
   if (pPhysicalModel == NULL)
      throw ForceModelException("Attempting to add a NULL force to " +
         instanceName);

   #ifdef DEBUG_FORCEMODEL_INIT
      MessageInterface::ShowMessage(
         "ForceModel::AddForce() entered for a %s force\n", 
         pPhysicalModel->GetTypeName().c_str());
   #endif       
    
   pPhysicalModel->SetDimension(dimension);
   initialized = false;
    
   // Handle the name issues
   std::string pmType = pPhysicalModel->GetTypeName();
   if (pmType == "DragForce")
      pPhysicalModel->SetName("Drag");

   std::string forceBody = pPhysicalModel->GetBodyName();

   // Trap multiple instances
   if ((pmType == "GravityField") || (pmType == "PointMassForce"))
   {
      std::string compType;
      
      for (std::vector<PhysicalModel*>::iterator i = forceList.begin();
           i != forceList.end(); ++i)
      {
         compType = (*i)->GetTypeName();
         if ((compType == "GravityField") || (compType == "PointMassForce"))
         {
            if ((*i)->GetBodyName() == forceBody)
               throw ForceModelException(
                  "Attempted to add a " + pmType + 
                  " force to the force model for the body " + forceBody +
                  ", but there is already a " + compType + 
                  " force in place for that body.");
         }
      }      
   }

   // Check to be sure there is an associated PrimaryBody for drag forces
   if (pmType == "DragForce")
   {
      bool hasGravityField = false;
      for (std::vector<PhysicalModel*>::iterator i = forceList.begin();
           i != forceList.end(); ++i)
      {
         if ((*i)->GetTypeName() == "GravityField")
         {
            if ((*i)->GetBodyName() == forceBody)
               hasGravityField = true;
         }
      }
      if (hasGravityField == false)
         throw ForceModelException(
            "Attempted to add a drag force for the body " + forceBody +
            ", but that body is not set as a primary body, so it does not " +
            "support additional forces.");
   }
   
   forceList.push_back(pPhysicalModel);
   numForces = forceList.size();
}

//------------------------------------------------------------------------------
// void DeleteForce(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Deletes named force from the force model.
 * 
 * @param name The name of the force to delete
 */
//------------------------------------------------------------------------------
void ForceModel::DeleteForce(const std::string &name)
{
    for (std::vector<PhysicalModel *>::iterator force = forceList.begin(); 
         force != forceList.end(); ++force) 
    {
        std::string pmName = (*force)->GetName();
        if (name == pmName)
        {
            forceList.erase(force);
            numForces = forceList.size();
            return;
        }
    }
}


//------------------------------------------------------------------------------
// void DeleteForce(PhysicalModel *pPhyscialModel)
//------------------------------------------------------------------------------
/**
 * Deletes force from the force model.
 * 
 * @param pPhyscialModel The force name to delete
 */
//------------------------------------------------------------------------------
void ForceModel::DeleteForce(PhysicalModel *pPhysicalModel)
{
    for (std::vector<PhysicalModel *>::iterator force = forceList.begin();
         force != forceList.end(); ++force) 
    {
        if (*force == pPhysicalModel)
        {
            forceList.erase(force);
            numForces = forceList.size();
            return;
        }
    }
}


//------------------------------------------------------------------------------
// bool HasForce(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Search force in the force model.
 * 
 * @param  name The force name to look up
 * @return true if force exists, else false
 */
//------------------------------------------------------------------------------
bool ForceModel::HasForce(const std::string &name)
{
   for (std::vector<PhysicalModel *>::iterator force = forceList.begin(); 
       force != forceList.end(); force++) 
   {
      if (name == (*force)->GetName())
         return true;
   }
   return false; 
}


//------------------------------------------------------------------------------
// Integer GetNumForces()
//------------------------------------------------------------------------------
Integer ForceModel::GetNumForces()
{
    return numForces;
}


//------------------------------------------------------------------------------
// StringArray& GetForceTypeNames()
//------------------------------------------------------------------------------
StringArray& ForceModel::GetForceTypeNames()
{
    forceTypeNames.clear();

    for (int i=0; i<numForces; i++)
        forceTypeNames.push_back(forceList[i]->GetTypeName());
        
    return forceTypeNames;
}

//------------------------------------------------------------------------------
// std::string GetForceTypeName(Integer index)
//------------------------------------------------------------------------------
std::string ForceModel::GetForceTypeName(Integer index)
{
    StringArray typeList = GetForceTypeNames();
    
    if (index >= 0 && index < numForces)
        return typeList[index];

    return "UNDEFINED_FORCE_TYPE";
}


//------------------------------------------------------------------------------
// void ClearSpacecraft()
//------------------------------------------------------------------------------
void ForceModel::ClearSpacecraft()
{
    spacecraft.clear();
}


//------------------------------------------------------------------------------
// PhysicalModel* GetForce(Integer index)
//------------------------------------------------------------------------------
PhysicalModel* ForceModel::GetForce(Integer index)
{
    if (index >= 0 && index < numForces)
       return forceList[index];
    
    return NULL;
}


//------------------------------------------------------------------------------
// PhysicalModel* GetForce(std::string forcetype, Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Search for a force in the force model.
 * 
 * @param  forcetype The kind of force desired.
 * @param  whichOne  Which force (zero-based index) of that type is desired.
 * 
 * @return The pointer to that force instance.
 */
//------------------------------------------------------------------------------
const PhysicalModel* ForceModel::GetForce(std::string forcetype, 
                                          Integer whichOne) const
{
   Integer i = 0;

   for (std::vector<PhysicalModel *>::const_iterator force = forceList.begin(); 
        force != forceList.end(); ++force) 
   {
      std::string pmName = (*force)->GetTypeName();
      if (pmName == forcetype) {
         if (whichOne <= i)
            return *force;
         else
            ++i;
      }
   }

   return NULL;
}


//------------------------------------------------------------------------------
// bool ForceModel::AddSpaceObject(SpaceObject *so)
//------------------------------------------------------------------------------
/**
 * Sets spacecraft and formations that use this force model.
 *
 * @param so The spacecraft or formation
 *
 * @return true is the object is added to the list, false if it was already
 *         in the list, or if it is NULL.
 */
//------------------------------------------------------------------------------
bool ForceModel::AddSpaceObject(SpaceObject *so)
{
    if (so == NULL)
        return false;
        
    if (find(spacecraft.begin(), spacecraft.end(), so) != spacecraft.end())
        return false;

    std::string soJ2KBodyName = so->GetStringParameter("J2000BodyName");

    // Set the refence body for the spacecraft states to match the J2000 body
    // on the first spacecraft added to the force model.
    if (spacecraft.size() == 0)
       j2kBodyName = soJ2KBodyName;
    else
    {
       if (j2kBodyName != soJ2KBodyName)
          throw ForceModelException(
             "Force model error -- the internal reference body for all "
             "spacecraft in a force model must be the same.\n"
             "The J2000Body for " + so->GetName() + " is " + soJ2KBodyName +
             ", but the force model is already using " + j2kBodyName + 
             " as the reference body.");
    }
    
    spacecraft.push_back(so);

    // Quick fix for the epoch update
    satIds[0] = so->GetParameterID("A1Epoch");
    epoch = so->GetRealParameter(satIds[0]);
    parametersSetOnce = false;
    return true;
}


//------------------------------------------------------------------------------
// void ForceModel::UpdateSpaceObject(Real newEpoch)
//------------------------------------------------------------------------------
/**
 * Updates state data for the spacecraft or formation that use this force model.
 */
//------------------------------------------------------------------------------
void ForceModel::UpdateSpaceObject(Real newEpoch)
{
   if (spacecraft.size() > 0) {
      Integer j = 0;
      Integer stateSize;
      Integer vectorSize;
      std::vector<SpaceObject *>::iterator sat;
      PropState *state;

      ReturnFromOrigin(newEpoch);
      
      for (sat = spacecraft.begin(); sat != spacecraft.end(); ++sat) {
         state = &((*sat)->GetState());
         stateSize = state->GetSize();
         vectorSize = stateSize * sizeof(Real);
         memcpy(&previousState[j*stateSize], state->GetState(), vectorSize);
         previousTime = 
            ((*sat)->GetRealParameter(satIds[0]) - epoch)
            * 86400.0;
            
         memcpy(state->GetState(), &rawState[j*stateSize], vectorSize);
         ++j;
            
         // Quick fix to get the epoch updated
         Real newepoch = epoch + elapsedTime / 86400.0;      

         // Update the epoch if it was passed in
         if (newEpoch != -1.0)
            newepoch = newEpoch;
         
         (*sat)->SetRealParameter(satIds[0], newepoch);
         #ifdef DEBUG_FORCEMODEL_EXE
             MessageInterface::ShowMessage
                ("ForceModel::UpdateSpacecraft() on \"%s\" prevElapsedTime=%f "
                 "elapsedTime=%f newepoch=%f\n", (*sat)->GetName().c_str(), 
                 prevElapsedTime, elapsedTime, newepoch);
         #endif
         if ((*sat)->GetType() == Gmat::FORMATION)
            ((Formation*)(*sat))->UpdateElements();
      }
   }
}


//------------------------------------------------------------------------------
// void UpdateFromSpacecraft()
//------------------------------------------------------------------------------
/**
 * Updates the model state data from the spacecraft state -- useful to revert
 * to a previous step.
 *
 * @note This method will need to be updated when the multi-step integrators are
 *       folded into the code
 */
//------------------------------------------------------------------------------
void ForceModel::UpdateFromSpaceObject()
{
    if (spacecraft.size() > 0) 
    {
        Integer j = 0;
        Integer stateSize;
        std::vector<SpaceObject *>::iterator sat;
        PropState *state;
        for (sat = spacecraft.begin(); sat != spacecraft.end(); ++sat) 
        {
            state = &((*sat)->GetState());
            stateSize = state->GetSize();
            memcpy(&rawState[j*stateSize], state->GetState(), 
                   stateSize * sizeof(Real));
            ++j;
        }
    }
    
    // Transform to the force model origin
    MoveToOrigin();
}


//------------------------------------------------------------------------------
// void RevertSpacecraft()
//------------------------------------------------------------------------------
/**
 * Resets the model state data from the previous spacecraft state.
 *
 * @note This method will need to be updated when the multi-step integrators are
 *       folded into the code
 */
//------------------------------------------------------------------------------
void ForceModel::RevertSpaceObject()
{
   #ifdef DEBUG_FORCEMODEL_EXE
      MessageInterface::ShowMessage
         ("ForceModel::RevertSpacecraft() prevElapsedTime=%f elapsedTime=%f\n",
          prevElapsedTime, elapsedTime);
   #endif
   //loj: 7/1/04 elapsedTime = previousTime;
   elapsedTime = prevElapsedTime;
   
   memcpy(rawState, previousState, dimension*sizeof(Real)); 
   MoveToOrigin();
}


//------------------------------------------------------------------------------
// bool ForceModel::Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes model and all contained models
 */
//------------------------------------------------------------------------------
bool ForceModel::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("ForceModel::Initialize() entered\n");
   #endif
   Integer stateSize = 6;      // Will change if we integrate more variables
   Integer satCount = 1;
   std::vector<SpaceObject *>::iterator sat;

   if (!solarSystem)
      throw ForceModelException(
         "Cannot initialize force model; no solar system on '" + 
         instanceName + "'");

   j2kBody = solarSystem->GetBody(j2kBodyName);
   if (j2kBody == NULL) 
      throw ForceModelException("Satellite J2000 body (" + j2kBodyName + 
         ") was not found in the solar system");

   if (spacecraft.size() > 0)
      satCount = spacecraft.size();
    
   PropState *state;
   StringArray finiteSats;
    
   // Calculate the dimension of the state
   dimension = 0;
   for (sat = spacecraft.begin(); sat != spacecraft.end(); ++sat) 
   {
      state = &((*sat)->GetState());
      // Determine if there are any spacecraft that have thrusters turned on
      if ((*sat)->IsManeuvering())
         finiteSats.push_back((*sat)->GetName());
      if ((*sat)->GetType() == Gmat::FORMATION)
         ((Formation*)(*sat))->BuildState();
      stateSize = state->GetSize();
      dimension += stateSize;  
   }

   if (dimension == 0)
      throw ForceModelException("Attempting to initialize force model '" +
         instanceName + "' with dimension 0 -- No referenced spacecraft?");

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
         "Calling PhysicalModel::Initialize(); dimension = %d\n", dimension);
   #endif

   if (!PhysicalModel::Initialize())
      return false;

   rawState = new Real[dimension];

   if (spacecraft.size() == 0) 
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Setting state data; dimension = %d\n", 
            dimension);
      #endif
      
      modelState[0] = 7000.0;
      modelState[1] =    0.0;
      modelState[2] = 1000.0;
      modelState[3] =    0.0;
      modelState[4] =    7.4;
      modelState[5] =    0.0;
   }
   else 
   {
      Integer j = 0;
      for (sat = spacecraft.begin(); sat != spacecraft.end(); ++sat) 
      {
         state = &((*sat)->GetState());
         stateSize = state->GetSize();
         memcpy(&rawState[j], state->GetState(), stateSize * sizeof(Real));
         j += stateSize;
      }
      MoveToOrigin();
   }
    
   previousTime = 0.0;
   if (previousState)
      delete [] previousState;
   previousState = new Real[dimension];
   memcpy(previousState, rawState, dimension*sizeof(Real));

   UpdateTransientForces();

   Integer cf = currentForce;
   PhysicalModel *current = GetForce(cf);  // waw: added 06/04/04
   PhysicalModel *currentPm;

   // Variables used to set spacecraft parameters
   std::string parmName, stringParm;
   Integer i;

   while (current) 
   {
      #ifdef DEBUG_FORCEMODEL_INIT
         std::string name, type;
         name = current->GetName();
         if (name == "")
            name = "unnamed";
         type = current->GetTypeName();
         MessageInterface::ShowMessage
            ("ForceModel::Initialize() initializing object %s of type %s\n",
             name.c_str(), type.c_str());
      #endif
      
      currentPm = current;  // waw: added 06/04/04 
      currentPm->SetDimension(dimension);
      
      // Only initialize the spacecraft independent pieces once
      if (forceMembersNotInitialized)
      {
         currentPm->SetSolarSystem(solarSystem);
         
         // Handle missing coordinate system issues for GravityFields
         if (currentPm->IsOfType("HarmonicField"))
         {
            SetInternalCoordinateSystem("InputCoordinateSystem", currentPm);
            SetInternalCoordinateSystem("FixedCoordinateSystem", currentPm);
            SetInternalCoordinateSystem("TargetCoordinateSystem", currentPm);

            if (body == NULL)
               body = solarSystem->GetBody(centralBodyName); // or should we get bodyName?
         }
      }

      // Initialize the forces
      if (!currentPm->Initialize()) 
      {
         std::string msg = "Component force ";
         msg += currentPm->GetTypeName();
         msg += " failed to initialize";
         throw ForceModelException(msg);
      }
      currentPm->SetState(modelState);

      // Set spacecraft parameters for forces that need them
      i = 0;
      for (sat = spacecraft.begin(); sat != spacecraft.end(); ++sat) 
      {
         i = SetupSpacecraftData(*sat, currentPm, i);
         ++i;
      }
        
      cf++;
      current = GetForce(cf);
   }
   
   #ifdef DEBUG_FORCE_EPOCHS
      std::string epfile = "ForceEpochs.txt";
      if (instanceName != "")
         epfile = instanceName + "_" + epfile;
      if (!epochFile.is_open()) 
      {
         epochFile.open(epfile.c_str());
         epochFile << "Epoch data for the force model '" 
                   << instanceName << "'\n";
      }
   #endif

   #ifdef DEBUG_FIRST_CALL
   firstCallFired = false;
   #endif

   // Set flag stating that Initialize was successful once
   forceMembersNotInitialized = false;
   return true;
}


//------------------------------------------------------------------------------
// void ClearInternalCoordinateSystems()
//------------------------------------------------------------------------------
/**
 * Manages the deallocation of coordinate systems used internally.
 */
//------------------------------------------------------------------------------
void ForceModel::ClearInternalCoordinateSystems()
{
   for (std::vector<CoordinateSystem*>::iterator i = 
           InternalCoordinateSystems.begin();
        i != InternalCoordinateSystems.end(); ++i)
   {
      delete (*i);
   }
   InternalCoordinateSystems.clear();
}

//------------------------------------------------------------------------------
// void SetInternalCoordinateSystem(const std::string csId, 
//      PhysicalModel *currentPm)
//------------------------------------------------------------------------------
/**
 * Manages the allocation of coordinate systems used internally.
 * 
 * @param <csId>        Parameter name for the coordinate system label.
 * @param <currentPm>   Force that needs the CoordinateSystem.
 */
//------------------------------------------------------------------------------
void ForceModel::SetInternalCoordinateSystem(const std::string csId, PhysicalModel *currentPm)
{
   std::string csName;
   CoordinateSystem *cs = NULL;

   #ifdef DEBUG_FORCEMODEL_INIT     
      MessageInterface::ShowMessage(
         "Setting internal CS with ID '%s' for force type '%s'\n",
         csId.c_str(), currentPm->GetTypeName().c_str());
   #endif
   csName = currentPm->GetStringParameter(csId);

   try
   {
      currentPm->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
   }
   catch (BaseException &ex)
   {
      #ifdef DEBUG_FORCEMODEL_INIT
         MessageInterface::ShowMessage(
            "Adding a coordinate system named '%s' for the full field model\n",
            csName.c_str());
      #endif
      
      for (std::vector<CoordinateSystem*>::iterator i = 
              InternalCoordinateSystems.begin();
           i != InternalCoordinateSystems.end(); ++i)
         if ((*i)->GetName() == csName)
            cs = *i;
      
      if (cs == NULL)
      {
         // We need to handle both intertial and fixed CS's here
         if (earthEq == NULL)
            throw ForceModelException(
               "Error setting force model coordinate system: EarthEq pointer "
               "has not been initialized!");
         if (earthFixed == NULL)
            throw ForceModelException(
               "Error setting force model coordinate system: EarthFixed "
               "pointer has not been initialized!");
               
         if (csName.find("Fixed", 0) == std::string::npos)
            cs = (CoordinateSystem *)earthEq->Clone();
         else
            cs = (CoordinateSystem *)earthFixed->Clone();

         cs->SetName(csName);
         cs->SetStringParameter("Origin", centralBodyName);
         cs->SetRefObject(forceOrigin, Gmat::CELESTIAL_BODY, 
            centralBodyName);
         InternalCoordinateSystems.push_back(cs);

         #ifdef DEBUG_FORCEMODEL_INIT
            MessageInterface::ShowMessage("Created %s with description\n\n%s\n", 
               csName.c_str(), 
               cs->GetGeneratingString(Gmat::SCRIPTING).c_str());
         #endif
      }
      
      cs->SetSolarSystem(solarSystem);
      cs->SetJ2000BodyName(j2kBody->GetName());
      cs->SetJ2000Body(j2kBody);
      cs->Initialize();

      #ifdef DEBUG_FORCEMODEL_INIT     
         MessageInterface::ShowMessage(
            "New coordinate system named '%s' has definition\n%s\n",
            csName.c_str(), 
            cs->GetGeneratingString(Gmat::SCRIPTING, "   ").c_str());
      #endif
         
      currentPm->SetRefObject(cs, Gmat::COORDINATE_SYSTEM, csName);
   }
}


//------------------------------------------------------------------------------
// Integer ForceModel::GetOwnedObjectCount()
//------------------------------------------------------------------------------
Integer ForceModel::GetOwnedObjectCount()
{
   return numForces;
}


//------------------------------------------------------------------------------
// GmatBase* ForceModel::GetOwnedObject(Integer whichOne)
//------------------------------------------------------------------------------
GmatBase* ForceModel::GetOwnedObject(Integer whichOne)
{
   if (whichOne < numForces) 
      return GetForce(whichOne);
   
   return NULL;
}


//------------------------------------------------------------------------------
// virtual std::string BuildPropertyName(GmatBase *ownedObj)
//------------------------------------------------------------------------------
/*
 * Builds property name of owned object.
 * This method is called when special handling of object property name is
 * required when writing object. For example, ForceModel requires additional
 * name Earth for GravityField as in FM.GravityField.Earth.Degree.
 *
 * @param ownedObj The object of property handling
 * @return The property name
 */
//------------------------------------------------------------------------------
std::string ForceModel::BuildPropertyName(GmatBase *ownedObj)
{
   #ifdef DEBUG_OWNED_OBJECT_STRINGS
   MessageInterface::ShowMessage
      ("ForceModel::BuildPropertyName() called with ownedObj type=%s\n",
       ownedObj->GetTypeName().c_str());
   #endif
   
   return BuildForceNameString((PhysicalModel*)ownedObj);
}


//------------------------------------------------------------------------------
// void UpdateInitialData()
//------------------------------------------------------------------------------
/**
 * Updates model and all contained models to catch changes in Spacecraft, etc.
 */
//------------------------------------------------------------------------------
void ForceModel::UpdateInitialData()
{
   Integer cf = currentForce;
   PhysicalModel *current = GetForce(cf);  // waw: added 06/04/04

   // Variables used to set spacecraft parameters
   std::string parmName, stringParm;
   std::vector<SpaceObject *>::iterator sat;
   Integer i;
   
   // Detect if spacecraft parameters need complete refresh
   // Set spacecraft parameters for forces that need them
   for (sat = spacecraft.begin(); sat != spacecraft.end(); ++sat) 
      if ((*sat)->ParametersHaveChanged())
      {
         #ifdef DEBUG_SATELLITE_PARAMETERS
            MessageInterface::ShowMessage("Parms changed for %s\n", 
               (*sat)->GetName().c_str());
         #endif
         parametersSetOnce = false;
         (*sat)->ParametersHaveChanged(false);
      }
         
   while (current) 
   {
      if (!parametersSetOnce)
      {
         current->ClearSatelliteParameters();
      }
      i = 0;
      // Set spacecraft parameters for forces that need them
      for (sat = spacecraft.begin(); sat != spacecraft.end(); ++sat) 
      {
         SetupSpacecraftData(*sat, current, i);
         ++i;
      }
      cf++;
      current = GetForce(cf);
   }
   
   parametersSetOnce = true;
}


//------------------------------------------------------------------------------
// void UpdateTransientForces()
//------------------------------------------------------------------------------
/**
 * Tells the transient forces in the model about the propagated SpaceObjects.
 * 
 * In GMAT, a "transient force" is a force that is applied based on state 
 * changes made to the elements that are propagated during a run.  In otehr 
 * words, a transient force is a force that gets applied when needed, but not
 * typically throughout the mission.  An example is a finite burn: the 
 * acceleration for a finite burn is calculated and applied only when a 
 * spacecraft has a thruster that has been turned on.
 * 
 * @param transientSats The list of spacecraft that report active transient 
 *                      forces. 
 */
//------------------------------------------------------------------------------
void ForceModel::UpdateTransientForces()
{
   for (std::vector<PhysicalModel *>::iterator tf = forceList.begin(); 
        tf != forceList.end(); ++tf) {
      if ((*tf)->IsTransient())
         (*tf)->SetPropList(&spacecraft);
         
   }
}


//------------------------------------------------------------------------------
// Integer SetupSpacecraftData(GmatBase *sat, PhysicalModel *pm, Integer i)
//------------------------------------------------------------------------------
/**
 * Passes spacecraft parameters into the force model.
 * 
 * @param <sat>   The SpaceObject that supplies the parameters.
 * @param <pm>    The PhysicalModel receiving the data.
 * @param <i>     The index of the SpaceObject in the physical model.
 * 
 * @return For Spacecraft, the corresponding index; for formations, a count of
 *         the number of spacecraft in the formation.
 */
//------------------------------------------------------------------------------
Integer ForceModel::SetupSpacecraftData(GmatBase *sat, PhysicalModel *pm, 
                                        Integer i)
{
   Integer retval = i; //, id;
   Real parm;
   std::string stringParm;
   
   // Only retrieve the parameter IDs once
   if ((satIds[1] < 0) && sat->IsOfType("Spacecraft"))
   {
      satIds[0] = sat->GetParameterID("A1Epoch");
      if (satIds[0] < 0)
         throw ForceModelException("Epoch parameter undefined on object " +
                                   sat->GetName());

      modelEpochId = pm->GetParameterID("Epoch");
      if (modelEpochId < 0)
         throw ForceModelException("Epoch parameter undefined on PhysicalModel");

      satIds[1] = sat->GetParameterID("CoordinateSystem");
      if (satIds[1] < 0)
         throw ForceModelException(
            "CoordinateSystem parameter undefined on object " + sat->GetName());
      
      // Should this be total mass?
      satIds[2] = sat->GetParameterID("DryMass");
      if (satIds[2] < 0)
         throw ForceModelException("DryMass parameter undefined on object " +
                                   sat->GetName());

      satIds[3] = sat->GetParameterID("Cd");
      if (satIds[3] < 0)
         throw ForceModelException("Cd parameter undefined on object " +
                                   sat->GetName());

      satIds[4] = sat->GetParameterID("DragArea");
      if (satIds[4] < 0)
         throw ForceModelException("Drag Area parameter undefined on object " +
                                   sat->GetName());

      satIds[5] = sat->GetParameterID("SRPArea");
      if (satIds[5] < 0)
         throw ForceModelException("SRP Area parameter undefined on object " +
                                   sat->GetName());

      satIds[6] = sat->GetParameterID("Cr");
      if (satIds[6] < 0)
         throw ForceModelException("Cr parameter undefined on object " +
                                   sat->GetName());
                                   
      #ifdef DEBUG_SATELLITE_PARAMETERS
         MessageInterface::ShowMessage(
            "Parameter ID Array: [%d %d %d %d %d %d %d]; PMepoch id  = %d\n",
            satIds[0], satIds[1], satIds[2], satIds[3], satIds[4], satIds[5], 
            satIds[6], modelEpochId);
      #endif
   }

   if (sat->GetType() == Gmat::SPACECRAFT)
   { 
      #ifdef DEBUG_SATELLITE_PARAMETERS
         MessageInterface::ShowMessage(
            "ForceModel '%s', Member %s: %s->ParmsChanged = %s, "
            "parametersSetOnce = %s\n",
            GetName().c_str(), pm->GetTypeName().c_str(), 
            sat->GetName().c_str(), 
            (((SpaceObject*)sat)->ParametersHaveChanged() ? "true" : "false"), 
            (parametersSetOnce ? "true" : "false"));
      #endif

      // Manage the epoch ...
      parm = sat->GetRealParameter(satIds[0]);
      // Update local value for epoch
      epoch = parm;
      pm->SetRealParameter(modelEpochId, parm);

      if (((SpaceObject*)sat)->ParametersHaveChanged() || !parametersSetOnce)
      {
         #ifdef DEBUG_SATELLITE_PARAMETERS
            MessageInterface::ShowMessage("Setting parameters for %s\n", 
               pm->GetTypeName().c_str());
         #endif
         
         // ... Coordinate System ...
         stringParm = sat->GetStringParameter(satIds[1]);
         
         CoordinateSystem *cs =
            (CoordinateSystem*)(sat->GetRefObject(Gmat::COORDINATE_SYSTEM, 
                                stringParm));
         if (!cs)
         {
            char sataddr[20];
            std::sprintf(sataddr, "%lx", (unsigned long)sat);
            throw ForceModelException(
               "CoordinateSystem is NULL on Spacecraft " + sat->GetName() +
               " at address " + sataddr);
         }
         pm->SetSatelliteParameter(i, "ReferenceBody", cs->GetOriginName());
         
         // ... Mass ...
         parm = sat->GetRealParameter(satIds[2]);
         if (parm <= 0)
            throw ForceModelException("Mass parameter unphysical on object " + 
                                            sat->GetName());
         pm->SetSatelliteParameter(i, "DryMass", parm);
           
         // ... Coefficient of drag ...
         parm = sat->GetRealParameter(satIds[3]);
         if (parm < 0)
            throw ForceModelException("Cd parameter unphysical on object " + 
                                      sat->GetName());
         pm->SetSatelliteParameter(i, "Cd", parm);
         
         // ... Drag area ...
         parm = sat->GetRealParameter(satIds[4]);
         if (parm < 0)
            throw ForceModelException("Drag Area parameter unphysical on object " + 
                                      sat->GetName());
         pm->SetSatelliteParameter(i, "DragArea", parm);
         
         // ... SRP area ...
         parm = sat->GetRealParameter(satIds[5]);
         if (parm < 0)
            throw ForceModelException("SRP Area parameter unphysical on object " + 
                                      sat->GetName());
         pm->SetSatelliteParameter(i, "SRPArea", parm);
         
         // ... and Coefficient of reflectivity
         parm = sat->GetRealParameter(satIds[6]);
         if (parm < 0)
            throw ForceModelException("Cr parameter unphysical on object " + 
                                      sat->GetName());
         pm->SetSatelliteParameter(i, "Cr", parm);
         
         ((SpaceObject*)sat)->ParametersHaveChanged(false);
      }
   }
   else if (sat->GetType() == Gmat::FORMATION) 
   {
      Integer j = -1;
      ObjectArray elements = sat->GetRefObjectArray("SpaceObject");
      for (ObjectArray::iterator n = elements.begin(); n != elements.end();
           ++n) 
      {
         ++j;
         j = SetupSpacecraftData(*n, pm, j);
      }
      retval = j;
   }
   else
      throw ForceModelException(
         "Setting SpaceObject parameters on unknown type for " + 
         sat->GetName());

   return retval;
}


//------------------------------------------------------------------------------
// void ForceModel::IncrementTime(Real dt)
//------------------------------------------------------------------------------
void ForceModel::IncrementTime(Real dt)
{
    PhysicalModel::IncrementTime(dt);
}

//------------------------------------------------------------------------------
// void ForceModel::SetTime(Real t)
//------------------------------------------------------------------------------
void ForceModel::SetTime(Real t)
{
    PhysicalModel::SetTime(t);
}

//------------------------------------------------------------------------------
// bool ForceModel::GetDerivatives(Real * state, Real dt, Integer order)
//------------------------------------------------------------------------------
/**
 * Returns the accumulated superposition of forces 
 * 
 * This method applies superposition of forces in order to calculate the total
 * acceleration applied to the state vector.
 * 
 * @param    state   The current state vector
 * @param    dt      The current time interval from epoch
 * @param    order   Order of the derivative to be taken
 */
//------------------------------------------------------------------------------
bool ForceModel::GetDerivatives(Real * state, Real dt, Integer order)
{
   if (order > 2)
      return false;
   if (!initialized)
      return false;

   Integer satCount = dimension / stateSize, i, iOffset;

   #ifdef DEBUG_FORCEMODEL_EXE
      for (i = 0; i < satCount; ++i)
         MessageInterface::ShowMessage(
            "  Input state = %le %le %le %le %le %le\n", state[i*stateSize + 0], 
            state[i*stateSize + 1], state[i*stateSize + 2], 
            state[i*stateSize + 3], state[i*stateSize + 4], 
            state[i*stateSize + 5]);
   #endif

   #ifdef DEBUG_FORCEMODEL_EPOCHS
      MessageInterface::ShowMessage(
         "Input time offset = %16.14le; epoch = %16.10lf\n", dt, epoch);
   #endif
  
   // Initialize the derivative array
   for (i = 0; i < satCount; ++i) {
      iOffset = i*stateSize;
      if (order == 1) //loj: changed from =
      {
          deriv[ iOffset ] = state[iOffset+3];
          deriv[iOffset+1] = state[iOffset+4];
          deriv[iOffset+2] = state[iOffset+5];
          deriv[iOffset+3] = deriv[iOffset+4] = deriv[iOffset+5] = 0.0;
      }
      else 
      {
          deriv[iOffset] = deriv[iOffset+1] = deriv[iOffset+2] = 
          deriv[iOffset+3] = deriv[iOffset+4] = deriv[iOffset+5] = 0.0;
      }
   }
   
   Integer cf = currentForce;
   PhysicalModel *current = GetForce(cf);  

   const Real * ddt;
   while (current) 
   {
      ddt = current->GetDerivativeArray();
      if (!current->GetDerivatives(state, dt, order))
         return false;

      #ifdef DEBUG_FORCEMODEL_EXE
      for (i = 0; i < satCount; ++i)
         MessageInterface::ShowMessage("  ddt(%s[%s])[%d] = %le %le %le\n",
            (current->GetTypeName().c_str()), 
            (current->GetStringParameter(
               current->GetParameterID("BodyName"))).c_str(), i, 
            ddt[i*stateSize + 3], ddt[i*stateSize + 4], ddt[i*stateSize + 5]);
      #endif

      for (i = 0; i < satCount; ++i) {
         iOffset = i*stateSize;
         if (order == 1) //loj: changed from =
         {
            deriv[iOffset+3] += ddt[iOffset+3];
            deriv[iOffset+4] += ddt[iOffset+4];
            deriv[iOffset+5] += ddt[iOffset+5];
         }
         else 
         {
            deriv[ iOffset ] += ddt[ iOffset ];
            deriv[iOffset+1] += ddt[iOffset+1];
            deriv[iOffset+2] += ddt[iOffset+2];
         }
         #ifdef DEBUG_FORCEMODEL_EXE
            MessageInterface::ShowMessage("  deriv[%d] = %le %le %le\n", i, 
               deriv[iOffset + 3], deriv[iOffset + 4], 
               deriv[iOffset + 5]);
         #endif
      }

      #ifdef DEBUG_FIRST_CALL
         if (firstCallFired == false)
         {
            MessageInterface::ShowMessage(
               "   %s(%s)::GetDerivatives --> "
               "[%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
               current->GetTypeName().c_str(), current->GetName().c_str(), 
               ddt[0], ddt[1], ddt[2], ddt[3], ddt[4], ddt[5]);
         }
      #endif

      ++cf;
      current = GetForce(cf);
   }
   #ifdef DEBUG_FORCEMODEL_EXE
      MessageInterface::ShowMessage("  ===============================\n");
   #endif

   #ifdef DEBUG_FIRST_CALL
      if (firstCallFired == false)
      {
         MessageInterface::ShowMessage(
            "GetDerivatives: [%.16lf %.16lf %.16lf %.16lf %.16lf %.16lf]\n",
            deriv[0], deriv[1], deriv[2], deriv[3], deriv[4], deriv[5]);
      }

      firstCallFired = true;
   #endif

   return true;
}

//------------------------------------------------------------------------------
// Real ForceModel::EstimateError(Real *diffs, Real *answer) const
//------------------------------------------------------------------------------
/**
 * Interface used to estimate the error in the current step
 * 
 * The method calculates the largest local estimate of the error from the 
 * integration given the components of the differences calculated bt the 
 * integrator.  It returns the largest error estimate found.  
 *   
 * The default implementation returns the largest single relative error 
 * component found based on the input arrays.  In other words, the 
 * implementation provided here returns the largest component of the following
 * vector:
 * 
 * \f[\vec \epsilon = |{{{EE}_n}\over{x_n^f - x_n^i}}|\f]
 *   
 * There are several alternatives that users of this class can implement: the 
 * error could be calculated based on the largest error in the individual 
 * components of the state vector, as the magnitude of the state vector (that 
 * is, the L2 (rss) norm of the error estimate vector).  The estimated error 
 * should never be negative, so a return value less than 0.0 can be used to 
 * indicate an error condition.
 *     
 * @param diffs         Array of differences calculated by the integrator.  This
 *                      array must be the same size as the state vector.
 * @param answer        Candidate new state from the integrator.
 */
//------------------------------------------------------------------------------
Real ForceModel::EstimateError(Real *diffs, Real *answer) const
{
    if (estimationMethod == ESTIMATE_IN_BASE)
        return PhysicalModel::EstimateError(diffs, answer);

    Real retval = 0.0, err, mag, vec[3];

//MessageInterface::ShowMessage("normType == %d\n", normType);

    for (int i = 0; i < dimension; i += 3) 
    {
        switch (normType) 
        {

            case -2:
                // Code for the L2 norm, based on sep from central body
                vec[0] = 0.5 * (answer[ i ] + modelState[ i ]); 
                vec[1] = 0.5 * (answer[i+1] + modelState[i+1]); 
                vec[2] = 0.5 * (answer[i+2] + modelState[i+2]);

                mag = vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2];        
                err = diffs[i]*diffs[i] + diffs[i+1]*diffs[i+1] + diffs[i+2]*diffs[i+2];
                if (mag >relativeErrorThreshold) 
                    err = sqrt(err / mag);
                else
                    err = sqrt(err);
                break;

            case -1:
                // L1 norm, based on sep from central body
                vec[0] = fabs(0.5 * (answer[ i ] + modelState[ i ])); 
                vec[1] = fabs(0.5 * (answer[i+1] + modelState[i+1])); 
                vec[2] = fabs(0.5 * (answer[i+2] + modelState[i+2]));

                mag = vec[0] + vec[1] + vec[2];        
                err = fabs(diffs[i]) + fabs(diffs[i+1]) + fabs(diffs[i+2]);
                if (mag >relativeErrorThreshold) 
                    err = err / mag;
                break;

            case 0:         // Report no error here
                return 0.0;

            case 1:
                // L1 norm
                vec[0] = fabs(answer[ i ] - modelState[ i ]); 
                vec[1] = fabs(answer[i+1] - modelState[i+1]); 
                vec[2] = fabs(answer[i+2] - modelState[i+2]);

                mag = vec[0] + vec[1] + vec[2];        
                err = fabs(diffs[i]) + fabs(diffs[i+1]) + fabs(diffs[i+2]);
                if (mag >relativeErrorThreshold) 
                    err = err / mag;
                break;

            case 2:
            default:
                // Code for the L2 norm
                vec[0] = answer[ i ] - modelState[ i ]; 
                vec[1] = answer[i+1] - modelState[i+1]; 
                vec[2] = answer[i+2] - modelState[i+2];

                mag = vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2];        
                err = diffs[i]*diffs[i] + diffs[i+1]*diffs[i+1] + diffs[i+2]*diffs[i+2];
                if (mag > relativeErrorThreshold) 
                    err = sqrt(err / mag);
                else
                    err = sqrt(err);
        }

        if (err > retval)
        {
            retval = err;
        }
    }

    return retval;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/*
 * Renames referenced object name.
 *
 * @param <type> type of the reference object.
 * @param <oldName> old name of the reference object.
 * @param <newName> new name of the reference object.
 *
 * @return always true to indicate RenameRefObject() was implemented.
 */
//---------------------------------------------------------------------------
bool ForceModel::RenameRefObject(const Gmat::ObjectType type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   // There are no renamable objects
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the ForceModel.
 *
 * @return clone of the ForceModel.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ForceModel::Clone() const
{
   return (new ForceModel(*this));
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& ForceModel::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SPACE_POINT);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref objects used by the member forces.
 *
 * @param <type> The type of object desired, or Gmat::UNKNOWN_OBJECT for the
 *               full list.
 * 
 * @return the list of object names.
 * 
 */
//------------------------------------------------------------------------------
const StringArray& 
   ForceModel::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   std::string pmName;
   StringArray pmRefs;
   
   forceReferenceNames.clear();
   
   // Provide point mass body names for validation checking
   if (type == Gmat::SPACE_POINT)
   {
      forceReferenceNames = BuildBodyList("PointMassForce");
      
      // Add central body (loj: 2/23/07 added)
      if (find(forceReferenceNames.begin(), forceReferenceNames.end(),
               centralBodyName) == forceReferenceNames.end())
         forceReferenceNames.push_back(centralBodyName);
      
      return forceReferenceNames;
      //return BuildBodyList("PointMassForce");
      
   }
   
   // Provide space object names for validation checking
   if (type == Gmat::SPACEOBJECT)
   {
      for (std::vector<SpaceObject *>::iterator sc = spacecraft.begin(); 
           sc != spacecraft.end(); ++sc)
      {
         forceReferenceNames.push_back((*sc)->GetName());
      }
      return forceReferenceNames;
   }
   
   // Always grab these two:
   forceReferenceNames.push_back("EarthMJ2000Eq");
   forceReferenceNames.push_back("EarthFixed");
   
   // Do the base class call
   try
   {
      pmName = PhysicalModel::GetRefObjectName(type);
      if (find(forceReferenceNames.begin(), forceReferenceNames.end(), 
               pmName) != forceReferenceNames.end())
         forceReferenceNames.push_back(pmName);
   }
   catch (BaseException &ex)
   {
      // Do nothing
   }
   
   try
   {
      pmRefs = PhysicalModel::GetRefObjectNameArray(type);

      // Add them all to the list
      for (StringArray::iterator j = pmRefs.begin(); j != pmRefs.end(); ++j)
      {
         if (find(forceReferenceNames.begin(), forceReferenceNames.end(), 
                  *j) != forceReferenceNames.end())
            forceReferenceNames.push_back(pmName);
      }
   }
   catch (BaseException &ex)
   {
      // Do nothing
   }
   
   // Build the list of references
   for (std::vector<PhysicalModel*>::iterator i =  forceList.begin(); 
        i != forceList.end(); ++i)
   {
      try
      {
         pmName = (*i)->GetRefObjectName(type);
         if (find(forceReferenceNames.begin(), forceReferenceNames.end(), 
                  pmName) == forceReferenceNames.end())
            forceReferenceNames.push_back(pmName);
      }
      catch (BaseException &ex)
      {
         // Do nothing
      }
      
      try
      {
         pmRefs = (*i)->GetRefObjectNameArray(type);
         // Add them all to the list
         for (StringArray::iterator j = pmRefs.begin(); j != pmRefs.end(); ++j)
         {
            if (find(forceReferenceNames.begin(), forceReferenceNames.end(), 
                     *j) == forceReferenceNames.end())
               forceReferenceNames.push_back(*j);
         }
      }
      catch (BaseException &ex)
      {
         // Do nothing
      }
   }
   
   // Add central body (loj: 2/23/07 added)
   if (find(forceReferenceNames.begin(), forceReferenceNames.end(),
            centralBodyName) == forceReferenceNames.end())
      forceReferenceNames.push_back(centralBodyName);
   
   #ifdef FORCE_REFERENCE_OBJECTS
      MessageInterface::ShowMessage("Reference object names for '%s'\n", 
         instanceName.c_str());
      for (StringArray::iterator i = forceReferenceNames.begin(); 
           i != forceReferenceNames.end(); ++i)
         MessageInterface::ShowMessage("   %s\n", i->c_str());
   #endif
         
   // and return it
   return forceReferenceNames;
}

//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer
 * 
 * @param ss Pointer to the solar system used in the modeling.
 */
//------------------------------------------------------------------------------
void ForceModel::SetSolarSystem(SolarSystem *ss)
{
   PhysicalModel::SetSolarSystem(ss);

   if (ss == NULL)
      MessageInterface::ShowMessage("Setting NULL solar system on %s\n", 
         instanceName.c_str());
   
   if (solarSystem != NULL)
   {
      forceOrigin = solarSystem->GetBody(centralBodyName);
      
      if (forceOrigin == NULL) 
         throw ForceModelException(
            "Force model origin (" + centralBodyName + 
            ") was not found in the solar system");

      for (std::vector<PhysicalModel*>::iterator i = forceList.begin();
           i != forceList.end(); ++i)
         (*i)->SetForceOrigin(forceOrigin);
   }
}

//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets ref objects used by the member forces.
 *
 * @param <obj>  Pointer to the refence object.
 * @param <type> The type of object being set.
 * @param <name> Name of the reference object.
 * 
 * @return true if the object was set, false if not.
 * 
 * @note This method catches exceptions, and just returns true or false to
 *       indicate success or failure.
 */
//------------------------------------------------------------------------------
bool ForceModel::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                              const std::string &name)
{
   bool wasSet = false;
   
   // Handle the CS pointers we always want
   if (name == "EarthMJ2000Eq")
      if (type == Gmat::COORDINATE_SYSTEM)
         earthEq = (CoordinateSystem*)obj;
      else
         throw ForceModelException(
            "Object named EarthMJ2000Eq is not a coordinate system.");
   if (name == "EarthFixed")
      if (type == Gmat::COORDINATE_SYSTEM)
         earthFixed = (CoordinateSystem*)obj;
      else
         throw ForceModelException(
            "Object named EarthFixed is not a coordinate system.");

   // Attempt to set the object for the base class    
   try
   {
      if (PhysicalModel::SetRefObject(obj, type, name))
         wasSet = true;
   }
   catch (BaseException &ex)
   {
      // Do nothing
   }
   
   // Build the list of references
   for (std::vector<PhysicalModel*>::iterator i =  forceList.begin(); 
        i != forceList.end(); ++i)
   {
      try
      {
         if ((*i)->SetRefObject(obj, type, name))
         {
            wasSet = true;
            if (type == Gmat::COORDINATE_SYSTEM)
            {
               CoordinateSystem *cs = (CoordinateSystem*)obj;
               if (cs->GetOriginName() == "")
               {
                  // Finish setting CS data
                  cs->SetOriginName((*i)->GetBodyName());
               }
            }
         } 
      }
      catch (BaseException &ex)
      {
         // Do nothing
      }
   }
   
   return (wasSet);
}


//------------------------------------------------------------------------------
// Integer GetParameterCount() const
//------------------------------------------------------------------------------
Integer ForceModel::GetParameterCount() const
{
   return parameterCount;
}


// Access methods 
//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string ForceModel::GetParameterText(const Integer id) const
{
   if (id >= PhysicalModelParamCount && id < ForceModelParamCount)
      return PARAMETER_TEXT[id - PhysicalModelParamCount];
   else
      return PhysicalModel::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer ForceModel::GetParameterID(const std::string &str) const
{
   std::string alias = str;
    
   // Script document required two different names for the primary body
   // force descriptor
   if (alias == "Gravity")
      alias = "PrimaryBodies";
    
   for (int i = PhysicalModelParamCount; i < ForceModelParamCount; i++)
   {
      if (alias == PARAMETER_TEXT[i - PhysicalModelParamCount])
         return i;
   }

   return PhysicalModel::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType ForceModel::GetParameterType(const Integer id) const
{
   if (id >= PhysicalModelParamCount && id < ForceModelParamCount)
      return PARAMETER_TYPE[id - PhysicalModelParamCount];
   else
      return PhysicalModel::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string ForceModel::GetParameterTypeString(const Integer id) const
{
   if (id >= PhysicalModelParamCount && id < ForceModelParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return PhysicalModel::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
bool ForceModel::IsParameterReadOnly(const Integer id) const
{
   if (id == COORDINATE_SYSTEM_LIST)
      return true;
      
   return PhysicalModel::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
bool ForceModel::IsParameterReadOnly(const std::string &label) const
{
   if (label == PARAMETER_TEXT[COORDINATE_SYSTEM_LIST-PhysicalModelParamCount])
      return true;
      
   return PhysicalModel::IsParameterReadOnly(label);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string ForceModel::GetStringParameter(const Integer id) const
{
   switch (id)
   {
      case CENTRAL_BODY:
         return centralBodyName;
         
      case  DRAG:
      {
         // Find the drag force
         const PhysicalModel *pm = GetForce("DragForce");
         // No drag force, return "None"
         if (pm == NULL)
            return "None";
         // Get the atmosphere model from the drag force
         Integer id = pm->GetParameterID("AtmosphereModel");
         std::string am = pm->GetStringParameter(id);
         return am;
      }
      
//       case  SRP:
//       {
//          const PhysicalModel *pm = GetForce("SolarRadiationPressure");
//          if (pm == NULL)
//             return "Off";
//          return "On";
//       }
      
      case NORM_TYPE:
         switch (normType)
         {
            case -2:
               return "RSSState";
            case -1:
               return "LargestState";
            case 0:
               return "None";
            case 1:
               return "LargestStep";
            case 2:
               return "RSSStep";
            default:
               throw ForceModelException("Unrecognized error control method.");
         }
         break;
      
      default:
         return PhysicalModel::GetStringParameter(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string ForceModel::GetStringParameter(const std::string &label) const
{
    return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool ForceModel::SetStringParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
      case CENTRAL_BODY:
         centralBodyName = value;
         bodyName = centralBodyName;
         return true;
         
      case PRIMARY_BODIES:
         return false;
         
      case POINT_MASSES:
         return false;
         
      case  DRAG:
         return false;
         
//       case  SRP:
//          return false;
         
      case NORM_TYPE:
         if (value == "RSSState")
         {
            normType = -2;
            return true;
         }
         if (value == "LargestState")
         {
            normType = -1;
            return true;
         }
         if (value == "None")
         {
            normType = 0;
            return true;
         }
         if (value == "LargestStep")
         {
            normType = 1;
            return true;
         }
         if (value == "RSSStep")
         {
            normType = 2;
            return true;
         }
         throw ForceModelException("Unrecognized error control method.");

      default:
         return PhysicalModel::SetStringParameter(id, value);
    }
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,const std::string &value)
//------------------------------------------------------------------------------
bool ForceModel::SetStringParameter(const std::string &label,
                                    const std::string &value)
{
    return SetStringParameter(GetParameterID(label), value);
}

//---------------------------------------------------------------------------
//  std::string GetOnOffParameter(const Integer id) const
//---------------------------------------------------------------------------
std::string ForceModel::GetOnOffParameter(const Integer id) const
{
   switch (id)
   {
   case SRP:
      {
         const PhysicalModel *pm = GetForce("SolarRadiationPressure");
         if (pm == NULL)
            return "Off";
         return "On";
      }
   default:
      return PhysicalModel::GetOnOffParameter(id);
   }
}


//---------------------------------------------------------------------------
//  bool SetOnOffParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
bool ForceModel::SetOnOffParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case SRP:
      return true;
   default:
      return PhysicalModel::SetOnOffParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// std::string ForceModel::GetOnOffParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string ForceModel::GetOnOffParameter(const std::string &label) const
{
   return GetOnOffParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetOnOffParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool ForceModel::SetOnOffParameter(const std::string &label, 
                                   const std::string &value)
{
   return SetOnOffParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& ForceModel::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
   case PRIMARY_BODIES:
      return BuildBodyList("GravityField");
   case POINT_MASSES:
      return BuildBodyList("PointMassForce");
   case COORDINATE_SYSTEM_LIST:
      return BuildCoordinateList();   
   default:
      return PhysicalModel::GetStringArrayParameter(id);
    }
}


//------------------------------------------------------------------------------
// StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& ForceModel::GetStringArrayParameter(const std::string &label) const
{
    return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& BuildBodyList(std::string type) const
//------------------------------------------------------------------------------
const StringArray& ForceModel::BuildBodyList(std::string type) const
{
   static StringArray bodylist;
   bodylist.clear();
   
   // Run through list of forces, adding body names for GravityField instances
   std::vector<PhysicalModel*>::const_iterator i;
   
   std::string actualType = GetScriptAlias(type);
   
   for (i = forceList.begin(); i != forceList.end(); ++i) {
       if ((*i)->GetTypeName() == actualType) {
          bodylist.push_back((*i)->GetStringParameter("BodyName"));
       }
   }
   return bodylist;
}


//------------------------------------------------------------------------------
// const StringArray& BuildCoordinateList() const
//------------------------------------------------------------------------------
const StringArray& ForceModel::BuildCoordinateList() const
{
   static StringArray cslist;
   cslist.clear();
   
   // Run through list of forces, adding body names for GravityField instances
   std::vector<PhysicalModel*>::const_iterator i;
   
   for (i = forceList.begin(); i != forceList.end(); ++i) 
   {
      if ((*i)->GetTypeName() == "GravityField") 
      {
// For now, only build the body fixed CS's in list because others already exist.
//         cslist.push_back((*i)->GetStringParameter("InputCoordinateSystem"));
         cslist.push_back((*i)->GetStringParameter("FixedCoordinateSystem"));
//         cslist.push_back((*i)->GetStringParameter("TargetCoordinateSystem"));
      }
   }
   return cslist;
}


//------------------------------------------------------------------------------
// void SetScriptAlias(const std::string& alias, const std::string& typeName)
//------------------------------------------------------------------------------
/**
 * Sets mapping between script descriptions of forces and their class names.
 * 
 * The GMAT script document specifies descriptors for some forces that is 
 * different from the actual class names, and in some cases actually uses more
 * than one name for the same force.  This method is used to build the map
 * between the script strings and the actual class names.  The constructor for
 * the Interpreter base class fills in this data (at least for now -- we might
 * move it to the Moderator's initialization once it is working and tested).
 * 
 * @param alias script string used for the force.
 * @param typeName actual class name as used by the factory that creates it.
 * 
 * @todo Put the initialization for force aliases in a convenient location.
 */
//------------------------------------------------------------------------------
void ForceModel::SetScriptAlias(const std::string& alias, 
                                const std::string& typeName)
{
   if (scriptAliases.find(alias) == scriptAliases.end()) {
      scriptAliases[alias] = typeName;
   }
}


//------------------------------------------------------------------------------
// std::string& GetScriptAlias(const std::string& alias)
//------------------------------------------------------------------------------
/**
 * Accesses mapping between script descriptions of forces and their class names.
 * 
 * This method provides access to class names given an alias in the script.
 * 
 * @param alias script string used for the force.
 * 
 * @return The class name.
 */
//------------------------------------------------------------------------------
std::string& ForceModel::GetScriptAlias(const std::string& alias)
{
   static std::string type;
   type = alias;
   if (scriptAliases.find(alias) != scriptAliases.end()) {
      type = scriptAliases[alias];
   }
   return type;
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Accesses an internal object used in the ForceModel.
 * 
 * This method provides access to the forces used in the ForceModel.  It is used
 * to set and read the specific force attributes -- for example, the file name 
 * used for the full field (GravityField) model.
 * 
 * @param type Base class type for the requested object.  Must be set to
 *             Gmat::PHYSICAL_MODEL for this build.
 * @param name String used for the object.
 * 
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
GmatBase* ForceModel::GetRefObject(const Gmat::ObjectType type,
                                   const std::string &name)
{
   if (type != Gmat::PHYSICAL_MODEL)
       throw ForceModelException(
          "Only forces are accessed in ForceModel::GetRefObject");
   
   // Run through list of forces, adding body names for GravityField instances
   std::vector<PhysicalModel*>::const_iterator i;
   
   std::string actualType = GetScriptAlias(name);
   
   for (i = forceList.begin(); i != forceList.end(); ++i) {
       if ((*i)->GetTypeName() == actualType) {
          return *i;
       }
   }
   return NULL;
}

                                    
//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name, 
//                        const Integer index)
//------------------------------------------------------------------------------
/**
 * Accesses an internal object used in the ForceModel.
 * 
 * This method provides access to the forces used in the ForceModel.  It is used
 * to set and read the specific force attributes -- for example, the file name 
 * used for the full field (GravityField) model.  This version of the method 
 * provides a mechanism to access more than one object with the same type and
 * name by using an index to reach later intances.
 * 
 * @param type Base class type for the requested object.  Must be set to
 *             Gmat::PHYSICAL_MODEL for this build.
 * @param name String used for the object.
 * @param index Index into the list of objects of this type.
 * 
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
GmatBase* ForceModel::GetRefObject(const Gmat::ObjectType type, 
                                   const std::string &name, const Integer index)
{
   if (type != Gmat::PHYSICAL_MODEL)
       throw ForceModelException(
          "Only forces are accessed in ForceModel::GetRefObject");
   
   // Run through list of forces, adding body names for GravityField instances
   std::vector<PhysicalModel*>::const_iterator i;
   std::string actualType = GetScriptAlias(name);
   Integer j = 0;
   
   for (i = forceList.begin(); i != forceList.end(); ++i) {
       if ((*i)->GetTypeName() == actualType) {
          ++j;
          if (j == index)
             return *i;       // Ignore names for forces.
       }
   }
   return NULL;
}


//------------------------------------------------------------------------------
// ObjectArray& GetRefObjectArray(const std::string& typeString)
//------------------------------------------------------------------------------
/**
 * Accesses arrays of internal objects used in the ForceModel.
 * 
 * @param typeString String used for the objects.
 * 
 * @return A reference to the ObjectArray.
 */
//------------------------------------------------------------------------------
ObjectArray& ForceModel::GetRefObjectArray(const std::string& typeString)
{
   static ObjectArray objects;
   objects.clear();
   
   // Run through list of forces, adding body names for GravityField instances
   std::vector<PhysicalModel*>::const_iterator i;
   std::string actualType = GetScriptAlias(typeString);

   if (typeString == "PhysicalModel") {
      for (i = forceList.begin(); i != forceList.end(); ++i) {
         objects.push_back(*i);       // Ignore names for forces.
      }
   }
   else {
      for (i = forceList.begin(); i != forceList.end(); ++i) {
          if ((*i)->GetTypeName() == actualType) {
             objects.push_back(*i);       // Ignore names for forces.
          }
      }
   }
   return objects;
}


//We can remove GetGeneratingString() and WriteFMParameters() now (loj: 2008.01.25)
#ifdef __WITH_FM_GEN_STRING__
//------------------------------------------------------------------------------
// StringArray GetGeneratingString(Gmat::WriteMode mode,
//                const std::string &prefix, const std::string &useName)
//------------------------------------------------------------------------------
/**
 * Produces a string, possibly multi-line, containing the text that produces an
 * object.
 *
 * @param mode Specifies the type of serialization requested.
 * @param prefix Optional prefix appended to the object's name
 * @param useName Name that replaces the object's name.
 *
 * @return A string containing the text.
 *
 * @note Temporarily (?) put this code here to facilitate writing force model
 *       parms prior to 5/25/05 demo
 */
//------------------------------------------------------------------------------
const std::string& ForceModel::GetGeneratingString(Gmat::WriteMode mode,
                                                   const std::string &prefix,
                                                   const std::string &useName)
{
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("ForceModel::GetGeneratingString() this=%p, mode=%d, prefix=%s, "
       "useName=%s\n", this, mode, prefix.c_str(), useName.c_str());
   #endif
   
   std::stringstream data;
   
   // Crank up data precision so we don't lose anything
   data.precision(GmatGlobal::Instance()->GetDataPrecision());
   std::string preface = "", nomme;
   
   if ((mode == Gmat::SCRIPTING) || (mode == Gmat::OWNED_OBJECT) ||
       (mode == Gmat::SHOW_SCRIPT))
      inMatlabMode = false;
   if (mode == Gmat::MATLAB_STRUCT || mode == Gmat::EPHEM_HEADER)
      inMatlabMode = true;

   if (useName != "")
      nomme = useName;
   else
      nomme = instanceName;

   if ((mode == Gmat::SCRIPTING) || (mode == Gmat::SHOW_SCRIPT) ||
       (mode == Gmat::EPHEM_HEADER))
   {
      std::string tname = typeName;
      if (tname == "PropSetup")
         tname = "Propagator";
      
      if (mode == Gmat::EPHEM_HEADER)
      {
         data << tname << " = " << "'" << nomme << "';\n";
         preface = "";
      }
      else
      {
         data << "Create " << tname << " " << nomme << ";\n";
         preface = "GMAT ";
      }
   }

   nomme += ".";

   if (mode == Gmat::OWNED_OBJECT) {
      preface = prefix;
      nomme = "";
   }
   
   preface += nomme;
   WriteFMParameters(mode, preface, data);
   
   generatingString = data.str();
   
   return generatingString;
   
   // Call the base class method for preface and inline comments
   //return GmatBase::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// void WriteFMParameters(std::string &prefix, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Code that writes the parameter details for an object.
 *
 * @param prefix Starting portion of the script string used for the parameter.
 * @param obj The object that is written.
 *
 * @note Temporarily (?) put this code here to facilitate writing force model
 *       parms prior to 5/25/05 demo
 */
//------------------------------------------------------------------------------
void ForceModel::WriteFMParameters(Gmat::WriteMode mode, std::string &prefix,
                                   std::stringstream &stream)
{
   Integer i;
   Gmat::ParameterType parmType;
   std::stringstream value;
   value.precision(GmatGlobal::Instance()->GetDataPrecision());
      
   for (i = 0; i < parameterCount; ++i)
   {
      if (IsParameterReadOnly(i) == false)
      {
         parmType = GetParameterType(i);
         // Handle StringArray parameters separately
         if (parmType != Gmat::STRINGARRAY_TYPE)
         {
            // Skip unhandled types
            if (
                (parmType != Gmat::UNSIGNED_INTARRAY_TYPE) &&
                (parmType != Gmat::RVECTOR_TYPE) &&
                (parmType != Gmat::RMATRIX_TYPE) &&
                (parmType != Gmat::UNKNOWN_PARAMETER_TYPE)
               )
            {
               // Fill in the l.h.s.
               value.str("");
               WriteParameterValue(i, value);
               if (value.str() != "")
                  stream << prefix << GetParameterText(i)
                         << " = " << value.str() << ";\n";
            }
         }
         else
         {
            // Handle StringArrays
            StringArray sar = GetStringArrayParameter(i);
            if (sar.size() > 0)
            {
               stream << prefix << GetParameterText(i) << " = {";
               for (StringArray::iterator n = sar.begin(); n != sar.end(); ++n)
               {
                  if (n != sar.begin())
                     stream << ", ";
                  if (inMatlabMode)
                     stream << "'";
                  stream << (*n);
                  if (inMatlabMode)
                     stream << "'";
               }
               stream << "};\n";
            }
         }
      }
   }
   
   GmatBase *ownedObject;
   std::string nomme, newprefix;
   
   #ifdef DEBUG_OWNED_OBJECT_STRINGS
      MessageInterface::ShowMessage
         ("ForceModel::WriteFMParameters() \"%s\" has %d owned objects\n",
          instanceName.c_str(), GetOwnedObjectCount());
   #endif
      
   for (i = 0; i < GetOwnedObjectCount(); ++i)
   {
      newprefix = prefix;
      ownedObject = GetOwnedObject(i);
      if (ownedObject != NULL)
      {
         nomme = BuildForceNameString((PhysicalModel*)ownedObject);

         #ifdef DEBUG_OWNED_OBJECT_STRINGS
             MessageInterface::ShowMessage(
                "   id %d has type %s and name \"%s\", addr=%p\n",
                i, ownedObject->GetTypeName().c_str(),
                nomme.c_str(), ownedObject);
         #endif
         
         if (nomme != "")
            newprefix += nomme + ".";
         else if (GetType() == Gmat::FORCE_MODEL)
            newprefix += ownedObject->GetTypeName() + ".";
         
         #ifdef DEBUG_OWNED_OBJECT_STRINGS
         MessageInterface::ShowMessage
            ("   Calling ownedObject->GetGeneratingString() with "
             "newprefix='%s'\n", newprefix.c_str());
         #endif
         
         stream << ownedObject->GetGeneratingString(Gmat::OWNED_OBJECT, newprefix);
      }
      else
         MessageInterface::ShowMessage("Cannot access force %d\n", i);
   }
}
#endif


//------------------------------------------------------------------------------
// std::string BuildForceNameString(PhysicalModel *force)
//------------------------------------------------------------------------------
std::string ForceModel::BuildForceNameString(PhysicalModel *force)
{
   std::string retval = "UnknownForce", forceType = force->GetTypeName();
   
   if (forceType == "DragForce")
      retval = "Drag";
   if (forceType == "GravityField")
      // Changed "Gravity" to "GravityField" to be consistent with type name (loj: 2007.01.25)
      retval = "GravityField." + force->GetStringParameter("BodyName");
   if (forceType == "PointMassForce")
      retval = force->GetStringParameter("BodyName");
   if (forceType == "SolarRadiationPressure")
      retval = "SRP";
   
   // Add others here
   
   return retval;
}


//------------------------------------------------------------------------------
// void MoveToOrigin(Real newEpoch)
//------------------------------------------------------------------------------
/**
 * Transforms the state vector from the internal coordinate system to the force 
 * model origin.
 */
//------------------------------------------------------------------------------
void ForceModel::MoveToOrigin(Real newEpoch)
{
   Integer satCount = dimension / stateSize;
   Integer currentScState = 0;
    
   if (centralBodyName == j2kBodyName)
      memcpy(modelState, rawState, dimension*sizeof(Real));
   else
   {
      Rvector6 cbState, j2kState, delta;
      Real now = ((newEpoch < 0.0) ? epoch : newEpoch);
      cbState = forceOrigin->GetState(now);
      j2kState = j2kBody->GetState(now);

      delta = cbState - j2kState;
      
      for (Integer i = 0; i < satCount; ++i)
      {
         for (int j = 0; j < 6; ++j)
            modelState[currentScState+j] = rawState[currentScState+j] - delta[j];
            
         // Copy any remaining state elements
         if (stateSize > 6)
            memcpy(&modelState[currentScState+6], &rawState[currentScState+6], 
                (stateSize-6)*sizeof(Real));
         // Move to the next state
         currentScState += stateSize;
      }
      #ifdef DEBUG_REORIGIN
         MessageInterface::ShowMessage(
             "ForceModel::MoveToOrigin()\n   Input state: [%lf %lf %lf %lf %lf "
             "%lf]\n   j2k state:   [%lf %lf %lf %lf %lf %lf]\n"
             "   cb state:    [%lf %lf %lf %lf %lf %lf]\n"
             "   delta:       [%lf %lf %lf %lf %lf %lf]\n"
             "   model state: [%lf %lf %lf %lf %lf %lf]\n\n",
             rawState[0], rawState[1], rawState[2], rawState[3], rawState[4], 
             rawState[5],    
             j2kState[0], j2kState[1], j2kState[2], j2kState[3], j2kState[4], 
             j2kState[5],    
             cbState[0], cbState[1], cbState[2], cbState[3], cbState[4], 
             cbState[5],    
             delta[0], delta[1], delta[2], delta[3], delta[4], delta[5],    
             modelState[0], modelState[1], modelState[2], modelState[3], 
             modelState[4], modelState[5]);    
      #endif
   }
}


//------------------------------------------------------------------------------
// void ReturnFromOrigin(Real newEpoch)
//------------------------------------------------------------------------------
/**
 * Transforms the state vector from the force model origin to the internal 
 * coordinate system.
 */
//------------------------------------------------------------------------------
void ForceModel::ReturnFromOrigin(Real newEpoch)
{
//static Integer counter = 0;

   Integer satCount = dimension / stateSize;
   Integer currentScState = 0;
    
   if (centralBodyName == j2kBodyName)
      memcpy(rawState, modelState, dimension*sizeof(Real));
   else
   {
      Rvector6 cbState, j2kState, delta;
      Real now = ((newEpoch < 0.0) ? epoch : newEpoch);
      cbState = forceOrigin->GetState(now);
      j2kState = j2kBody->GetState(now);
      
      delta = j2kState - cbState;
      
      for (Integer i = 0; i < satCount; ++i)
      {
         for (int j = 0; j < 6; ++j)
            rawState[currentScState+j] = modelState[currentScState+j] - delta[j];
            
         // Copy any remaining state elements
         if (stateSize > 6)
            memcpy(&rawState[currentScState+6], &modelState[currentScState+6], 
                (stateSize-6)*sizeof(Real));
         // Move to the next state
         currentScState += stateSize;
      }
   }
}


//------------------------------------------------------------------------------
// void ReportEpochData()
//------------------------------------------------------------------------------
/**
 * Utility to help debug epoch issues.
 */
//------------------------------------------------------------------------------
void ForceModel::ReportEpochData()
{
   if (epochFile.is_open())
   {
      epochFile.precision(16);
         epochFile << epoch << " " << elapsedTime;

      epochFile << " Members ";
      for (std::vector<PhysicalModel*>::iterator i = forceList.begin();
           i != forceList.end(); ++i)
      {
         epochFile.precision(16);
         epochFile << " " 
                   << (*i)->GetRealParameter(satIds[0]) 
                   << " " << (*i)->GetTime();
      }

      epochFile << " Sats " ;
      for (std::vector<SpaceObject*>::iterator i = spacecraft.begin(); 
           i != spacecraft.end(); ++i)
      {
         epochFile.precision(16);
         epochFile << " " << (*i)->GetEpoch();
      }
      epochFile << "\n";
   }
   else
      throw ForceModelException(
         "ForceModel::ReportEpochData: Attempting to write epoch data without "
         "opening the data file.");
}
