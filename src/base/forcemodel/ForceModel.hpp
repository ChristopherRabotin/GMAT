//$Id$
//------------------------------------------------------------------------------
//                              ForceModel
//------------------------------------------------------------------------------
// *** File Name : ForceModel.hpp
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
/**
 * Container for forces acting on spacecraft.
 * 
 * ForceModel is a class that accumulates the forces acting on one or
 * more spacecraft
*/

#ifndef ForceModel_hpp
#define ForceModel_hpp

#include "PhysicalModel.hpp"
#include "MessageInterface.hpp"
#include "gmatdefs.hpp"

#include <fstream>
#include <vector>
#include <map>
#include <string.h>

#include <fstream>            // Used for streams in debugging methods


class GMAT_API ForceModel : public PhysicalModel
{
public:
   ForceModel(const std::string &nomme = "");
   virtual ~ForceModel(void);
   ForceModel(const ForceModel& fdf);
   ForceModel&   operator=(const ForceModel& fdf);
   
   void AddForce(PhysicalModel *pPhyscialModel);
   void DeleteForce(const std::string &name);
   void DeleteForce(PhysicalModel *pPhyscialModel);
   bool HasForce(const std::string &name);
   Integer GetNumForces();
   StringArray& GetForceTypeNames();
   std::string GetForceTypeName(Integer index);
   void ClearSpacecraft();
   PhysicalModel* GetForce(Integer index);
   const PhysicalModel* GetForce(std::string forcetype, Integer whichOne = 0) const; 
   bool AddSpaceObject(SpaceObject *so);
   void UpdateSpaceObject(Real newEpoch = -1.0);
   void UpdateFromSpaceObject();
   void RevertSpaceObject();
   
   virtual bool Initialize();
   virtual void IncrementTime(Real dt);
   virtual void SetTime(Real t);
   
   bool GetDerivatives(Real * state, Real dt = 0.0, Integer order = 1);
   Real EstimateError(Real *diffs, Real *answer) const;
   
   virtual Integer GetParameterCount(void) const;
   
   
   // inherited from GmatBase
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   
   virtual void         SetSolarSystem(SolarSystem *ss);
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
   
   static void          SetScriptAlias(const std::string& alias, 
                                       const std::string& typeName);
   static std::string&  GetScriptAlias(const std::string& alias);
   
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);
   
   virtual Integer      GetOwnedObjectCount();
   virtual GmatBase*    GetOwnedObject(Integer whichOne);
   virtual std::string  BuildPropertyName(GmatBase *ownedObj);
   
   #ifdef __WITH_FM_GEN_STRING__
   virtual const std::string&
   GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                       const std::string &prefix = "",
                       const std::string &useName = "");
   #endif
   
   void                 UpdateInitialData();
   void                 ReportEpochData();

protected:
   /// Count of the number of forces in the model
   Integer numForces;
   /// Index of the force currently in use
   Integer currentForce;
   /// The size of the state for a single spacecraft
   Integer stateSize;
   
   /// List of spacecraft and formations that get propagated
   std::vector<SpaceObject *> spacecraft;
   
   /// Names of the forces in the force model
   StringArray forceTypeNames;
   /// Array of the forces
   std::vector<PhysicalModel *> forceList;
   
   /// Epoch for the previous state
   Real previousTime;
   /// Buffer that allows quick reversion to the previous state
   Real *previousState;
   /// Determines if the method of error estimation is local or remote.
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
   /// Flag used to prevent unnecessary initialization calls
   bool forceMembersNotInitialized;
   
   Integer satIds[7];
   Integer modelEpochId;
   
   
   /// Mapping between script descriptions and force names.
   static std::map<std::string, std::string> scriptAliases;
   
   const StringArray&  BuildBodyList(std::string type) const;
   const StringArray&  BuildCoordinateList() const;
   virtual Integer     SetupSpacecraftData(GmatBase *sat, 
                                           PhysicalModel *pm, Integer i);
   void                UpdateTransientForces();
   
   #ifdef __WITH_FM_GEN_STRING__
   void                WriteFMParameters(Gmat::WriteMode mode,
                                         std::string &prefix,
                                         std::stringstream &stream);
   #endif
   
   std::string         BuildForceNameString(PhysicalModel *force);
   void                ClearInternalCoordinateSystems();
   void                SetInternalCoordinateSystem(const std::string csId,
                                                   PhysicalModel *currentPm);
   
   // Pieces for prop with origin code
   /// Name of the common J2000 body that the spacecraft atates all use
   std::string               j2kBodyName;
   /// Pointer to the spacecraft J2000 body
   CelestialBody             *j2kBody;
   
   /// Locally defined coordinate systems, if needed
   std::vector <CoordinateSystem*>
   InternalCoordinateSystems;
   
   /// EarthMJ2000Eq pointer, so that it can be cloned to make other Eq systems 
   CoordinateSystem          *earthEq;
   /// EarthFixed pointer, so that it can be cloned to make other fixed systems 
   CoordinateSystem          *earthFixed;
   
   void                      MoveToOrigin(Real newEpoch = -1.0);
   void                      ReturnFromOrigin(Real newEpoch = -1.0);
   
   /// Data file used when debugging epoch data
   std::ofstream             epochFile;
   
   enum
   {
      CENTRAL_BODY = PhysicalModelParamCount,
      PRIMARY_BODIES,
      POINT_MASSES,
      DRAG,
      SRP,
      NORM_TYPE,
      COORDINATE_SYSTEM_LIST,
      ForceModelParamCount
   };
   
   enum
   {
      ESTIMATE_IN_BASE = 1,
      ESTIMATE_LOCALLY
   };
   
   enum
   {
      L2_MAGNITUDE = -2,
      L1_MAGNITUDE,
      NO_CONTROL,
      L1_DIFFERENCES,
      L2_DIFFERENCES
   };
   
   static const std::string 
   PARAMETER_TEXT[ForceModelParamCount - PhysicalModelParamCount];
   static const Gmat::ParameterType 
   PARAMETER_TYPE[ForceModelParamCount - PhysicalModelParamCount];
};

#endif  // ForceModel_hpp

