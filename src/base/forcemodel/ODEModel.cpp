//$Id$
//------------------------------------------------------------------------------
//                              ODEModel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// *** File Name : ODEModel.cpp
// *** Created   : October 1, 2002
// ***************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)      ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                   ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                              ***
// ***                                                                     ***
// ***  This software is subject to the Software Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Software Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is   ***
// ***  strictly prohibited.                                               ***
// ***                                                                     ***
// ***                                                                     ***
// ***  Header Version: July 12, 2002                                      ***
// ***************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 2/28/2003 - D. Conway, Thinking Systems, Inc.
//                             Filled in some code to model cases with 
//                             discontinuous forces (e.g. SRP)
//
//                           : 10/01/2003 - W. Waktola, Missions Applications
//                                                      Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//                                - Changed FlightDynamicsForces class to
//                                  ODEModel class
//
//                           : 10/20/2003 - W. Waktola, Missions Applications
//                                                      Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//
//                           : 11/9/2003 - D. Conway, Thinking Systems, Inc.
//                             Overrode GetParameterCount so the count 
//                             increases based on the member forces
// ***************************************************************************

#include "ODEModel.hpp"
#include "MessageInterface.hpp"
#include "PropagationStateManager.hpp"
#include "TimeTypes.hpp"

#include "GravityField.hpp"
#include "FormationInterface.hpp"

#include <string.h> 
#include <algorithm>    // for find()
#include <sstream>


//#define DEBUG_ODEMODEL
//#define DEBUG_ODEMODEL_INIT
//#define DEBUG_ODEMODEL_EXE
//#define DEBUG_INITIALIZATION
//#define DEBUG_FORCE_REF_OBJ
//#define DEBUG_ODEMODEL_EPOCHS
//#define DEBUG_SATELLITE_PARAMETERS
//#define DEBUG_FIRST_CALL
//#define DEBUG_GEN_STRING
//#define DEBUG_OWNED_OBJECT_STRINGS
//#define DEBUG_BUILDING_MODELS
//#define DEBUG_STATE
//#define DEBUG_MASS_FLOW
//#define DEBUG_REORIGIN
//#define DEBUG_ERROR_ESTIMATE
//#define DEBUG_STM_AMATRIX_DERIVS
//#define DEBUG_MU_MAP
//#define DEBUG_PM_EPOCH
//#define DEBUG_EVENTLOCATION
//#define DEBUG_FOR_CINTERFACE
//#define DEBUG_TRANSIENT_FORCES
//#define DEBUG_PARAMETER_INITIALIZATION
//#define DEBUG_DERIVATIVES_FOR_SPACECRAFT

//#define DUMP_ERROR_ESTIMATE_DATA
//#define DUMP_TOTAL_DERIVATIVE
//#define DUMP_INITIAL_STATE_DERIVATIVES_ONLY



//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

#ifdef DEBUG_FOR_CINTERFACE
#include <fstream>
std::ofstream debugFile;
#endif

//---------------------------------
// static data
//---------------------------------

#ifdef DEBUG_FIRST_CALL
static bool firstCallFired = false;
#endif


const std::string
ODEModel::PARAMETER_TEXT[ODEModelParamCount - PhysicalModelParamCount] =
{
   "CentralBody",
   "PrimaryBodies",
   "PolyhedralBodies",
   "PointMasses",
   "Drag",
   "SRP",
   "RelativisticCorrection",
   "ErrorControl",
   "CoordinateSystemList",
   
   // owned object parameters
   "Degree",
   "Order",
   "PotentialFile",

   // Parameters for Polyhedral Gravity
   "CreateForceBody",
   "ShapeFileName",
   "BodyDensity",

   "UserDefined"
};


const Gmat::ParameterType
ODEModel::PARAMETER_TYPE[ODEModelParamCount - PhysicalModelParamCount] =
{
   Gmat::OBJECT_TYPE,       // "CentralBody",
   Gmat::OBJECTARRAY_TYPE,  // "PrimaryBodies",
   Gmat::OBJECTARRAY_TYPE,  // "PolyhedralBodies",
   Gmat::OBJECTARRAY_TYPE,  // "PointMasses",
   Gmat::OBJECT_TYPE,       // "Drag",
   Gmat::ON_OFF_TYPE,       // "SRP",
   Gmat::ON_OFF_TYPE,       // "RelativisticCorrection",
   Gmat::ENUMERATION_TYPE,  // "ErrorControl",
   Gmat::OBJECTARRAY_TYPE,  // "CoordinateSystemList"
   
   // owned object parameters
   Gmat::INTEGER_TYPE,      // "Degree",
   Gmat::INTEGER_TYPE,      // "Order",
   Gmat::STRING_TYPE,       // "PotentialFile",

   // Additions for the polyhedral gravity model
   Gmat::OBJECT_TYPE,       // "CreateForceBody",
   Gmat::FILENAME_TYPE,     // "ShapeFileName",
   Gmat::REAL_TYPE,         // "BodyDensity",


   // plugin forces in the style of the solar sail plugin
   Gmat::OBJECTARRAY_TYPE,  // "UserDefined",
};


// Table of alternative words used in force model scripting
std::map<std::string, std::string> ODEModel::scriptAliases;

