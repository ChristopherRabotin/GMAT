//$Id$
//------------------------------------------------------------------------------
//                             MessageInterface
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include <cstdlib>               // for malloc() and free() - Required for GCC 4.3
#include <stdio.h>               // for vsprintf()

//---------------------------------
//  static data
//---------------------------------
MessageReceiver* MessageInterface::theMessageReceiver = NULL;
const int MessageInterface::MAX_MESSAGE_LENGTH = 20000;


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


//------------------------------------------------------------------------------
// bool SetMessageReceiver(MessageReceiver *mr)
//------------------------------------------------------------------------------
bool MessageInterface::SetMessageReceiver(MessageReceiver *mr)
{
   theMessageReceiver = mr;
   return true;
}


//------------------------------------------------------------------------------
// MessageReceiver* GetMessageReceiver()
//------------------------------------------------------------------------------
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
   ShowMessage("%s", msgString.c_str());
}


//------------------------------------------------------------------------------
//  void ShowMessage(const char *format, ...)
//------------------------------------------------------------------------------
/**
 * Passes a variable argument delimited message to the MessageReceiver.
 *
 * @param format The format, possibly including markers for variable argument
 *                  substitution.
 * @param ...    The optional list of parameters that are inserted into the format
 *                  string.
 */
//------------------------------------------------------------------------------
void MessageInterface::ShowMessage(const char *format, ...)
{
   if (theMessageReceiver != NULL)
   {
      short    ret;
      short    size;
      va_list  marker;
      char     *msgBuffer = NULL;
      std::string msgStr("*** WARNING *** Cannot allocate enough memory to show the message.\n");
      
      // format is vsprintf format
      // actual max message length is MAX_MESSAGE_LENGTH
      size = strlen(format) + MAX_MESSAGE_LENGTH;
      //LogMessage("strlen(format)=%d, size=%d\n", strlen(format), size);
      
      if( (msgBuffer = (char *)malloc(size)) != NULL )
      {
         for (int i=0; i<size; i++)
            msgBuffer[i] = '\0';
         va_start(marker, format);
         ret = vsprintf(msgBuffer, format, marker);
         if (ret < 0)
            theMessageReceiver->ShowMessage("Unable to complete messaging");
         else
         {
            va_end(marker);
            theMessageReceiver->ShowMessage(std::string(msgBuffer));
         }
      }
      else
      {
         theMessageReceiver->ShowMessage(msgStr);
//         msgBuffer = "*** WARNING *** Cannot allocate enough memory to show the message.\n";
      }

//      theMessageReceiver->ShowMessage(std::string(msgBuffer));
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
 *                   {ERROR_, WARNING_, INFO_} enumerated in the Gmat namespace.
 * @param msg     The message.
 */
//------------------------------------------------------------------------------
void MessageInterface::PopupMessage(Gmat::MessageType msgType, const std::string &msg)
{
   PopupMessage(msgType, "%s", msg.c_str());
}


//------------------------------------------------------------------------------
//  static void PopupMessage(Gmat::MessageType msgType, const char *format, ...)
//------------------------------------------------------------------------------
/**
 * Passes a variable argument delimited popup message to the MessageReceiver.
 *
 * @param msgType The type of message that is displayed, selected from the set
 *                   {ERROR_, WARNING_, INFO_} enumerated in the Gmat namespace.
 * @param format  The format, possibly including markers for variable argument
 *                    substitution.
 * @param ...     The optional list of parameters that are inserted into the format
 *                   string.
 */
//------------------------------------------------------------------------------
void MessageInterface::PopupMessage(Gmat::MessageType msgType, const char *format,
      ...)
{
   if (theMessageReceiver != NULL)
   {
      short    ret;
      short    size;
      va_list  marker;
      char     *msgBuffer = NULL;
      std::string msgStr("*** WARNING *** Cannot allocate enough memory to show the message.\n");

      // format is vsprintf format
      // actual max message length is MAX_MESSAGE_LENGTH
      size = strlen(format) + MAX_MESSAGE_LENGTH;
      
      if ( (msgBuffer = (char *)malloc(size)) != NULL )
      {
         for (int i=0; i<size; i++)
            msgBuffer[i] = '\0';
         va_start(marker, format);
         ret = vsprintf(msgBuffer, format, marker);
         if (ret < 0)
            theMessageReceiver->PopupMessage(msgType,
                  "Unable to complete messaging");
         else
         {
            va_end(marker);

            // if no EOL then append it
            if (msgBuffer[strlen(msgBuffer)-1] != '\n')
               msgBuffer[strlen(msgBuffer)] = '\n';
            theMessageReceiver->PopupMessage(msgType, std::string(msgBuffer));
         }
      }
      else
      {
         theMessageReceiver->PopupMessage(msgType, msgStr);
//         msgBuffer = "*** WARNING *** Cannot allocate enough memory to show the message.\n";
      }
      
//      theMessageReceiver->PopupMessage(msgType, std::string(msgBuffer));
      
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
   {
      short    ret;
      short    size;
      va_list  marker;
      char     *msgBuffer = NULL;
      std::string msgStr("*** WARNING *** Cannot allocate enough memory to show the message.\n");
      
      // msg is vsprintf format
      // actual max message length is MAX_MESSAGE_LENGTH
      size = strlen(msg) + MAX_MESSAGE_LENGTH;
      //LogMessage("strlen(msg)=%d, size=%d\n", strlen(msg), size);
      
      if( (msgBuffer = (char *)malloc(size)) != NULL )
      {
         for (int i=0; i<size; i++)
            msgBuffer[i] = '\0';
         va_start(marker, msg);
         ret = vsprintf(msgBuffer, msg, marker);
         if (ret < 0) // vsprintf failed
            theMessageReceiver->LogMessage("Unable to complete messaging\n");
         else
         {
            va_end(marker);
            theMessageReceiver->LogMessage(std::string(msgBuffer));
         }
      }
      else
      {
         theMessageReceiver->LogMessage(msgStr);
//         msgBuffer = "*** WARNING *** Cannot allocate enough memory to show "
//            "the message.\n";
      }
      
//      theMessageReceiver->LogMessage(std::string(msgBuffer));
      free(msgBuffer);
   }
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

//------------------------------------------------------------------------------
// std::string GetQueuedMessage()
//------------------------------------------------------------------------------
/**
 * Tells the MessageReceiver to retrieve all message from the queue.
 */
//------------------------------------------------------------------------------
std::string MessageInterface::GetQueuedMessage()
{
   if (theMessageReceiver != NULL)
      return theMessageReceiver->GetMessage();
   else
      return "";
}

//------------------------------------------------------------------------------
// void PutMessage(const std::string &msg)
//------------------------------------------------------------------------------
/**
 * Tells the MessageReceiver to push the message into queue
 */
//------------------------------------------------------------------------------
void MessageInterface::PutMessage(const std::string &msg)
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->PutMessage(msg);
}

//------------------------------------------------------------------------------
// void ClearMessageQueue()
//------------------------------------------------------------------------------
/**
 * Tells the MessageReceiver to clear the message queue.
 */
//------------------------------------------------------------------------------
void MessageInterface::ClearMessageQueue()
{
   if (theMessageReceiver != NULL)
      theMessageReceiver->ClearMessageQueue();
}

