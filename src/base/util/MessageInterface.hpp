#ifndef MESSAGEINTERFACE_HPP_
#define MESSAGEINTERFACE_HPP_

#include <string>
#include "MessageReceiver.hpp"


class MessageInterface
{
public:
   static bool SetMessageReceiver(MessageReceiver *mr);
   static MessageReceiver* GetMessageReceiver();

   static void ShowMessage(const std::string &msg);
   static void ShowMessage(const char *msg, ...);

   static void PopupMessage(Gmat::MessageType msgType, const std::string &msg);
   static void PopupMessage(Gmat::MessageType msgType, const char *msg, ...);
   
   static std::string GetLogFileName();
   static void SetLogEnable(bool flag);
   static void SetLogPath(const std::string &pathname, bool append = false);
   static void SetLogFile(const std::string &filename);

   static void LogMessage(const std::string &msg);
   static void LogMessage(const char *msg, ...);

   static void ClearMessage();

private:
   static MessageReceiver     *theMessageReceiver;
   static const int MAX_MESSAGE_LENGTH;

   MessageInterface();
   virtual ~MessageInterface();
};

#endif /*MESSAGEINTERFACE_HPP_*/
