//$Id$
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
#include <sstream>
#include <queue>                   // for queue
#include "MessageInterface.hpp"    // for MessageInterface functions
#include "BaseException.hpp"
#include "FileManager.hpp"         // for GetFullPathname()
#include "GmatGlobal.hpp"          // for RunBachMode()
//#include "StringUtil.hpp"          // for GmatStringUtil::Replace()

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
bool MessageInterface::logFileSet = false;
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
   
#if !defined __CONSOLE_APP__
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
#else   
   LogMessage(std::string(msgBuffer) + "\n");
#endif
   
   free(msgBuffer);
   
} // end PopupMessage()


//------------------------------------------------------------------------------
// std::string GetLogFileName()
//------------------------------------------------------------------------------
std::string MessageInterface::GetLogFileName()
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
      MessageInterface::ShowMessage
         ("**** ERROR **** " + e.GetFullMessage() + 
          "So setting log file name to GmatLog.txt");
      
      fname = "GmatLog.txt";
   }
   
   return fname;
}


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
void MessageInterface::LogMessage(const char *msg, ...)
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
void MessageInterface::SetLogEnable(bool flag)
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
void MessageInterface::SetLogPath(const std::string &pathname, bool append)
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
      MessageInterface::ShowMessage
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
void MessageInterface::SetLogFile(const std::string &filename)
{
   OpenLogFile(filename);
}


//------------------------------------------------------------------------------
// void OpenLogFile(const std::string &filename, bool append = false)
//------------------------------------------------------------------------------
void MessageInterface::OpenLogFile(const std::string &filename, bool append)
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
      fprintf(logFile, "MessageInterface::SetLogFile() Log file set to %s\n",
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
void MessageInterface::CloseLogFile()
{
   if (logFile)
      fclose(logFile);
}
