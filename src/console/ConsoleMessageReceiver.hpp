#ifndef CONSOLEMESSAGERECEIVER_HPP_
#define CONSOLEMESSAGERECEIVER_HPP_

#include "MessageReceiver.hpp"

class ConsoleMessageReceiver : public MessageReceiver
{
public:
   static ConsoleMessageReceiver*   Instance();

   virtual void ShowMessage(const std::string &msg);
   virtual void ShowMessage(const char *msg, ...);

   virtual void PopupMessage(Gmat::MessageType msgType, const std::string &msg);
   virtual void PopupMessage(Gmat::MessageType msgType, const char *msg, ...);
   
   virtual std::string GetLogFileName();
   virtual void SetLogEnable(bool flag);
   virtual void SetLogPath(const std::string &pathname, bool append = false);
   virtual void SetLogFile(const std::string &filename);

   virtual void LogMessage(const std::string &msg);
   virtual void LogMessage(const char *msg, ...);

   virtual void ClearMessage();

   virtual void OpenLogFile(const std::string &filename, bool append = false);
   virtual void CloseLogFile();

private:
   static ConsoleMessageReceiver* theInstance;
   const int MAX_MESSAGE_LENGTH;
   
   std::queue<std::string> messageQueue;
   std::string popupMessage;
   std::string abortMessage;
   std::string continueMessage;
   Gmat::MessageType messageType;
   int showIntervalInMilSec;
   short messageExist;
   std::string logFileName;
   FILE *logFile;
   bool logEnabled;
   bool logFileSet;
   std::ostream *msgStream;
   
   ConsoleMessageReceiver();
   virtual ~ConsoleMessageReceiver();
};

#endif /*CONSOLEMESSAGERECEIVER_HPP_*/
