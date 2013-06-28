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
// *** File Name : ODEModel.hpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
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
//                           : 10/01/2003 - W. Waktola, Missions Applications
//                                          Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//                                - Changed FlightDynamicsForces class to
//                                  ForceModel class
//
//                           : 10/20/2003 - W. Waktola, Missions Applications
//                                          Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//
//                           : 11/9/2003 - D. Conway, Thinking Systems, Inc.
//                             Overrode GetParameterCount so the count 
//                             increases based on the member forces
// **************************************************************************
/**
 * Container for forces acting on spacecraft.
 * 
 * ODEModel is a class that accumulates the forces acting on one or
 * more spacecraft
*/

#ifndef ODEModel_hpp
#define ODEModel_hpp

#include "PhysicalModel.hpp"
#include "MessageInterface.hpp"
#include "gmatdefs.hpp"

#include <fstream>
#include <vector>
#include <map>
#include <string.h>

#include <fstream>            // Used for streams in debugging methods


class PropagationStateManager;



/**
 * ODEModel is a container class for ordinary differential equations
 * 
 * The ODEModel class receives PhysicalModels that are used either singly or 
 * together to model the differential equations integrated by the numerical 
 * integrators in GMAT.  The ODEModel class implements the superposition of 
 * these contributors, and manages mapping into the correct elements of the 
 * output vector of derivative information. 
 */
class GMAT_API ODEModel : public PhysicalModel
{
public:
   //--------------------------------------------------------------------------------
   // static methods
   //--------------------------------------------------------------------------------
   static void          SetScriptAlias(const std::string& alias,
                                       const std::string& typeName);
   static std::string&  GetScriptAlias(const std::string& alias);

   //--------------------------------------------------------------------------------
   // class methods
   //--------------------------------------------------------------------------------
   ODEModel(const std::string &nomme = "", 
            const std::string typeNomme = "ODEModel");
   virtual ~ODEModel();
   ODEModel(const ODEModel& fdf);
   ODEModel&   operator=(const ODEModel& fdf);
   
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   
   // Methods needed to run an ODEModel 
   virtual void SetSolarSystem(SolarSystem *ss);
   virtual void SetForceOrigin(CelestialBody* toBody);

   virtual bool BuildModelFromMap();
   virtual bool CheckQualifier(const std::string &qualifier,
                               const std::string &forType);

   // Methods used in propagation
   virtual bool Initialize();
   virtual bool GetDerivatives(Real * state, Real dt = 0.0, Integer order = 1, 
         const Integer id = -1);
   virtual Real EstimateError(Real *diffs, Real *answer) const;

   // Methods used for parameter access
   virtual Rvector6 GetDerivativesForSpacecraft(Spacecraft *sc);
   virtual PhysicalModel* GetForceOfType(const std::string& forceType,
         const std::string& forBody = "Earth");


   void AddForce(PhysicalModel *pPhysicalModel);
   
   void DeleteForce(const std::string &name);
   void DeleteForce(PhysicalModel *pPhyscialModel);
   bool HasForce(const std::string &name);
   Integer GetNumForces();
   StringArray& GetForceTypeNames();
   std::string GetForceTypeName(Integer index);
   PhysicalModel* GetForce(Integer index) const;
   const PhysicalModel* GetForce(std::string forcetype,
                                 Integer whichOne = 0) const;
   void UpdateSpaceObject(Real newEpoch = -1.0);
   void UpdateFromSpaceObject();
   void RevertSpaceObject();
   void BufferState();
   Integer SetupSpacecraftData(ObjectArray *sats, Integer i);

   Integer UpdateDynamicSpacecraftData(ObjectArray *sats, Integer i);


   // Methods that were removed
//   void ClearSpacecraft();
//   bool AddSpaceObject(SpaceObject *so);

//   virtual void IncrementTime(Real dt);
//   virtual void SetTime(Real t);
   
   // Take action method inherited from GmatBase
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");
   
