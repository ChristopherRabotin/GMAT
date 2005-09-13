//$Header$
//------------------------------------------------------------------------------
//                             MessageInterface
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/11/03
//
/**
 * Defines operations on messages.
 */
//------------------------------------------------------------------------------
#if !defined __CONSOLE_APP__
#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#endif

#include <stdarg.h>                // for va_start(), va_end()
#include <stdio.h>                 // for vsprintf()
#include <string.h>                // for strlen()

#ifdef __WXMSW__                   // for malloc.h if MS Windows only
#include <malloc.h>                
#endif

#include <iostream>                // for cout, endl
#include <fstream>
#include <queue>                   // for queue
#include "MessageInterface.hpp"    // for MessageInterface functions
#include "BaseException.hpp"
#include "FileManager.hpp"         // for GetFullPathname()

//---------------------------------
//  static data
//---------------------------------
std::queue<std::string> MessageInterface::messageQueue;
std::string MessageInterface::popupMessage = "Initial popup message";
std::string MessageInterface::abortMessage = "Abort";
std::string MessageInterface::continueMessage = "Continue";
Gmat::MessageType MessageInterface::messageType = Gmat::INFO_;
int MessageInterface::showIntervalInMilSec = 2000;
short MessageInterface::messageExist = 0;
std::string MessageInterface::logFileName = "GmatLog.txt";
bool MessageInterface::logEnabled = true;
FILE* MessageInterface::logFile = NULL;

//---------------------------------
//  public functions
//---------------------------------

//------------------------------------------------------------------------------
//  MessageInterface()
//------------------------------------------------------------------------------
MessageInterface::MessageInterface()
{
//   msgStream = new std::ostream(std::cout.rdbuf());
//   msgStream = new std::ostream(std::cout);
   
   MessageInterface::messageQueue.push
      ("MessageInterface.cpp:MessageInterface(): Starting GMAT ...");
}

//------------------------------------------------------------------------------
//  MessageInterface()
//------------------------------------------------------------------------------
MessageInterface::~MessageInterface()
{
}

//------------------------------------------------------------------------------
//  void GetMessage()
//------------------------------------------------------------------------------
/**
 * Pops one message from message queue and concatenates.
 */
//------------------------------------------------------------------------------
std::string MessageInterface::GetMessage()
{
   std::string msg;
   
   while (!MessageInterface::messageQueue.empty())
   {
      msg = msg + MessageInterface::messageQueue.front().c_str();
      MessageInterface::messageQueue.pop();
   }
   MessageInterface::messageExist = 0;

   return msg;
}


//------------------------------------------------------------------------------
//  void ClearMessage()
//------------------------------------------------------------------------------
void MessageInterface::ClearMessage()
{
#if !defined __CONSOLE_APP__
   if (GmatAppData::GetMessageTextCtrl() != NULL)
   {
      GmatAppData::GetMessageTextCtrl()->Clear();
   }
//   else
//   {
//      wxLogError("MessageInterface::ClearMessage(): MessageWindow was not created.");
//      wxLog::FlushActive();
//   }
#endif
}


//------------------------------------------------------------------------------
//  int GetNumberOfMessageLines()
//------------------------------------------------------------------------------
int MessageInterface::GetNumberOfMessageLines()
{
#if !defined __CONSOLE_APP__
   if (GmatAppData::theMessageWindow != NULL)
   {
      return GmatAppData::theMessageWindow->GetNumberOfLines();
   }
   else
   {
      wxLogError("MessageInterface::GetNumberOfMessageLines(): "
                 "MessageWindow was not created.");
      wxLog::FlushActive();
      return 0;
   }
#endif
   return 0;
}


//------------------------------------------------------------------------------
//  void ShowMessage(const std::string &msgString)
//------------------------------------------------------------------------------
/**
 * Displays the message.
 */
//------------------------------------------------------------------------------
void MessageInterface::ShowMessage(const std::string &msgString)
{
   ShowMessage(msgString.c_str());
   
}


//------------------------------------------------------------------------------
//  void ShowMessage(const char *msg, ...)
//------------------------------------------------------------------------------
/**
 * Displays the message.
 */
//------------------------------------------------------------------------------
void MessageInterface::ShowMessage(const char *msg, ...)
{
   short    ret;
   short    size;
   va_list  marker;
   char     *msgBuffer;
   
   size = strlen(msg) + MAX_MESSAGE_LENGTH;
   
   if( (msgBuffer = (char *)malloc(size)) != NULL )
   {
      va_start(marker, msg);      
      ret = vsprintf(msgBuffer, msg, marker);      
      va_end(marker);
   }
   
#if !defined __CONSOLE_APP__
   if (GmatAppData::GetMessageTextCtrl() != NULL)
      GmatAppData::GetMessageTextCtrl()->AppendText(wxString(msgBuffer));
#endif
   
   LogMessage(std::string(msgBuffer));
   free(msgBuffer);
   
} // end ShowMessage()


