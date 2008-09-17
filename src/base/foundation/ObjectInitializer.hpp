//$Id$
//------------------------------------------------------------------------------
//                                  ObjectInitializer
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CCA54C
//
// Author: Wendy Shoan
// Created: 2008.06.10
//
// Original code from the Sandbox.
// Author: Darrel J. Conway
//
/**
 * Definition for the ObjectInitializer class.
 * This class initializes objects of the specified LocalObjectStore and ,
 * on option, GlobalObjectStore.
 */
//------------------------------------------------------------------------------
#ifndef ObjectInitializer_hpp
#define ObjectInitializer_hpp

#include "gmatdefs.hpp"

// included definitionss
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "AxisSystem.hpp"

// forward references
class Moderator;
class Publisher;

class GMAT_API ObjectInitializer
{
public:
   ObjectInitializer(SolarSystem *solSys, ObjectMap *objMap,
                     ObjectMap *globalObjMap, CoordinateSystem* internalCS, bool useGOS = false);
   ObjectInitializer(const ObjectInitializer &objInit);
   ObjectInitializer& operator= (const ObjectInitializer &objInit);
   virtual ~ObjectInitializer();
   
   void SetSolarSystem(SolarSystem *solSys);
   void SetObjectMap(ObjectMap *objMap);
   void SetCoordinateSystem(CoordinateSystem* internalCS);
   //   void SetGlobalObjectMap(ObjectMap *globalObjMap);
   //   void SetCoordinateSystem(CoordinateSystem *internalCS);
   
   bool InitializeObjects(bool registerSubs = false);
   
protected:
   
   SolarSystem      *ss;
   ObjectMap        *LOS;
   ObjectMap        *GOS;
   Moderator        *mod;
   CoordinateSystem *cs;
   Publisher        *publisher;
   
   bool            includeGOS;

   void            InitializeInternalObjects();

   //*********************  TEMPORARY  *****************************************
   void  InitializeCoordinateSystem(CoordinateSystem *cs);
   //*********************  END OF TEMPORARY  **********************************

   void            BuildReferences(GmatBase *obj);
   void            SetRefFromName(GmatBase *obj,
                                  const std::string &oName);
   void            BuildAssociations(GmatBase * obj);
   SpacePoint *    FindSpacePoint(const std::string &spName);
   
   GmatBase*       FindObject(const std::string &name);
   
private:
   
   ObjectInitializer();

};

#endif // ObjectInitializer_hpp
