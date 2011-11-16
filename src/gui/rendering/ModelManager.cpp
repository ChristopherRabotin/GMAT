//$Id$
//------------------------------------------------------------------------------
//                            ModelManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Phillip Silvia
// Created: 2009/09/11
/**
 * Contains all of the models loaded and maps thems to an ID to be referenced
 * by the spacecraft and used by the canvases. 
 */
//------------------------------------------------------------------------------

#include "ModelManager.hpp"
#include "MessageInterface.hpp"

ModelManager* ModelManager::theInstance = NULL;

ModelManager* ModelManager::Instance()
{
   if (theInstance == NULL)
      theInstance = new ModelManager;
   return theInstance;
}

ModelManager::ModelManager()
{
   modelContext = NULL;
   numElements = 0;
   modelMap[-1] = new ModelObject();
}

ModelManager::~ModelManager()
{
   for (ModelMap::iterator pos = modelMap.begin();
        pos != modelMap.end(); ++pos)
   {
      if (pos->second)
      {
         delete pos->second;
      }
   }

   // Patch from Tristan Moody
   //   This patch moves the modelContext deletion to the ModelManager
   //   destructor from the OrbitViewCanvas and GroundTrackCanvas destructors.
   //   As long as the ModelManager destructor is called after all other
   //   OpenGL-related code is finished (to be verified), this should fix the
   //   problem reported in Bug 2591.
   //
   // New code (next 2 lines):
   if (modelContext != NULL)
      delete modelContext;
}

ModelObject* ModelManager::GetModel(int id){
   if (modelMap.find(id) != modelMap.end())
      return modelMap[id];
   return NULL;
}

int ModelManager::LoadModel(wxString &modelPath){
	#ifdef __USE_WX280_GL__
   if (stringMap.find(modelPath) != stringMap.end())
      return stringMap[modelPath];
	#endif
   ModelObject *newModel = new ModelObject();
   newModel->Load(modelPath);
   modelMap[numElements] = newModel;
   stringMap[modelPath] = numElements;
   numElements++;
   return numElements-1;
}
