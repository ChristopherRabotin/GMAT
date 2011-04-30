//$Id$
//------------------------------------------------------------------------------
//                            SolverException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/1/4
//
/**
 * Definition for exceptions thrown by the Solver subsystem. 
 */
//------------------------------------------------------------------------------


#ifndef SolverException_hpp
#define SolverException_hpp

#include "BaseException.hpp"
#include "gmatdefs.hpp"          // For GMAT_API


class GMAT_API SolverException : public BaseException
{
public:
   //---------------------------------------------------------------------------
   // SolverException(const std::string &details, const std::string &message)
   //---------------------------------------------------------------------------
   /**
    * Default constructor.
    *
    * @param <details> Message explaining why the exception was thrown.
    * @param <message> Initial part of the exception message.
    */
   //---------------------------------------------------------------------------
   SolverException(const std::string &details,
                   const std::string &message = "Solver subsystem exception: "):
      BaseException       (message, details)
   {
   }
   
   //---------------------------------------------------------------------------
   // ~SolverException()
   //---------------------------------------------------------------------------
   /**
    * Destructor.
    */
   //---------------------------------------------------------------------------
   virtual ~SolverException()
   {
   }
};


#endif // SolverException_hpp
