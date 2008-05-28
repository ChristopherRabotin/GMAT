//$Id$
//------------------------------------------------------------------------------
//                             GuiMessageReceiver
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel Conway, based on code by Linda Jun
// Created: 2008/04/28
//
/**
 * Defines operations on messages.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"

#include <stdarg.h>                // for va_start(), va_end()
#include <stdio.h>                 // for vsprintf()
#include <string.h>                // for strlen()

#ifdef __WXMSW__                   // for malloc.h if MS Windows only
#include <malloc.h>                
#endif

#include <iostream>                // for cout, endl
#include <fstream>
#include <sstream>
#include <queue>                   // for queue
#include "GuiMessageReceiver.hpp"    // for GuiMessageReceiver functions
#include "BaseException.hpp"
#include "FileManager.hpp"         // for GetFullPathname()
#include "GmatGlobal.hpp"          // for RunBachMode()
//#include "StringUtil.hpp"          // for GmatStringUtil::Replace()

//---------------------------------
//  static data
//---------------------------------
GuiMessageReceiver*  GuiMessageReceiver::instance = NULL;
//---------------------------------
//  public functions
//---------------------------------

//------------------------------------------------------------------------------
//  GuiMessageReceiver()
//------------------------------------------------------------------------------
GuiMessageReceiver::GuiMessageReceiver() :
   MAX_MESSAGE_LENGTH      (10000)
{
   messageQueue.push
      ("GuiMessageReceiver.cpp:GuiMessageReceiver(): Starting GMAT ...");
}

//------------------------------------------------------------------------------
//  GuiMessageReceiver()
//------------------------------------------------------------------------------
GuiMessageReceiver::~GuiMessageReceiver()
{
}

GuiMessageReceiver* GuiMessageReceiver::Instance()
{
   if (instance == NULL)
      instance = new GuiMessageReceiver;
   
   return instance;   
}


//------------------------------------------------------------------------------
//  void GetMessage()
//------------------------------------------------------------------------------
/**
 * Pops one message from message queue and concatenates.
 */
//------------------------------------------------------------------------------
std::string GuiMessageReceiver::GetMessage()
{
   std::string msg;
   
   while (!GuiMessageReceiver::messageQueue.empty())
   {
      msg = msg + GuiMessageReceiver::messageQueue.front().c_str();
      GuiMessageReceiver::messageQueue.pop();
   }
   GuiMessageReceiver::messageExist = 0;

   return msg;
}


//------------------------------------------------------------------------------
//  void ClearMessage()
//------------------------------------------------------------------------------
void GuiMessageReceiver::ClearMessage()
{
   GmatAppData *appData = GmatAppData::Instance();
   if (appData->GetMessageTextCtrl() != NULL)
   {
      appData->GetMessageTextCtrl()->Clear();
   }
}


//------------------------------------------------------------------------------
//  int GetNumberOfMessageLines()
//------------------------------------------------------------------------------
int GuiMessageReceiver::GetNumberOfMessageLines()
{
   GmatAppData *appData = GmatAppData::Instance();
   ViewTextFrame* theMessageWindow = appData->GetMessageWindow();
   if (theMessageWindow != NULL)
   {
      return theMessageWindow->GetNumberOfLines();
   }
   else
   {
      wxLogError("GuiMessageReceiver::GetNumberOfMessageLines(): "
                 "MessageWindow was not created.");
      wxLog::FlushActive();
      return 0;
   }

   return 0;
}


