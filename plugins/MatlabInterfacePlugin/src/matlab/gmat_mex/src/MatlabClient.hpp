//$Header: /GMAT/dev/cvs/matlab/gmat_mex/src/MatlabClient.hpp,v 1.1 2005/12/22 19:07:19 jgurgan Exp $
//------------------------------------------------------------------------------
//                               MatlabClient
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
// Author:  Joey Gurganus
// Created: 2005/11/30
//
/**
 * Declares MatlabClient which creates new connection to GMAT server.
 */
//------------------------------------------------------------------------------

#ifndef MatlabClient_hpp
#define MatlabClient_hpp

#include "gmatwxdefs.hpp"
#include "wx/ipc.h"
#include "MatlabConnection.hpp"

class MatlabClient: public wxClient
{
public:
    MatlabClient();
    ~MatlabClient();
    bool Connect();
    bool Connect(const wxString& sHost, const wxString& sService, 
                 const wxString& sTopic);
    bool Disconnect();
    wxConnectionBase *OnMakeConnection();
    bool IsConnected() { return m_connection != NULL; };
    MatlabConnection *GetConnection() { return m_connection; };

protected:
    MatlabConnection     *m_connection;
};

#endif // MatlabClient_hpp
