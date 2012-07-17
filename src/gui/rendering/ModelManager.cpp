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

//#define DEBUG_LOAD_MODEL

ModelManager* ModelManager::theInstance = NULL;

//------------------------------------------------------------------------------
// static ModelManager* ModelManager::Instance()
//------------------------------------------------------------------------------
ModelManager* ModelManager::Instance()
{
   if (theInstance == NULL)
      theInstance = new ModelManager;
   return theInstance;
}

//------------------------------------------------------------------------------
// ModelManager()
//------------------------------------------------------------------------------
ModelManager::ModelManager()
{
   #ifdef DEBUG_MODEL_MANAGER
   MessageInterface::ShowMessage("ModelManager::ModelManager() entered\n");
   #endif
   
   modelContext = NULL;
   numElements = 0;
	modelMap.clear();
	modelIdMap.clear();
	// Why create new ModelObject here and set to [-1]? (LOJ: 2011.12.08)
   //modelMap[-1] = new ModelObject(); // Commented out (LOJ: 2012.07.17)
   
   #ifdef DEBUG_MODEL_MANAGER
   MessageInterface::ShowMessage("ModelManager::ModelManager() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// ModelManager()
//------------------------------------------------------------------------------
ModelManager::~ModelManager()
{
   #ifdef DEBUG_MODEL_MANAGER
   MessageInterface::ShowMessage
      ("ModelManager::~ModelManager() entered, modelMap.size() = %d\n", modelMap.size());
   #endif
   
   for (ModelMap::iterator pos = modelMap.begin(); pos != modelMap.end(); ++pos)
   {
      if (pos->second)
         delete pos->second;
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


//------------------------------------------------------------------------------
// void ClearModel()
//------------------------------------------------------------------------------
void ModelManager::ClearModel()
{
   if (modelContext)
      delete modelContext;
   
   modelContext = NULL;
   numElements = 0;
	modelMap.clear();
	modelIdMap.clear();
}


//------------------------------------------------------------------------------
// ModelObject* GetModel(int id)
//------------------------------------------------------------------------------
ModelObject* ModelManager::GetModel(int id)
{
   if (modelMap.find(id) != modelMap.end())
      return modelMap[id];
   return NULL;
}


//------------------------------------------------------------------------------
// int LoadModel(wxString &modelPath)
//------------------------------------------------------------------------------
int ModelManager::LoadModel(wxString &modelPath)
{
	#ifdef DEBUG_LOAD_MODEL
	MessageInterface::ShowMessage
		("ModelManager::LoadModel() entered,  modelIdMap.size() = %d\n   modelPath = '%s'\n",
		 modelIdMap.size(), modelPath.c_str());
	for (ModelIdMap::iterator pos = modelIdMap.begin(); pos != modelIdMap.end(); ++pos)
		MessageInterface::ShowMessage
			("    modelPath = '%s', id = %d\n", (pos->first).c_str(),  pos->second);
	#endif
	
	// Do we need this flag here? Commented out (LOJ: 2011.12.08)
	//#ifdef __USE_WX280_GL__
   if (modelIdMap.find(modelPath) != modelIdMap.end())
	{
	   #ifdef DEBUG_LOAD_MODEL
		MessageInterface::ShowMessage
			("ModelManager::LoadModel() modelPath found, returning %d\n",
			 modelIdMap[modelPath]);
		#endif
      return modelIdMap[modelPath];
	}
	//#endif
	
   ModelObject *newModel = new ModelObject();
	
	#ifdef DEBUG_LOAD_MODEL	
	MessageInterface::ShowMessage
		("ModelManager::LoadModel() Created new ModelObject <%p>, numElements = %d\n",
		 newModel, numElements);
	#endif
	
   newModel->Load(modelPath);
   modelMap[numElements] = newModel;
   modelIdMap[modelPath] = numElements;
   numElements++;
	
	#ifdef DEBUG_LOAD_MODEL	
	MessageInterface::ShowMessage
		("ModelManager::LoadModel() returning %d\n", numElements-1);
	#endif
	
   return numElements-1;
}

