//$Id$
//------------------------------------------------------------------------------
//                             PlotInterface
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Thomas G. Grubb
// Created: 2015/08/21
//
/**
 * Implements ListenerManagerInterface class.
 */
//------------------------------------------------------------------------------

#include "ListenerManagerInterface.hpp"
#include "MessageInterface.hpp"


//---------------------------------
//  static data
//---------------------------------
ListenerManager *ListenerManagerInterface::theListenerManager = NULL;

//---------------------------------
//  public functions
//---------------------------------


//------------------------------------------------------------------------------
// void SetListenerManager(ListenerManager *lm)
//------------------------------------------------------------------------------
void ListenerManagerInterface::SetListenerManager(ListenerManager *lm)
{
   theListenerManager = lm;
}


//------------------------------------------------------------------------------
//  ListenerManagerInterface()
//------------------------------------------------------------------------------
ListenerManagerInterface::ListenerManagerInterface()
{
}


//------------------------------------------------------------------------------
//  ~ListenerManagerInterface()
//------------------------------------------------------------------------------
ListenerManagerInterface::~ListenerManagerInterface()
{
}


//------------------------------------------------------------------------------
//  bool CreateListener()
//------------------------------------------------------------------------------
/*
 * Creates OpenGlPlot window
 *
 * @param <plotName> plot name
 */
//------------------------------------------------------------------------------
ISolverListener* ListenerManagerInterface::CreateSolverListener(const std::string &tableName,
                                         const std::string &oldName,
                                         Real positionX, Real positionY,
                                         Real width, Real height, bool isMaximized)
{
   #if DEBUG_PLOTIF_GL_CREATE
   MessageInterface::ShowMessage
      ("PI::CreateListener() %s entered, x = %12.10f, y = %12.10f, w = %12.10f, h = %12.10f,"
            "theListenerManager=<%p>\n", tableName.c_str(), positionX, positionY, width, height,
       thePlotReceiver);
   #endif
   
   if (theListenerManager != NULL)
      return theListenerManager->CreateSolverListener(tableName, oldName, positionX, positionY,
            width, height, isMaximized);
   
   return NULL;
}


