//$Id$
//------------------------------------------------------------------------------
//                             PlotInterface
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Thomas G. Grubb
// Created: 2015/08/21
//
/**
 * Declares ListenerManagerInterface class.
 */
//------------------------------------------------------------------------------
#ifndef ListenerManagerInterface_hpp
#define ListenerManagerInterface_hpp

#include "gmatdefs.hpp"
#include "ListenerManager.hpp"
#include "ISolverListener.hpp"


/**
 * Interface functions for the ListenerManager classes.
 */
class GMAT_API ListenerManagerInterface
{

public:
   
   static void SetListenerManager(ListenerManager *lm);
   
   // for solver window listener
   static  ISolverListener* CreateSolverListener(const std::string &tableName,
                              const std::string &oldName,
                              Real positionX, Real positionY,
                              Real width, Real height, bool isMaximized);
   
private:

   ListenerManagerInterface();
   ~ListenerManagerInterface();
   
   static ListenerManager *theListenerManager;

};

#endif
