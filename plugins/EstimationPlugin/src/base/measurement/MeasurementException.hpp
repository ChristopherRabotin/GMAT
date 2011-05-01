//$Id: MeasurementException.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         MeasurementException
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
 * Exception class used by the Measurement hierarchy.
 */
//------------------------------------------------------------------------------


#ifndef MeasurementException_hpp
#define MeasurementException_hpp

#include "estimation_defs.hpp"
#include "BaseException.hpp"


class ESTIMATION_API MeasurementException : public BaseException
{
public:
   //---------------------------------------------------------------------------
   // MeasurementException(const std::string &details)
   //---------------------------------------------------------------------------
   /**
    * Default constructor.
    *
    * @param <details> Message explaining why the exception was thrown.
    */
   //---------------------------------------------------------------------------
   MeasurementException(const std::string &details = "") :
      BaseException("Measurement Exception Thrown: ", details)
   {
   }

   //---------------------------------------------------------------------------
   // ~MeasurementException()
   //---------------------------------------------------------------------------
   /**
    * Destructor.
    */
   //---------------------------------------------------------------------------
   ~MeasurementException()
   {
   }

   //---------------------------------------------------------------------------
   // MeasurementException(const MeasurementException &soe)
   //---------------------------------------------------------------------------
   /**
    * Copy constructor.
    *
    * @param <soe> Exception used to generate this one.
    */
   //---------------------------------------------------------------------------
   MeasurementException(const MeasurementException &soe) :
      BaseException(soe)
   {
   }
};

#endif // MeasurementException_hpp
