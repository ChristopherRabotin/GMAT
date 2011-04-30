//$Id$
//------------------------------------------------------------------------------
//                                ddesetup.hpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/08/27
//
/**
 * This is DDE setup file.
 */
//------------------------------------------------------------------------------
#ifndef ddesetup_hpp
#define ddesetup_hpp

// You may set this to 0 to prevent DDE from being used even under Windows
//#define wxUSE_DDE_FOR_IPC 0

#include <wx/ipc.h>

// the default service name
#define IPC_SERVICE _T("4242")

// the IPC topic
#define IPC_TOPIC _T("GMAT-MATLAB")

// the name of the item we're being advised about
#define IPC_ADVISE_NAME _T("Item")

#endif
