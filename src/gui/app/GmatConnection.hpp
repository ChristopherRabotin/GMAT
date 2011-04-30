//$Id$
//------------------------------------------------------------------------------
//                               GmatConnection
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
 * Implements GmatConnection which provides service to client.
 */
//------------------------------------------------------------------------------
#ifndef GmatConnection_hpp
#define GmatConnection_hpp

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/ipc.h"

class GmatConnection : public wxConnection
{
public:
   GmatConnection();
   ~GmatConnection();
   
   virtual wxChar* OnRequest(const wxString& topic, const wxString& item,
                             int *size, wxIPCFormat format);
   virtual bool    OnExecute(const wxString& topic, wxChar *data, int size,
                             wxIPCFormat format);
   virtual bool    OnPoke(const wxString& topic, const wxString& item, wxChar *data,
                          int size, wxIPCFormat format);
   virtual bool    OnStartAdvise(const wxString& topic, const wxString& item);
   virtual bool    OnDisconnect();
   
};

#endif // GmatConnection_hpp
