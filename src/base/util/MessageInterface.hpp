//$Id$
//------------------------------------------------------------------------------
//                             MessageInterface
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel Conway, based on code by Linda Jun
// Created: 2008/04/29
//
/**
 * Defines operations on messages.
 */
//------------------------------------------------------------------------------


#ifndef MessageInterface_hpp
#define MessageInterface_hpp

#include <string>
#include "MessageReceiver.hpp"

/**
 * The MessageInterface class defines static methods that are called to send
 * messages to the user.  MessageInterface passes these messages to an 
 * implementation specific class rerived from teh abstract  MessageReceiver 
 * class.  Display to the user is handled in the derived MessageReceiver.
 */
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
   static MessageReceiver  *theMessageReceiver;
   static const int        MAX_MESSAGE_LENGTH;

   MessageInterface();
   virtual ~MessageInterface();
};

#endif /* MessageInterface_hpp */
