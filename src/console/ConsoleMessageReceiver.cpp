#include "ConsoleMessageReceiver.hpp"

#include <stdarg.h>                // for va_start(), va_end()
#include <stdio.h>                 // for vsprintf()
#include <string.h>                // for strlen()
#include <iostream>                // for cout, endl
#include <fstream>
//#include <sstream>
#include <queue>                   // for queue
#include "MessageInterface.hpp"    // for MessageInterface functions
#include "BaseException.hpp"
#include "FileManager.hpp"         // for GetFullPathname()
#include "GmatGlobal.hpp"          // for RunBachMode()

ConsoleMessageReceiver* ConsoleMessageReceiver::theInstance = NULL;

ConsoleMessageReceiver* ConsoleMessageReceiver::Instance()
{
   if (theInstance == NULL)
      theInstance = new ConsoleMessageReceiver;
   
   return theInstance;
}

ConsoleMessageReceiver::ConsoleMessageReceiver() :
   MAX_MESSAGE_LENGTH       (10000),
   logFile                  (NULL)
{
   messageQueue.push("ConsoleMessageReceiver: Starting GMAT ...");
}

ConsoleMessageReceiver::~ConsoleMessageReceiver()
{
}

void ConsoleMessageReceiver::ShowMessage(const std::string &msg)
{
   ShowMessage(msg.c_str());
}

void ConsoleMessageReceiver::ShowMessage(const char *msg, ...)
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
   
   LogMessage(std::string(msgBuffer));
   free(msgBuffer);
}

void ConsoleMessageReceiver::PopupMessage(Gmat::MessageType msgType, const std::string &msg)
{
   popupMessage = msg;
   messageType = msgType;
   
   PopupMessage(msgType, msg.c_str());
}

void ConsoleMessageReceiver::PopupMessage(Gmat::MessageType msgType, const char *msg, ...)
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
   
   LogMessage(std::string(msgBuffer) + "\n");
   
   free(msgBuffer);
}

std::string ConsoleMessageReceiver::GetLogFileName()
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
      ShowMessage
         ("**** ERROR **** " + e.GetFullMessage() + 
          "So setting log file name to GmatLog.txt");
      
      fname = "GmatLog.txt";
   }
   
   return fname;
}

void ConsoleMessageReceiver::SetLogEnable(bool flag)
{
   logEnabled = flag;
}

void ConsoleMessageReceiver::SetLogPath(const std::string &pathname, bool append)
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
      ShowMessage
         ("**** ERROR **** " + e.GetFullMessage() + 
          "So setting log file name to GmatLog.txt");
      
      fname = "GmatLog.txt";
   }
   
   OpenLogFile(fname, append);
}

void ConsoleMessageReceiver::SetLogFile(const std::string &filename)
{
   OpenLogFile(filename);
}

void ConsoleMessageReceiver::OpenLogFile(const std::string &filename, 
      bool append)
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
void ConsoleMessageReceiver::CloseLogFile()
{
   if (logFile)
      fclose(logFile);
}


void ConsoleMessageReceiver::LogMessage(const std::string &msg)
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

void ConsoleMessageReceiver::LogMessage(const char *msg, ...)
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
}

void ConsoleMessageReceiver::ClearMessage()
{
}
