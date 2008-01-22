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
 * Declares operations on messages.
 */
//------------------------------------------------------------------------------
#ifndef MessageInterface_hpp
#define MessageInterface_hpp

#include "gmatdefs.hpp"
#include <queue>        // for queue

namespace Gmat
{
   enum MessageType
   {
      ERROR_ = 10, //loj: cannot have ERROR
      WARNING_,
      INFO_,
      DEBUG_
   };

}

class MessageInterface
{

public:

   static std::string GetMessage();
   static void ClearMessage();
   static int  GetNumberOfMessageLines();
   static void ShowMessage(const std::string &msg);
   static void ShowMessage(const char *msg, ...);
   static void PopupMessage(Gmat::MessageType msgType, const std::string &msg);
   static void PopupMessage(Gmat::MessageType msgType, const char *msg, ...);
   //static void PopupMessage(Gmat::MessageType msgType, const std::string &msg,
   //                         int interval);
   static void PopupAbortContinue(const std::string &abortMsg,
                                  const std::string &continueMsg,
                                  const std::string &msg);
   
   static void LogMessage(const std::string &msg);
   static void LogMessage(const char *msg, ...);
   
   static std::string GetLogFileName();

   static void SetLogEnable(bool flag);
   static void SetLogPath(const std::string &pathname, bool append = false);
   static void SetLogFile(const std::string &filename);
   
   static void OpenLogFile(const std::string &filename, bool append = false);
   static void CloseLogFile();
   
private:
   
   static const int MAX_MESSAGE_LENGTH = 10000;
   
   static std::queue<std::string> messageQueue;
   static std::string popupMessage;
   static std::string abortMessage;
   static std::string continueMessage;
   static Gmat::MessageType messageType;
   static int showIntervalInMilSec;
   static short messageExist;
   static std::string logFileName;
   static FILE *logFile;
   static bool logEnabled;
   static bool logFileSet;
   static std::ostream *msgStream;
   
   MessageInterface();
   ~MessageInterface();
};

#endif
