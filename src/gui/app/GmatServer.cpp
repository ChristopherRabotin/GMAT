//$Id$
//------------------------------------------------------------------------------
//                               GmatServer
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
 * Implements GmatServer which creates new connection to client.
 */
//------------------------------------------------------------------------------

#include "GmatServer.hpp"
#include "GmatConnection.hpp"
#include "ddesetup.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SERVER

//------------------------------------------------------------------------------
// wxConnectionBase* OnAcceptConnection(const wxString& topic)
//------------------------------------------------------------------------------
wxConnectionBase* GmatServer::OnAcceptConnection(const wxString& topic)
{
   #ifdef DEBUG_SERVER
   MessageInterface::ShowMessage
      ("GmatServer::OnAcceptConnection() topic=%s\n", topic.c_str());
   #endif
   
   if ( topic == IPC_TOPIC )
   {
      mConnection = new GmatConnection();
      
      #ifdef DEBUG_SERVER
      MessageInterface::ShowMessage
         ("GmatServer::OnAcceptConnection() mConnection=%p\n", mConnection);
      #endif
      
      return mConnection;
   }
   
   wxMessageBox(_T("OnAcceptConnection(): Unknown topic() entered"),
                _T("GmatServer"));
   
   // unknown topic
   mConnection = NULL;
   return NULL;
}


//------------------------------------------------------------------------------
// wxConnectionBase* GetConnection()
//------------------------------------------------------------------------------
wxConnectionBase* GmatServer::GetConnection()
{
   return mConnection;
}


//------------------------------------------------------------------------------
// bool Disconnect()
//------------------------------------------------------------------------------
bool GmatServer::Disconnect()
{
   #ifdef DEBUG_SERVER
   MessageInterface::ShowMessage
      ("GmatServer::Disconnect() mConnection=%p\n", mConnection);
   #endif
   
   if (mConnection)
      return mConnection->Disconnect();
   else
      return false;
}

