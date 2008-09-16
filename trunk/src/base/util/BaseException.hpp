//$Header$
//------------------------------------------------------------------------------
//                              BaseException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/09
//
/**
 * This class provides base exception class, from which all GMAT exceptions must 
 * be derived.
 */
//------------------------------------------------------------------------------
#ifndef BaseException_hpp
#define BaseException_hpp

#include <string>

class BaseException
{
public:
   virtual std::string GetFullMessage() const 
      {return theMessage + theDetails;};
   virtual std::string GetDetails() const 
      {return theDetails;};
   virtual void SetMessage(const std::string &message)  
      {theMessage = message;};
   virtual void SetDetails(const std::string &details)  
      {theDetails = details;};
   const BaseException& operator=(const std::string &newMessage) 
      {theMessage = newMessage;  return *this;};
   
   virtual void SetDetails(const char *details, ...);
   
   static const int MAX_MESSAGE_LENGTH = 3000;
   
protected:
   BaseException(const std::string& message = "", const std::string &details = "") 
      {theMessage = message; theDetails = details;};
   BaseException(const BaseException& baseException) 
      {theMessage = baseException.theMessage; theDetails = baseException.theDetails;};
   virtual ~BaseException() 
      {};
   const BaseException& operator=(const BaseException& baseException) 
      {theMessage = baseException.theMessage; theDetails = baseException.theDetails;
      return *this;};
   
private:
   std::string theMessage;
   std::string theDetails;
};
#endif // BaseException_hpp

