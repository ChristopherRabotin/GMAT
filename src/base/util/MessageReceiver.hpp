#ifndef MESSAGERECEIVER_HPP_
#define MESSAGERECEIVER_HPP_


#include <iostream>                // for cout, endl
#include <fstream>
#include <sstream>
#include <queue>                   // for queue
#include "BaseException.hpp"
#include "GmatGlobal.hpp"          // for RunBachMode()


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


class MessageReceiver
{
public:
   // Derived classes must implement the singleton accessor:
   // static MessageReceiver*  Instance();
   
   virtual void ShowMessage(const std::string &msg) = 0;
   virtual void ShowMessage(const char *msg, ...) = 0;

   virtual void PopupMessage(Gmat::MessageType msgType, const std::string &msg) = 0;
   virtual void PopupMessage(Gmat::MessageType msgType, const char *msg, ...) = 0;
   
   virtual std::string GetLogFileName() = 0;
   virtual void SetLogEnable(bool flag) = 0;
   virtual void SetLogPath(const std::string &pathname, bool append = false) = 0;
   virtual void SetLogFile(const std::string &filename) = 0;

   virtual void LogMessage(const std::string &msg) = 0;
   virtual void LogMessage(const char *msg, ...) = 0;

   virtual void ClearMessage() = 0;

protected:
	MessageReceiver();
	virtual ~MessageReceiver();
};

#endif /*MESSAGERECEIVER_HPP_*/
