//$Header$
//------------------------------------------------------------------------------
//                               GmatConnection
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
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

   bool OnExecute(const wxString& topic, wxChar *data, int size,
                  wxIPCFormat format);
   wxChar *OnRequest(const wxString& topic, const wxString& item, int *size,
                     wxIPCFormat format);
   bool OnPoke(const wxString& topic, const wxString& item, wxChar *data,
               int size, wxIPCFormat format);
   bool OnStartAdvise(const wxString& topic, const wxString& item);

};