//------------------------------------------------------------------------------
//  void PopupAbortContinue(const std::string abortMsg, ...)
//------------------------------------------------------------------------------
/**
 * Pops up Abort or Continue message box.
 */
//------------------------------------------------------------------------------
void MessageInterface::PopupAbortContinue(const std::string &abortMsg,
                                          const std::string &continueMsg,
                                          const std::string &msg)
{  
   MessageInterface::popupMessage = msg;
   MessageInterface::abortMessage = abortMsg;
   MessageInterface::continueMessage = continueMsg;
   
} // end PopupAbortContinue()


//------------------------------------------------------------------------------
//  static void PopupMessage(Gmat::MessageType msgType, const std::string &msg)
//------------------------------------------------------------------------------
/**
 * Pops up message box.
 */
//------------------------------------------------------------------------------
void MessageInterface::PopupMessage(Gmat::MessageType msgType, const std::string &msg)
{
   MessageInterface::popupMessage = msg;
   MessageInterface::messageType = msgType;
   
   PopupMessage(msgType, msg.c_str());
   
} // end PopupMessage()


//------------------------------------------------------------------------------
//  static void PopupMessage(Gmat::MessageType msgType, const char *msg, ...)
//------------------------------------------------------------------------------
/**
 * Pops up message box.
 */
//------------------------------------------------------------------------------
void MessageInterface::PopupMessage(Gmat::MessageType msgType, const char *msg, ...)
{
   short    ret;
   short    size;
   va_list  marker;
   char     *msgBuffer;
   
   size = strlen(msg) + MAX_MESSAGE_LENGTH;
   
   if( (msgBuffer = (char *)malloc(size)) != NULL )
   {
      va_start(marker, msg);      
      ret = vsprintf(msgBuffer, msg, marker);      
      va_end(marker);
   }
   
   
#if !defined __CONSOLE_APP__
   switch (msgType)
   {
   case Gmat::ERROR_:
      (void)wxMessageBox(wxT(wxString(msgBuffer)),
                         wxT("GMAT Error"));
      //loj: 9/13/05 use wxMessageBox so that messages shows immediately
      //wxLogError(wxT(wxString(msgBuffer)));
      //wxLog::FlushActive();
      break;
   case Gmat::WARNING_:
      //loj: 7/14/05 use wxMessageBox so that messages shows immediately
      //wxLogWarning(wxT(wxString(msgBuffer)));
      //wxLog::FlushActive();
      (void)wxMessageBox(wxT(wxString(msgBuffer)),
                         wxT("GMAT Warning"));
      break;
   case Gmat::INFO_:
      (void)wxMessageBox(wxT(wxString(msgBuffer)),
                         wxT("Information"));
      break;
   default:
      break;
   };
#endif
   
   LogMessage(std::string(msgBuffer));
   free(msgBuffer);
   
} // end PopupMessage()


//  //------------------------------------------------------------------------------
//  //  static void PopupMessage(Gmat::MessageType msgType, const std::string &msg,
//  //                           int interval)
//  //------------------------------------------------------------------------------
//  //  Purpose:
//  //     Shows popup message and closes itself.
//  //------------------------------------------------------------------------------
//  void MessageInterface::PopupMessage(Gmat::MessageType msgType, const std::string &msg,
//                                      int interval)
//  {
//      MessageInterface::popupMessage = msg;
//      MessageInterface::messageType = msgType;
//      MessageInterface::showIntervalInMilSec = interval;
   
//  } // end PopupMessage()


//------------------------------------------------------------------------------
// void LogMessage(const std::string &msg)
//------------------------------------------------------------------------------
void MessageInterface::LogMessage(const std::string &msg)
{
   std::cout << msg;

   if (logEnabled)
   {
      if (logFile == NULL)
      {
         FileManager *fm = FileManager::Instance();
         try
         {
            std::string filename = fm->GetFullPathname("LOG_FILE");
            logFile = fopen(filename.c_str(), "w");
         }
         catch (BaseException &e)
         {
            logFile = fopen(logFileName.c_str(), "w");
         }
      }
      
      if (logFile)
      {
         fprintf(logFile, "%s", msg.c_str());
         fflush(logFile);
      }
   }
}


//------------------------------------------------------------------------------
// void SetLogFile(const std::string &filename)
//------------------------------------------------------------------------------
void MessageInterface::SetLogFile(const std::string &filename)
{
   logFileName = filename;
   
   if (logFile)
      fclose(logFile);
   
   logFile = fopen(logFileName.c_str(), "w");
   fprintf(logFile, "MessageInterface::SetLogFile() Log file set to %s\n",
           logFileName.c_str());
}


//------------------------------------------------------------------------------
// void CloseLogFile()
//------------------------------------------------------------------------------
void MessageInterface::CloseLogFile()
{
   if (logFile)
      fclose(logFile);
}
