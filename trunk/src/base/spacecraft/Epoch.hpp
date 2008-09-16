//$Header$
//------------------------------------------------------------------------------
//                               Epoch
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus
// Created: 2005/06/20
//
/**
 * Definition of the Epoch class
 */
//------------------------------------------------------------------------------

#ifndef Epoch_hpp
#define Epoch_hpp

#include "gmatdefs.hpp"
#include "TimeConverter.hpp"

class GMAT_API Epoch
{
public:
    // Implements exception
    class EpochException : public BaseException
    {
       public:
          EpochException(const std::string &message =
           "EpochException: Can't convert due to invalid date format")
           : BaseException(message) {};
    };


   // Default constructor
   Epoch();
   Epoch(const std::string &mFormat);
   Epoch(const std::string &mFormat, const std::string &mValue);

   // Copy constructor
   Epoch(const Epoch &e);
   // Assignment operator
   Epoch& operator=(const Epoch &e);

   // Destructor
   virtual ~Epoch(void);

   // public methods
   std::string GetValue() const;
   std::string GetValue(const std::string &mFormat) const;
   bool        SetValue(const std::string &mFormat);
   bool        SetValue(const std::string &mFormat, const std::string &mValue);
   bool        SetValue(const Real e);

   Real        GetRealValue();
   Real        GetRealValue(const std::string &mFormat);
  
   bool        UpdateValue(Real e);

   std::string GetFormat() const;
   bool        SetFormat(const std::string &mFormat);

   bool        IsValidFormat(const std::string &label) const;

   std::string GetLabel() const;

protected:
   // Epoch's date format list
   enum DateFormat
   {
        TAI_MJD, TAI_GREGORIAN, UTC_MJD, UTC_GREGORIAN, DateFormatCount
   };
          
   static const std::string FORMAT[DateFormatCount];
   
private:
   std::string             format;
   std::string             value;
   mutable TimeConverter   timeConverter;

   // private methods
   void        DefineDefault();
   std::string GetFormatTrim(const std::string &mFormat) const;
};

#endif // Epoch_hpp
