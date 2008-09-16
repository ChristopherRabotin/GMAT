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
#include "GuiMessageReceiver.hpp"  // for GuiMessageReceiver functions
#include "BaseException.hpp"
#include "FileManager.hpp"         // for GetFullPathname()
#include "GmatGlobal.hpp"          // for RunBachMode()
#include "FileUtil.hpp"            // for ParsePathName()
//#include "StringUtil.hpp"          // for GmatStringUtil::Replace()

//---------------------------------
//  static data
//---------------------------------
GuiMessageReceiver*  GuiMessageReceiver::instance = NULL;

//---------------------------------
//  private functions
//---------------------------------

//------------------------------------------------------------------------------
//  GuiMessageReceiver()
//------------------------------------------------------------------------------
/**
 * Constructor, called from the Instance method to create the singleton.
 */
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
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
GuiMessageReceiver::~GuiMessageReceiver()
{
}

//---------------------------------
//  public functions
//---------------------------------

//------------------------------------------------------------------------------
// GuiMessageReceiver* Instance()
//------------------------------------------------------------------------------
/**
 * Singleton accessor method
 * 
 * This method creates the GuiMessageReceiver singleton if it has not been
 * constructed, and returns the singleton instance.
 * 
 * @return The GuiMessageReceiver instance.
 */
//------------------------------------------------------------------------------
GuiMessageReceiver* GuiMessageReceiver::Instance()
{
   if (instance == NULL)
      instance = new GuiMessageReceiver;
   
   return instance;   
}


//------------------------------------------------------------------------------
//  std::string GetMessage()
//------------------------------------------------------------------------------
/**
 * Pops the messages off the message queue and concatenates them together.
 * 
 * @return The concatenated messages.
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
/**
 * Clears the message window.
 */  
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
/**
 * Returns the number of lines of text in the message window.
 * 
 * @return The number of lines used.
 */
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
 * Displays the message passed in as an std::string.
 * 
 * @param msgString The message that is displayed.
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::ShowMessage(const std::string &msgString)
{
   GmatAppData *appData = GmatAppData::Instance();
   if (appData->GetMessageTextCtrl() != NULL)
      appData->GetMessageTextCtrl()->AppendText(wxString(msgString.c_str()));
   
   LogMessage(msgString);   
}


//------------------------------------------------------------------------------
//  void ShowMessage(const char *msg, ...)
//------------------------------------------------------------------------------
/**
 * Displays a message passed in as a char* and a variable argument list.
 * 
 * @param msg The message, possibly including markers for variable argument 
 *            substitution.
 * @param ... The optional list of parameters that are inserted into the msg 
 *            string.
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
      msgBuffer = 
         "*** WARNING *** Cannot allocate enough memory to show the message.\n";
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
 * 
 * @param abortMsg    The abort message
 * @param continueMsg The continue message
 * @param msg         The message
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::PopupAbortContinue(const std::string &abortMsg,
                                          const std::string &continueMsg,
                                          const std::string &msg)
{  
   popupMessage = msg;
   abortMessage = abortMsg;
   continueMessage = continueMsg;
} // end PopupAbortContinue()


//------------------------------------------------------------------------------
//  static void PopupMessage(Gmat::MessageType msgType, const std::string &msg)
//------------------------------------------------------------------------------
/**
 * Pops up a message in a message box.
 * 
 * This method logs informational messages directed at pop-up message boxes and  
 * shows them in a pop-up.
 * 
 * @param msgType The type of message that is displayed, selected from the set
 *                {ERROR_, WARNING_, INFO_} enumerated in the Gmat namespace.
 * @param msg The message.
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::PopupMessage(Gmat::MessageType msgType, const std::string &msg)
{
   popupMessage = msg;
   messageType = msgType;
   
   // always show message
   ShowMessage(msg);
      
   if (GmatGlobal::Instance()->IsBatchMode() != true)
   {
      switch (msgType)
      {
      case Gmat::ERROR_:
         (void)wxMessageBox(wxT(wxString(msg.c_str())),
                            wxT("GMAT Error"));
         break;
      case Gmat::WARNING_:
         (void)wxMessageBox(wxT(wxString(msg.c_str())),
                            wxT("GMAT Warning"));
         break;
      case Gmat::INFO_:
         (void)wxMessageBox(wxT(wxString(msg.c_str())),
                            wxT("Information"));
         break;
      default:
         break;
      };
   }
   
} // end PopupMessage()


//------------------------------------------------------------------------------
//  static void PopupMessage(Gmat::MessageType msgType, const char *msg, ...)
//------------------------------------------------------------------------------
/**
 * Pops up a message in a message box.
 * 
 * This method logs informational messages directed at pop-up message boxes and
 * shows the message as a pop-up.
 * 
 * @param msgType The type of message that is displayed, selected from the set
 *                {ERROR_, WARNING_, INFO_} enumerated in the Gmat namespace.
 * @param msg The message, possibly including markers for variable argument 
 *            substitution.
 * @param ... The optional list of parameters that are inserted into the msg 
 *            string.
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
/**
 * Retrieves the fully qualified name of the log file.
 * 
 * @return The name of the log file, including path information.
 */
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
/**
 * Logs the message to the log file.
 * 
 * This method displays the input message on the console and writes it to the 
 * log file.
 * 
 * @param msg The message.
 */
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
 * Logs a variable argument formatted message to the log file.
 * 
 * This method calls teh std::string vrrersion of LogMessage to do the actual 
 * logging.
 * 
 * @param msg The message, possibly including markers for variable argument 
 *            substitution.
 * @param ... The optional list of parameters that are inserted into the msg 
 *            string.
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
/**
 * Turns logging on or off.
 * 
 * @param flag The new loggign state -- true enables logging, and false disables 
 *             it.  The logging state is idempotent.
 */
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
 * Calls OpenLogFile() to set the log file path and name and then open the log.
 *
 * @param  filename  log file name, such as "/newpath/test1/GmatLog.txt"
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::SetLogFile(const std::string &filename)
{
   std::string fname = filename;
   
   if (GmatFileUtil::ParsePathName(fname) == "")
   {
      FileManager *fm = FileManager::Instance();
      std::string outPath = fm->GetFullPathname(FileManager::OUTPUT_PATH);
      fname = outPath + fname;
   }
   
   OpenLogFile(fname);
}


//------------------------------------------------------------------------------
// void OpenLogFile(const std::string &filename, bool append = false)
//------------------------------------------------------------------------------
/*
 * Sets the log file name and opens the log file.
 *
 * @param filename  log file name, such as "/newpath/test1/GmatLog.txt"
 * @param append  true if appending log message
 */
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
      fprintf(logFile, "GMAT Log file set to %s\n", logFileName.c_str());
      
      logFileSet = true;
   }
}


//------------------------------------------------------------------------------
// void CloseLogFile()
//------------------------------------------------------------------------------
/**
 * Closes the log file.
 */
//------------------------------------------------------------------------------
void GuiMessageReceiver::CloseLogFile()
{
   if (logFile)
      fclose(logFile);
}