   // Parameter definition and accessor methods inherited from GmatBase
   virtual Integer      GetParameterCount() const;
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual bool         HasRefObjectTypeArray();
   virtual const        ObjectTypeArray& GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   
   
   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;
   
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
//   virtual Real         GetRealParameter(const Integer id,
//                                         const Integer index) const;
//   virtual Real         GetRealParameter(const Integer id, const Integer row,
//                                         const Integer col) const;
//   virtual Real         SetRealParameter(const Integer id,
//                                         const Real value,
//                                         const Integer index);
//   virtual Real         SetRealParameter(const Integer id, const Real value,
//                                         const Integer row, const Integer col);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
//   virtual Real         GetRealParameter(const std::string &label,
//                                         const Integer index) const;
//   virtual Real         SetRealParameter(const std::string &label,
//                                         const Real value,
//                                         const Integer index);
//   virtual Real         GetRealParameter(const std::string &label,
//                                         const Integer row,
//                                         const Integer col) const;
//   virtual Real         SetRealParameter(const std::string &label,
//                                         const Real value, const Integer row,
//                                         const Integer col);

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &value) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
   virtual std::string  GetOnOffParameter(const Integer id) const;
   virtual bool         SetOnOffParameter(const Integer id, 
                                          const std::string &value);
   virtual std::string  GetOnOffParameter(const std::string &label) const;
   virtual bool         SetOnOffParameter(const std::string &label, 
                                          const std::string &value);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);
   
   virtual Integer      GetOwnedObjectCount();
   virtual GmatBase*    GetOwnedObject(Integer whichOne);
   virtual std::string  BuildPropertyName(GmatBase *ownedObj);
   
   void                 UpdateInitialData(bool dynamicOnly = false);
   void                 ReportEpochData();
   
   void                 SetPropStateManager(PropagationStateManager *sm);
   void                 SetState(GmatState *gms);

   virtual bool         HasLocalClones();
   virtual void         UpdateClonedObject(GmatBase *obj);
   virtual void         UpdateClonedObjectParameter(GmatBase *obj,
                                                    Integer updatedParameterId);


   // Interface added for the C Interface to force epoch updates
   bool                 SetEpoch(const GmatEpoch newEpoch); 

protected:
   /// Count of the number of forces in the model
   Integer numForces;
   /// Index of the force currently in use
   Integer currentForce;
   /// The size of the state for a single spacecraft
   Integer stateSize;
   /// The state that the model uses; set by a StateManager
   GmatState *state;
   /// Associated Prop State Manager
   PropagationStateManager *psm;
   
//   /// List of spacecraft and formations that get propagated
//   std::vector<SpaceObject *> spacecraft;
   
   /// Names of the forces in the force model
   StringArray forceTypeNames;
   /// Array of the forces
   std::vector<PhysicalModel *> forceList;
//   /// User defined forces
//   std::vector<PhysicalModel *> userForces;
   
   /// Buffer that allows quick reversion to the previous state
   GmatState previousState;
//   /// Epoch for the previous state
//   Real previousTime;
//   /// Buffer that allows quick reversion to the previous state
//   Real *previousState;
   /// Estimation error relative to the change in state or the size of state.
   Integer estimationMethod;
   /// Defines the type of norm used in the error estimation.
   Integer normType;
   /// List of transient forces that need removal before reusing this instance 
   StringArray transientForceNames;
   /// List of reference objects for the owned forces
   StringArray forceReferenceNames;
   // Flag indicating if forces have been passed all spacecraft parms once
   bool parametersSetOnce;
   
   /// Name of the force model origin; actual body is the inherited forceOrigin
   std::string centralBodyName;
   /// Flag indicating whether or not the first time of coverage has already been
   /// determined for the force origin
   bool        coverageStartDetermined;
   /// Flag used to prevent unnecessary initialization calls
   bool forceMembersNotInitialized;
   
   /// Number of objects that propagate a 6-vector in this ODEModel
   Integer satCount;
   
   /// Parameter IDs on spacecraft needed to access the parms during integration
   Integer satIds[7];

   /// ID for CartesianState start for processing dynamic state data
   Integer stateStart;
   /// ID for CartesianState end for processing dynamic state data
   Integer stateEnd;

   /// Number of elements in the Cartesian state data
   Integer cartStateSize;
   /// If spacecraft properties are dynamic, update at every call
   bool dynamicProperties;
   
   IntegerArray dynamicsIndex;
   ObjectArray  dynamicObjects;
   IntegerArray dynamicIDs;

   // Used with force model parameters
   /// Flag used to detect if minimal for parm initialization has been performed
   bool isInitializedForParameters;
   /// Flag used to print parameter warnings only one time
   bool warnedOnceForParameters;

   /// Mapping between script descriptions and force names.
   static std::map<std::string, std::string> scriptAliases;
   
   const StringArray&  BuildBodyList(std::string type) const;
   const StringArray&  BuildCoordinateList() const;
   const StringArray&  BuildUserForceList() const;
   
