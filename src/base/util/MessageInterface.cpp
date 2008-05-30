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
/**
 * Implements operations on messages.
 */
//------------------------------------------------------------------------------
#include "MessageInterface.hpp"
#include <stdarg.h>              // for va_start() and va_end()

//---------------------------------
//  static data
//---------------------------------
MessageReceiver* MessageInterface::theMessageReceiver = NULL;
const int MessageInterface::MAX_MESSAGE_LENGTH = 10000;


//---------------------------------
//  private methods
//---------------------------------


//------------------------------------------------------------------------------
// MessageInterface()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
MessageInterface::MessageInterface()
{
}

//------------------------------------------------------------------------------
// ~MessageInterface()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
MessageInterface::~MessageInterface()
{
}


//---------------------------------
//  public static methods
//---------------------------------


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
 * Passes an std::string message to the MessageReceiver.
 * 
 * @param msgString The message that is displayed.
 */
//------------------------------------------------------------------------------
void MessageInterface::ShowMessage(const std::string &msgString)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->ShowMessage(msgString.c_str());
}


//------------------------------------------------------------------------------
//  void ShowMessage(const char *msg, ...)
//------------------------------------------------------------------------------
/**
 * Passes a variable argument delimited message to the MessageReceiver.
 * 
 * @param msg The message, possibly including markers for variable argument 
 *            substitution.
 * @param ... The optional list of parameters that are inserted into the msg 
 *            string.
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
 * Passes a popup message to the MessageReceiver.
 * 
 * @param msgType The type of message that is displayed, selected from the set
 *                {ERROR_, WARNING_, INFO_} enumerated in the Gmat namespace.
 * @param msg The message.
 */
//------------------------------------------------------------------------------
void MessageInterface::PopupMessage(Gmat::MessageType msgType, const std::string &msg)
{
   if (theMessageReceiver != NULL)
   {
      PopupMessage(msgType, msg.c_str());
   }
} // end PopupMessage()


//------------------------------------------------------------------------------
//  static void PopupMessage(Gmat::MessageType msgType, const char *msg, ...)
//------------------------------------------------------------------------------
/**
 * Passes a variable argument delimited popup message to the MessageReceiver.
 * 
 * @param msgType The type of message that is displayed, selected from the set
 *                {ERROR_, WARNING_, INFO_} enumerated in the Gmat namespace.
 * @param msg The message, possibly including markers for variable argument 
 *            substitution.
 * @param ... The optional list of parameters that are inserted into the msg 
 *            string.
 */
//------------------------------------------------------------------------------
void MessageInterface::PopupMessage(Gmat::MessageType msgType, const char *msg, 
      ...)
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
      
      theMessageReceiver->PopupMessage(msgType, std::string(msgBuffer));
      
      free(msgBuffer);
   }
} // end PopupMessage()

//------------------------------------------------------------------------------
// std::string GetLogFileName()
//------------------------------------------------------------------------------
/**
 * Retrieves the fully qualified name of the log file from the MessageReceiver.
 * 
 * @return The name of the log file, including path information.
 */
//------------------------------------------------------------------------------
std::string MessageInterface::GetLogFileName()
{
   if (theMessageReceiver == NULL)
      return "";
   return theMessageReceiver->GetLogFileName();
}

//------------------------------------------------------------------------------
// void SetLogEnable(bool flag)
//------------------------------------------------------------------------------
/**
 * Tells the MessageReceiver to turn logging on or off.
 * 
 * @param flag The new logging state -- true enables logging, and false disables 
 *             it.  The logging state is idempotent.
 */
//------------------------------------------------------------------------------
void MessageInterface::SetLogEnable(bool flag)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->SetLogEnable(flag);
}

//------------------------------------------------------------------------------
// void SetLogPath(const std::string &pathname, bool append = false)
//------------------------------------------------------------------------------
/*
 * Sends log file path and append state to the MessageReceiver.
 *
 * @param  pathname  log file path name, such as "/newpath/test1/"
 * @param  append  true if appending log message (false)
 */
//------------------------------------------------------------------------------
void MessageInterface::SetLogPath(const std::string &pathname, bool append)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->SetLogPath(pathname, append);
}

//------------------------------------------------------------------------------
// void SetLogFile(const std::string &filename)
//------------------------------------------------------------------------------
/*
 * Sends the log file path and name to the MessageReceiver.
 *
 * @param  filename  log file name, such as "/newpath/test1/GmatLog.txt"
 */
//------------------------------------------------------------------------------
void MessageInterface::SetLogFile(const std::string &filename)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->SetLogFile(filename);
}

//------------------------------------------------------------------------------
//  void LogMessage(const std::string &msg)
//------------------------------------------------------------------------------
/**
 * Sends a message to the MessageReceiver for logging.
 * 
 * @param msg The message.
 */
//------------------------------------------------------------------------------
void MessageInterface::LogMessage(const std::string &msg)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->LogMessage(msg);
}

//------------------------------------------------------------------------------
//  void LogMessage(const std::string &msg)
//------------------------------------------------------------------------------
/**
 * Sends a variable argument message to the MessageReceiver for logging.
 * 
 * @param msg The message, possibly including markers for variable argument 
 *            substitution.
 * @param ... The optional list of parameters that are inserted into the msg 
 *            string.
 */
//------------------------------------------------------------------------------
void MessageInterface::LogMessage(const char *msg, ...)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->LogMessage(
            "Hey, you still need to hook up LogMessage in MessageInterface!!!");
}

//------------------------------------------------------------------------------
//  void ClearMessage()
//------------------------------------------------------------------------------
/**
 * Tells the MessageReceiver to clear the message window.
 */  
//------------------------------------------------------------------------------
void MessageInterface::ClearMessage()
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->ClearMessage();
}
