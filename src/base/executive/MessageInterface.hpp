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
    static void NoteMessage(const std::string &msg);
    static void PopupMessage(Gmat::MessageType msgType, const std::string &msg);
    static void PopupAbortContinue(const std::string &abortMsg,
                                   const std::string &continueMsg,
                                   const std::string &msg);
    static void ShowMessage(Gmat::MessageType msgType, int interval,
                            const std::string &msg);
    static void SetLog(bool flag)
        { logFlag = flag; };
    
private:
    static std::queue<std::string> messageQueue;
    static std::string popupMessage;
    static std::string abortMessage;
    static std::string continueMessage;
    static Gmat::MessageType messageType;
    static int showIntervalInMilSec;
    static short messageExist;
    static bool logFlag;
    
    MessageInterface();
   ~MessageInterface();

};

#endif
