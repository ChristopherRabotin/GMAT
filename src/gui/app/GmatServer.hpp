//$Id$
//------------------------------------------------------------------------------
//                               GmatServer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// ** Legal **
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/08/27
//
/**
 * Declares GmatServer which creates new connection to client.
 */
//------------------------------------------------------------------------------
#ifndef GmatServer_hpp
#define GmatServer_hpp

#include "gmatwxdefs.hpp"
#include "GmatConnection.hpp"
#include "wx/ipc.h"

class GmatServer: public wxServer
{
public:
   GmatServer();
   wxConnectionBase *OnAcceptConnection(const wxString& topic);
   wxConnectionBase *GetConnection();
   bool Disconnect();
   
protected:
   GmatConnection *mConnection;
};

#endif // GmatServer_hpp
