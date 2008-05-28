#include "MessageInterface.hpp"
#include <stdarg.h>              // for va_start() and va_end()

// Set static data
MessageReceiver* MessageInterface::theMessageReceiver = NULL;
const int MessageInterface::MAX_MESSAGE_LENGTH = 10000;


MessageInterface::MessageInterface()
{
}


MessageInterface::~MessageInterface()
{
}


bool MessageInterface::SetMessageReceiver(MessageReceiver *mr)
{
   theMessageReceiver = mr;
   return true;
}

MessageReceiver* MessageInterface::GetMessageReceiver()
{
   return theMessageReceiver;
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
   if (theMessageReceiver != NULL)
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
      }
      else
      {
         msgBuffer = "*** WARNING *** Cannot allocate enough memory to show "
            "the message.\n";
      }
      
      theMessageReceiver->ShowMessage(std::string(msgBuffer));
      free(msgBuffer);
   }
} // end ShowMessage()


//------------------------------------------------------------------------------
//  static void PopupMessage(Gmat::MessageType msgType, const std::string &msg)
//------------------------------------------------------------------------------
/**
 * Pops up message box.
 */
//------------------------------------------------------------------------------
void MessageInterface::PopupMessage(Gmat::MessageType msgType, const std::string &msg)
{
   if (theMessageReceiver != NULL)
   {
//      MessageInterface::popupMessage = msg;
//      MessageInterface::messageType = msgType;

      PopupMessage(msgType, msg.c_str());
   }
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
   if (theMessageReceiver != NULL)
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
      theMessageReceiver->ShowMessage(msgBuffer);
      
      free(msgBuffer);
   }
} // end PopupMessage()

std::string MessageInterface::GetLogFileName()
{
   if (theMessageReceiver == NULL)
      return "";
   return theMessageReceiver->GetLogFileName();
}

void MessageInterface::SetLogEnable(bool flag)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->SetLogEnable(flag);
}

void MessageInterface::SetLogPath(const std::string &pathname, bool append)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->SetLogPath(pathname, append);
}

void MessageInterface::SetLogFile(const std::string &filename)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->SetLogFile(filename);
}

void MessageInterface::LogMessage(const std::string &msg)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->LogMessage(msg);
}

void MessageInterface::LogMessage(const char *msg, ...)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->LogMessage(
            "Hey, you still need to hook up LogMessage in MessageInterface!!!");
}

void MessageInterface::ClearMessage()
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->ClearMessage();
}
