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
 * Exception / Error Handler class used by the TdmReadWriter class.
 */
//------------------------------------------------------------------------------

#include "TdmErrorHandler.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TdmErrorHandler()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
TdmErrorHandler::TdmErrorHandler()
{ 
   errType = XMLErrorReporter::ErrTypes_Unknown;
   errName = " Unknown ";
}


//------------------------------------------------------------------------------
// ~TdmErrorHandler()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TdmErrorHandler::~TdmErrorHandler()
{}


//------------------------------------------------------------------------------
// TdmErrorHandler(const TdmErrorHandler &teh)
//------------------------------------------------------------------------------
/**
 * Copy Constructor
 */
//------------------------------------------------------------------------------
TdmErrorHandler::TdmErrorHandler(const TdmErrorHandler &teh)
{
   errName = teh.errName;
}


//------------------------------------------------------------------------------
// void fatalError(const SAXParseException &ex)
//------------------------------------------------------------------------------
/**
 * Virtual fatalError() implementation.
 *
 * This method receives notification of a non-recoverable fatal error encapsulated 
 * in a SAX parse exception.
 * A validating Parser would use this callback to report the violation of a validity
 * constraint.
 *
 * @param ex SAXParseException object type
 *
 * @return void
 */
//------------------------------------------------------------------------------
void TdmErrorHandler::fatalError(const SAXParseException &ex)
{
   errType = XMLErrorReporter::ErrType_Fatal;
   reportParseException(ex);
}


//------------------------------------------------------------------------------
// void error(const SAXParseException &ex)
//------------------------------------------------------------------------------
/**
 * Virtual error() implementation.
 *
 * This method receives notification of a recoverable error encapsulated 
 * in a SAX parse exception.
 * A validating Parser would use this callback to report the violation of a validity
 * constraint.
 *
 * @param ex SAXParseException object type
 *
 * @return void
 */
//------------------------------------------------------------------------------
void TdmErrorHandler::error(const SAXParseException &ex)
{
   errType = XMLErrorReporter::ErrType_Error;
   reportParseException(ex);
}


//------------------------------------------------------------------------------
// void warning(const SAXParseException &ex)
//------------------------------------------------------------------------------
/**
 * Virtual warning() implementation.
 *
 * This method receives notification of a warning encapsulated 
 * in a SAX parse exception.
 * A validating Parser would use this callback to report the violation of a validity
 * constraint.
 *
 * @param ex SAXParseException object type
 *
 * @return void
 */
//------------------------------------------------------------------------------
void TdmErrorHandler::warning(const SAXParseException &ex)
{
   errType = XMLErrorReporter::ErrType_Warning;
   reportParseException(ex);
}


//------------------------------------------------------------------------------
// void resetErrors()
//------------------------------------------------------------------------------
/**
 * Virtual resetErrors() implementation.
 *
 * This method resets the Error Handler object on its reuse.
 * It helps in reseting the Error Handler object implementation defaults
 * each time the Error Handler is begun.
 *
 * @param none
 *
 * @return void
 */
//------------------------------------------------------------------------------
void TdmErrorHandler::resetErrors()
{}


//------------------------------------------------------------------------------
// Private Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void reportParseException(const SAXParseException &ex)
//------------------------------------------------------------------------------
/**
 * Reports Parse Exception.
 *
 * This method is called upon notification of errors/warnings during the parsing.
 * it throws an object of type MeasurementException with its error message.
 *
 * @param ex SAXParseException
 *
 * @return void
 */
//------------------------------------------------------------------------------
void TdmErrorHandler::reportParseException(const SAXParseException &ex)
{
   switch(errType)
   {
      case 0:
         errName = " Warning ";
         break;
      case 1:
         errName = " Error ";
         break;
	  case 2:
		  errName = " Fatal Error ";
		  break;
	  default:
		  errName = " Unknown ";
		  break;
	}

   char *eMsg = XMLString::transcode(ex.getMessage());

   std::string strErr("TDM exception encountered: ");
   std::string strE = errName + strErr + eMsg;

   // throw a MeasurementException object.
   throw MeasurementException(strE);

   // release
   XMLString::release(&eMsg); 
}


