//$Header$
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


//#define DEBUG_FORCEMODEL_INIT 1
//#define DEBUG_FORCEMODEL_EXE 1
//#define FORCE_REFERENCE_OBJECTS

#define normType 2

//---------------------------------
// static data
//---------------------------------

const std::string
ForceModel::PARAMETER_TEXT[ForceModelParamCount - PhysicalModelParamCount] =
{
    "CentralBody",
    "PrimaryBodies",
    "PointMasses",
    "Drag",
    "SRP"
};


const Gmat::ParameterType
ForceModel::PARAMETER_TYPE[ForceModelParamCount - PhysicalModelParamCount] =
{
    Gmat::STRING_TYPE,
    Gmat::STRINGARRAY_TYPE,
    Gmat::STRINGARRAY_TYPE,
    Gmat::STRING_TYPE,
    Gmat::STRING_TYPE
};


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
   centralBodyName   ("Earth"),
//   rawState          (NULL),
   j2kBodyName       ("Earth"),
   j2kBody           (NULL)  
{
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
    PhysicalModel    (fdf),
    previousState    (fdf.previousState),
    estimationMethod (fdf.estimationMethod),
    centralBodyName  (fdf.centralBodyName),
//    rawState         (NULL),
    j2kBodyName      (fdf.j2kBodyName),
    j2kBody          (NULL)  
{
   numForces = fdf.numForces;
   stateSize = fdf.stateSize;
   dimension = fdf.dimension;
   currentForce = fdf.currentForce;
   parameterCount = ForceModelParamCount;

   // Copy the forces.  May not work -- the copy constructors need to be checked
   for (std::vector<PhysicalModel *>::const_iterator pm = fdf.forceList.begin();
        pm != fdf.forceList.end(); ++pm)
      forceList.push_back((PhysicalModel*)(*pm)->Clone());
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

   numForces       = fdf.numForces;
   stateSize       = fdf.stateSize;
   dimension       = fdf.dimension;
   currentForce    = fdf.currentForce;
   parameterCount  = ForceModelParamCount;
   centralBodyName = fdf.centralBodyName;
   j2kBodyName     = fdf.j2kBodyName;
   j2kBody         = NULL;  
   
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
//    if ((pmType == "GravityField") || (pmType == "PointMassForce"))
//       pPhysicalModel->SetName("Gravity." +
//          pPhysicalModel->GetStringParameter("BodyName"));

    if (pmType == "DragForce")
       pPhysicalModel->SetName("Drag");


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
    epoch = so->GetRealParameter(so->GetParameterID("Epoch"));
    return true;
}


//------------------------------------------------------------------------------
// void ForceModel::UpdateSpaceObject(void)
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

//MessageInterface::ShowMessage("USO1  = [%lf %lf %lf]\n", rawState[0], rawState[1], rawState[2]);
      ReturnFromOrigin();
//MessageInterface::ShowMessage("USO2  = [%lf %lf %lf]\n", rawState[0], rawState[1], rawState[2]);
      
      for (sat = spacecraft.begin(); sat != spacecraft.end(); ++sat) {
         state = &((*sat)->GetState());
         stateSize = state->GetSize();
         vectorSize = stateSize * sizeof(Real);
         memcpy(&previousState[j*stateSize], state->GetState(), vectorSize);
         previousTime = 
            ((*sat)->GetRealParameter((*sat)->GetParameterID("Epoch")) - epoch)
            * 86400.0;
            
//         memcpy(state->GetState(), &modelState[j*stateSize], vectorSize);

         memcpy(state->GetState(), &rawState[j*stateSize], vectorSize);
         ++j;
            
         // Quick fix to get the epoch updated
         Real newepoch = epoch + elapsedTime / 86400.0;      

         //loj: 6/16/04 uncommented lines setting newEpoch.
         // for consecutive Propagate command, spacecraft epoch doesn't get updated
         
         // Update the epoch if it was passed in
         if (newEpoch != -1.0)
            newepoch = newEpoch;
         
         (*sat)->SetRealParameter((*sat)->GetParameterID("Epoch"), newepoch);
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
        Integer stateSize = 6;
        std::vector<SpaceObject *>::iterator sat;
        PropState *state;
        for (sat = spacecraft.begin(); sat != spacecraft.end(); ++sat) 
        {
            state = &((*sat)->GetState());
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
   Integer stateSize = 6;      // Will change if we integrate more variables
   Integer satCount = 1;
   std::vector<SpaceObject *>::iterator sat;

   if (!solarSystem)
      throw ForceModelException("Cannot initialize force model; no solar system");

   j2kBody = solarSystem->GetBody(j2kBodyName);
   if (j2kBody == NULL) 
      throw ForceModelException("Satellite J2000 body (" + j2kBodyName + 
         ") was not found in the solar system");

   forceOrigin = solarSystem->GetBody(centralBodyName);
   if (forceOrigin == NULL) 
      throw ForceModelException(
         "Force model origin (" + centralBodyName + 
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
    
   if (!PhysicalModel::Initialize())
      return false;
   rawState = new Real[dimension];

   if (spacecraft.size() == 0) 
   {
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
      currentPm->SetSolarSystem(solarSystem);
      currentPm->SetForceOrigin(forceOrigin);

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

   return true;
}


Integer ForceModel::GetOwnedObjectCount()
{
   return numForces;
}


GmatBase* ForceModel::GetOwnedObject(Integer whichOne)
{
   if (whichOne < numForces)
      return GetForce(whichOne);
      
   return NULL;
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
//MessageInterface::ShowMessage("UID1  = [%lf %lf %lf]\n", rawState[0], rawState[1], rawState[2]);
   Integer cf = currentForce;
   PhysicalModel *current = GetForce(cf);  // waw: added 06/04/04

   // Variables used to set spacecraft parameters
   std::string parmName, stringParm;
   std::vector<SpaceObject *>::iterator sat;
   Integer i;
   while (current) 
   {
      current->ClearSatelliteParameters();
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
//MessageInterface::ShowMessage("UID2  = [%lf %lf %lf]\n", rawState[0], rawState[1], rawState[2]);
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


Integer ForceModel::SetupSpacecraftData(GmatBase *sat, PhysicalModel *pm, 
                                        Integer i)
{
   Integer retval = i, id;
   std::string parmName;
   Real parm;
   std::string stringParm;
   
   if (sat->GetType() == Gmat::SPACECRAFT) {
      // Set epoch for the PhysicalModel to match the Spacecraft's
      parmName = "Epoch";
      id = sat->GetParameterID(parmName);
      if (id < 0)
         throw ForceModelException("Epoch parameter undefined on object " +
                                   sat->GetName());
      parm = sat->GetRealParameter(id);
      // Update local value for epoch
      epoch = parm;
      id = pm->GetParameterID(parmName);
      if (id < 0)
         throw ForceModelException("Epoch parameter undefined on PhysicalModel");
      pm->SetRealParameter(id, parm);

      //loj: 4/28/05 Replaced "ReferenceBody" with "CoordinateSystem"
      //parmName = "ReferenceBody";
      //id = sat->GetParameterID(parmName);
      //if (id < 0)
      //   throw ForceModelException("Reference body parameter undefined on object " +
      //                                   sat->GetName());
      
      //stringParm = sat->GetStringParameter(id);
      //pm->SetSatelliteParameter(i, parmName, stringParm);

      parmName = "CoordinateSystem";
      id = sat->GetParameterID(parmName);
      if (id < 0)
         throw ForceModelException(parmName + " parameter undefined on object " +
                                   sat->GetName());
      
      stringParm = sat->GetStringParameter(id);
      
      CoordinateSystem *cs =
         (CoordinateSystem*)(sat->GetRefObject(Gmat::COORDINATE_SYSTEM, stringParm));
      if (!cs)
         throw ForceModelException("CoordinateSystem is NULL\n");

      pm->SetSatelliteParameter(i, "ReferenceBody", cs->GetOriginName());
      
      parmName = "DryMass";
      id = sat->GetParameterID(parmName);
      if (id < 0)
         throw ForceModelException("Dry Mass parameter undefined on object " +
                                   sat->GetName());
      parm = sat->GetRealParameter(id);
      if (parm <= 0)
         throw ForceModelException("Mass parameter unphysical on object " + 
                                         sat->GetName());
      pm->SetSatelliteParameter(i, parmName, parm);
        
      parmName = "Cd";
      id = sat->GetParameterID(parmName);
      if (id < 0)
         throw ForceModelException("Cd parameter undefined on object " +
                                   sat->GetName());
      parm = sat->GetRealParameter(id);
      if (parm < 0)
         throw ForceModelException("Cd parameter unphysical on object " + 
                                   sat->GetName());
      pm->SetSatelliteParameter(i, parmName, parm);
      
      parmName = "DragArea";
      id = sat->GetParameterID(parmName);
      if (id < 0)
         throw ForceModelException("Drag Area parameter undefined on object " +
                                   sat->GetName());
      parm = sat->GetRealParameter(id);
      if (parm < 0)
         throw ForceModelException("Drag Area parameter unphysical on object " + 
                                   sat->GetName());
      pm->SetSatelliteParameter(i, parmName, parm);
      
      parmName = "SRPArea";
      id = sat->GetParameterID(parmName);
      if (id < 0)
         throw ForceModelException("SRP Area parameter undefined on object " +
                                   sat->GetName());
      parm = sat->GetRealParameter(id);
      if (parm < 0)
         throw ForceModelException("SRP Area parameter unphysical on object " + 
                                   sat->GetName());
      pm->SetSatelliteParameter(i, parmName, parm);
      
      parmName = "Cr";
      id = sat->GetParameterID(parmName);
      if (id < 0)
         throw ForceModelException("Cr parameter undefined on object " +
                                   sat->GetName());
      parm = sat->GetRealParameter(id);
      if (parm < 0)
         throw ForceModelException("Cr parameter unphysical on object " + 
                                   sat->GetName());
      pm->SetSatelliteParameter(i, parmName, parm);
   }
   else if (sat->GetType() == Gmat::FORMATION) {
      Integer j = -1;
      ObjectArray elements = sat->GetRefObjectArray("SpaceObject");
      for (ObjectArray::iterator n = elements.begin(); n != elements.end(); ++n) {
         ++j;
         j = SetupSpacecraftData(*n, pm, j);
      }
      retval = j;
   }
   else
      throw ForceModelException("Setting SpaceObject parameters on unknown type for " +
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
try
{
   if (order > 2)
      return false;
   if (!initialized)
      return false;
      
   Integer satCount = dimension / stateSize, i, iOffset;

   #ifdef DEBUG_FORCEMODEL_EXE
       MessageInterface::ShowMessage(
          "  Input state = %le %le %le %le %le %le\n", state[0], state[1], 
          state[2], state[3], state[4], state[5]);
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
         MessageInterface::ShowMessage("  ddt(%s[%s]) = %le %le %le\n",
            (current->GetTypeName().c_str()), 
            (current->GetStringParameter(
               current->GetParameterID("BodyName"))).c_str(), 
            ddt[3], ddt[4], ddt[5]);
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
            MessageInterface::ShowMessage("  deriv = %le %le %le\n", deriv[3], 
                                          deriv[4], deriv[5]);
         #endif
      }
      ++cf;
      current = GetForce(cf);
   }
   #ifdef DEBUG_FORCEMODEL_EXE
      MessageInterface::ShowMessage("  ===============================\n");
   #endif
}
catch (BaseException &ex)
{
    MessageInterface::ShowMessage(
       "Exception caught in ForceModel::GetDerivatives():\n   '%s'\n",
       ex.GetMessage().c_str());
    throw;
}
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
                if (mag >relativeErrorThreshold) 
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
 * @note This method catches exceptions, and just returns true or false to
 *       indicate success or failure.
 */
//------------------------------------------------------------------------------
const StringArray& 
   ForceModel::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   std::string pmName;
   StringArray pmRefs;
   
   forceReferenceNames.clear();
   
   // First do the base class call
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
                  pmName) != forceReferenceNames.end())
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
                     *j) != forceReferenceNames.end())
               forceReferenceNames.push_back(pmName);
         }
      }
      catch (BaseException &ex)
      {
         // Do nothing
      }
   }
   
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
            wasSet = true;
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
        return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - PhysicalModelParamCount)];
    else
        return PhysicalModel::GetParameterTypeString(id);
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
    case  SRP:
    {
       const PhysicalModel *pm = GetForce("SolarRadiationPressure");
       if (pm == NULL)
          return "Off";
       return "On";
    }
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
    case  SRP:
       return false;
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
   std::stringstream data;

   data.precision(18);   // Crank up data precision so we don't lose anything
   std::string preface = "", nomme;

   if ((mode == Gmat::SCRIPTING) || (mode == Gmat::OWNED_OBJECT) ||
       (mode == Gmat::SHOW_SCRIPT))
      inMatlabMode = false;
   if (mode == Gmat::MATLAB_STRUCT)
      inMatlabMode = true;

   if (useName != "")
      nomme = useName;
   else
      nomme = instanceName;

   if ((mode == Gmat::SCRIPTING) || (mode == Gmat::SHOW_SCRIPT))
   {
      std::string tname = typeName;
      if (tname == "PropSetup")
         tname = "Propagator";
      data << "Create " << tname << " " << nomme << ";\n";
      preface = "GMAT ";
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
   value.precision(18);

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
//                (parmType != Gmat::OBJECT_TYPE) &&
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
      MessageInterface::ShowMessage("\"%s\" has %d owned objects\n",
         instanceName.c_str(), GetOwnedObjectCount());
   #endif

   for (i = 0; i < GetOwnedObjectCount(); ++i)
   {
      newprefix = prefix;
      ownedObject = GetOwnedObject(i);
      if ((ownedObject != NULL))// &&
//          (ownedObject->GetTypeName() != "SolarRadiationPressure"))
      {
         nomme = BuildForceNameString((PhysicalModel*)ownedObject);

         #ifdef DEBUG_OWNED_OBJECT_STRINGS
             MessageInterface::ShowMessage(
                "   id %d has type %s and name \"%s\"\n",
                i, ownedObject->GetTypeName().c_str(),
                nomme.c_str());
         #endif

         if (nomme != "")
            newprefix += nomme + ".";
         else if (GetType() == Gmat::FORCE_MODEL)
            newprefix += ownedObject->GetTypeName();
         stream << ownedObject->GetGeneratingString(Gmat::OWNED_OBJECT, newprefix);
      }
      else
         MessageInterface::ShowMessage("Cannot access force %d\n", i);
   }
}


