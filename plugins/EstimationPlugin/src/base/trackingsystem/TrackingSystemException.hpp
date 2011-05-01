//$Id: TrackingSystemException.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         TrackingSystemException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2004/07/26
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
/**
 * Exception class used by the TrackingSystem hierarchy.
 */
//------------------------------------------------------------------------------


#ifndef TrackingSystemException_hpp
#define TrackingSystemException_hpp

#include "estimation_defs.hpp"
#include "BaseException.hpp"


class ESTIMATION_API TrackingSystemException : public BaseException
{
public:
   //---------------------------------------------------------------------------
   // TrackingSystemException(const std::string &details)
   //---------------------------------------------------------------------------
   /**
    * Default constructor.
    *
    * @param <details> Message explaining why the exception was thrown.
    */
   //---------------------------------------------------------------------------
   TrackingSystemException(const std::string &details = "") :
      BaseException("Tracking System Exception Thrown: ", details)
   {
   }

   //---------------------------------------------------------------------------
   // ~TrackingSystemException()
   //---------------------------------------------------------------------------
   /**
    * Destructor.
    */
   //---------------------------------------------------------------------------
   ~TrackingSystemException()
   {
   }

   //---------------------------------------------------------------------------
   // TrackingSystemException(const TrackingSystemException &soe)
   //---------------------------------------------------------------------------
   /**
    * Copy constructor.
    *
    * @param <soe> Exception used to generate this one.
    */
   //---------------------------------------------------------------------------
   TrackingSystemException(const TrackingSystemException &soe) :
      BaseException(soe)
   {
   }
};

#endif // TrackingSystemException_hpp
