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
 * Declares operation of MessageWindow class.
 */
//------------------------------------------------------------------------------
#ifndef MessageWindow_hpp
#define MessageWindow_hpp

#include "Subscriber.hpp"
#include <sstream>

class MessageWindow : public Subscriber
{
public:
   MessageWindow(const std::string &name);
   MessageWindow(const MessageWindow &mw);
   virtual ~MessageWindow(void);
   
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                     GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   
   virtual Integer     GetIntegerParameter(const Integer id) const;
   virtual Integer     SetIntegerParameter(const Integer id,
                                          const Integer value);

protected:
   /// Precision for output of real data
   Integer precision;
   
   enum
   {
      PRECISION = SubscriberParamCount,
      MessageWindowParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[MessageWindowParamCount - SubscriberParamCount];
   static const std::string
      PARAMETER_TEXT[MessageWindowParamCount - SubscriberParamCount];
   
   virtual bool Distribute(Integer len);
   virtual bool Distribute(const Real * dat, Integer len);
   
   std::stringstream dstream;
};

#endif
