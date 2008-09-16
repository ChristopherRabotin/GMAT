//$Header: /GMAT/dev/cvs/matlab/gmat_mex/src/MatlabConnection.hpp,v 1.1 2005/12/22 19:08:49 jgurgan Exp $
//------------------------------------------------------------------------------
//                            MatlabConnection
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus
// Created: 2005/11/30
//
/**
 * Declares MatlabConnnection to provide server/client.
 */
//------------------------------------------------------------------------------

#ifndef MatlabConnection_hpp 
#define MatlabConnection_hpp

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "ipcsetup.h"

class MatlabConnection: public wxConnection
{
public:
   virtual bool Execute(const wxChar *data, int size = -1, 
                        wxIPCFormat format = wxIPC_TEXT);
   virtual wxChar *Request(const wxString& item, int *size = NULL, 
                           wxIPCFormat format = wxIPC_TEXT);
   virtual bool Poke(const wxString& item, wxChar *data, int size = -1, 
                     wxIPCFormat format = wxIPC_TEXT);
   virtual bool OnAdvise(const wxString& topic, const wxString& item, 
                         wxChar *data, int size, wxIPCFormat format);
   virtual bool OnDisconnect();
protected:    
   void Log(const wxString& command, const wxString& topic,
            const wxString& item, wxChar *data, int size, wxIPCFormat format);
};

#endif // MatlabConnection_hpp
