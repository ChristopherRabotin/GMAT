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
 * Defines operations on messages.
 */
//------------------------------------------------------------------------------
#if !defined __CONSOLE_APP__
#include "gmatwxdefs.hpp"
#endif

#include <iostream>                // for cout, endl
#include <queue>                   // for queue
#include "MessageInterface.hpp"    // for MessageInterface functions

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
FILE* MessageInterface::logFile = NULL;
bool MessageInterface::logFlag = false;

//---------------------------------
//  public functions
//---------------------------------

//------------------------------------------------------------------------------
//  MessageInterface()
//------------------------------------------------------------------------------
MessageInterface::MessageInterface()
{
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
//  Purpose:
//     Pops one message from message queue and concatenates.
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
//  void NoteMessage(const std::string &msg)
//------------------------------------------------------------------------------
//  Purpose:
//     Pushes message into message queue and set the output message event.
//------------------------------------------------------------------------------
void MessageInterface::NoteMessage(const std::string &msg)
{

   MessageInterface::messageQueue.push(msg);
   
#if !defined __CONSOLE_APP__
   wxLogError(wxT(wxString(msg.c_str())));
   wxLog::FlushActive();
#endif

} // end NoteMessage()


//------------------------------------------------------------------------------
//  void PopupAbortContinue(const std::string abortMsg, ...)
//------------------------------------------------------------------------------
//  Purpose:
//     Pop up Abort or Continue message box
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
//  Purpose:
//     Shows popup message
//------------------------------------------------------------------------------
void MessageInterface::PopupMessage(Gmat::MessageType msgType, const std::string &msg)
{
   MessageInterface::popupMessage = msg;
   MessageInterface::messageType = msgType;

#if !defined __CONSOLE_APP__
   switch (msgType)
   {
   case Gmat::ERROR_:
       wxLogError(wxT(wxString(msg.c_str())));
       wxLog::FlushActive();
       break;
   case Gmat::WARNING_:
       wxLogWarning(wxT(wxString(msg.c_str())));
       wxLog::FlushActive();
       break;
   case Gmat::INFO_:
       (void)wxMessageBox(wxT(wxString(msg.c_str())),
                          wxT("Information"));
       break;
       //loj: there should be more
   default:
       break;
   };
#else
   LogMessage(msg);
#endif
   
} // end PopupMessage()


//------------------------------------------------------------------------------
//  static void ShowMessage(Gmat::MessageType msgType, int interval, ...)
//------------------------------------------------------------------------------
//  Purpose:
//     Shows popup message and closes itself.
//------------------------------------------------------------------------------
void MessageInterface::ShowMessage(Gmat::MessageType msgType, int interval,
                                   const std::string &msg)
{
   MessageInterface::popupMessage = msg;
   MessageInterface::messageType = msgType;
   MessageInterface::showIntervalInMilSec = interval;
   
} // end ShowMessage()

//------------------------------------------------------------------------------
// void void LogMessage(const std::string &msg)
//------------------------------------------------------------------------------
void MessageInterface::LogMessage(const std::string &msg)
{
    if (logFile == NULL)
    {
        std::cout << msg << std::endl;
    }
    else
    {
        // work on this
    }
}
