
#ifndef GmatPluginFunctions_hpp
#define GmatPluginFunctions_hpp

#include "pythoninterface_defs.hpp"
#include "Factory.hpp"

class MessageReceiver;

extern "C"
{
   Integer    PYTHON_API GetFactoryCount();
   Factory    PYTHON_API *GetFactoryPointer(Integer index);
   void       PYTHON_API SetMessageReceiver(MessageReceiver* mr);
};


#endif /*GmatPluginFunctions_hpp*/
