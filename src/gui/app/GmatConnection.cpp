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
   else
   {
      GmatInterface::Instance()->PutScript((char*)data);
   }
   
   return TRUE;
}

//------------------------------------------------------------------------------
// wxChar* OnRequest(const wxString& WXUNUSED(topic),
//------------------------------------------------------------------------------
//  wxChar* GmatConnection::OnRequest(const wxString& WXUNUSED(topic),
//                                    const wxString& WXUNUSED(item),
//                                    int * WXUNUSED(size),
//                                    wxIPCFormat WXUNUSED(format))
wxChar* GmatConnection::OnRequest(const wxString& WXUNUSED(topic),
                                  const wxString& item,
                                  int * WXUNUSED(size),
                                  wxIPCFormat WXUNUSED(format))
{
#if DEBUG_CONNECTION
   MessageInterface::ShowMessage
      ("GmatConnection::OnRequest() %s\n", item.c_str());
#endif
  
   char *data;
   data = GmatInterface::Instance()->GetParameterData(std::string(item.c_str()));
   return _T(data);
}

//------------------------------------------------------------------------------
// bool OnStartAdvise(const wxString& WXUNUSED(topic),
//------------------------------------------------------------------------------
bool GmatConnection::OnStartAdvise(const wxString& WXUNUSED(topic),
                                   const wxString& WXUNUSED(item))
{
   return TRUE;
}

