//$Header$
//------------------------------------------------------------------------------
//                             GmatBaseException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2004/1/15
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
/**
 * Exception class used by the GmatBase base class.
 */
//------------------------------------------------------------------------------


#include "GmatBaseException.hpp"


GmatBaseException::GmatBaseException(const std::string &details) :
   BaseException       ("GmatBase Exception Thrown: ", details)
{}


GmatBaseException::~GmatBaseException()
{}


GmatBaseException::GmatBaseException(const GmatBaseException &gbe) :
   BaseException       (gbe)
{}
