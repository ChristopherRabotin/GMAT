//$Id$
//------------------------------------------------------------------------------
//                              GregorianDate     
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus
// Created: 2004/05/19
//
/**
 * Definition of the GregorianDate class base
 */
//------------------------------------------------------------------------------

#ifndef GregorianDate_hpp
#define GregorianDate_hpp

#include <iostream>
#include <sstream>
#include "gmatdefs.hpp"
#include "Date.hpp"
#include "DateUtil.hpp"
#include "StringTokenizer.hpp"
#include "MessageInterface.hpp"

class GMAT_API GregorianDate
{
public:
   // Implement exception
   class GregorianDateException : public BaseException
   { 
      public: 
         GregorianDateException(const std::string& message =
          "GregorianDateException:  Invalid date format")
         : BaseException(message) {};
   };
   
   GregorianDate();
   GregorianDate(const std::string &str);
   GregorianDate(Date *newDate, Integer format = 1);
   ~GregorianDate();
   
   std::string      GetDate() const;
   bool             SetDate(const std::string &str);
   bool             SetDate(Date *newDate, Integer format = 1);
   
   std::string      GetType() const;
   bool             SetType(const std::string &str);
   
   std::string      GetYMDHMS() const;
   
   bool             IsValid() const;
   static bool      IsValid(const std::string &greg);
   
private:
   // function method
   void           Initialize(const std::string &str);
   void           ParseOut(const std::string &str);
   
   std::string    NumToString(const Integer num);
   std::string    NumToString(const Real num);
   Integer        ToInteger(const std::string &str);
   Real           ToReal(const std::string &str);
   
   std::string    GetMonthName(const Integer month);
   
   // data method
   std::string      stringDate;
   std::string      stringYMDHMS;
   std::string      type;
   Integer          outFormat;
   bool             isValid;
   bool             initialized;
};

#endif // GregorianDate_hpp
