//$Header$
//------------------------------------------------------------------------------
//                              CoordinateSystemException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy Shoan
// Created: 2004/12/22
//
/**
 * This class provides an exception class for the CoordinateSystem classes.
 */
//------------------------------------------------------------------------------
#ifndef CoordinateSystemException_hpp
#define CoordinateSystemException_hpp

#include "gmatdefs.hpp"
#include "BaseException.hpp"

class GMAT_API CoordinateSystemException : public BaseException
{
public:

   CoordinateSystemException(std::string details = "");

protected:

private:
};
#endif // CoordinateSystemException_hpp
