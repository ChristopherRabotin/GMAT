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

#include "GmatConnection.hpp"
#include "GmatInterface.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_CONNECTION 1

//---------------------------------
// global variables
//---------------------------------

GmatConnection *theGmatConnection = NULL;

//------------------------------------------------------------------------------
// GmatConnection()
//------------------------------------------------------------------------------
GmatConnection::GmatConnection()
   : wxConnection()
{
   theGmatConnection = this;
}


//------------------------------------------------------------------------------
// ~GmatConnection()
//------------------------------------------------------------------------------
GmatConnection::~GmatConnection()
{
   if (theGmatConnection)
   {
      theGmatConnection = NULL;
   }
}


//------------------------------------------------------------------------------
// bool OnExecute(const wxString& WXUNUSED(topic),
//------------------------------------------------------------------------------
bool GmatConnection::OnExecute(const wxString& WXUNUSED(topic),
                               wxChar *data,
                               int WXUNUSED(size),
                               wxIPCFormat WXUNUSED(format))
{
   #if DEBUG_CONNECTION
   MessageInterface::ShowMessage
      ("GmatConnection::OnExecute() command: %s\n", data);
   #endif
   
   return TRUE;
}


//------------------------------------------------------------------------------
// bool OnPoke(const wxString& WXUNUSED(topic),
//------------------------------------------------------------------------------
bool GmatConnection::OnPoke(const wxString& WXUNUSED(topic),
                            const wxString& item,
                            wxChar *data,
                            int WXUNUSED(size),
                            wxIPCFormat WXUNUSED(format))
{
   #if DEBUG_CONNECTION
   MessageInterface::ShowMessage
      ("GmatConnection::OnPoke() %s = %s\n", item.c_str(), data);
   #endif

   //------------------------------
   // save data to string stream
   //------------------------------
   
   if (strcmp(data, "Open;") == 0)
   {
      GmatInterface::Instance()->OpenScript();
   }
   else if (strcmp(data, "Clear;") == 0)
   {
      GmatInterface::Instance()->ClearScript();
   }
   else if (strcmp(data, "Build;") == 0)
   {
      GmatInterface::Instance()->BuildObject();
   }
   else if (strcmp(data, "Update;") == 0)
   {
      GmatInterface::Instance()->UpdateObject();
   }
   else if (strcmp(data, "Build+Run;") == 0)
   {
      GmatInterface::Instance()->BuildObject();
      GmatInterface::Instance()->RunScript();
   }
   else if (strcmp(data, "Run;") == 0)
   {
      GmatInterface::Instance()->RunScript();
   }
   else if (strcmp(data, "Callback;") == 0)
   {
      GmatInterface::Instance()->ExecuteCallback();
   }
   else if (strncmp(data, "CallbackData", 12) == 0)
   {
      std::string theString(data);
      std::string callbackData = theString.substr(13,1024);
      #if DEBUG_CONNECTION
         MessageInterface::ShowMessage
            ("GmatConnection::callbackData = %s\n", callbackData.c_str());
      #endif
      GmatInterface::Instance()->PutCallbackData(callbackData);
   }
   else
   {
      GmatInterface::Instance()->PutScript((char*)data);
   }
   
   return TRUE;
}


//------------------------------------------------------------------------------
// wxChar* OnRequest(const wxString& WXUNUSED(topic), const wxString& item,
//                   int * WXUNUSED(size), wxIPCFormat WXUNUSED(format))
//------------------------------------------------------------------------------
/*
 * This method responds to the client application to request data from the server.
 *
 * @param <topic>  Unused
 * @param <item>   Object or parameter name to retrive value from
 * @param <size>   Unused
 * @param <format> Unused
 *
 * @return Object or parameter value string.
 */
//------------------------------------------------------------------------------
wxChar* GmatConnection::OnRequest(const wxString& WXUNUSED(topic),
                                  const wxString& item,
                                  int * WXUNUSED(size),
                                  wxIPCFormat WXUNUSED(format))
{
   #if DEBUG_CONNECTION
   MessageInterface::ShowMessage
      ("GmatConnection::OnRequest() %s\n", item.c_str());
   #endif
   
   // Check for user interrupt first (loj: 2007.05.11 Added)
   GmatInterface::Instance()->CheckUserInterrupt();
   
   // How can I tell whether item is an object or a parameter?
   // For now GetGMATObject.m appends '.' for object name.
   
   char *data;
   if (item.Last() == '.')
   {
      wxString tempItem = item;
      tempItem.RemoveLast();
      data = GmatInterface::Instance()->GetInternalObject(tempItem.c_str());
   }
   else if (item == "RunState") //loj: 8/2/05 Added
   {
      data = GmatInterface::Instance()->GetRunState();
         
      #if DEBUG_CONNECTION
      MessageInterface::ShowMessage
         ("GmatConnection::OnRequest() data=%s\n", data);
      #endif
   }
   else if (item == "CallbackStatus") //wcs: 2006.08.24 Added
   {
      data = GmatInterface::Instance()->GetCallbackStatus();
         
      #if DEBUG_CONNECTION
      MessageInterface::ShowMessage
         ("GmatConnection::OnRequest() data=%s\n", data);
      #endif
   }
   else if (item == "CallbackResults") //wcs: 2006.09.21 Added
   {
      data = GmatInterface::Instance()->GetCallbackResults();
         
      #if DEBUG_CONNECTION
      MessageInterface::ShowMessage
         ("GmatConnection::OnRequest() data=%s\n", data);
      #endif
   }
   else
   {
      data = GmatInterface::Instance()->GetParameter(std::string(item.c_str()));
   }
   
   return _T(data);
}


//------------------------------------------------------------------------------
// bool OnStartAdvise(const wxString& WXUNUSED(topic),
//------------------------------------------------------------------------------
bool GmatConnection::OnStartAdvise(const wxString& WXUNUSED(topic),
                                   const wxString& item)
{
   #if DEBUG_CONNECTION
   MessageInterface::ShowMessage
      ("GmatConnection::OnRequest() %s\n", item.c_str());
   #endif
   
   //char* data = GmatInterface::Instance()->GetRunState();
   
   #if DEBUG_CONNECTION
   //MessageInterface::ShowMessage
   //   ("GmatConnection::OnStartAdvise() data=%s\n", data);
   #endif
   
   return TRUE;
}