//--------------------------------------------------------------------------------
// static methods
//--------------------------------------------------------------------------------

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
void ODEModel::SetScriptAlias(const std::string& alias,
                              const std::string& typeName)
{
   if (scriptAliases.find(alias) == scriptAliases.end())
   {
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
std::string& ODEModel::GetScriptAlias(const std::string& alias)
{
   static std::string type;
   type = alias;
   if (scriptAliases.find(alias) != scriptAliases.end())
   {
      type = scriptAliases[alias];
   }
   return type;
}

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// ODEModel(const std::string &modelName, const std::string typeName)
//------------------------------------------------------------------------------
/**
 * The constructor
 *
 * @param modelName  Scripted name for the model
 * @param typeName   Type of model being built
 */
//------------------------------------------------------------------------------
ODEModel::ODEModel(const std::string &modelName, const std::string typeName) :
   PhysicalModel     (Gmat::ODE_MODEL, typeName, modelName),
//   previousState     (NULL),
   state             (NULL),
   psm               (NULL),
   estimationMethod  (ESTIMATE_STEP),     // Should this be removed?
   normType          (L2_DIFFERENCES),
   parametersSetOnce (false),
   centralBodyName   ("Earth"),
   coverageStartDetermined (false),
   forceMembersNotInitialized (true),
   satCount          (0),
   stateStart        (-1),
   stateEnd          (-1),
   cartStateSize     (0),
   dynamicProperties (false),
   isInitializedForParameters (false),
   warnedOnceForParameters (false),
   j2kBodyName       ("Earth"),
   j2kBody           (NULL),
   transientCount    (0)
{
   satIds[0] = satIds[1] = satIds[2] = satIds[3] = satIds[4] = 
   satIds[5] = satIds[6] = -1;
   
   objectTypes.push_back(Gmat::ODE_MODEL);
   objectTypeNames.push_back("ODEModel");
   objectTypeNames.push_back("ForceModel"); // For backwards compatibility

   numForces = 0;
   stateSize = 6;
   dimension = 6;
   currentForce = 0;
   parameterCount = ODEModelParamCount;

   // Do not allow ODE model changes in command mode
   blockCommandModeAssignment = true;

   muMap.clear();
}


//------------------------------------------------------------------------------
// ~ODEModel()
//------------------------------------------------------------------------------
/**
 * The destructor
 *
 * The destructor frees the memory used by the owned PhysicalModel instances.
 */
//------------------------------------------------------------------------------
ODEModel::~ODEModel()
{
   #ifdef DEBUG_ODEMODEL
      MessageInterface::ShowMessage
         ("ODEModel destructor entered, this=<%p>'%s', has %d forces\n",
          this, GetName().c_str(), forceList.size());
   #endif
   
//   if (previousState)
//      delete [] previousState;
   
   // Delete the owned objects
   ClearForceList();
   ClearInternalCoordinateSystems();

   muMap.clear();

   #ifdef DEBUG_FORCE_EPOCHS
      if (epochFile.is_open())
         epochFile.close();
   #endif
      
   #ifdef DEBUG_ODEMODEL
   MessageInterface::ShowMessage("ODEModel destructor exiting, has %d forces\n",
                                 forceList.size());
   #endif
}

//------------------------------------------------------------------------------
// ODEModel(const ODEModel& fdf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * Copies the ODEModel along with all of the owned PhysicalModels in the model.
 * This code depends on the copy constructors in the owned models; errors in
 * those pieces will impact the owned models in this code.
 *
 * @param fdf   The original of the ODEModel that is copied
 */
//------------------------------------------------------------------------------
ODEModel::ODEModel(const ODEModel& fdf) :
   PhysicalModel              (fdf),
//   previousState              (fdf.previousState),
   state                      (NULL),
   psm                        (NULL),
   estimationMethod           (fdf.estimationMethod),
   normType                   (fdf.normType),
   parametersSetOnce          (false),
   centralBodyName            (fdf.centralBodyName),
   coverageStartDetermined    (fdf.coverageStartDetermined),
   forceMembersNotInitialized (true),
   satCount                   (0),
   stateStart                 (fdf.stateStart),
   stateEnd                   (fdf.stateEnd),
   cartStateSize              (0),
   dynamicProperties          (false),
   isInitializedForParameters (false),
   warnedOnceForParameters    (false),
   j2kBodyName                (fdf.j2kBodyName),
   /// @note: Since the next three are global objects or reset by the Sandbox, 
   ///assignment works
   j2kBody                    (fdf.j2kBody),
   transientCount             (fdf.transientCount)
{
   #ifdef DEBUG_ODEMODEL
   MessageInterface::ShowMessage("ODEModel copy constructor entered\n");
   #endif
   
   satIds[0] = satIds[1] = satIds[2] = satIds[3] = satIds[4] = 
   satIds[5] = satIds[6] = -1;

   numForces           = fdf.numForces;
   stateSize           = fdf.stateSize;
   dimension           = fdf.dimension;
   currentForce        = fdf.currentForce;
   forceTypeNames      = fdf.forceTypeNames;
//   previousTime        = fdf.previousTime;
   transientForceNames = fdf.transientForceNames;
   forceReferenceNames = fdf.forceReferenceNames;
   
   parameterCount = ODEModelParamCount;
   
//   spacecraft.clear();
   forceList.clear();
   internalCoordinateSystems.clear();
   muMap.clear();
   
   // Copy the forces.  May not work -- the copy constructors need to be checked
   for (std::vector<PhysicalModel *>::const_iterator pm = fdf.forceList.begin();
        pm != fdf.forceList.end(); ++pm)
   {
      #ifdef DEBUG_ODEMODEL
      GmatBase *obj = (*pm);
      MessageInterface::ShowMessage
         ("   Cloning PhysicalModel <%p><%s>'%s'\n", obj, obj->GetTypeName().c_str(),
          obj->GetName().c_str());
      #endif
      PhysicalModel *newPm = (PhysicalModel*)(*pm)->Clone();
      forceList.push_back(newPm);
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (newPm, newPm->GetName(), "ODEModel::ODEModel(copy)",
          "*newPm = (*pm)->Clone()", this);
      #endif
   }
}


//------------------------------------------------------------------------------
// ODEModel& operator=(const ODEModel& fdf)
//------------------------------------------------------------------------------
/**
 * The assignment operator.
 * 
 * @param fdf   The original of the ODEModel that are copied
 * 
 * @return This ODEModel, configured to match fdf
 */
//------------------------------------------------------------------------------
ODEModel& ODEModel::operator=(const ODEModel& fdf)
{
   #ifdef DEBUG_ODEMODEL
      MessageInterface::ShowMessage("ODEModel <%p> Assignment Operator "
            "entered\n", this);
   #endif

   if (&fdf == this)
        return *this;

   PhysicalModel::operator=(fdf);

   satIds[0] = satIds[1] = satIds[2] = satIds[3] = satIds[4] = 
   satIds[5] = satIds[6] = -1;
   
   state = NULL;
   psm   = NULL;
   satCount = 0;
   stateStart = fdf.stateStart;
   stateEnd   = fdf.stateEnd;

   cartStateSize       = 0;
   isInitializedForParameters = false;
   warnedOnceForParameters = false;
   dynamicProperties   = false;

   numForces           = fdf.numForces;
   stateSize           = fdf.stateSize;
   dimension           = fdf.dimension;
   currentForce        = fdf.currentForce;
   forceTypeNames      = fdf.forceTypeNames;
//   previousTime        = fdf.previousTime;
//   previousState       = fdf.previousState;
   estimationMethod    = fdf.estimationMethod;
   normType            = fdf.normType;
   transientForceNames = fdf.transientForceNames;
   forceReferenceNames = fdf.forceReferenceNames;
   parametersSetOnce   = false;
//   modelEpochId        = -1;
   
   parameterCount      = ODEModelParamCount;
   centralBodyName     = fdf.centralBodyName;
   j2kBodyName         = fdf.j2kBodyName;
   
   coverageStartDetermined = fdf.coverageStartDetermined;

   /// @note: Since the next three are global objects or reset by the Sandbox, 
   ///assignment works
   j2kBody             = fdf.j2kBody;
   forceMembersNotInitialized = fdf.forceMembersNotInitialized;
   transientCount      = fdf.transientCount;

   // Clear owned objects before clone
   ClearForceList();
   ClearInternalCoordinateSystems();
   
//   spacecraft.clear();
   forceList.clear();
   
   // Copy the forces.  May not work -- the copy constructors need to be checked
   for (std::vector<PhysicalModel *>::const_iterator pm = fdf.forceList.begin();
        pm != fdf.forceList.end(); ++pm)
   {
      PhysicalModel *newPm = (PhysicalModel*)(*pm)->Clone();
      forceList.push_back(newPm);
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (newPm, newPm->GetName(), "ODEModel::operator=",
          "*newPm = (*pm)->Clone()", this);
      #endif
   }
   muMap = fdf.muMap;
   
   return *this;
}

//------------------------------------------------------------------------------
// void AddForce(PhysicalModel *pPhysicalModel)
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
 * destroyed externally.  In addition, every force that is passed to this class
 * needs to have a copy constructor and an assignment operator so that it can be 
 * cloned for distribution to multiple sandboxes.
 * 
 * @param pPhysicalModel        The force that is being added to the force model
 * 
 * @todo Document the mass depletion approach.
 */
//------------------------------------------------------------------------------
void ODEModel::AddForce(PhysicalModel *pPhysicalModel)
{
   if (pPhysicalModel == NULL)
      throw ODEModelException("Attempting to add a NULL force to " +
         instanceName);

   #ifdef DEBUG_ODEMODEL_INIT
      // Break into 2 pieces in case pPhysicalModel is out of scope
      MessageInterface::ShowMessage(
         "ODEModel::AddForce() <%p>'%s' entered, adding force = <%p> ", this,
         GetName().c_str(), pPhysicalModel);
      MessageInterface::ShowMessage(
         "<%s>'%s'\n", pPhysicalModel->GetTypeName().c_str(),
         pPhysicalModel->GetName().c_str());
   #endif

   #ifdef DEBUG_TRANSIENT_FORCES
      if (pPhysicalModel->IsTransient())
         MessageInterface::ShowMessage("+++ Trying to add transient %s with "
               "sat %s\n",
               pPhysicalModel->GetTypeName().c_str(),
               (pPhysicalModel->GetRefObjectNameArray(Gmat::SPACECRAFT))[0].c_str());
   #endif


	#ifdef DEBUG_EVENTLOCATION
      if (pPhysicalModel->IsOfType("EventModel"))
      {
         MessageInterface::ShowMessage("Adding an EventModel at \n");
         MessageInterface::ShowMessage("                        <%p>\n",
            pPhysicalModel);
      }
   #endif

   pPhysicalModel->SetDimension(dimension);
   isInitialized = false;
   
   // Handle the name issues
   std::string pmType = pPhysicalModel->GetTypeName();
   if (pmType == "DragForce")
      pPhysicalModel->SetName("Drag");

   std::string forceBody = pPhysicalModel->GetBodyName();

   // Trap multiple instances
   if ((pPhysicalModel->IsOfType("GravityBase"))||(pmType == "PointMassForce"))
   {
      std::string compType;
      
      for (std::vector<PhysicalModel*>::iterator i = forceList.begin();
           i != forceList.end(); ++i)
      {
         compType = (*i)->GetTypeName();
         if (((*i)->IsOfType("GravityBase")) || (compType == "PointMassForce"))
         {
            if ((*i)->GetBodyName() == forceBody && (*i) != pPhysicalModel)
               throw ODEModelException(
                  "Attempted to add a " + pmType + 
                  " force to the force model for the body " + forceBody +
                  ", but there is already a " + compType + 
                  " force in place for that body.");
         }

         if ((pPhysicalModel->IsOfType("GravityBase")) &&
             ((*i)->IsOfType("GravityBase")))
         {
            throw ODEModelException(
               "Attempted to add a GravityField (aka primary body) force to "
               "the force model \"" + instanceName + "\" for the body " +
               forceBody + ", but there already is a Gravity Field in the ODE "
               "Model and only one GravityField is supported per ODE Model in "
               "the current GMAT release");
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
         throw ODEModelException(
            "Attempted to add a drag force for the body " + forceBody +
            ", but that body is not set as a primary body, so it does not " +
            "support additional forces.");
   }
   
   if (pmType == "RelativisticCorrection")
   {
      std::map<std::string, Real>::iterator pos;
      std::string rcBodyName = pPhysicalModel->GetBodyName();
      for (pos = muMap.begin(); pos != muMap.end(); ++pos)
      {
         if (pos->first == rcBodyName)
         {
            #ifdef DEBUG_MU_MAP
               MessageInterface::ShowMessage("ODEModel::AddForce ---  setting "
                     "mu value of %12.10f on newly-added RC for body %s\n",
                     pos->second, rcBodyName.c_str());
            #endif
            pPhysicalModel->SetRealParameter(pPhysicalModel->GetParameterID("Mu"), pos->second);
         }
      }
   }

   // For some forces, if the force list already has the same type of force as
   // is associated as the new one, remove it so the new one is a replacement
   if (pPhysicalModel->IsUnique(forceBody))
   {
      for (std::vector<PhysicalModel*>::iterator i = forceList.begin();
            i != forceList.end(); ++i)
      {
         #ifdef DEBUG_ODEMODEL_INIT
            MessageInterface::ShowMessage("Checking uniqueness of <%p> "
                  "against <%p>\n", pPhysicalModel, *i);
         #endif
         if ((*i)->GetTypeName() == pmType)
         {
            if ((*i)->IsUnique(pPhysicalModel->GetBodyName()))
            {
               PhysicalModel *toBeReplaced = *i;
               forceList.erase(i);
               if (pmType == "DragForce")
               {
                  // Preserve the drag parameters
                  pPhysicalModel->SetRealParameter(toBeReplaced->GetParameterID("F107"),
                        toBeReplaced->GetRealParameter(toBeReplaced->GetParameterID("F107")));
                  pPhysicalModel->SetRealParameter(toBeReplaced->GetParameterID("F107A"),
                        toBeReplaced->GetRealParameter(toBeReplaced->GetParameterID("F107A")));
                  pPhysicalModel->SetRealParameter(toBeReplaced->GetParameterID("MagneticIndex"),
                        toBeReplaced->GetRealParameter(toBeReplaced->GetParameterID("MagneticIndex")));
               }
               delete toBeReplaced;
               break;
            }
         }
      }
   }

   // Add if new PhysicalModel pointer is not found in the forceList
   if (find(forceList.begin(), forceList.end(), pPhysicalModel) == forceList.end())
   {
      bool skipAdd = false;

      if (pPhysicalModel->IsTransient())
      {
         #ifdef DEBUG_ODEMODEL_INIT
            MessageInterface::ShowMessage("Adding a %s (%p) to this list:\n",
                  pPhysicalModel->GetTypeName().c_str(), pPhysicalModel);
            for (UnsignedInt i = 0; i < forceList.size(); ++i)
               MessageInterface::ShowMessage("   %s (%p)\n",
                     forceList[i]->GetTypeName().c_str(), forceList[i]);
            MessageInterface::ShowMessage("Transient count before "
                  "addition = %d\n", transientCount);
         #endif

         for (UnsignedInt i = 0; i < forceList.size(); ++i)
         {
            if (forceList[i]->IsTransient())
            {
               #ifdef DEBUG_TRANSIENT_FORCES
                  MessageInterface::ShowMessage("*** Found transient %s with "
                        "sat %s\n*** Addend is for sat %s\n",
                        forceList[i]->GetTypeName().c_str(),
                        forceList[i]->GetRefObjectNameArray(Gmat::SPACECRAFT)[0].c_str(),
                        (pPhysicalModel->GetRefObjectNameArray(Gmat::SPACECRAFT))[0].c_str());
               #endif
               if (forceList[i]->GetRefObjectNameArray(Gmat::SPACECRAFT)[0] !=
                     pPhysicalModel->GetRefObjectNameArray(Gmat::SPACECRAFT)[0])
                  skipAdd = true;
               #ifdef DEBUG_TRANSIENT_FORCES
                  else
                     MessageInterface::ShowMessage("*** Did not find sat %s "
                           "for transient force %s\n",
                           forceList[i]->GetRefObjectNameArray(Gmat::SPACECRAFT)[0].c_str(),
                           forceList[i]->GetTypeName().c_str());

               #endif
            }
         }

         if (!skipAdd)
            ++transientCount;

         // Temporary code: prevent multiple finite burns in single force model
         if (transientCount > 1)
            throw ODEModelException("Multiple Finite burns are not allowed in "
                  "a single propagator; try breaking commands of the form"
                  "\"Propagate prop(sat1, sat2)\" into two synchronized "
                  "propagators; e.g. \"Propagate Synchronized prop(sat1) "
                  "prop(sat2)\"\nexiting");
      }

      // Full field models come first to facilitate setting their parameters
      if (pPhysicalModel->IsOfType("GravityBase"))
         forceList.insert(forceList.begin(), pPhysicalModel);
      else if (!skipAdd)
         forceList.push_back(pPhysicalModel);
   }

   numForces = forceList.size();
   
   // Update owned object count
   ownedObjectCount = numForces;

   #ifdef DEBUG_ODEMODEL_INIT
      MessageInterface::ShowMessage("Leaving ODEModel::AddForce()\n");
   #endif
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
void ODEModel::DeleteForce(const std::string &name)
{
   #ifdef DEBUG_ODEMODEL
   MessageInterface::ShowMessage
      ("ODEModel::DeleteForce() entered, name='%s'\n", name.c_str());
   #endif
   
   for (std::vector<PhysicalModel *>::iterator force = forceList.begin(); 
        force != forceList.end(); ++force) 
   {
      std::string pmName = (*force)->GetName();
      if (name == pmName)
      {
         PhysicalModel* pm = *force;
         forceList.erase(force);
         
         if (!pm->IsTransient())
         {
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (pm, pm->GetName(), "ODEModel::DeleteForce()",
                "deleting non-transient force of " + pm->GetTypeName(), this);
            #endif
            delete pm;
         }
         else
            --transientCount;

         numForces = forceList.size();
         ownedObjectCount = numForces;
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
void ODEModel::DeleteForce(PhysicalModel *pPhysicalModel)
{
   #ifdef DEBUG_FORCELIST
      MessageInterface::ShowMessage("At start of ODEModel::"
            "DeleteForce(%p)\n", pPhysicalModel);
   #endif

   for (std::vector<PhysicalModel *>::iterator force = forceList.begin();
        force != forceList.end(); ++force) 
   {
      if (*force == pPhysicalModel)
      {
         PhysicalModel *pm = *force;
         forceList.erase(force);
         
         if (!pm->IsTransient())
         {
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (pm, pm->GetName(), "ODEModel::DeleteForce()",
                "deleting non-transient force of " + pm->GetTypeName(), this);
            #endif
            delete pm;
         }
         else
            --transientCount;

         numForces = forceList.size();
         ownedObjectCount = numForces;
         return;
      }
   }

      #ifdef DEBUG_FORCELIST
         MessageInterface::ShowMessage("At end of ODEModel::DeleteForce(), the "
               "%d forces on %s are:\n", numForces, instanceName.c_str());
         for (std::vector<PhysicalModel *>::iterator current = forceList.begin();
              current != forceList.end(); ++current)
            MessageInterface::ShowMessage("   %s at <%p> named '%s'\n",
                  (*current)->GetTypeName().c_str(), *current,
                  (*current)->GetName().c_str());
      #endif
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
bool ODEModel::HasForce(const std::string &name)
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
// Integer ODEModel::GetNumForces()
//------------------------------------------------------------------------------
/**
 * Gets the number of forces in the ODEModel
 *
 * @return The size of the force vector
 */
//------------------------------------------------------------------------------
Integer ODEModel::GetNumForces()
{
   numForces = forceList.size();
   return numForces;
}


//------------------------------------------------------------------------------
// StringArray& GetForceTypeNames()
//------------------------------------------------------------------------------
StringArray& ODEModel::GetForceTypeNames()
{
    forceTypeNames.clear();

    for (int i=0; i<numForces; i++)
        forceTypeNames.push_back(forceList[i]->GetTypeName());
        
    return forceTypeNames;
}

//------------------------------------------------------------------------------
// std::string GetForceTypeName(Integer index)
//------------------------------------------------------------------------------
std::string ODEModel::GetForceTypeName(Integer index)
{
    StringArray typeList = GetForceTypeNames();
    
    if (index >= 0 && index < numForces)
        return typeList[index];

    return "UNDEFINED_FORCE_TYPE";
}


////------------------------------------------------------------------------------
//// void ClearSpacecraft()
////------------------------------------------------------------------------------
//void ODEModel::ClearSpacecraft()
//{
//    spacecraft.clear();
//}


//------------------------------------------------------------------------------
// PhysicalModel* GetForce(Integer index) const
//------------------------------------------------------------------------------
PhysicalModel* ODEModel::GetForce(Integer index) const
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
const PhysicalModel* ODEModel::GetForce(std::string forcetype, 
										Integer whichOne) const
{
   Integer i = 0;

   for (std::vector<PhysicalModel *>::const_iterator force = forceList.begin(); 
        force != forceList.end(); ++force) 
   {
      std::string pmName = (*force)->GetTypeName();
      if (pmName == forcetype) 
	  {
         if (whichOne <= i)
            return *force;
         else
            ++i;
      }
   }

   return NULL;
}


////------------------------------------------------------------------------------
//// bool ODEModel::AddSpaceObject(SpaceObject *so)
////------------------------------------------------------------------------------
///**
// * Sets spacecraft and formations that use this force model.
// *
// * @param so The spacecraft or formation
// *
// * @return true is the object is added to the list, false if it was already
// *         in the list, or if it is NULL.
// */
////------------------------------------------------------------------------------
//bool ODEModel::AddSpaceObject(SpaceObject *so)
//{
//    if (so == NULL)
//        return false;
//
//    if (find(spacecraft.begin(), spacecraft.end(), so) != spacecraft.end())
//        return false;
//
//    std::string soJ2KBodyName = so->GetStringParameter("J2000BodyName");
//
//    // Set the refence body for the spacecraft states to match the J2000 body
//    // on the first spacecraft added to the force model.
//    if (spacecraft.size() == 0)
//       j2kBodyName = soJ2KBodyName;
//    else
//    {
//       if (j2kBodyName != soJ2KBodyName)
//          throw ODEModelException(
//             "Force model error -- the internal reference body for all "
//             "spacecraft in a force model must be the same.\n"
//             "The J2000Body for " + so->GetName() + " is " + soJ2KBodyName +
//             ", but the force model is already using " + j2kBodyName +
//             " as the reference body.");
//    }
//
//    spacecraft.push_back(so);
//
//    // Quick fix for the epoch update
//    satIds[0] = so->GetParameterID("A1Epoch");
//    epoch = so->GetRealParameter(satIds[0]);
//    parametersSetOnce = false;
//    return true;
//}


void ODEModel::BufferState()
{
   state = psm->GetState();
   previousState = (*state);
}
    

//------------------------------------------------------------------------------
// void UpdateSpaceObject(Real newEpoch)
//------------------------------------------------------------------------------
/**
 * Updates state data for the spacecraft or formation that use this force model.
 */
//------------------------------------------------------------------------------
void ODEModel::UpdateSpaceObject(Real newEpoch)
{
   #ifdef DEBUG_ODEMODEL_EXE
      MessageInterface::ShowMessage(
            "ODEModel::UpdateSpaceObject(%.12lf) called\n", newEpoch);
   #endif

   Integer stateSize;
   Integer vectorSize;
   GmatState *state;
   ReturnFromOrigin(newEpoch);
   
   state = psm->GetState();
   stateSize = state->GetSize();
   vectorSize = stateSize * sizeof(Real);
   
   previousState = (*state);

   #ifdef DEBUG_ODEMODEL_EXE
      MessageInterface::ShowMessage("Raw state: [%lf %lf %lf...\n", rawState[0],
            rawState[1], rawState[2]);
   #endif

   memcpy(state->GetState(), rawState, vectorSize);
   
   Real newepoch = epoch + elapsedTime / GmatTimeConstants::SECS_PER_DAY;
   
   // Update the epoch if it was passed in
   if (newEpoch != -1.0)
      newepoch = newEpoch;
   
   state->SetEpoch(newepoch);
   psm->MapVectorToObjects();
   
   // Update elements for each Formation
   for (UnsignedInt i = 0; i < stateObjects.size(); ++i)
      if (stateObjects[i]->IsOfType(Gmat::FORMATION))
         ((FormationInterface*)stateObjects[i])->UpdateElements();
   
   #ifdef DEBUG_ODEMODEL_EXE
      MessageInterface::ShowMessage
            ("ODEModel::UpdateSpaceObject() on \"%s\" prevElapsedTime = %f "
             "elapsedTime = %f newepoch = %f passed in epoch = %f "
             "dX's: [%.12lf] - [%.12lf] = [%.12lf]\n", 
             GetName().c_str(), previousState.GetEpoch(), elapsedTime, 
             newepoch, newEpoch, (*state)[0], previousState[0], 
             ((*state)[0] - previousState[0]));
   #endif
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
void ODEModel::UpdateFromSpaceObject()
{
   #ifdef DEBUG_ODEMODEL_EXE
      MessageInterface::ShowMessage
         ("ODEModel::UpdateFromSpaceObject() \n");
   #endif
   // Update elements for each Formation
   for (UnsignedInt i = 0; i < stateObjects.size(); ++i)
      if (stateObjects[i]->IsOfType(Gmat::FORMATION))
         ((FormationInterface*)stateObjects[i])->UpdateState();

   psm->MapObjectsToVector();
   GmatState *state = psm->GetState();
   memcpy(rawState, state->GetState(), state->GetSize() * sizeof(Real));

    // Transform to the force model origin
    // MoveToOrigin();					// made changes by TUAN NGUYEN: Notice that: without epoch, it will get wrong state of center body
   MoveToOrigin(state->GetEpoch());		// made changes by TUAN NGUYEN
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
void ODEModel::RevertSpaceObject()
{
   #ifdef DEBUG_ODEMODEL_EXE
      MessageInterface::ShowMessage
         ("ODEModel::RevertSpacecraft() prevElapsedTime=%f elapsedTime=%f\n",
          prevElapsedTime, elapsedTime);
   #endif

   elapsedTime = prevElapsedTime;

   memcpy(rawState, previousState.GetState(), dimension*sizeof(Real));
   MoveToOrigin();
}



//------------------------------------------------------------------------------
// bool BuildModelFromMap()
//------------------------------------------------------------------------------
/**
 * Sets up the PhysicalModels in the ODEModel.
 *
 * The model information is all contained in an associated
 * PropagationStateManager.  The PSM is passed in as a parameter on this
 * call.
 *
 * The derivative model is built based on the order of the items in the state
 * vector.  State vectors are built so that like elements of different objects
 * are grouped together -- so, for example, if a state vector contains multiple
 * spacecraft, the state vector has all of the CartesianState data that is
 * integrated grouped into one block, and other pieces grouped together in
 * separate blocks.  All of the like data for a given object is grouped
 * sequentially before moving to a second object.  Thus for a state vector set
 * for the Cartesian state propagation for two spacecraft and the STM for the
 * first but not the second, the state vector has 48 elements, and it looks like
 * this (the ellipses are used for brevity here):
 *
 *    vec = [sat1.X, sat1.y,...,sat1.VZ, sat2.X,...,sat2.VZ,
 *           sat1.STM_XX, sat1.STM_XY,...,sat1.STM_VZVZ]
 *
 * The mapping between the state elements and the differential equations modeled
 * in this ODEModel are constructed using integer identifiers for the data built
 * in each PhysicalModel.
 *
 * @return true if the physical models were mapped successfully, false if not.
 */
//------------------------------------------------------------------------------
bool ODEModel::BuildModelFromMap()
{
   bool retval = false;
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("ODEModel::BuildModelFromMap() Entered\n");
   #endif

   if (psm == NULL)
   {
      MessageInterface::ShowMessage("ODEModel::BuildModelFromMap():  Cannot "
            "build the model %s: PropStateManager is NULL\n",
            instanceName.c_str());
      return retval;
   }

   const std::vector<ListItem*> *map = psm->GetStateMap();

   if (map == NULL)
   {
      MessageInterface::ShowMessage("ODEModel::BuildModelFromMap():  Cannot "
            "build the model: the map is NULL for %s\n", instanceName.c_str());
      return retval;
   }

   Integer start = 0, objectCount = 0;
   Gmat::StateElementId id = Gmat::UNKNOWN_STATE;
   GmatBase *currentObject = NULL;

   dynamicProperties = false;
   dynamicsIndex.clear();
   dynamicObjects.clear();
   dynamicIDs.clear();

   // Loop through the state map, counting objects for each type needed
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("ODEModel map has %d entries\n",
            map->size());
   #endif
   for (UnsignedInt index = 0; index < map->size(); ++index)
   {
      if ((*map)[index]->dynamicObjectProperty)
      {
         dynamicProperties = true;

         dynamicsIndex.push_back(index);
         dynamicObjects.push_back((*map)[index]->object);
         dynamicIDs.push_back((*map)[index]->parameterID);
      }

      // When the elementID changes, act on the previous data processed
      if (id != (Gmat::StateElementId)((*map)[index]->elementID))
      {
         // Only build elements if an object needs them
         if (objectCount > 0)
         {
            // Build the derivative model piece for this element
            retval = BuildModelElement(id, start, objectCount);
            if (retval == false)
            {
//               throw ODEModelException(
               MessageInterface::ShowMessage(
                     "Failed to build an element (id:%d) of the ODEModel.\n",
                     id);
               
               retval = true;
            }
         }
         // A new element type was encountered, so reset the pointers & counters
         id = (Gmat::StateElementId)((*map)[index]->elementID);
         objectCount = 0;
         start = index;
         currentObject = NULL;
      }

      // Increment the count for each new object
      if (currentObject != (*map)[index]->object)
      {
         currentObject = (*map)[index]->object;
         if (currentObject->IsOfType(Gmat::FORMATION))
         {
            FormationInterface *form = (FormationInterface*)currentObject;
            ObjectArray oa = form->GetRefObjectArray(Gmat::SPACEOBJECT);
            Integer fc = oa.size();
            objectCount += fc;
         }
         else
            ++objectCount;
      }
   }

   // Catch the last element
   if (objectCount > 0)
   {
      retval = BuildModelElement(id, start, objectCount);
      if (retval == false)
      {
         // throw ODEModelException(
         MessageInterface::ShowMessage(
            "Failed to build an element (id=%d) of the ODEModel.\n",
            id);
         retval = true;
      }
   }

   #ifdef DEBUG_BUILDING_MODELS
      // Show the state structure
      MessageInterface::ShowMessage("State vector has the following structure:\n");
      MessageInterface::ShowMessage("   ID     Start   Count\n");
      for (std::vector<StateStructure>::iterator i = sstruct.begin();
            i != sstruct.end(); ++i)
         MessageInterface::ShowMessage("   %4d      %2d    %d\n", i->id, i->index,
               i->count);
   #endif
         
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("ODEModel::BuildModelFromMap() Finished\n");
         #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool BuildModelElement(Gmat::StateElementId id, Integer start,
//               Integer objectCount, std::map<GmatBase*,Integer> associateMap)
//------------------------------------------------------------------------------
/**
 * Constructs the derivative mapping an element of the ODEModel.
 *
 * This method tells each PhysicalModel about the information in the state
 * vector that expects derivative information used in the superposition.
 *
 * The PhysicalModel class has two methods that facilitate this mapping:
 *
 *    SupportsDerivative(Gmat::StateElementId id) -- Checks the model to see if
 *                                      it supports derivative information for a
 *                                      specific state parameter
 *
 *    SetStart(Gmat::StateElementId id, Integer index, Integer quantity) --
 *                                      Sets the start index for the
 *                                      derivative information in the state
 *                                      vector and the number of objects in the
 *                                      vector that receive this information.
 *
 * When this method is called, it loops through the PhysicalModels in the
 * ODEModel and, for each model supporting the element, registers the start
 * index and size of the portion of the state vector that receives the
 * derivative data.
 *
 * @param id      The integer ID for the element that is being registered
 * @param start   The index for the first element in the state vector
 * @param objectCount The number of objects that need the derivative data
 *
 * @return true if the derivative was set successfully for at least one
 *         PhysicalModel, false if it failed.
 */
//------------------------------------------------------------------------------
bool ODEModel::BuildModelElement(Gmat::StateElementId id, Integer start,
      Integer objectCount)
{
   bool retval = false, tf;
   Integer modelsUsed = 0;

   #ifdef DEBUG_BUILDING_MODELS
      MessageInterface::ShowMessage("Building ODEModel element; id = %d, "
            "index = %d, count = %d; force list has %d elements\n", id, start,
            objectCount, forceList.size());
   #endif

   // Loop through the PhysicalModels, checking to see if any support the
   // indicated element.  If there is no derivative model anywhere in the PM
   // list, then the element will not change during integration, and the return
   // value for building that derivative model is false.  If support is
   // registered, then the PhysicalModel is handed information about where to
   // place the derivative data in the derivative state vector.
   for (std::vector<PhysicalModel *>::iterator i = forceList.begin();
         i != forceList.end(); ++i)
   {
      if ((*i)->SupportsDerivative(id))
      {
         tf = (*i)->SetStart(id, start, objectCount);
         if (tf == false)
            MessageInterface::ShowMessage("PhysicalModel %s was not set, even "
                  "though it registered support for derivatives of type %d\n",
                  (*i)->GetTypeName().c_str(), id);
         else
            ++modelsUsed;

         if (retval == false)
            retval = tf;
      }
   }
    
   StateStructure newStruct;
   newStruct.id = id;
   newStruct.index = start;
   newStruct.count = objectCount;
   // newStruct.size = ??;
   sstruct.push_back(newStruct);

   // Cartesian state as a special case so error control can do RSS tests
   /// @todo Check this piece again for 6DoF
   if (id == Gmat::CARTESIAN_STATE)
   {
      cartesianCount   = objectCount;
      cartesianStart = start;
      cartStateSize  = objectCount * 6;
   }

   if (id == Gmat::ORBIT_STATE_TRANSITION_MATRIX)
   {
      fillSTM = true;
      if (stmStart == -1)
         stmStart = start;
      stmCount = objectCount;
   }

   if (id == Gmat::ORBIT_A_MATRIX)
   {
      fillAMatrix = true;
      if (aMatrixStart == -1)
         aMatrixStart = start;
      ++aMatrixCount;
   }

   if (id == Gmat::EVENT_FUNCTION_STATE)
   {
      // todo Fill this in?
   }

   #ifdef DEBUG_BUILDING_MODELS
      MessageInterface::ShowMessage(
            "ODEModel is using %d components for element %d\n", modelsUsed, id);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool CheckQualifier(const std::string &qualifier, const std::string &forType)
//------------------------------------------------------------------------------
/**
 * Ensures that the string qualifier applies to an owned model
 *
 * This method was added so that qualified force model settings can validate
 * that the force receiving a setting is correct.  It is used, for example, with
 * the full field gravity model strings of the form
 *
 *    Forces.GravityField.Earth.Order = 8;
 *
 * to ensure that the model is actually Earth based.
 *
 * @param qualifier The string qualifier
 * @param forType String identifying owned object type, if needed
 *
 * @return true if the qualifier matches the model, false if not
 */
//------------------------------------------------------------------------------
bool ODEModel::CheckQualifier(const std::string &qualifier,
                              const std::string &forType)
{
   bool retval = false;

   // Find the owned model
   for (UnsignedInt i = 0; i < forceList.size(); ++i)
   {
      if (forceList[i]->IsOfType(forType))
      {
         retval = forceList[i]->CheckQualifier(qualifier);
      }
      if (retval)
         break;
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes model and all contained models
 */
//------------------------------------------------------------------------------
bool ODEModel::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("ODEModel::Initialize() entered\n");
   #endif

//   Integer stateSize = 6;      // Will change if we integrate more variables
//   Integer satCount = 1;
//   std::vector<SpaceObject *>::iterator sat;
   
   if (!solarSystem)
      throw ODEModelException(
         "Cannot initialize force model; no solar system on '" + 
         instanceName + "'");

   if (j2kBodyName != "")
   {
      j2kBody = solarSystem->GetBody(j2kBodyName);
      if (j2kBody == NULL)
         throw ODEModelException("ODEModel J2000 body (" + j2kBodyName +
            ") was not found in the solar system");
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
         "Calling PhysicalModel::Initialize(); dimension = %d, state = <%p>\n", 
         dimension, state);
   #endif

   // If the PSM hasn't set the state yet, it's not yet time to initialize
   if (state == NULL)
   {
      isInitialized = false;
      return false;
   }

   dimension = state->GetSize();

   if (!PhysicalModel::Initialize())
      return false;

   // Incorporate any temporary affects -- e.g. finite burn
   UpdateTransientForces();

   Integer newDim = state->GetSize();
   if (newDim != dimension)
   {
      // If dimensions don't match with transient forces, reinitialize
      dimension = newDim;
      if (!PhysicalModel::Initialize())
         return false;
   }

   #ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("Configuring for state of dimension %d\n",
         dimension);
   #endif

   // rawState deallocated in PhysicalModel::Initialize() method so reallocate
   rawState = new Real[dimension];
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (rawState, "rawState", "ODEModel::Initialize()",
       "rawState = new Real[dimension]", this);
   #endif

   memcpy(rawState, state->GetState(), dimension * sizeof(Real));

   // update the starting time to the first available time for the origin.
   // This is necessary because there may not be data for the force origin body
   // at the default initial epoch (esp. if the source is SPK).
   if (!coverageStartDetermined)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Determining start of coverage for body %s\n",
               forceOrigin->GetName().c_str());
      #endif
      Real coverageStart = forceOrigin->GetFirstStateTime();
      if (coverageStart > epoch)
      {
//         std::stringstream warn;
//         warn << "Warning: Data for force origin "
//              << forceOrigin->GetName() << " at epoch ";
//         warn << epoch << " is not available.  Setting initial epoch to "
//                          "earliest time of available data, "
//              << coverageStart << ".\n";
//         MessageInterface::PopupMessage(Gmat::WARNING_, warn.str());
         epoch = coverageStart;
         state->SetEpoch(epoch);
      }
      coverageStartDetermined = true;
   }
      
   MoveToOrigin();

   // Variables used to set spacecraft parameters
   std::string parmName, stringParm;

   for (std::vector<PhysicalModel *>::iterator current = forceList.begin();
        current != forceList.end(); ++current)
   {
      #ifdef DEBUG_ODEMODEL_INIT
      { // Manage scoping for vars named same as in the class
         std::string name, type, bodyNm;
         name = (*current)->GetName();
         if (name == "")
            name = "unnamed";
         type = (*current)->GetTypeName();
         bodyNm = (*current)->GetBodyName();
         MessageInterface::ShowMessage
            ("ODEModel::Initialize() initializing object %s of type %s for "
                  "body %s\n", name.c_str(), type.c_str(), bodyNm.c_str());
         MessageInterface::ShowMessage(" and the state epoch = %le\n", state->GetEpoch());
      }
      #endif
      
      (*current)->SetDimension(dimension);
      (*current)->SetState(state);
      
      // Only initialize the spacecraft independent pieces once
      if (forceMembersNotInitialized)
      {
         (*current)->SetSolarSystem(solarSystem);
         
         // Handle missing coordinate system issues for GravityFields
		  if ((*current)->IsOfType("HarmonicField"))
         {
			   SetInternalCoordinateSystem("InputCoordinateSystem", (*current));
            SetInternalCoordinateSystem("FixedCoordinateSystem", (*current));
            SetInternalCoordinateSystem("TargetCoordinateSystem", (*current));

            if (body == NULL)
               body = solarSystem->GetBody(centralBodyName); // or should we get bodyName?
         }

         if ((*current)->IsOfType("DragForce"))
         {
            // This one is not a DragForce parameter:
            // SetInternalCoordinateSystem("InputCoordinateSystem", (*current));
            SetInternalCoordinateSystem("FixedCoordinateSystem", (*current));
         }
      }

      // Initialize the forces
      if (!(*current)->Initialize())
      {
         std::string msg = "Component force ";
         msg += (*current)->GetTypeName();
         msg += " failed to initialize";
         throw ODEModelException(msg);
      }
      if ((*current)->IsOfType("GravityField"))
      {
         // get the name and mu value for the muMap
         std::string itsName;
         Real        itsMu;
         ((GravityField*) (*current))->GetBodyAndMu(itsName, itsMu);
         #ifdef DEBUG_MU_MAP
            MessageInterface::ShowMessage("ODEModel::Initialize - saving mu "
                  "value of %12.10f for body %s\n", itsMu, itsName.c_str());
         #endif
         muMap[itsName] = itsMu;
      }
      (*current)->SetState(modelState);
   }

   for (std::vector<PhysicalModel *>::iterator current = forceList.begin();
        current != forceList.end(); ++current)
   {
      if ((*current)->IsOfType("RelativisticCorrection"))
      {
         std::map<std::string, Real>::iterator pos;
         std::string rcBodyName = (*current)->GetBodyName();
         for (pos = muMap.begin(); pos != muMap.end(); ++pos)
         {
            if (pos->first == rcBodyName)
            {
               #ifdef DEBUG_MU_MAP
                  MessageInterface::ShowMessage("ODEModel::Initialize ---  "
                        "setting mu value of %12.10f on RC for body %s\n",
                        pos->second, rcBodyName.c_str());
               #endif
               (*current)->SetRealParameter((*current)->GetParameterID("Mu"),
                     pos->second);
            }
         }
      }
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

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("ODEModel::Initialize() complete\n");
   #endif

//   modelState = state->GetState();

   if (forceList.size() == 0)
      throw ODEModelException("The ODE model " + instanceName +
            " is empty, so it cannot be used for propagation.");

   isInitialized = true;

   #ifdef DEBUG_MU_MAP
      std::map<std::string, Real>::iterator pos;
      MessageInterface::ShowMessage("----> At end of Initialize(), the muMap "
            "is:");
      for (pos = muMap.begin(); pos != muMap.end(); ++pos)
      {
         MessageInterface::ShowMessage("      %s      %12.10f\n",
               (pos->first).c_str(), pos->second);
      }
   #endif

   #ifdef DEBUG_FORCELIST
      MessageInterface::ShowMessage("At end of ODEModel::Initialize(), the "
            "forces on %s are:\n", instanceName.c_str());
      for (std::vector<PhysicalModel *>::iterator current = forceList.begin();
           current != forceList.end(); ++current)
         MessageInterface::ShowMessage("   %s at <%p> named '%s'\n",
               (*current)->GetTypeName().c_str(), *current,
               (*current)->GetName().c_str());
   #endif

   return true;
}


//------------------------------------------------------------------------------
// void ClearForceList()
//------------------------------------------------------------------------------
void ODEModel::ClearForceList(bool deleteTransient)
{
   #ifdef DEBUG_ODEMODEL_CLEAR
      MessageInterface::ShowMessage("ODEModel::ClearForceList() entered, there "
            "are %d forces\n", forceList.size());
   #endif

   #ifdef DEBUG_FORCELIST
      MessageInterface::ShowMessage("In ODEModel::ClearForceList(%s), the "
            "forces on %s are:\n", deleteTransient ? "true" : "false",
            instanceName.c_str());
      int i = 0;
      for (std::vector<PhysicalModel *>::iterator current = forceList.begin();
           current != forceList.end(); ++current)
      {
         MessageInterface::ShowMessage("   %d: ", ++i);
         MessageInterface::ShowMessage("%s at <%p> named '%s'\n",
               (*current)->GetTypeName().c_str(), *current,
               (*current)->GetName().c_str());
      }
   #endif
   
   // Delete the owned forces
   std::vector<PhysicalModel *>::iterator ppm = forceList.begin();
   PhysicalModel *pm;
   while (ppm != forceList.end())
   {
      pm = *ppm;
      forceList.erase(ppm);
      
      #ifdef DEBUG_ODEMODEL_CLEAR
         MessageInterface::ShowMessage("   Checking if pm<%p> is transient\n",
               pm);
      #endif
      
      // Transient forces are managed in the Sandbox.
      if (pm->IsTransient())
         --transientCount;

      if (!pm->IsTransient()) // || (deleteTransient && pm->IsTransient()))
      {
         #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (pm, pm->GetName(), "ODEModel::~ODEModel()",
                "deleting non-transient \"" + pm->GetTypeName() +
                "\" PhysicalModel", this);
         #endif
         delete pm;
      }
      ppm = forceList.begin();
   }
   
}


//------------------------------------------------------------------------------
// void ClearInternalCoordinateSystems()
//------------------------------------------------------------------------------
/**
 * Manages the deallocation of coordinate systems used internally.
 */
//------------------------------------------------------------------------------
void ODEModel::ClearInternalCoordinateSystems()
{
   for (std::vector<CoordinateSystem*>::iterator i = 
           internalCoordinateSystems.begin();
        i != internalCoordinateSystems.end(); ++i)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         ((*i), (*i)->GetName(), "ODEModel::ClearInternalCoordinateSystems()",
          "deleting ICS", this);
      #endif
      delete (*i);
   }
   internalCoordinateSystems.clear();
}

//------------------------------------------------------------------------------
// void SetInternalCoordinateSystem(const std::string csId, 
//      PhysicalModel *currentPm)
//------------------------------------------------------------------------------
/**
 * Manages the allocation of coordinate systems used internally.
 * 
 * @param csId        Parameter name for the coordinate system label.
 * @param currentPm   Force that needs the CoordinateSystem.
 */
//------------------------------------------------------------------------------
void ODEModel::SetInternalCoordinateSystem(const std::string csId,
                                           PhysicalModel *currentPm)
{
   std::string csName;
   CoordinateSystem *cs = NULL;

   #ifdef DEBUG_ODEMODEL_INIT     
      MessageInterface::ShowMessage(
         "Setting internal CS with ID '%s' for force type '%s'\n",
         csId.c_str(), currentPm->GetTypeName().c_str());
   #endif
   csName = currentPm->GetStringParameter(csId);

   try
   {
      currentPm->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
   }
   catch (BaseException &)
   {
      #ifdef DEBUG_ODEMODEL_INIT
         MessageInterface::ShowMessage(
            "Adding a coordinate system named '%s' for the %s physical model\n",
            csName.c_str(), currentPm->GetTypeName().c_str());
      #endif
      
      for (std::vector<CoordinateSystem*>::iterator i =
              internalCoordinateSystems.begin();
           i != internalCoordinateSystems.end(); ++i)
         if ((*i)->GetName() == csName)
            cs = *i;
      
      if (cs == NULL)
      {
         std::string axisString;
         if (csName.find("Fixed", 0) != std::string::npos)
            axisString = "BodyFixed";
         else
            axisString = "MJ2000Eq";

         if (solarSystem == NULL)
            throw ODEModelException("Trying to create a local coordinate "
                  "system, but the solar system pointer is NULL");

         SpacePoint *earthPtr = solarSystem->GetBody(SolarSystem::EARTH_NAME);
         cs = CoordinateSystem::CreateLocalCoordinateSystem(csName,
               axisString, earthPtr, NULL, NULL, j2kBody, solarSystem);
         
         cs->SetName(csName);
         cs->SetStringParameter("Origin", centralBodyName);
         cs->SetRefObject(forceOrigin, Gmat::CELESTIAL_BODY, 
            centralBodyName);
         internalCoordinateSystems.push_back(cs);

         #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Add
               (cs, csName, "ODEModel::SetInternalCoordinateSystem()",
                "cs = earthFixed->Clone()", this);
         #endif

         #ifdef DEBUG_ODEMODEL_INIT
            MessageInterface::ShowMessage("Created %s with description\n\n%s\n", 
               csName.c_str(), 
               cs->GetGeneratingString(Gmat::SCRIPTING).c_str());
         #endif
      }
      
      cs->SetSolarSystem(solarSystem);
      cs->SetJ2000BodyName(j2kBody->GetName());
      cs->SetJ2000Body(j2kBody);
      cs->Initialize();

      #ifdef DEBUG_ODEMODEL_INIT     
         MessageInterface::ShowMessage(
            "New coordinate system named '%s' has definition\n%s\n",
            csName.c_str(), 
            cs->GetGeneratingString(Gmat::SCRIPTING, "   ").c_str());
      #endif
         
      currentPm->SetRefObject(cs, Gmat::COORDINATE_SYSTEM, csName);
   }
}


//------------------------------------------------------------------------------
// Integer GetOwnedObjectCount()
//------------------------------------------------------------------------------
/**
 * Retrieves the number of owned objects in the ODEModel
 *
 * @return The count
 */
//------------------------------------------------------------------------------
Integer ODEModel::GetOwnedObjectCount()
{
   #ifdef DEBUG_ODEMODEL_OWNED_OBJECT
   MessageInterface::ShowMessage
      ("ODEModel::GetOwnedObjectCount() this=<%p>'%s' returning %d\n",
       this, GetName().c_str(), numForces);
   #endif

   numForces = forceList.size();
   return numForces;
}


//------------------------------------------------------------------------------
// GmatBase* GetOwnedObject(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Retrieves an owned object for configuration
 *
 * @param whichOne Index of the owned object
 *
 * @return The object
 */
//------------------------------------------------------------------------------
GmatBase* ODEModel::GetOwnedObject(Integer whichOne)
{
   if (whichOne < numForces) 
      return forceList[whichOne];
   
   return NULL;
}


//------------------------------------------------------------------------------
// virtual std::string BuildPropertyName(GmatBase *ownedObj)
//------------------------------------------------------------------------------
/*
 * Builds property name of owned object.
 * This method is called when special handling of object property name is
 * required when writing object. For example, ODEModel requires additional
 * name Earth for GravityField as in FM.GravityField.Earth.Degree.
 *
 * @param ownedObj The object of property handling
 * @return The property name
 */
//------------------------------------------------------------------------------
std::string ODEModel::BuildPropertyName(GmatBase *ownedObj)
{
   #ifdef DEBUG_OWNED_OBJECT_STRINGS
   MessageInterface::ShowMessage
      ("ODEModel::BuildPropertyName() called with ownedObj type=%s\n",
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
void ODEModel::UpdateInitialData(bool dynamicOnly)
{
   PhysicalModel *current; // = forceList[cf];  // waw: added 06/04/04

   // Variables used to set spacecraft parameters
   std::string parmName, stringParm;
   std::vector<SpaceObject *>::iterator sat;

   for (std::vector<PhysicalModel*>::iterator i = forceList.begin();
        i != forceList.end(); ++i)
   {
      stateObjects.clear();
      psm->GetStateObjects(stateObjects, Gmat::SPACEOBJECT);
      
      if (dynamicOnly)
      {
         UpdateDynamicSpacecraftData(&stateObjects, 0);
      }
      else
      {
         current = (*i);
         if (!parametersSetOnce)
         {
            current->ClearSatelliteParameters();
         }

         SetupSpacecraftData(&stateObjects, 0);
      }
   }
   if (!dynamicOnly)
      psm->MapObjectsToVector();

   parametersSetOnce = true;
}


//------------------------------------------------------------------------------
// void UpdateTransientForces()
//------------------------------------------------------------------------------
/**
 * Tells the transient forces in the model about the propagated SpaceObjects.
 *
 * In GMAT, a "transient force" is a force that is applied based on state
 * changes made to the elements that are propagated during a run.  In other
 * words, a transient force is a force that gets applied when needed, but not
 * typically throughout the mission.  An example is a finite burn: the
 * acceleration for a finite burn is calculated and applied only when a
 * spacecraft has a thruster that has been turned on.
 *
 * @param transientSats The list of spacecraft that report active transient
 *                      forces.
 */
//------------------------------------------------------------------------------
void ODEModel::UpdateTransientForces()
{
   #ifdef DEBUG_INITIALIZATION
     MessageInterface::ShowMessage("ODEModel::UpdateTransientForces entered\n");
   #endif
   if (psm == NULL)
   {
//      MessageInterface::ShowMessage("PSM is NULL\n");
//      throw ODEModelException(
//            "Cannot initialize ODEModel; no PropagationStateManager");
      return;
   }
   
   const std::vector<ListItem*> *propList = psm->GetStateMap();
         
   // This version won't work for Formations.  
   // todo: make into a recursive method for Formations
   ObjectArray propObjects;
   for (std::vector<ListItem*>::const_iterator i = propList->begin(); 
        i != propList->end(); ++i)
   {
      GmatBase *obj =  (*i)->object;
      if (obj != NULL)
      {
         if (obj->IsOfType(Gmat::SPACECRAFT))
         {
            if (find(propObjects.begin(), propObjects.end(), obj) == 
                      propObjects.end())
            {
               #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage
                  ("ODEModel::UpdateTransientForces() Adding <%p>'%s' to propObjects\n",
                   obj, obj->GetName().c_str());
               #endif
               propObjects.push_back(obj);
            }
         }
      }
   }
   
   transientCount = 0;
   for (std::vector<PhysicalModel *>::iterator tf = forceList.begin();
        tf != forceList.end(); ++tf) 
   {
      if ((*tf)->IsTransient())
      {
         ++transientCount;
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("Updating transient force %s\n",
               (*tf)->GetName().c_str());
         #endif
         (*tf)->SetPropList(&propObjects);
      }
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
Integer ODEModel::SetupSpacecraftData(ObjectArray *sats, Integer i)
{
   #ifdef DEBUG_SPACECRAFT_PROPERTIES
      MessageInterface::ShowMessage("ODEModel::SetupSpacecraftData(*, %d) "
            "called\n", i);
   #endif

   Real parm;
   std::string stringParm;

   GmatBase* sat;
   Integer increment = 1;
   
   for (ObjectArray::iterator j = sats->begin(); 
        j != sats->end(); ++j)
   {
      sat = *j;

      #ifdef DEBUG_SPACECRAFT_PROPERTIES
         MessageInterface::ShowMessage("   Looking at %s\n",
               sat->GetName().c_str());
      #endif
      // Only retrieve the parameter IDs once
      if ((satIds[1] < 0) && sat->IsOfType("Spacecraft"))
      {
         satIds[0] = sat->GetParameterID("A1Epoch");
         if (satIds[0] < 0)
            throw ODEModelException("Epoch parameter undefined on object " +
                                    sat->GetName());
         
         satIds[1] = sat->GetParameterID("CoordinateSystem");
         if (satIds[1] < 0)
            throw ODEModelException(
               "CoordinateSystem parameter undefined on object " + sat->GetName());
         
         satIds[2] = sat->GetParameterID("TotalMass");
         if (satIds[2] < 0)
            throw ODEModelException("TotalMass parameter undefined on object " +
                                    sat->GetName());
         
         satIds[3] = sat->GetParameterID("Cd");
         if (satIds[3] < 0)
            throw ODEModelException("Cd parameter undefined on object " +
                                    sat->GetName());
         
         satIds[4] = sat->GetParameterID("DragArea");
         if (satIds[4] < 0)
            throw ODEModelException("Drag Area parameter undefined on object " +
                                    sat->GetName());
         
         satIds[5] = sat->GetParameterID("SRPArea");
         if (satIds[5] < 0)
            throw ODEModelException("SRP Area parameter undefined on object " +
                                    sat->GetName());
         
         satIds[6] = sat->GetParameterID("Cr");
         if (satIds[6] < 0)
            throw ODEModelException("Cr parameter undefined on object " +
                                    sat->GetName());
         
         stateStart = sat->GetParameterID("CartesianX");
         stateEnd   = sat->GetParameterID("CartesianVZ");

         #ifdef DEBUG_SATELLITE_PARAMETERS
         MessageInterface::ShowMessage(
            "Parameter ID Array: [%d %d %d %d %d %d %d]; PMepoch id  = %d\n",
            satIds[0], satIds[1], satIds[2], satIds[3], satIds[4], satIds[5], 
               satIds[6], PhysicalModel::EPOCH);
         #endif
      }
      
      PhysicalModel *pm;
      for (std::vector<PhysicalModel *>::iterator current = forceList.begin();
           current != forceList.end(); ++current)
      {
         pm = *current;
         
         if (sat->GetType() == Gmat::SPACECRAFT)
         { 
            #ifdef DEBUG_SATELLITE_PARAMETERS
               MessageInterface::ShowMessage(
                   "ODEModel '%s', Member %s: %s->ParmsChanged = %s, "
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
            pm->SetRealParameter(PhysicalModel::EPOCH, parm);
            #ifdef DEBUG_PM_EPOCH
               MessageInterface::ShowMessage("ODEModel '%s', Member %s: epoch being set to %le (from sc %s)\n",
                   GetName().c_str(), pm->GetTypeName().c_str(), parm, sat->GetName().c_str());
            #endif
            
            if (((SpaceObject*)sat)->ParametersHaveChanged() ||
                !parametersSetOnce)
            {
               #ifdef DEBUG_SATELLITE_PARAMETERS
                  MessageInterface::ShowMessage(
                     "Setting parameters for %s using data from %s\n",
                     pm->GetTypeName().c_str(), sat->GetName().c_str());
               #endif
               
               // ... Coordinate System ...
               stringParm = sat->GetStringParameter(satIds[1]);
               

               if (pm->UsesSpacecraftOrigin())
               {
                  CoordinateSystem *cs =
                     (CoordinateSystem*)(sat->GetRefObject(Gmat::COORDINATE_SYSTEM,
                         stringParm));
                  if (!cs)
                  {
                     char sataddr[20];
                     std::sprintf(sataddr, "%lx", (unsigned long)sat);
                     throw ODEModelException(
                        "CoordinateSystem is NULL on Spacecraft " + sat->GetName() +
                        " at address " + sataddr);
                  }
                  pm->SetSatelliteParameter(i, "ReferenceBody", cs->GetOriginName());
               }
               else
                  pm->SetSatelliteParameter(i, "ReferenceBody", centralBodyName);

               // ... Mass ...
               parm = sat->GetRealParameter(satIds[2]);
               if (parm <= 0)
                  throw ODEModelException("Total spacecraft mass is less than zero for Spacecraft \"" +
                                    sat->GetName() + "\"" +  " used by Forcemodel \"" + instanceName + "\"");
               pm->SetSatelliteParameter(i, "Mass", parm, satIds[2]);
               
               // ... Coefficient of drag ...
               parm = sat->GetRealParameter(satIds[3]);
               if (parm < 0)
                  throw ODEModelException("Drag coefficient (Cd) is less than zero for Spacecraft \"" +
                                    sat->GetName() + "\"" +  " used by Forcemodel \"" + instanceName + "\"");
               pm->SetSatelliteParameter(i, "Cd", parm, satIds[3]);
               
               // ... Drag area ...
               parm = sat->GetRealParameter(satIds[4]);
               if (parm < 0)
                  throw ODEModelException("Drag area is less than zero for Spacecraft \"" +
                                    sat->GetName() + "\"" +  " used by Forcemodel \"" + instanceName + "\"");
               pm->SetSatelliteParameter(i, "DragArea", parm, satIds[4]);
               
               // ... SRP area ...
               parm = sat->GetRealParameter(satIds[5]);
               if (parm < 0)
                  throw ODEModelException("SRP area is less than zero for Spacecraft \"" +
                                    sat->GetName() + "\"" +  " used by Forcemodel \"" + instanceName + "\"");
               pm->SetSatelliteParameter(i, "SRPArea", parm, satIds[5]);
               
               // ... and Coefficient of reflectivity
               parm = sat->GetRealParameter(satIds[6]);
               if (parm < 0)
                  throw ODEModelException("SRP coefficient (Cr) is less than zero for Spacecraft \"" +
                                    sat->GetName() + "\"" +  " used by Forcemodel \"" + instanceName + "\"");
               pm->SetSatelliteParameter(i, "Cr", parm, satIds[6]);
               
               ((SpaceObject*)sat)->ParametersHaveChanged(false);
            }
            increment = 1;
         }
         else if (sat->GetType() == Gmat::FORMATION) 
         {
            ObjectArray formSats;
            ObjectArray elements = sat->GetRefObjectArray("SpaceObject");
            for (ObjectArray::iterator n = elements.begin(); n != elements.end();
                 ++n) 
            {
               if ((*n)->IsOfType(Gmat::SPACEOBJECT))
                  formSats.push_back((SpaceObject *)(*n));
               else
                  throw ODEModelException("Object \"" + sat->GetName() +
                                          "\" is not a SpaceObject.");
            }
            increment = SetupSpacecraftData(&formSats, i) - i;
         }
         else
            throw ODEModelException("Setting SpaceObject parameters on unknown "
                  "type for " + sat->GetName());
      }
      i += increment;
   }
   
   #ifdef DEBUG_SPACECRAFT_PROPERTIES
      MessageInterface::ShowMessage("   ---> %d returned\n", i);
   #endif

   return i;
}


Integer ODEModel::UpdateDynamicSpacecraftData(ObjectArray *sats, Integer i)
{
   Real parm;
   std::string stringParm;

   GmatBase* sat;

   for (ObjectArray::iterator j = sats->begin();
        j != sats->end(); ++j)
   {
      sat = *j;

      if (satIds[1] < 0)
         throw ODEModelException("Epoch parameter undefined on object " +
                                    sat->GetName());

      PhysicalModel *pm;
      for (std::vector<PhysicalModel *>::iterator current = forceList.begin();
           current != forceList.end(); ++current)
      {
         pm = *current;

         if (sat->GetType() == Gmat::SPACECRAFT)
         {
            #ifdef DEBUG_SATELLITE_PARAMETER_UPDATES
               MessageInterface::ShowMessage(
                   "ODEModel '%s', Member %s: %s->ParmsChanged = %s, "
                   "parametersSetOnce = %s\n",
                   GetName().c_str(), pm->GetTypeName().c_str(),
                   sat->GetName().c_str(),
                   (((SpaceObject*)sat)->ParametersHaveChanged() ? "true" : "false"),
                   (parametersSetOnce ? "true" : "false"));
            #endif

               // ... Mass ...
               parm = sat->GetRealParameter(satIds[2]);
               if (parm <= 0)
                  throw ODEModelException("Mass parameter unphysical on object " +
                     sat->GetName());
               pm->SetSatelliteParameter(i, satIds[2], parm);

               // ... Drag area ...
               parm = sat->GetRealParameter(satIds[4]);
               if (parm < 0)
                  throw ODEModelException("Drag Area parameter unphysical on object " +
                     sat->GetName());
               pm->SetSatelliteParameter(i, satIds[4], parm);

               // ... SRP area ...
               parm = sat->GetRealParameter(satIds[5]);
               if (parm < 0)
                  throw ODEModelException("SRP Area parameter unphysical on object " +
                     sat->GetName());
               pm->SetSatelliteParameter(i, satIds[5], parm);
         }
         else if (sat->GetType() == Gmat::FORMATION)
         {
            ObjectArray formSats;
            ObjectArray elements = sat->GetRefObjectArray("SpaceObject");
            for (ObjectArray::iterator n = elements.begin(); n != elements.end();
                 ++n)
            {
               if ((*n)->IsOfType(Gmat::SPACEOBJECT))
                  formSats.push_back((SpaceObject *)(*n));
               else
                  throw ODEModelException("Object \"" + sat->GetName() +
                                          "\" is not a SpaceObject.");
            }
            UpdateDynamicSpacecraftData(&formSats, i);
         }
         else
            throw ODEModelException(
                                    "Setting SpaceObject parameters on unknown "
                                    "type for " + sat->GetName());
      }
      ++i;
   }

   return i;
}


//------------------------------------------------------------------------------
// bool GetDerivatives(Real * state, Real dt, Integer order)
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
 * @param    id      StateElementId for the requested derivative
 */
//------------------------------------------------------------------------------
bool ODEModel::GetDerivatives(Real * state, Real dt, Integer order,
      const Integer id)
{
   #ifdef DEBUG_ODEMODEL_EXE
      MessageInterface::ShowMessage("  ODEModel Input state = [");
      for (Integer i = 0; i < dimension-1; ++i)
         MessageInterface::ShowMessage("%le, ", state[i]);
      MessageInterface::ShowMessage("%le]\n", state[dimension - 1]);
      MessageInterface::ShowMessage(" and epoch = %le]\n", epoch);
      MessageInterface::ShowMessage("   dynamicProperties: %s\n", (dynamicProperties? "true" : "false"));
   #endif

   #ifdef DEBUG_FOR_CINTERFACE
      if (debugFile.is_open() == false)
         debugFile.open("ODE_Debug.txt");
      debugFile << GetGeneratingString(Gmat::NO_COMMENTS);
      debugFile.precision(16);
      debugFile << "GetDv called at epoch " 
                << epoch 
                << " with dt = " 
                << dt 
                << ":\n   state:";
      for (Integer i = 0; i < dimension; ++i)
         debugFile << "   " << state[i];
      debugFile << "\n";
   #endif

   #ifdef DEBUG_FIRST_CALL
      if (firstCallFired == false)
      {
         MessageInterface::ShowMessage(
            "GetDerivatives input state at dt = %.15lf: "
            "[%.15lf %.15lf %.15lf %.16le %.16le %.16le]\n", dt,
            state[0], state[1], state[2], state[3], state[4], state[5]);
      }
   #endif

   // Temporary code: prevent multiple spacecraft in finite burn PropSetup
   stateObjects.clear();
   psm->GetStateObjects(stateObjects, Gmat::SPACEOBJECT);
   if ((transientCount > 0) && (stateObjects.size() > 1))
      throw ODEModelException("Multiple Spacecraft are not allowed in "
            "a propagator driving a finite burn; try breaking commands "
            "of the form \"Propagate prop(sat1, sat2)\" that execute "
            "with a finite burn on one of the spacecraft into two "
            "synchronized propagators; e.g. \"Propagate Synchronized "
            "prop(sat1) prop(sat2)\"\nexiting");

   if (order > 2)
   {
      return false;
   }
   if (!isInitialized)
   {
      return false;
   }

   if ((order == 2) && (fillSTM))
	   throw ODEModelException("Second order integrators cannot be used when "
			   "propagating the Orbit State Transition Matrix (STM); please "
			   "use a different integrator.");

   if (dynamicProperties)
   {
      for (UnsignedInt i = 0; i < dynamicsIndex.size(); ++i)
      {
         #ifdef DEBUG_MASS_FLOW
            MessageInterface::ShowMessage("Updating %s on %s to %.12le\n",
                  dynamicObjects[i]->GetParameterText(dynamicIDs[i]).c_str(),
                  dynamicObjects[i]->GetName().c_str(),
                  state[dynamicsIndex[i]]);
         #endif
         if (((dynamicIDs[i] >= stateStart) && (dynamicIDs[i] <= stateEnd)))
         {
            SpacePoint *objBody = NULL;
            if (dynamicObjects[i]->IsOfType(Gmat::SPACEOBJECT))
               objBody = ((SpaceObject*)(dynamicObjects[i]))->GetOrigin();
            if (objBody != NULL)
            {
//               Rvector6 offset = objBody->GetMJ2000State(((SpaceObject*)
               Rvector6 offset = forceOrigin->GetMJ2000State(((SpaceObject*)
                     (dynamicObjects[i]))->GetEpoch() + dt /
                     GmatTimeConstants::SECS_PER_DAY);

               #ifdef DEBUG_DYNAMIC_FORCES
                  MessageInterface::ShowMessage("Dynamics: resetting %s.%s "
                        "from value %lf to %lf + %lf = %lf\n",
                        dynamicObjects[i]->GetName().c_str(),
                        dynamicObjects[i]->GetParameterText(dynamicIDs[i]).c_str(),
                        dynamicObjects[i]->GetRealParameter(dynamicIDs[i]),
                        state[dynamicsIndex[i]], offset[dynamicIDs[i]-stateStart],
                        state[dynamicsIndex[i]] + offset[dynamicIDs[i]-stateStart]);
               #endif

               dynamicObjects[i]->SetRealParameter(dynamicIDs[i],
                     state[dynamicsIndex[i]] +
                     offset[dynamicIDs[i]-stateStart]);
            }
            else
               dynamicObjects[i]->SetRealParameter(dynamicIDs[i],
                     state[dynamicsIndex[i]]);
         }
         else  // Handle the other (non-Cartesian) dynamical property changes
            dynamicObjects[i]->SetRealParameter(dynamicIDs[i],
                  state[dynamicsIndex[i]]);
      }
//      UpdateInitialData(true);
   }
   UpdateInitialData(true);

   #ifdef DEBUG_ODEMODEL_EXE
      MessageInterface::ShowMessage("Initializing derivative array\n");
   #endif

   #ifdef DEBUG_STATE
      MessageInterface::ShowMessage(
         "Top of GetDeriv; State with dimension %d = [", dimension);
      for (Integer i = 0; i < dimension-1; ++i)
         MessageInterface::ShowMessage("%le, ", state[i]);
      MessageInterface::ShowMessage("%le]\n", state[dimension-1]);
      MessageInterface::ShowMessage("in GetDeriv, epoch = %le\n", epoch);
   #endif
  
   PrepareDerivativeArray();

   const Real* ddt;

   #ifdef DEBUG_ODEMODEL_EXE
      MessageInterface::ShowMessage("Looping through %d PhysicalModels\n",
            forceList.size());
   #endif

   #ifdef DEBUG_FOR_CINTERFACE
      debugFile << "Forces:";
   #endif

   // Apply superposition of forces/derivatives
   for (std::vector<PhysicalModel *>::iterator i = forceList.begin();
         i != forceList.end(); ++i)
   {
      #ifdef DEBUG_ODEMODEL_EXE
         MessageInterface::ShowMessage("   %s\n", ((*i)->GetTypeName()).c_str());
      #endif

      #ifdef DEBUG_FOR_CINTERFACE
         debugFile << "   " << (*i)->GetTypeName();
      #endif

      ddt = (*i)->GetDerivativeArray();
      if (!(*i)->GetDerivatives(state, dt, order))
      {
         #ifdef DEBUG_ODEMODEL_EXE
            MessageInterface::ShowMessage("Derivative %s failed\n",
                  ((*i)->GetTypeName()).c_str());
         #endif
         return false;

      }
      
      #ifdef DEBUG_ODEMODEL_EXE
      for (Integer j = 0; j < dimension; ++j)
         MessageInterface::ShowMessage("  ddt(%s[%s])[%d] = %le\n",
            ((*i)->GetTypeName().c_str()),
            ((*i)->GetStringParameter((*i)->GetParameterID("BodyName"))).c_str(),
            j, ddt[j]);
      #endif
      
      #ifdef DEBUG_ODEMODEL_EXE
         MessageInterface::ShowMessage("  deriv = [");
      #endif
      for (Integer j = 0; j < dimension; ++j)
      {
         deriv[j] += ddt[j];
         #ifdef DEBUG_ODEMODEL_EXE
            MessageInterface::ShowMessage(" %16.14le ", ddt[j]);
         #endif
      }
      #ifdef DEBUG_ODEMODEL_EXE
         MessageInterface::ShowMessage("]\n");
      #endif


      #ifdef DEBUG_FIRST_CALL
         if (firstCallFired == false)
         {
            MessageInterface::ShowMessage(
               "   %s(%s)::GetDerivatives --> "
               "[%.15lf %.15lf %.15lf %.16le %.16le %.16le]\n",
               (*i)->GetTypeName().c_str(),
               (*i)->GetName().c_str(),
               ddt[0], ddt[1], ddt[2], ddt[3], ddt[4], ddt[5]);
         }
      #endif
   }

   #ifdef DEBUG_FOR_CINTERFACE
      debugFile << "\n";
   #endif
   
   if (fillCartesian)
   {
      if (order == 1)  // Fill in 1st dv of position with the input velocity
      {
         for (Integer i = 0; i < cartStateSize; i += 6)
         {
            deriv[cartesianStart + i]     = state[cartesianStart + i + 3];
            deriv[cartesianStart + i + 1] = state[cartesianStart + i + 4];
            deriv[cartesianStart + i + 2] = state[cartesianStart + i + 5];
         }
      }
   }

   if (psm->RequiresCompletion())
      CompleteDerivativeCalculations(state);

   #ifdef DEBUG_ODEMODEL_EXE
      MessageInterface::ShowMessage("  ===============================\n");
   #endif

   #ifdef DEBUG_FIRST_CALL
      if (firstCallFired == false)
      {
         MessageInterface::ShowMessage(
            "GetDerivatives: [%.16lf %.16lf %.16lf %.16lf %.16lf %.16lf] "
            "(%s)\n", deriv[0], deriv[1], deriv[2], deriv[3], deriv[4], deriv[5],
            (psm->RequiresCompletion() ? "completion was required" : 
            "completion was not required"));
      }

      firstCallFired = true;
   #endif

   #ifdef DEBUG_STATE
      MessageInterface::ShowMessage(
               "End of GetDeriv; State with dimension %d = [", dimension);
      for (Integer i = 0; i < dimension-1; ++i) //< state->GetSize()-1; ++i)
         MessageInterface::ShowMessage("%.12le, ", state[i]); //(*state)[i]);
      MessageInterface::ShowMessage("%.12le]\n", state[dimension-1]); //(*state)[state->GetSize()-1]);

      MessageInterface::ShowMessage(
               "   Derivative = [");
      for (Integer i = 0; i < dimension-1; ++i) //< state->GetSize()-1; ++i)
         MessageInterface::ShowMessage("%.12le, ", deriv[i]); //(*state)[i]);
      MessageInterface::ShowMessage("%.12le]\n", deriv[dimension-1]); //(*state)[state->GetSize()-1]);
   #endif

   #ifdef DUMP_TOTAL_DERIVATIVE
      bool initialStatesOnly = false;

      #ifdef DUMP_INITIAL_STATE_DERIVATIVES_ONLY
         initialStatesOnly = true;
      #endif

      if ((dt == 0.0) || !initialStatesOnly)
      {
         MessageInterface::ShowMessage("   Derivative A1MJD Epoch = %.12lf",
               epoch + (dt / 86400.0));
         MessageInterface::ShowMessage(
                  "   Derivative = [");
         for (Integer i = 0; i < dimension-1; ++i) //< state->GetSize()-1; ++i)
            MessageInterface::ShowMessage("%.12le, ", deriv[i]); //(*state)[i]);
         MessageInterface::ShowMessage("%.12le]\n", deriv[dimension-1]); //(*state)[state->GetSize()-1]);
      }
   #endif

   #ifdef DEBUG_STM_AMATRIX_DERIVS
      MessageInterface::ShowMessage("Final dv array:\n");

      for (Integer i = 0; i < 6; ++i)
      {
         MessageInterface::ShowMessage("  %lf  ", deriv[i]);
      }
      MessageInterface::ShowMessage("\n");
      for (Integer i = 6; i < dimension; i += 6)
      {
         for (Integer j = 0; j < 6; ++j)
            MessageInterface::ShowMessage("  %le  ", deriv[i+j]);
         MessageInterface::ShowMessage("\n");
      }
   #endif

   #ifdef DEBUG_FOR_CINTERFACE
      debugFile << "Derivative:\n";
      for (Integer i = 0; i < dimension; ++i)
         debugFile << "   " << deriv[i];
      debugFile << "\n";
   #endif

   // Sanity check the derivative
   for (Integer index = 0; index < stateSize; ++index)
   {
      if (GmatMathUtil::IsNaN(deriv[index]))
         throw ODEModelException("The ForceModel " + instanceName +
               " generated a derivative that is not a number");

      if (GmatMathUtil::IsInf(deriv[index]))
         throw ODEModelException("The ForceModel " + instanceName +
               " generated a derivative that is infinite");
   }

   return true;
}


//------------------------------------------------------------------------------
// bool PrepareDerivativeArray()
//------------------------------------------------------------------------------
/**
 * Sets up the propagation vector for derivative data
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool ODEModel::PrepareDerivativeArray()
{
   bool retval = true;

   #ifdef DEBUG_STM_AMATRIX_DERIVS
      static bool eins = false;
   #endif

   const std::vector<ListItem*> *smap = psm->GetStateMap();

   // Initialize the derivative array
   for (Integer i = 0; i < dimension; ++i)
   {
      #ifdef DEBUG_STM_AMATRIX_DERIVS
         if (eins == false)
            MessageInterface::ShowMessage("Mapping [%d] %s\n", i,
                  ((*smap)[i]->nonzeroInit == true ? "true" : "false"));
      #endif

      if ((*smap)[i]->nonzeroInit)
      {
         deriv[i] = (*smap)[i]->initialValue;
      }
      else
         deriv[i] = 0.0;
   }

   #ifdef DEBUG_STM_AMATRIX_DERIVS
      if (eins == false)
      {
         MessageInterface::ShowMessage("Initial dv array:\n");

         for (Integer i = 0; i < 6; ++i)
         {
            MessageInterface::ShowMessage("  %lf  ", deriv[i]);
         }
         MessageInterface::ShowMessage("\n");
         for (Integer i = 6; i < dimension; i += 6)
         {
            for (Integer j = 0; j < 6; ++j)
               MessageInterface::ShowMessage("  %lf  ", deriv[i+j]);
            MessageInterface::ShowMessage("\n");
         }
      }

      eins = true;
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool CompleteDerivativeCalculations(Real *state)
//------------------------------------------------------------------------------
/**
 * Completes the computations needed for the A-Matrix and STM
 *
 * @param state The current state vector
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool ODEModel::CompleteDerivativeCalculations(Real *state)
{
   #ifdef DEBUG_AMATRIX_AND_STM
      MessageInterface::ShowMessage("Entered ODEModel::CompleteDerivative"
            "Calculations with stmCount = %d and fillSTM = %s\n", stmCount,
            fillSTM ? "true" : "false");
   #endif

   bool retval = true;

   for (Integer i = 0; i < stmCount; ++i)
   {
      Integer i6 = stmStart + i * 36;

      // Build aTilde
      Real aTilde[36];
      for (Integer m = 0; m < 36; ++m)
         aTilde[m] = deriv[i6+m];

      if (fillSTM)
      {
         // Convert A to Phi dot for STM pieces
         // \Phi\dot = A\tilde \Phi
         for (Integer j = 0; j < 6; ++j)
         {
            for (Integer k = 0; k < 6; ++k)
            {
               Integer element = j * 6 + k;
               deriv[i6+element] = 0.0;
               for (Integer l = 0; l < 6; ++l)
               {
                  deriv[i6+element] += aTilde[j*6+l] * state[i6+l*6+k];
               }
            }
         }
      }
   }
   return retval;
}


//------------------------------------------------------------------------------
// Real ODEModel::EstimateError(Real *diffs, Real *answer) const
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
Real ODEModel::EstimateError(Real *diffs, Real *answer) const
{
   if (estimationMethod == ESTIMATE_IN_BASE)
      return PhysicalModel::EstimateError(diffs, answer);

   Real retval = 0.0, err, mag, vec[3];

   #ifdef DEBUG_ERROR_ESTIMATE
      MessageInterface::ShowMessage("ODEModel::EstimateError normType == %d; "
            "dimension = %d\n", normType, dimension);
   #endif

   #ifdef DUMP_ERROR_ESTIMATE_DATA
      MessageInterface::ShowMessage("EstimateError; normType == %d, "
            "dimension = %d\n", normType, dimension);

      MessageInterface::ShowMessage("   Starting state:       [%.12lf",
            modelState[0]);
      for (Integer i = 1; i < dimension; ++i)
         MessageInterface::ShowMessage(" %.12lf", modelState[i]);
      MessageInterface::ShowMessage("]\n");

      MessageInterface::ShowMessage("   Reported differences: [%.12le",
            diffs[0]);
      for (Integer i = 1; i < dimension; ++i)
         MessageInterface::ShowMessage(" %.12le", diffs[i]);
      MessageInterface::ShowMessage("]\n");

      MessageInterface::ShowMessage("   Current answer      : [%.12lf",
            answer[0]);
      for (Integer i = 1; i < dimension; ++i)
         MessageInterface::ShowMessage(" %.12lf", answer[i]);
      MessageInterface::ShowMessage("]\n");
   #endif

   // Handle non-Cartesian state elements as an L1 norm
   for (int i = 0; i < cartesianStart; ++i)
   {
      // L1 norm
      mag = fabs(answer[ i ] - modelState[ i ]);
      err = fabs(diffs[i]);
      if (mag >relativeErrorThreshold)
         err = err / mag;

      #ifdef DEBUG_ERROR_ESTIMATE
         MessageInterface::ShowMessage("   {%d EstErr = %le} ", i, err);
      #endif

      if (err > retval)
      {
         retval = err;
      }
   }

   // Handle the Cartesian piece
   for (int i = cartesianStart; i < cartesianStart + cartStateSize; i += 3)
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
            break;
      }

      #ifdef DEBUG_ERROR_ESTIMATE
         MessageInterface::ShowMessage("   {%d EstErr = %le} ", i, err);
      #endif

      if (err > retval)
      {
         retval = err;
      }
   }

   // Handle non-Cartesian state elements as an L1 norm
   for (int i = cartesianStart + cartStateSize; i < dimension; ++i)
   {
      // L1 norm
      mag = fabs(answer[ i ] - modelState[ i ]);
      err = fabs(diffs[i]);
      if (mag >relativeErrorThreshold)
         err = err / mag;

      #ifdef DEBUG_ERROR_ESTIMATE
         MessageInterface::ShowMessage("   {%d EstErr = %le} ", i, err);
      #endif

      if (err > retval)
      {
         retval = err;
      }
   }

   #ifdef DEBUG_ERROR_ESTIMATE
      MessageInterface::ShowMessage("   >>> Estimated Error = %le\n", retval);
   #endif

   #ifdef DUMP_ERROR_ESTIMATE_DATA
      MessageInterface::ShowMessage("   ---> Estimated error = %.12le\n",
            retval);
   #endif

   return retval;
}


//---------------------------------------------------------------------------
// bool TakeAction(const std::string &action, const std::string &actionData = "")
//---------------------------------------------------------------------------
bool ODEModel::TakeAction(const std::string &action, const std::string &actionData)
{
   if (action == "ClearDefaultForce")
   {
      std::vector<PhysicalModel*>::iterator oldIter = forceList.end();
      for (std::vector<PhysicalModel*>::iterator i =  forceList.begin(); 
           i != forceList.end(); ++i)
      {
         std::string pmName = (*i)->GetName();
         if (pmName == "_DefaultInternalForce_")
            oldIter = i;
      }
      
      if (oldIter != forceList.end())
      {
         PhysicalModel *oldForce = *oldIter;
         forceList.erase(oldIter);
         
         #ifdef DEBUG_DEFAULT_FORCE
         MessageInterface::ShowMessage
            ("ODEModel::TakeAction() deleting default force <%p>\n", oldForce);
         #endif
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (oldForce, oldForce->GetName(), "ODEModel::DeleteForce()",
             "deleting non-transient force of " + oldForce->GetTypeName(), this);
         #endif
         delete oldForce;
      }
   }
   
   if (action == "ClearForcesOfType")
   {
      std::vector<PhysicalModel*> deleteList;

      // Build a list of forces to delete
      for (std::vector<PhysicalModel*>::iterator i =  forceList.begin();
            i != forceList.end(); ++i)
      {
         if ((*i)->IsOfType(actionData))
            deleteList.push_back(*i);
      }

      #ifdef DEBUG_FORCE_MANAGEMENT
         MessageInterface::ShowMessage("Removing %d forces of type %s\n",
               deleteList.size(), actionData.c_str());
      #endif

      // Delete them by...
      for (UnsignedInt i = 0; i < deleteList.size(); ++i)
      {
         // ...finding the item,
         std::vector<PhysicalModel*>::iterator item =
               find(forceList.begin(), forceList.end(), deleteList[i]);
         // ...removing it from the list, and
         if (item != forceList.end())
            forceList.erase(item);
         // deleting it
         delete deleteList[i];
      }
   }

   return true;
}


//------------------------------------------------------------------------------
// Rvector6 GetDerivativesForSpacecraft(Spacecraft *sc)
//------------------------------------------------------------------------------
/**
 * Calculates the Cartesian state vector derivatives for a single spacecraft
 *
 * @param sc The Spacecraft
 *
 * @return The [CartesianVelocity CartesianAcceleration] 6-vector
 */
//------------------------------------------------------------------------------
Rvector6 ODEModel::GetDerivativesForSpacecraft(Spacecraft *sc)
{
   if (!isInitializedForParameters)
   {
      #ifdef DEBUG_PARAMETER_INITIALIZATION
         MessageInterface::ShowMessage("Initializing %s for parameters\n",
               instanceName.c_str());
      #endif

      for (std::vector<PhysicalModel *>::iterator i = forceList.begin();
            i != forceList.end(); ++i)
      {
         (*i)->SetSolarSystem(solarSystem);
         (*i)->Initialize();
      }
      isInitializedForParameters = true;
   }

   Rvector6 dv, component;
   dv[0] = sc->GetRealParameter("CartesianVX");
   dv[1] = sc->GetRealParameter("CartesianVY");
   dv[2] = sc->GetRealParameter("CartesianVZ");
   dv[3] = dv[4] = dv[5] = 0.0;

   if (sc == NULL)
      throw ODEModelException("Derivative information cannot be computed; the "
            "Spacecraft in the call to GetDerivativesForSpacecraft() is NULL");

   // Apply superposition of forces/derivatives
   Integer warningsIssued = 0;

   for (std::vector<PhysicalModel *>::iterator i = forceList.begin();
         i != forceList.end(); ++i)
   {
      try
      {
         component = (*i)->GetDerivativesForSpacecraft(sc);
      }
      catch (BaseException &ex)
      {
         // ignore for now but post the message
         if (warnedOnceForParameters == false)
         {
            MessageInterface::ShowMessage("*** Warning *** When computing "
                  "derivative data for the force model %s, the following "
                  "exception was caught:\n   %s\n", instanceName.c_str(),
                  ex.GetFullMessage().c_str());
            ++warningsIssued;
         }

         component.MakeZeroVector();
      }

      for (Integer j = 3; j < 6; ++j)
         dv[j] += component[j];

      #ifdef DEBUG_DERIVATIVES_FOR_SPACECRAFT
         MessageInterface::ShowMessage("   %s -> [%.12le %.12le %.12le]\n",
               (*i)->GetName().c_str(), component[3], component[4],
               component[5]);
      #endif
   }

   #ifdef DEBUG_DERIVATIVES_FOR_SPACECRAFT
      MessageInterface::ShowMessage("Total:  [%.12le %.12le %.12le]\n", dv[3],
            dv[4], dv[5]);
   #endif

   // Sanity check the derivative
   for (Integer index = 0; index < 6; ++index)
   {
      if (GmatMathUtil::IsNaN(dv[index]))
         throw ODEModelException("The ForceModel " + instanceName +
               " generated a derivative that is not a number");

      if (GmatMathUtil::IsInf(dv[index]))
         throw ODEModelException("The ForceModel " + instanceName +
               " generated a derivative that is infinite");
   }

   if (warningsIssued > 0)
      warnedOnceForParameters = true;

   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage("ODE Accel: [%le %le %le]\n", dv[3],
               dv[4], dv[5]);
   #endif

   return dv;
}

//------------------------------------------------------------------------------
// PhysicalModel* GetForceOfType(const std::string& forceType,
//       const std::string& forBody = "Earth");
//------------------------------------------------------------------------------
/**
 * Accesses a specific force model component for use in parameter computations
 *
 * @param forceType The type of component ("DragForce", "PointMassForce", etc)
 * @param forBody Where applicable, the name of the body supplying the force
 *
 * @return The force
 */
//------------------------------------------------------------------------------
PhysicalModel* ODEModel::GetForceOfType(const std::string& forceType,
      const std::string& forBody)
{
   PhysicalModel *theModel = NULL;

   // Build a list of forces to delete
   for (std::vector<PhysicalModel*>::iterator i =  forceList.begin();
         i != forceList.end(); ++i)
   {
      if ((*i)->IsOfType(forceType))
      {
         if (forBody != "")
         {
            // Make sure the body also matches
            if ((*i)->GetBodyName() == forBody)
            {
               theModel = *i;
               break;
            }
         }
         else
         {
            theModel = *i;
            break;
         }
      }
   }

   // Make sure the model is initialized by init'ing them all if any needed
   if ((theModel != NULL) && !isInitializedForParameters)
   {
      #ifdef DEBUG_PARAMETER_INITIALIZATION
         MessageInterface::ShowMessage("Initializing %s for parameters\n",
               instanceName.c_str());
      #endif
      for (std::vector<PhysicalModel *>::iterator i = forceList.begin();
            i != forceList.end(); ++i)
      {
         (*i)->SetSolarSystem(solarSystem);
         (*i)->Initialize();
      }
      isInitializedForParameters = true;
   }

   return theModel;
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
bool ODEModel::RenameRefObject(const Gmat::ObjectType type,
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
 * This method returns a clone of the ODEModel.
 *
 * @return clone of the ODEModel.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ODEModel::Clone() const
{
   return (new ODEModel(*this));
}


//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void ODEModel::Copy(const GmatBase* orig)
{
   operator=(*((ODEModel *)(orig)));
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool ODEModel::HasRefObjectTypeArray()
{
   return true;
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
const ObjectTypeArray& ODEModel::GetRefObjectTypeArray()
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
const StringArray& ODEModel::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   std::string pmName;
   StringArray pmRefs;
   
   forceReferenceNames.clear();
   
   // Provide point mass body names for validation checking
   if (type == Gmat::SPACE_POINT)
   {
      forceReferenceNames = BuildBodyList("PointMassForce");
      
      // Add central body
      if (find(forceReferenceNames.begin(), forceReferenceNames.end(),
               centralBodyName) == forceReferenceNames.end())
         forceReferenceNames.push_back(centralBodyName);
      
      // Add GravityFields
      StringArray gfList = BuildBodyList("GravityField");

      for (UnsignedInt i = 0; i < gfList.size(); ++i)
         if (find(forceReferenceNames.begin(), forceReferenceNames.end(),
               gfList[i]) == forceReferenceNames.end())
            forceReferenceNames.push_back(gfList[i]);

      return forceReferenceNames;      
   }
   
   // Provide space object names for validation checking
//   if (type == Gmat::SPACEOBJECT)
//   {
//      for (std::vector<SpaceObject *>::iterator sc = spacecraft.begin();
//           sc != spacecraft.end(); ++sc)
//      {
//         forceReferenceNames.push_back((*sc)->GetName());
//      }
//      return forceReferenceNames;
//   }
   
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
   catch (BaseException &)
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
   catch (BaseException &)
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
      catch (BaseException &)
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
      catch (BaseException &)
      {
         // Do nothing
      }
   }
   
   // Add central body
   if (find(forceReferenceNames.begin(), forceReferenceNames.end(),
            centralBodyName) == forceReferenceNames.end())
      forceReferenceNames.push_back(centralBodyName);
   
   #ifdef DEBUG_FORCE_REF_OBJ
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
void ODEModel::SetSolarSystem(SolarSystem *ss)
{
   PhysicalModel::SetSolarSystem(ss);

   if (ss == NULL)
      MessageInterface::ShowMessage("Setting NULL solar system on %s\n", 
         instanceName.c_str());
   
   if (solarSystem != NULL)
   {
      forceOrigin = solarSystem->GetBody(centralBodyName);
      
      if (forceOrigin == NULL) 
         throw ODEModelException(
            "Force model origin (" + centralBodyName + 
            ") was not found in the solar system");

      for (std::vector<PhysicalModel*>::iterator i = forceList.begin();
           i != forceList.end(); ++i)
         (*i)->SetForceOrigin(forceOrigin);

      coverageStartDetermined = false;
   }
}

void ODEModel::SetForceOrigin(CelestialBody* toBody)
{
    PhysicalModel::SetForceOrigin(toBody);
    coverageStartDetermined = false;
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
bool ODEModel::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                              const std::string &name)
{
   #ifdef DEBUG_FORCE_REF_OBJ
   MessageInterface::ShowMessage
      ("ODEModel::SetRefObject() <%s> entered, obj=<%p><%s><%s>, type=%d, name='%s'\n",
       GetName().c_str(), obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL", type, name.c_str());
   #endif
   
   bool wasSet = false;
   
   // Attempt to set the object for the base class    
   try
   {
      if (PhysicalModel::SetRefObject(obj, type, name))
         wasSet = true;
   }
   catch (BaseException &)
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
      catch (BaseException &)
      {
         // Do nothing
      }
   }
   
   return (wasSet);
}


//------------------------------------------------------------------------------
// Integer GetParameterCount() const
//------------------------------------------------------------------------------
Integer ODEModel::GetParameterCount() const
{
   return parameterCount;
}


// Access methods 
//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string ODEModel::GetParameterText(const Integer id) const
{
   if (id >= PhysicalModelParamCount && id < ODEModelParamCount)
      return PARAMETER_TEXT[id - PhysicalModelParamCount];
   else
      return PhysicalModel::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer ODEModel::GetParameterID(const std::string &str) const
{
   std::string alias = str;
    
   // Script document required two different names for the primary body
   // force descriptor
   if (alias == "Gravity")
      alias = "PrimaryBodies";
   
   for (int i = PhysicalModelParamCount; i < ODEModelParamCount; i++)
   {
      if (alias == PARAMETER_TEXT[i - PhysicalModelParamCount])
      {
         return i;
      }
   }
   
   return PhysicalModel::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType ODEModel::GetParameterType(const Integer id) const
{
   if (id >= PhysicalModelParamCount && id < ODEModelParamCount)
      return PARAMETER_TYPE[id - PhysicalModelParamCount];
   else
      return PhysicalModel::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string ODEModel::GetParameterTypeString(const Integer id) const
{
   if (id >= PhysicalModelParamCount && id < ODEModelParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return PhysicalModel::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
bool ODEModel::IsParameterReadOnly(const Integer id) const
{
   if (id == COORDINATE_SYSTEM_LIST || id == DEGREE || id == ORDER ||
       id == POTENTIAL_FILE || id == POLYHEDRAL_BODY || id == SHAPE_FILE_NAME ||
       id == BODY_DENSITY)
      return true;
   
   return PhysicalModel::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
bool ODEModel::IsParameterReadOnly(const std::string &label) const
{
   if (label == PARAMETER_TEXT[COORDINATE_SYSTEM_LIST-PhysicalModelParamCount])
      return true;
   
   return PhysicalModel::IsParameterReadOnly(label);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method gets real parameter data
 *
 * @param id The parameter's ID
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real ODEModel::GetRealParameter(const Integer id) const
{
   if (id == BODY_DENSITY)
   {
      // Find a Polyhedral gravity model
      const PhysicalModel *pm = GetForce("PolyhedronGravityModel");
      if (pm == NULL)
         throw ODEModelException("The BodyDensity parameter only applies to "
               "Polyhedral gravity fields.");
      // Get the atmosphere model from the drag force
      Integer id = pm->GetParameterID("BodyDensity");
      return pm->GetRealParameter(id);
   }

   return PhysicalModel::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets real parameter data
 *
 * @param id The parameter's ID
 * @param value The new value
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real ODEModel::SetRealParameter(const Integer id, const Real value)
{
   if (id == BODY_DENSITY)
   {
      // Find a Polyhedral gravity model; cast away const for data change
      PhysicalModel *pm = (PhysicalModel *)GetForce("PolyhedronGravityModel");
      if (pm == NULL)
         throw ODEModelException("The BodyDensity parameter only applies to "
               "Polyhedral gravity fields.");

      if (value <= 0.0)
      {
         char msg[1024];
         std::stringstream val;
         val.precision(16);
         val << value;
         sprintf(msg, errorMessageFormat.c_str(), val.str().c_str(),
               "BodyDensity", "Real number > 0.0");
         throw ODEModelException(msg);
      }
      // Get the atmosphere model from the drag force
      Integer id = pm->GetParameterID("BodyDensity");
      return pm->SetRealParameter(id, value);
   }

   return PhysicalModel::SetRealParameter(id, value);
}

////------------------------------------------------------------------------------
//// Real GetRealParameter(const Integer id, const Integer index) const
////------------------------------------------------------------------------------
///**
// * This method is a pass through method, calling the base class version
// */
////------------------------------------------------------------------------------
//Real ODEModel::GetRealParameter(const Integer id, const Integer index) const
//{
//   return PhysicalModel::GetRealParameter(id, index);
//}
//
////------------------------------------------------------------------------------
//// Real GetRealParameter(const Integer id, const Integer row,
////       const Integer col) const
////------------------------------------------------------------------------------
///**
// * This method is a pass through method, calling the base class version
// */
////------------------------------------------------------------------------------
//Real ODEModel::GetRealParameter(const Integer id, const Integer row,
//      const Integer col) const
//{
//   return PhysicalModel::GetRealParameter(id, row, col);
//}
//
////------------------------------------------------------------------------------
//// Real SetRealParameter(const Integer id, const Real value,
////       const Integer index)
////------------------------------------------------------------------------------
///**
// * This method is a pass through method, calling the base class version
// */
////------------------------------------------------------------------------------
//Real ODEModel::SetRealParameter(const Integer id, const Real value,
//      const Integer index)
//{
//   return PhysicalModel::SetRealParameter(id, value, index);
//}
//
////------------------------------------------------------------------------------
//// Real SetRealParameter(const Integer id, const Real value,
////       const Integer row, const Integer col)
////------------------------------------------------------------------------------
///**
// * This method is a pass through method, calling the base class version
// */
////------------------------------------------------------------------------------
//Real ODEModel::SetRealParameter(const Integer id, const Real value,
//      const Integer row, const Integer col)
//{
//   return PhysicalModel::SetRealParameter(id, value, row, col);
//}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer &label)
//------------------------------------------------------------------------------
/**
 * This method gets real parameter data
 *
 * @param label The parameter's scripted string
 *
 * @return The parameter value
 */
Real ODEModel::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer &label, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets real parameter data
 *
 * @param label The parameter's scripted string
 * @param value The new value
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real ODEModel::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

////------------------------------------------------------------------------------
//// Real GetRealParameter(const std::string &label,
////       const Integer index) const
////------------------------------------------------------------------------------
///**
// * This method is a pass through method, calling the base class version
// */
////------------------------------------------------------------------------------
//Real ODEModel::GetRealParameter(const std::string &label,
//      const Integer index) const
//{
//   return GetRealParameter(GetParameterID(label), index);
//}
//
////------------------------------------------------------------------------------
//// Real SetRealParameter(const std::string &label, const Real value,
////       const Integer index)
////------------------------------------------------------------------------------
///**
// * This method is a pass through method, calling the base class version
// */
////------------------------------------------------------------------------------
//Real ODEModel::SetRealParameter(const std::string &label, const Real value,
//      const Integer index)
//{
//   return SetRealParameter(GetParameterID(label), value, index);
//}
//
////------------------------------------------------------------------------------
//// Real GetRealParameter(const std::string &label, const Integer row,
////       const Integer col) const
////------------------------------------------------------------------------------
///**
// * This method is a pass through method, calling the base class version
// */
////------------------------------------------------------------------------------
//Real ODEModel::GetRealParameter(const std::string &label, const Integer row,
//      const Integer col) const
//{
//   return GetRealParameter(GetParameterID(label), row, col);
//}
//
////------------------------------------------------------------------------------
//// Real SetRealParameter(const std::string &label, const Real value,
////       const Integer row, const Integer col)
////------------------------------------------------------------------------------
///**
// * This method is a pass through method, calling the base class version
// */
////------------------------------------------------------------------------------
//Real ODEModel::SetRealParameter(const std::string &label, const Real value,
//      const Integer row, const Integer col)
//{
//   return SetRealParameter(GetParameterID(label), value, row, col);
//}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string ODEModel::GetStringParameter(const Integer id) const
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
      
      case ERROR_CONTROL:
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
               throw ODEModelException("Unrecognized error control method.");
         }
         break;

      case POTENTIAL_FILE:
         {
            // Get actual id
            GmatBase *owner = NULL;
            Integer actualId = GetOwnedObjectId(id, &owner);
            return owner->GetStringParameter(actualId);
         }
         
      default:
         return PhysicalModel::GetStringParameter(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string ODEModel::GetStringParameter(const std::string &label) const
{
    return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool ODEModel::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_FM_SET
   MessageInterface::ShowMessage
      ("ODEModel::SetStringParameter() entered, id=%d, value='%s'\n",
       id, value.c_str());
   #endif
   
   switch (id)
   {
      case CENTRAL_BODY:
         centralBodyName = value;
         bodyName = centralBodyName;
         return true;
         
      case PRIMARY_BODIES:
         return false;
         
      case POLYHEDRAL_BODIES:
         return false;

      case POINT_MASSES:
         return false;
         
      case  DRAG:
         return false;
         
      case  USER_DEFINED:
         return false;
         
//       case  SRP:
//          return false;
         
      case ERROR_CONTROL:
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
         throw ODEModelException("Unrecognized error control method.");
         
      case POTENTIAL_FILE:
         {
            // Get actual id
            GmatBase *owner = NULL;
            Integer actualId = GetOwnedObjectId(id, &owner);
            return owner->SetStringParameter(actualId, value);
         }

      case POLYHEDRAL_BODY:
      {
         // Get actual id
         GmatBase *owner = NULL;
         Integer actualId = GetOwnedObjectId(id, &owner);
         return owner->SetStringParameter(actualId, value);
      }

      case SHAPE_FILE_NAME:
      {
         // Get actual id
         GmatBase *owner = NULL;
         Integer actualId = GetOwnedObjectId(id, &owner);
         return owner->SetStringParameter(actualId, value);
      }

      default:
         return PhysicalModel::SetStringParameter(id, value);
    }
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,const std::string &value)
//------------------------------------------------------------------------------
bool ODEModel::SetStringParameter(const std::string &label,
                                    const std::string &value)
{
    return SetStringParameter(GetParameterID(label), value);
}

//---------------------------------------------------------------------------
//  std::string GetOnOffParameter(const Integer id) const
//---------------------------------------------------------------------------
std::string ODEModel::GetOnOffParameter(const Integer id) const
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
   case RELATIVISTIC_CORRECTION:
      {
         const PhysicalModel *pm = GetForce("RelativisticCorrection");
         if (pm == NULL)
            return "Off";
         return "On";
      }
   default:
      return PhysicalModel::GetOnOffParameter(id);
   }
}


//------------------------------------------------------------------------------
// bool HasLocalClones()
//------------------------------------------------------------------------------
/**
 * Method to check for the presence of local clones
 *
 * @return true if there are local clones, false if not
 */
//------------------------------------------------------------------------------
bool ODEModel::HasLocalClones()
{
   return true;
}


//------------------------------------------------------------------------------
// void UpdateClonedObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Updates cloned objects that are copies of the object passed in
 *
 * @param obj The object with setting updates
 *
 * @note: This code currently does nothing.  If the code is set to make changes,
 *        there may be matching needs in ODEModel::UpdateClonedObjectParameter(
 *           GmatBase *obj, Integer updatedParameterId).
 */
//------------------------------------------------------------------------------
void ODEModel::UpdateClonedObject(GmatBase *obj)
{
   #ifdef DEBUG_ODEMODEL_INIT
      MessageInterface::ShowMessage("Called UpdateClonedObject on %s\n", 
         instanceName.c_str());
   #endif
   if (obj->IsOfType(Gmat::COORDINATE_SYSTEM))
   {
      /// @note Potential issue
//      MessageInterface::ShowMessage("ODEModel::UpdateClonedObject: Potential "
//            "cloning issue: Check cloned coordinate systems passed into the "
//            "ODE model for local clones in the member forces\n");
   }
}


//------------------------------------------------------------------------------
// void UpdateClonedObjectParameter(GmatBase *obj, Integer updatedParameterId)
//------------------------------------------------------------------------------
/**
 * This method changes a single parameter on an owned clone
 *
 * @param obj The master object holding the new parameter value
 * @param updatedParameterId The ID of the updated parameter
 *
 * @note: This code currently does nothing, but is in place as a partner to
 *        ODEModel::UpdateClonedObject(GmatBase *obj).  If that code is set to
 *        make changes, there may be matching needs here.
 */
//------------------------------------------------------------------------------
void ODEModel::UpdateClonedObjectParameter(GmatBase *obj,
      Integer updatedParameterId)
{

}


//------------------------------------------------------------------------------
// bool SetEpoch(const GmatEpoch newEpoch)
//------------------------------------------------------------------------------
/**
 * Updates the epoch for the ODE model and all member forces
 *
 * This interface into the ODEModel was added for users of external calls into
 * the model, to ensure that the model and all member forces reference the 
 * right initial epoch.  It was added to support the GMAT C-Interface (see JIRA
 * bug GMT-2936), but can be used from other places as well to make the epoch 
 * for a force model correct in the absense of other structures to force that 
 * update.
 *
 * @param newEpoch The new epoch
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool ODEModel::SetEpoch(const GmatEpoch newEpoch)
{
   bool retval = false;

   if (newEpoch > 0.0)
   {
      epoch = newEpoch;
      for (std::vector<PhysicalModel*>::iterator i = forceList.begin();
            i != forceList.end(); ++i)
      {
         if ((*i) != NULL)
            (*i)->SetRealParameter(EPOCH, newEpoch);
      }
      retval = true;
   }

   return retval;
}



//---------------------------------------------------------------------------
//  bool SetOnOffParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
bool ODEModel::SetOnOffParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case SRP:
      return true;
   case RELATIVISTIC_CORRECTION:
      return true;
   default:
      return PhysicalModel::SetOnOffParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// std::string ODEModel::GetOnOffParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string ODEModel::GetOnOffParameter(const std::string &label) const
{
   return GetOnOffParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetOnOffParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool ODEModel::SetOnOffParameter(const std::string &label, 
                                   const std::string &value)
{
   return SetOnOffParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& ODEModel::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
   case PRIMARY_BODIES:
      return BuildBodyList("GravityField");
   case POLYHEDRAL_BODIES:
      return BuildBodyList("PolyhedralBodies");
   case POINT_MASSES:
      return BuildBodyList("PointMassForce");
   case COORDINATE_SYSTEM_LIST:
      return BuildCoordinateList();
   case USER_DEFINED:
      return BuildUserForceList();

   default:
      return PhysicalModel::GetStringArrayParameter(id);
    }
}


//------------------------------------------------------------------------------
// StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& ODEModel::GetStringArrayParameter(const std::string &label) const
{
    return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer ODEModel::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
      case DEGREE:
      case ORDER:
         {
            // Get actual id
            GmatBase *owner = NULL;
            Integer actualId = GetOwnedObjectId(id, &owner);
            return owner->GetIntegerParameter(actualId);
         }
   default:
      return PhysicalModel::GetIntegerParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
Integer ODEModel::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer ODEModel::SetIntegerParameter(const Integer id, const Integer value)
{
   #ifdef DEBUG_FM_SET
   MessageInterface::ShowMessage
      ("ODEModel::SetIntegerParameter() id=%d, value=%d\n", id, value);
   #endif
   
   switch (id)
   {
   case DEGREE:
   case ORDER:
      {
         // Get actual id
         GmatBase *owner = NULL;
         Integer actualId = GetOwnedObjectId(id, &owner);
         #ifdef DEBUG_FM_SET
         MessageInterface::ShowMessage
            ("   Calling SetIntegerParameter() on '%s' of type '%s'\n",
             owner->GetName().c_str(), owner->GetTypeName().c_str());
         #endif
         Integer outval = owner->SetIntegerParameter(actualId, value);
         return outval;
      }
   default:
      return GmatBase::SetIntegerParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
Integer ODEModel::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// const StringArray& BuildBodyList(std::string type) const
//------------------------------------------------------------------------------
const StringArray& ODEModel::BuildBodyList(std::string type) const
{
   static StringArray bodylist;
   bodylist.clear();
   
   // Run through list of forces, adding body names for GravityField instances
   std::vector<PhysicalModel*>::const_iterator i;
   
   std::string actualType = GetScriptAlias(type);
   
   for (i = forceList.begin(); i != forceList.end(); ++i)
   {
      if ((*i)->GetTypeName() == actualType)
      {
         bodylist.push_back((*i)->GetStringParameter("BodyName"));
      }
   }
   return bodylist;
}


//------------------------------------------------------------------------------
// const StringArray& BuildCoordinateList() const
//------------------------------------------------------------------------------
const StringArray& ODEModel::BuildCoordinateList() const
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
         // If ODEModel is created inside of GmatFunction, not all CS exist, so added
         // InputCoordinateSystem and TargetCoordinateSystem to list(loj: 2008.06.25)
         cslist.push_back((*i)->GetStringParameter("InputCoordinateSystem"));
         cslist.push_back((*i)->GetStringParameter("FixedCoordinateSystem"));
         cslist.push_back((*i)->GetStringParameter("TargetCoordinateSystem"));
      }
   }
   return cslist;
}

//------------------------------------------------------------------------------
// const StringArray& BuildUserForceList() const
//------------------------------------------------------------------------------
/**
 * Builds the list of forces in the force model that are not part of the default
 * set of forces.
 * 
 * @return The list of user forces.
 */
//------------------------------------------------------------------------------
const StringArray& ODEModel::BuildUserForceList() const
{
   static StringArray uflist;
   uflist.clear();

   std::vector<PhysicalModel*>::const_iterator i;
   for (i = forceList.begin(); i != forceList.end(); ++i) 
      if ((*i)->IsUserForce())
         uflist.push_back((*i)->GetTypeName());
   
   return uflist;
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Accesses an internal object used in the ODEModel.
 * 
 * This method provides access to the forces used in the ODEModel.  It is used
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
GmatBase* ODEModel::GetRefObject(const Gmat::ObjectType type,
                                   const std::string &name)
{
   if (type != Gmat::PHYSICAL_MODEL)
       throw ODEModelException(
          "Only forces are accessed in ODEModel::GetRefObject");
   
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
 * Accesses an internal object used in the ODEModel.
 * 
 * This method provides access to the forces used in the ODEModel.  It is used
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
GmatBase* ODEModel::GetRefObject(const Gmat::ObjectType type, 
                                   const std::string &name, const Integer index)
{
   if (type != Gmat::PHYSICAL_MODEL)
       throw ODEModelException(
          "Only forces are accessed in ODEModel::GetRefObject");
   
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
 * Accesses arrays of internal objects used in the ODEModel.
 * 
 * @param typeString String used for the objects.
 * 
 * @return A reference to the ObjectArray.
 */
//------------------------------------------------------------------------------
ObjectArray& ODEModel::GetRefObjectArray(const std::string& typeString)
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
// std::string BuildForceNameString(PhysicalModel *force)
//------------------------------------------------------------------------------
std::string ODEModel::BuildForceNameString(PhysicalModel *force)
{
   std::string retval = "UnknownForce", forceType = force->GetTypeName();
   
   if (forceType == "DragForce")
      retval = "Drag";
//   if (forceType == "GravityField")
//      // Changed "Gravity" to "GravityField" to be consistent with type name (loj: 2007.01.25)
//      retval = "GravityField." + force->GetStringParameter("BodyName");
   if (force->IsOfType("GravityBase"))
      retval = forceType + "." + force->GetStringParameter("BodyName");
   if (forceType == "PointMassForce")
      retval = force->GetStringParameter("BodyName");
   if (forceType == "SolarRadiationPressure")
      retval = "SRP";
   if (forceType == "RelativisticCorrection")
      retval = "RelativisticCorrection";
   
   // Add others here
   
   // Handle user defined forces
   if (force->IsUserForce())
      retval = force->GetName();
      
        #ifdef DEBUG_USER_FORCES
                MessageInterface::ShowMessage("Force type %s named '%s' %s a user force\n",
        force->GetTypeName().c_str(), force->GetName().c_str(), 
        (force->IsUserForce() ? "is" : "is not"));
        #endif

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
void ODEModel::MoveToOrigin(Real newEpoch)
{
#ifdef DEBUG_REORIGIN
   MessageInterface::ShowMessage("ODEModel::MoveToOrigin entered with "
         "newEpoch = %le\n", newEpoch);
   MessageInterface::ShowMessage("      and epoch = %le\n", epoch);
#endif
   
#ifdef DEBUG_REORIGIN
   MessageInterface::ShowMessage(
         "SatCount = %d, dimension = %d, stateSize = %d\n",cartesianCount,
         dimension, stateSize);
   MessageInterface::ShowMessage(
         "StatePointers: rawState = %p, modelState = %p\n", rawState, 
         modelState);
   MessageInterface::ShowMessage(
       "ODEModel::MoveToOrigin()\n   Input state: [ ");
   for (Integer i = 0; i < dimension; ++i)
      MessageInterface::ShowMessage("%lf ", rawState[i]); 
   MessageInterface::ShowMessage("]\n   model state: [ ");
   for (Integer i = 0; i < dimension; ++i)
      MessageInterface::ShowMessage("%lf ", modelState[i]);
   MessageInterface::ShowMessage("]\n\n");
#endif
    
   memcpy(modelState, rawState, dimension*sizeof(Real));

   if (centralBodyName != j2kBodyName)
   {
      Rvector6 cbState, j2kState, delta;
      Real now = ((newEpoch < 0.0) ? epoch : newEpoch);
      cbState = forceOrigin->GetState(now);
      j2kState = j2kBody->GetState(now);

      delta = cbState - j2kState;

      for (Integer i = 0; i < cartesianCount; ++i)
      {
         Integer i6 = cartesianStart + i * 6;
         for (int j = 0; j < 6; ++j)
            modelState[i6+j] = rawState[i6+j] - delta[j];

         #ifdef DEBUG_REORIGIN
            MessageInterface::ShowMessage(
                "ODEModel::MoveToOrigin()\n"
                "   Input state: [%lf %lf %lf %lf %lf %lf]\n"
                "   j2k state:   [%lf %lf %lf %lf %lf %lf]\n"
                "   cb state:    [%lf %lf %lf %lf %lf %lf]\n"
                "   delta:       [%lf %lf %lf %lf %lf %lf]\n"
                "   model state: [%lf %lf %lf %lf %lf %lf]\n\n",
                rawState[i6], rawState[i6+1], rawState[i6+2], rawState[i6+3],
                rawState[i6+4], rawState[i6+5],
                j2kState[0], j2kState[1], j2kState[2], j2kState[3], j2kState[4],
                j2kState[5],
                cbState[0], cbState[1], cbState[2], cbState[3], cbState[4],
                cbState[5],
                delta[0], delta[1], delta[2], delta[3], delta[4], delta[5],
                modelState[i6], modelState[i6+1], modelState[i6+2],
                modelState[i6+3], modelState[i6+4], modelState[i6+5]);
         #endif
      }
   }
   
   #ifdef DEBUG_REORIGIN
      MessageInterface::ShowMessage(
          "   Move Complete\n   Input state: [ ");
      for (Integer i = 0; i < dimension; ++i)
         MessageInterface::ShowMessage("%lf ", rawState[i]);
      MessageInterface::ShowMessage("]\n   model state: [ ");
      for (Integer i = 0; i < dimension; ++i)
         MessageInterface::ShowMessage("%lf ", modelState[i]);
      MessageInterface::ShowMessage("]\n\n");

      MessageInterface::ShowMessage("ODEModel::MoveToOrigin Finished\n");
   #endif
}


//------------------------------------------------------------------------------
// void ReturnFromOrigin(Real newEpoch)
//------------------------------------------------------------------------------
/**
 * Transforms the state vector from the force model origin to the internal 
 * coordinate system.
 */
//------------------------------------------------------------------------------
void ODEModel::ReturnFromOrigin(Real newEpoch)
{
   #ifdef DEBUG_REORIGIN
      MessageInterface::ShowMessage("ODEModel::ReturnFromOrigin entered\n");
   #endif

   memcpy(rawState, modelState, dimension*sizeof(Real));
   if (centralBodyName != j2kBodyName)
   {
      Rvector6 cbState, j2kState, delta;
      Real now = ((newEpoch < 0.0) ? epoch : newEpoch);
      cbState = forceOrigin->GetState(now);
      j2kState = j2kBody->GetState(now);

      delta = j2kState - cbState;


      for (Integer i = 0; i < cartesianCount; ++i)
      {
         Integer i6 = cartesianStart + i * 6;
         for (int j = 0; j < 6; ++j)
            rawState[i6+j] = modelState[i6+j] - delta[j];
            #ifdef DEBUG_REORIGIN
               MessageInterface::ShowMessage(
                   "ODEModel::ReturnFromOrigin()\n   Input (model) state: [%lf %lf %lf %lf %lf"
                   " %lf]\n   j2k state:   [%lf %lf %lf %lf %lf %lf]\n"
                   "   cb state:    [%lf %lf %lf %lf %lf %lf]\n"
                   "   delta:       [%lf %lf %lf %lf %lf %lf]\n"
                   "   raw state: [%lf %lf %lf %lf %lf %lf]\n\n",
                   modelState[0], modelState[1], modelState[2], modelState[3], modelState[4],
                   modelState[5],
                   j2kState[0], j2kState[1], j2kState[2], j2kState[3], j2kState[4],
                   j2kState[5],
                   cbState[0], cbState[1], cbState[2], cbState[3], cbState[4],
                   cbState[5],
                   delta[0], delta[1], delta[2], delta[3], delta[4], delta[5],
                   rawState[0], rawState[1], rawState[2], rawState[3],
                   rawState[4], rawState[5]);
         #endif
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
void ODEModel::ReportEpochData()
{
//   if (epochFile.is_open())
//   {
//      epochFile.precision(16);
//         epochFile << epoch << " " << elapsedTime;
//
//      epochFile << " Members ";
//      for (std::vector<PhysicalModel*>::iterator i = forceList.begin();
//           i != forceList.end(); ++i)
//      {
//         epochFile.precision(16);
//         epochFile << " "
//                   << (*i)->GetRealParameter(satIds[0])
//                   << " " << (*i)->GetTime();
//      }
//
//      epochFile << " Sats " ;
//      for (std::vector<SpaceObject*>::iterator i = spacecraft.begin();
//           i != spacecraft.end(); ++i)
//      {
//         epochFile.precision(16);
//         epochFile << " " << (*i)->GetEpoch();
//      }
//      epochFile << "\n";
//   }
//   else
//      throw ODEModelException(
//         "ODEModel::ReportEpochData: Attempting to write epoch data without "
//         "opening the data file.");

   // Changed 3/4/2010, DJC
   // Repurposed this method so it is not file based.  For some reason all but
   // the final exception above was commented out, making the method totally
   // useless.

   MessageInterface::ShowMessage("  Epoch: %.12lf, elapsedTime: %.12lf\n",
            epoch, elapsedTime);
}


//------------------------------------------------------------------------------
// void SetPropStateManager(PropagationStateManager *sm)
//------------------------------------------------------------------------------
/**
 * Sets the ProagationStateManager pointer
 *
 * @param sm The PSM that the ODEModel uses
 */
//------------------------------------------------------------------------------
void ODEModel::SetPropStateManager(PropagationStateManager *sm)
{
   #ifdef DEBUG_OBJECT_SETTING
      MessageInterface::ShowMessage("Setting the PSM on %s\n",
            instanceName.c_str());
   #endif
   psm = sm;
}

//------------------------------------------------------------------------------
// void SetState(GmatState *gms)
//------------------------------------------------------------------------------
/**
 * Sets the GMAT state used with the ODE model
 *
 * @param gms The GMAT state
 */
//------------------------------------------------------------------------------
void ODEModel::SetState(GmatState *gms)
{
   state = gms;

   #ifdef DEBUG_STATE
      MessageInterface::ShowMessage("Setting state with epoch %le and "
            "dimension %d to\n   [", state->GetEpoch(), state->GetSize());
      for (Integer i = 0; i < state->GetSize()-1; ++i)
         MessageInterface::ShowMessage("%le, ", (*state)[i]);
      MessageInterface::ShowMessage("%le]\n", (*state)[state->GetSize()-1]);
   #endif
}


//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// Integer GetOwnedObjectId(Integer id, GmatBase **owner) const
//------------------------------------------------------------------------------
/*
 * Retrieves owned object property id.
 *
 * @param  id  ODEModel parameter id for for getting owned object parameter id
 * @param  owner  The pointer to owner to set if id provided found from the
 *                owned object
 *
 * @return  parameter ID from the owned object
 */
//------------------------------------------------------------------------------
Integer ODEModel::GetOwnedObjectId(Integer id, GmatBase **owner) const
{
   Integer actualId = -1;
   GmatBase *ownedObj = NULL;
   
   if (numForces == 0)
      throw ODEModelException("ODEModel::GetOwnedObjectId() failed, Has empty "
            "force list");
   
   for (Integer i=0; i<numForces; i++)
   {
      ownedObj = forceList[i];
      try
      {
         actualId = ownedObj->GetParameterID(GetParameterText(id));
         break;
      }
      catch (BaseException &)
      {
         throw;
      }
   }
   
   *owner = ownedObj;
   
   if (owner == NULL)
      throw ODEModelException("ODEModel::GetOwnedObjectId() failed, Owned force is NULL");
   
   #ifdef DEBUG_FM_OWNED_OBJECT
   MessageInterface::ShowMessage
      ("ODEModel::GetOwnedObjectId() returning %d, owner=<%p><%s><%s>\n",
       actualId, *owner, (*owner)->GetTypeName().c_str(), (*owner)->GetName().c_str());
   #endif
   
   return actualId;
}


