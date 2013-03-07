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

#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

#include <map>
#include "gmatwxdefs.hpp"
#include "ModelObject.hpp"

typedef std::map<int, ModelObject*> ModelMap;
typedef std::map<std::string, int> ModelIdMap;

class ModelManager
{
public:
   static ModelManager* Instance();
	
   wxGLContext* GetSharedGLContext();
	void SetSharedGLContext(wxGLContext *glContext);
   
	void ClearModel();
   ModelObject* GetModel(int id);
   int LoadModel(std::string &modelPath);
   
private:
   // Hide these so no one can call them
   ModelManager();
   ~ModelManager();

   /// Shared GL context between GL canvases
   wxGLContext *theGLContext;
   int numElements;
   ModelMap modelMap;
   ModelIdMap modelIdMap;
   static ModelManager *theInstance;
};

#endif // MODEL_MANAGER_H
