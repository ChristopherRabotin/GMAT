//$Id$
//------------------------------------------------------------------------------
//                               GmatServer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
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
