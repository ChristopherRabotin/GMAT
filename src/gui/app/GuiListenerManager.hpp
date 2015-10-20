//$Id$
//------------------------------------------------------------------------------
//                             PlotReceiver
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Thomas Grubb
// Created: 2008/08/21
//
/**
 * Declares GuiListenerManager class.
 */
//------------------------------------------------------------------------------
#ifndef GuiListenerManager_hpp
#define GuiListenerManager_hpp

#include "gmatdefs.hpp"
#include "ListenerManager.hpp"

/**
 * GuiListenerManager defines the interfaces used for creating listener classes (like solver window)
 */

class GuiListenerManager : public ListenerManager
{
public:
   static GuiListenerManager* Instance();
   virtual ISolverListener* CreateSolverListener(const std::string &tableName,
                                         const std::string &oldName,
                                         Real positionX, Real positionY,
                                         Real width, Real height, bool isMaximized);
   static void ClosingSolverListener();

protected:
   bool ComputePlotPositionAndSize(Real positionX,
                                   Real positionY, Real width, Real height,
                                   Integer &x, Integer &y, Integer &w, Integer &h,
                                   bool usingSaved = false);
private:
   GuiListenerManager();
   virtual ~GuiListenerManager();

   static int NumListeners;
   static GuiListenerManager* theGuiListenerManager;
};

#endif
