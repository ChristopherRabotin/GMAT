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
//#include "wx/wx.h"
#include "ModelObject.hpp"

typedef std::map<int, ModelObject*> ModelMap;
typedef std::map<wxString, int> StringMap;

class ModelManager {
public:
   static ModelManager* Instance();
   ModelManager();
   ~ModelManager();

   wxGLContext *modelContext;

   ModelObject* GetModel(int id);
   int LoadModel(wxString &modelPath);
private:
   int numElements;
   ModelMap modelMap;
   StringMap stringMap;
   static ModelManager *theInstance;
};

#endif // MODEL_MANAGER_H
