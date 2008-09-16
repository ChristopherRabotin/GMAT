//$Header$
//------------------------------------------------------------------------------
//                                SpaceObjectException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2004/07/26
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Exception class used by the SpaceObject hierarchy.
 */
//------------------------------------------------------------------------------


#ifndef SpaceObjectException_hpp
#define SpaceObjectException_hpp

#include "BaseException.hpp"


class GMAT_API SpaceObjectException : public BaseException
{
public:
   //---------------------------------------------------------------------------
   // SpaceObjectException(const std::string &details)
   //---------------------------------------------------------------------------
   /**
    * Default constructor.
    *
    * @param <details> Message explaining why the exception was thrown.
    */
   //---------------------------------------------------------------------------
   SpaceObjectException(const std::string &details = "") :
      BaseException("SpaceObject Exception Thrown: ", details)
   {
   }

   //---------------------------------------------------------------------------
   // ~SpaceObjectException()
   //---------------------------------------------------------------------------
   /**
    * Destructor.
    */
   //---------------------------------------------------------------------------
   ~SpaceObjectException()
   {
   }

   //---------------------------------------------------------------------------
   // SpaceObjectException(const SpaceObjectException &soe)
   //---------------------------------------------------------------------------
   /**
    * Copy constructor.
    *
    * @param <soe> Exception used to generate this one.
    */
   //---------------------------------------------------------------------------
   SpaceObjectException(const SpaceObjectException &soe) :
      BaseException(soe)
   {
   }
};

#endif // SpaceObjectException_hpp