std::string ForceModel::BuildForceNameString(PhysicalModel *force)
{
   std::string retval = "UnknownForce", forceType = force->GetTypeName();
   
   if (forceType == "DragForce")
      retval = "Drag";
   if (forceType == "GravityField")
      retval = "Gravity." + force->GetStringParameter("BodyName");
   if (forceType == "PointMassForce")
      retval = force->GetStringParameter("BodyName");
   if (forceType == "SolarRadiationPressure")
      retval = "SRP";

   // Add others here
      
   return retval;
}


//------------------------------------------------------------------------------
// void MoveToOrigin()
//------------------------------------------------------------------------------
/**
 * Transforms the state vector from the internal coordinate system to the force 
 * model origin.
 */
//------------------------------------------------------------------------------
void ForceModel::MoveToOrigin()
{
   Integer satCount = dimension / stateSize;
   Integer currentScState = 0;
    
   if (centralBodyName == j2kBodyName)
      memcpy(modelState, rawState, dimension*sizeof(Real));
   else
   {
      Rvector6 cbState, j2kState, delta;
      Real now = epoch + elapsedTime / 86400.0; ;
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
// void MoveToOrigin()
//------------------------------------------------------------------------------
/**
 * Transforms the state vector from the force model origin to the internal 
 * coordinate system.
 */
//------------------------------------------------------------------------------
void ForceModel::ReturnFromOrigin()
{
//static Integer counter = 0;

   Integer satCount = dimension / stateSize;
   Integer currentScState = 0;
    
   if (centralBodyName == j2kBodyName)
      memcpy(rawState, modelState, dimension*sizeof(Real));
   else
   {
      Rvector6 cbState, j2kState, delta;
      Real now = epoch + elapsedTime / 86400.0; ;
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
//MessageInterface::ShowMessage("ReturnFromOrigin[%d], \n", ++counter);
}