//   virtual Integer     SetupSpacecraftData(GmatBase *sat, 
//                                           PhysicalModel *pm, Integer i);
   void                UpdateTransientForces();
   
   std::string         BuildForceNameString(PhysicalModel *force);
   void                ClearForceList(bool deleteTransient = false);
   void                ClearInternalCoordinateSystems();
   void                SetInternalCoordinateSystem(const std::string csId,
                                                   PhysicalModel *currentPm);
   
   // Pieces for prop with origin code
   /// Name of the common J2000 body that the state providers all use
   std::string               j2kBodyName;
   /// Pointer to the J2000 body
   CelestialBody             *j2kBody;
   
   /// Locally defined coordinate systems, if needed
   std::vector <CoordinateSystem*>
                             internalCoordinateSystems;
   
   ObjectArray               stateObjects;
   
   void                      MoveToOrigin(Real newEpoch = -1.0);
   void                      ReturnFromOrigin(Real newEpoch = -1.0);
   
   
   // Elements from the redesign
   struct StateStructure
   {
      Gmat::StateElementId id;
      Integer index;
      Integer count;
   //   Integer size;
   };
   
private:
   // Throws a warning, but can be safely ignored because this vector is not
   // used elsewhere
   std::vector<StateStructure> sstruct;

protected:
   Integer transientCount;

   bool                      BuildModelElement(Gmat::StateElementId id, 
                                               Integer start, 
                                               Integer objectCount);
   bool                      PrepareDerivativeArray();
   bool                      CompleteDerivativeCalculations(Real *state);
   
//   /// Data file used when debugging epoch data
//   std::ofstream             epochFile;
   
   /// map of mu values for SpacePoints
   std::map<std::string, Real>    muMap;

   enum
   {
      CENTRAL_BODY = PhysicalModelParamCount,
      PRIMARY_BODIES,
      POLYHEDRAL_BODIES,
      POINT_MASSES,
      DRAG,
      SRP,
      RELATIVISTIC_CORRECTION,
      ERROR_CONTROL,
      COORDINATE_SYSTEM_LIST,
      
      // owned object parameters
      DEGREE,
      ORDER,
      POTENTIAL_FILE,

      // The polyhedral model settings
      POLYHEDRAL_BODY,
      SHAPE_FILE_NAME,
      BODY_DENSITY,


      // Plug-in forces not otherwise handled
      USER_DEFINED,
      ODEModelParamCount
   };
   
   /** 
    * Enumeration for the style of the error estimation calculations
    * 
    * This enumeration determines if error estimation is relative the the size 
    * of the step taken, or to the total size of the vector.  Its usage is 
    * usually in the denominator of the estimate in the state error -- 
    * typically, the estimated error is the difference between a step taken 
    * using one algorithm and that generated from another.  This difference is 
    * divided by a value representing the total change for the ESTIMATE_STEP
    * setting, or by the total magnitude of the vector for the ESTIMATE_VECTOR
    * setting. 
    */ 
   enum
   {
      ESTIMATE_IN_BASE = 1,
      ESTIMATE_STEP,          // Size of the step 
      ESTIMATE_VECTOR         // Size of the vector
   };

   /// Norm used when estimating error
   enum
   {
      L2_MAGNITUDE = -2,
      L1_MAGNITUDE,
      NO_CONTROL,
      L1_DIFFERENCES,
      L2_DIFFERENCES
   };
   
   static const std::string 
      PARAMETER_TEXT[ODEModelParamCount - PhysicalModelParamCount];
   static const Gmat::ParameterType 
      PARAMETER_TYPE[ODEModelParamCount - PhysicalModelParamCount];
   
private:
   
   Integer GetOwnedObjectId(Integer id, GmatBase **owner) const;
   
};

#endif  // ODEModel_hpp

