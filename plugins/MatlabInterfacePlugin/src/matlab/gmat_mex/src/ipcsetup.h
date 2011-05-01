//$Header: /GMAT/dev/cvs/matlab/gmat_mex/src/ipcsetup.h,v 1.1 2005/12/22 19:12:21 jgurgan Exp $
//------------------------------------------------------------------------------
//                               ipcsetup.h
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  J. Gurganus
// Created: 2005/11/30
//
/**
 * This is the IPC setup file.
 */
//------------------------------------------------------------------------------

// You may set this to 0 to prevent DDE from being used even under Windows
//#define wxUSE_DDE_FOR_IPC 0

#include <wx/ipc.h>

// the default service name
#define IPC_SERVICE _T("4242")

// the hostname
#define IPC_HOST _T("localhost")

// the IPC topic
#define IPC_TOPIC _T("GMAT-MATLAB")

// the name of the item we're being advised about
#define IPC_ADVISE_NAME _T("Item")
