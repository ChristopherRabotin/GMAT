//$Header$
//------------------------------------------------------------------------------
//                            GmatBaseException
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


#ifndef GmatBaseException_hpp
#define GmatBaseException_hpp

#include "BaseException.hpp"

class GmatBaseException : public BaseException
{
public:

   GmatBaseException(const std::string &details);
   virtual ~GmatBaseException();
   GmatBaseException(const GmatBaseException &gbe);
};

#endif // GmatBaseException_hpp
