//$Header$
//------------------------------------------------------------------------------
//                                  MessageWindow
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/11/6
//
/**
 * Defines operation of MessageWindow class.
 */
//------------------------------------------------------------------------------
#include "MessageWindow.hpp"
#include "MessageInterface.hpp" // for ShowMessage()

//---------------------------------
// static data
//---------------------------------
const std::string
MessageWindow::PARAMETER_TEXT[MessageWindowParamCount] =
{
   "Precision"
}; 

const Gmat::ParameterType
MessageWindow::PARAMETER_TYPE[MessageWindowParamCount] =
{
   Gmat::INTEGER_TYPE,
};

//------------------------------------------------------------------------------
// MessageWindow(const std::string &name)
//------------------------------------------------------------------------------
MessageWindow::MessageWindow(const std::string &name)
    : Subscriber      ("MessageWindow", name),
      precision       (12)
{
   // GmatBase data
    parameterCount = MessageWindowParamCount;
}

//------------------------------------------------------------------------------
// ~MessageWindow(void)
//------------------------------------------------------------------------------
MessageWindow::~MessageWindow(void)
{
    dstream.flush();
}

//------------------------------------------------------------------------------
// bool MessageWindow::Distribute(int len)
//------------------------------------------------------------------------------
bool MessageWindow::Distribute(int len)
{
    dstream.precision(precision);
    
    if (len == 0)
        dstream << data;
    else
        for (int i = 0; i < len; ++i)
            dstream << data[i];

    MessageInterface::ShowMessage(dstream.str());
    return true;
}

//------------------------------------------------------------------------------
// bool Distribute(const Real * dat, Integer len)
//------------------------------------------------------------------------------
bool MessageWindow::Distribute(const Real * dat, Integer len)
{       
    dstream.precision(precision);

    if (len == 0)
    {
        return false;
    }
    else
    {
        for (int i = 0; i < len-1; ++i)
            dstream << dat[i] << "  ";
        
        dstream << dat[len-1] << std::endl;
    }
    
    MessageInterface::ShowMessage(dstream.str());
    return true;
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string MessageWindow::GetParameterText(const Integer id) const
{
   switch (id)
   {
   case PRECISION:
      return PARAMETER_TEXT[id];
   default:
      return Subscriber::GetParameterText(id);
   }
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer MessageWindow::GetParameterID(const std::string &str) const
{
   for (int i=0; i<MessageWindowParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i])
          return i;
   }
   
   return Subscriber::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType MessageWindow::GetParameterType(const Integer id) const
{
    switch (id)
    {
    case PRECISION:
      return MessageWindow::PARAMETER_TYPE[id];
    default:
        return Subscriber::GetParameterType(id);
    }
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string MessageWindow::GetParameterTypeString(const Integer id) const
{
   switch (id)
   {
   case PRECISION:
       return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   default:
       return Subscriber::GetParameterTypeString(id);
   }
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer MessageWindow::GetIntegerParameter(const Integer id) const
{
    switch (id)
    {
    case PRECISION:
        return precision;
    default:
        return Subscriber::GetIntegerParameter(id);
    }
}

//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer MessageWindow::SetIntegerParameter(const Integer id, const Integer value)
{
    switch (id)
    {
    case PRECISION:
        if (value > 0)
            precision = value;
        return precision;
    default:
        return Subscriber::GetIntegerParameter(id);
    }
}