//------------------------------------------------------------------------------
//  void ShowMessage(const std::string &msgString)
//------------------------------------------------------------------------------
/**
 * Displays the message.
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::ShowMessage(const std::string &msgString)
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
void GuiMessageReceiver::ShowMessage(const char *msg, ...)
{
   short    ret;
   short    size;
   va_list  marker;
   char     *msgBuffer;
   
   // msg is vsprintf format
   // actual max message length is MAX_MESSAGE_LENGTH
   size = strlen(msg) + MAX_MESSAGE_LENGTH;
   //LogMessage("strlen(msg)=%d, size=%d\n", strlen(msg), size);
   
   if( (msgBuffer = (char *)malloc(size)) != NULL )
   {
      va_start(marker, msg);      
      ret = vsprintf(msgBuffer, msg, marker);
      va_end(marker);
      //LogMessage("ret from vsprintf()=%d\n", ret);
   }
   else
   {
      msgBuffer = "*** WARNING *** Cannot allocate enough memory to show the message.\n";
   }
   
   GmatAppData *appData = GmatAppData::Instance();
   if (appData->GetMessageTextCtrl() != NULL)
      appData->GetMessageTextCtrl()->AppendText(wxString(msgBuffer));
   
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
void GuiMessageReceiver::PopupAbortContinue(const std::string &abortMsg,
                                          const std::string &continueMsg,
                                          const std::string &msg)
{  
   GuiMessageReceiver::popupMessage = msg;
   GuiMessageReceiver::abortMessage = abortMsg;
   GuiMessageReceiver::continueMessage = continueMsg;
   
} // end PopupAbortContinue()


//------------------------------------------------------------------------------
//  static void PopupMessage(Gmat::MessageType msgType, const std::string &msg)
//------------------------------------------------------------------------------
/**
 * Pops up message box.
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::PopupMessage(Gmat::MessageType msgType, const std::string &msg)
{
   GuiMessageReceiver::popupMessage = msg;
   GuiMessageReceiver::messageType = msgType;
   
   PopupMessage(msgType, msg.c_str());
   
} // end PopupMessage()


//------------------------------------------------------------------------------
//  static void PopupMessage(Gmat::MessageType msgType, const char *msg, ...)
//------------------------------------------------------------------------------
/**
 * Pops up message box.
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::PopupMessage(Gmat::MessageType msgType, const char *msg, ...)
{
   short    ret;
   short    size;
   va_list  marker;
   char     *msgBuffer;
   
   // msg is vsprintf format
   // actual max message length is MAX_MESSAGE_LENGTH
   size = strlen(msg) + MAX_MESSAGE_LENGTH;
   
   if ( (msgBuffer = (char *)malloc(size)) != NULL )
   {
      va_start(marker, msg);      
      ret = vsprintf(msgBuffer, msg, marker);      
      va_end(marker);
      
      // if no EOL then append it
      if (msgBuffer[strlen(msgBuffer)-1] != '\n')
         msgBuffer[strlen(msgBuffer)] = '\n';
   }
   else
   {
      msgBuffer = "*** WARNING *** Cannot allocate enough memory to show the message.\n";
   }
   
   // always show message
   ShowMessage(msgBuffer);
   
   if (GmatGlobal::Instance()->IsBatchMode() != true)
   {
      switch (msgType)
      {
      case Gmat::ERROR_:
         (void)wxMessageBox(wxT(wxString(msgBuffer)),
                            wxT("GMAT Error"));
         break;
      case Gmat::WARNING_:
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
   }
   
   free(msgBuffer);
   
} // end PopupMessage()


//------------------------------------------------------------------------------
// std::string GetLogFileName()
//------------------------------------------------------------------------------
std::string GuiMessageReceiver::GetLogFileName()
{
   FileManager *fm = FileManager::Instance();
   std::string fname;
   try
   {
      if (logFileName == "")
      {
         fname = fm->GetFullPathname("LOG_FILE");
      }
      else
      {
         std::string outputPath = fm->GetPathname(FileManager::LOG_FILE);
         
         // add output path if there is no path
         if (logFileName.find("/") == logFileName.npos &&
             logFileName.find("\\") == logFileName.npos)
         {
            fname = outputPath + logFileName;
         }
      }
   }
   catch (BaseException &e)
   {
      GuiMessageReceiver::ShowMessage
         ("**** ERROR **** " + e.GetFullMessage() + 
          "So setting log file name to GmatLog.txt");
      
      fname = "GmatLog.txt";
   }
   
   return fname;
}


//------------------------------------------------------------------------------
// void LogMessage(const std::string &msg)
//------------------------------------------------------------------------------
void GuiMessageReceiver::LogMessage(const std::string &msg)
{
   std::cout << msg;
   
   if (logEnabled)
   {
      if (logFile == NULL)
      {
         SetLogFile(GetLogFileName());
      }
   }
   else if (!logFileSet)
   {
      OpenLogFile(logFileName);
   }
   
   if (logFile)
   {
      //std::string tempStr = GmatStringUtil::Replace(msg, "%", "%%");
      //fprintf(logFile, "%s", tempStr.c_str());
      fprintf(logFile, "%s", msg.c_str());
      fflush(logFile);
   }
}


//------------------------------------------------------------------------------
//  void LogMessage(const char *msg, ...)
//------------------------------------------------------------------------------
/**
 * Logs the message to a file.
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::LogMessage(const char *msg, ...)
{
   short    ret;
   short    size;
   va_list  marker;
   char     *msgBuffer;
   
   // msg is vsprintf format
   // actual max message length is MAX_MESSAGE_LENGTH
   size = strlen(msg) + MAX_MESSAGE_LENGTH;
   
   if ( (msgBuffer = (char *)malloc(size)) != NULL )
   {
      va_start(marker, msg);      
      ret = vsprintf(msgBuffer, msg, marker);      
      va_end(marker);
   }
   else
   {
      msgBuffer = "*** WARNING *** Cannot allocate enough memory to log the message.\n";
   }
   
   LogMessage(std::string(msgBuffer));
   free(msgBuffer);
   
} // end LogMessage()


//------------------------------------------------------------------------------
// void SetLogEnable(bool flag)
//------------------------------------------------------------------------------
void GuiMessageReceiver::SetLogEnable(bool flag)
{
   logEnabled = flag;
}


//------------------------------------------------------------------------------
// void SetLogPath(const std::string &pathname, bool append = false)
//------------------------------------------------------------------------------
/*
 * Sets log file path with keeping log file name as is.
 *
 * @param  pathname  log file path name, such as "/newpath/test1/"
 * @param  append  true if appending log message (false)
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::SetLogPath(const std::string &pathname, bool append)
{
   FileManager *fm = FileManager::Instance();
   std::string fname;
   try
   {
      std::string filename = fm->GetFilename(FileManager::LOG_FILE);
      fname = pathname + filename;
   }
   catch (BaseException &e)
   {
      GuiMessageReceiver::ShowMessage
         ("**** ERROR **** " + e.GetFullMessage() + 
          "So setting log file name to GmatLog.txt");
      
      fname = "GmatLog.txt";
   }
   
   OpenLogFile(fname, append);
   
}


//------------------------------------------------------------------------------
// void SetLogFile(const std::string &filename)
//------------------------------------------------------------------------------
/*
 * Sets log file path and name.
 *
 * @param  filename  log file name, such as "/newpath/test1/GmatLog.txt"
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::SetLogFile(const std::string &filename)
{
   OpenLogFile(filename);
}


//------------------------------------------------------------------------------
// void OpenLogFile(const std::string &filename, bool append = false)
//------------------------------------------------------------------------------
void GuiMessageReceiver::OpenLogFile(const std::string &filename, bool append)
{
   logFileName = filename;
   
   if (logFile)
      fclose(logFile);
   
   if (append)
      logFile = fopen(logFileName.c_str(), "a");
   else
      logFile = fopen(logFileName.c_str(), "w");
   
   if (!logFile)
   {
      std::cout << "**** ERROR **** Error setting the log file to " << logFileName
                << "\nSo setting it to \"GmatLog.txt\" in the executable directory\n";
      
      logFileName = "GmatLog.txt";
      
      if (append)
         logFile = fopen(logFileName.c_str(), "a");
      else
         logFile = fopen(logFileName.c_str(), "w");
   }
   
   if (logFile)
   {
      fprintf(logFile, "GMAT Build Date: %s %s\n\n",  __DATE__, __TIME__);
      fprintf(logFile, "GuiMessageReceiver::SetLogFile() Log file set to %s\n",
              logFileName.c_str());
      
      if (append)
         fprintf(logFile, "The log file mode is append\n");
      else
         fprintf(logFile, "The log file mode is create\n");
      
      logFileSet = true;
   }
}


//------------------------------------------------------------------------------
// void CloseLogFile()
//------------------------------------------------------------------------------
void GuiMessageReceiver::CloseLogFile()
{
   if (logFile)
      fclose(logFile);
}

