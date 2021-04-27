//$Id$
//------------------------------------------------------------------------------
//                            TdmErrorHandler
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Farideh Farahnak
// Created: 2014/7/22
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS.
//
/**
 * ErrorHandler class used by the TdmReadWriter class.
 */
//------------------------------------------------------------------------------

#ifndef TdmErrorHandler_hpp
#define TdmErrorHandler_hpp

#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/sax/ErrorHandler.hpp"
#include "xercesc/sax/SAXParseException.hpp"
#include "xercesc/util/XMLString.hpp"
#include <string>

#include "estimation_defs.hpp"

using namespace xercesc;

XERCES_CPP_NAMESPACE_USE

/**
* Class that implements the XML Error Handler
*
* This class implements the basic interface ErrorHandler
* functions to handle and report any SAX parsing errors. 
* A SAX parser must register an instance of this class
* by using the parser's setErrorHandler method.
*/
class  ESTIMATION_API TdmErrorHandler : public ErrorHandler
{
public:
   TdmErrorHandler();
   ~TdmErrorHandler();
   TdmErrorHandler(const TdmErrorHandler &teh);

   /// override pure virtual ErrorHandler base class member functions
   virtual void fatalError(const SAXParseException &ex);
   virtual void error(const SAXParseException &ex);
   virtual void warning(const SAXParseException &ex);
   virtual void resetErrors();

private:
   /// Called upon error occurrences during the parsing
   void reportParseException(const SAXParseException &ex);
   /// Error types
   XMLErrorReporter::ErrTypes errType;
   /// Error name, mapped to the error type
   std::string errName;
};

#endif   //TdmErrorHandler_hpp