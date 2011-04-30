//$Id$
//------------------------------------------------------------------------------
//                               Epoch
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
// Created: 2005/06/02
//
/**
 * Implements the Epoch class
 */
//------------------------------------------------------------------------------

#include <sstream>
#include "Epoch.hpp"
#include "StringTokenizer.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"
#include "TimeTypes.hpp"

// #define DEBUG_EPOCH 1 

// Epoch constant variables for the lists of state types and elements 
const std::string Epoch::FORMAT[DateFormatCount] =
      {
          "TAIModJulian", "TAIGregorian", "UTCModJulian", "UTCGregorian"
      };

//-------------------------------------
// public methods
//-------------------------------------

//---------------------------------------------------------------------------
//  Epoch()
//---------------------------------------------------------------------------
/**
 * Creates default constructor.
 *
 */
Epoch::Epoch()
{
   DefineDefault();
}

//---------------------------------------------------------------------------
//  Epoch(const std::string &mFormat)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <mFormat> Format date 
 *
 */
Epoch::Epoch(const std::string &mFormat)
{
   DefineDefault();
    
   // Check if invalid then use default
   if (!SetValue(mFormat))
      MessageInterface::ShowMessage("\n****Warning: Invalid date format***"
                      "\nUse default date format.\n");  
}

//---------------------------------------------------------------------------
//  Epoch(const std::string &mFormat, const std::string &mValue)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <mType> state's type 
 * @param <stateVector> state's values
 *
 */
Epoch::Epoch(const std::string &mFormat, const std::string &mValue)
{
   DefineDefault();

   // Check for invalid format date
   if (!SetValue(mFormat, mValue))
      MessageInterface::ShowMessage("\n****Warning: Invalid date format ***"
                      "\nUse default date format.\n");  
}

//---------------------------------------------------------------------------
//  Epoch(const Epoch &e)
//---------------------------------------------------------------------------
/**
 * Copy Constructor for base Epoch structures.
 *
 * @param <e> The original that is being copied.
 */
Epoch::Epoch(const Epoch &e) :
   format        (e.format),
   value         (e.value),
   timeConverter (e.timeConverter)
{
}

//---------------------------------------------------------------------------
//  ~Epoch(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
Epoch::~Epoch(void)
{
}

//---------------------------------------------------------------------------
//  Epoch& operator=(const Epoch &e)
//---------------------------------------------------------------------------
/**
 * Assignment operator for Epoch structures.
 *
 * @param <e> The original that is being copied.
 *
 * @return Reference to this object
 * 
 */
Epoch& Epoch::operator=(const Epoch &e)
{
    if (&e == this)
        return *this;

    // Duplicate the member data        
    format = e.format;
    value = e.value;
    timeConverter = e.timeConverter;

    return *this;
}

//---------------------------------------------------------------------------
//  std::string GetValue() const
//---------------------------------------------------------------------------
/**
 * Retrieve the value.
 *
 * @return the epoch's value.
 */
std::string Epoch::GetValue() const
{
   return value;
}

//---------------------------------------------------------------------------
//  std::string GetValue(const std::string &mFormat) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value with the specific state type.
 *
 * @param <mFormat>  Epoch's date format 
 *
 * @return the epoch's value.
 */
std::string Epoch::GetValue(const std::string &mFormat) const
{
#if DEBUG_EPOCH
   MessageInterface::ShowMessage("\nEpoch::GetValue(\"%s\") format = %s "
                   "and value = %s\n",
                   mFormat.c_str(),format.c_str(), value.c_str());
#endif

   if (mFormat == "Epoch")
      return GetFormat();

   if (!IsValidFormat(mFormat))
   {
      throw EpochException(
            "Epoch::GetValue() -> failure due to invalid date format"); 
   }   

   std::string tempFormat = GetFormatTrim(mFormat);

   if (GetFormat() == tempFormat)
   {
#if DEBUG_EPOCH
      MessageInterface::ShowMessage("\nEpoch::GetValue() doesn't needs "
                       "to call TimeConverter for conversion.\n");
#endif
      return value;
   }

   try
   {
      return timeConverter.Convert(value,format,tempFormat);
   }
   catch(TimeConverter::TimeConverterException &tce)
   {
      std::string msg = "\nEpoch::GetValue() -> Get error from " +
                         tce.GetFullMessage() + "\n";
      throw EpochException(msg);
   }
}

//---------------------------------------------------------------------------
//  Real GetRealValue()
//---------------------------------------------------------------------------
/**
 * Retrieve the value in real data type for TAI Modified Julian as default.
 *
 * @return the epoch's value in TAI Modified Julian.
 */
Real Epoch::GetRealValue()
{
   return GetRealValue("TAIModJulian");
}

//---------------------------------------------------------------------------
//  Real GetRealValue(const std::string &mFormat)
//---------------------------------------------------------------------------
/**
 * Retrieve the value in real data type.
 *
 * @return the epoch's value.
 */
Real Epoch::GetRealValue(const std::string &mFormat)
{
#if DEBUG_EPOCH
    MessageInterface::ShowMessage("\nEpoch::GetRealValue(\"%s\")\n",
                                  mFormat.c_str());
#endif
   std::string tempValue = GetValue(mFormat);

   std::string tempFormat = GetFormatTrim(mFormat);
       
   // Check for right format for converting into Real epoch
   if (tempFormat != FORMAT[TAI_MJD] && tempFormat != FORMAT[UTC_MJD])
   {
      throw EpochException(
            "\nEpoch::GetRealValue() -> Can't convert to Real Epoch.  "
            "Use TAIModJulian or UTCModJulian only.\n");
   }

   return (Real)atof(tempValue.c_str());
}

//---------------------------------------------------------------------------
//  bool UpdateValue(const Real mValue)
//---------------------------------------------------------------------------
/**
 * Update the value.
 *
 * @param <mValue>  Epoch's value in TAI Modified Julian
 *
 * @return true when successful; otherwise, false.
 */
bool Epoch::UpdateValue(const Real mValue) 
{
#if DEBUG_EPOCH
   MessageInterface::ShowMessage("\nEpoch::UpdateValue(%f)"
                    "\nvalue = %s and format = %s", 
                    mValue, value.c_str(), format.c_str());
#endif

   std::ostringstream valueBuffer;
   valueBuffer.precision(11);
   valueBuffer.setf(std::ios::fixed);
   valueBuffer << mValue;

   try
   {
      std::string newValue = timeConverter.Convert(valueBuffer.str(),
                                                   FORMAT[TAI_MJD], 
                                                   format);
      value = newValue;
   }
   catch (TimeConverter::TimeConverterException &) 
   {
      return false;
   }

   return true;
}

//---------------------------------------------------------------------------
//  bool SetValue(const std::string &mFormat)
//---------------------------------------------------------------------------
/**
 * Set the value with the specific state type.
 *
 * @param <mFormat>  Epoch's date format
 *
 * @return true when successful; otherwise, false.
 */
bool Epoch::SetValue(const std::string &mFormat) 
{
   if (!IsValidFormat(mFormat))
      return false; 
 
   std::string tempFormat = GetFormatTrim(mFormat); 

   if (mFormat != format)
   {
       try
       {
          value = timeConverter.Convert(value,format,mFormat);
          format = mFormat;
       }
       catch(UtilityException &)
       {
          return false; 
       }
   }
   return true;
}

//---------------------------------------------------------------------------
//  bool SetValue(const std::string &mFormat, const std::string &mValue)
//---------------------------------------------------------------------------
/**
 * Set the value with the specific date format.
 *
 * @param <mFormat>  Epoch's date format
 * @param <mValue>   Epoch's value or Epoch's date format if mFormat is "Epoch"
 *
 * @return true when successful; otherwise, false.
 */
bool Epoch::SetValue(const std::string &mFormat, const std::string &mValue) 
{
#if DEBUG_EPOCH
    MessageInterface::ShowMessage("\nEpoch::SetValue(\"%s\",\"%s\")\n",
                    mFormat.c_str(),mValue.c_str());
#endif

   // Check if it is "Epoch" with valid date format then do conversion
   if (mFormat == "Epoch")
   {
      if  (IsValidFormat(mValue))
      {
          // @todo: need to work on conversion and set format and value
          if (SetFormat(mValue))
             return true; 
      }
      else
      {
          format = FORMAT[TAI_MJD];
          value = mValue;    // @todo: need to check validity for real number
          return true;
      }
   }

   if (!IsValidFormat(mFormat))
      return false;

   format = GetFormatTrim(mFormat);
   value = mValue;     // @todo:  Need to check validity based on date format

#if DEBUG_EPOCH
    MessageInterface::ShowMessage("\nEpoch::SetValue(\"%s\",\"%s\") exits with "
                    "format: %s and value: %s\n",mFormat.c_str(),
                    mValue.c_str(),format.c_str(),value.c_str());
#endif

   return true;
}

//---------------------------------------------------------------------------
//  bool SetValue(const Real mValue)
//---------------------------------------------------------------------------
/**
 * Set the value in TAI Modified Julian.
 *
 * @param <mFormat>  Epoch's value. 
 *
 * @return true when successful; otherwise, false.
 */
bool Epoch::SetValue(const Real mValue)
{
   // @todo:  need to work on it..

   std::ostringstream valueBuffer;
   valueBuffer.precision(9);
   valueBuffer.setf(std::ios::fixed);
   valueBuffer << mValue;

   value = valueBuffer.str();
   format = FORMAT[TAI_MJD]; 

   // @todo: will work on it below
   if (IsValidFormat(format))
      return false;

   return true;
}


//---------------------------------------------------------------------------
//  std::string GetFormat() const
//---------------------------------------------------------------------------
/**
 * Get the epoch's date format.
 *
 * @return the epoch's date format. 
 */
std::string Epoch::GetFormat() const
{
   return format;
}

//---------------------------------------------------------------------------
//  bool SetFormat(const std::string &mFormat) 
//---------------------------------------------------------------------------
/**
 * Set the epoch's date format.
 *
 * @param <mFormat> Given epoch's date format. 
 *
 * @return true if successful; otherwise, false.
 */
bool Epoch::SetFormat(const std::string &mFormat)
{
   if (!SetValue(mFormat))
      return false; 
  
   return true; 
}

//---------------------------------------------------------------------------
//  bool IsValidFormat(const std::string &mFormat) const
//---------------------------------------------------------------------------
/**
 * Check validity on the given date format. 
 *
 * @param <mFormat> The date format.
 *
 * @return true if valid; otherwise, false.
 */
bool Epoch::IsValidFormat(const std::string &mFormat) const
{
    std::string tempFormat = mFormat;

    StringTokenizer token(mFormat,".");
    Integer count = token.CountTokens();

    // Check if too many tokens then invalid
    if (count > 2) 
       return false;

    if (count == 2 && token.GetToken(0) == "Epoch")
        tempFormat = token.GetToken(1);

#if DEBUG_EPOCH
    MessageInterface::ShowMessage("\nEpoch::IsValidFormat(%s)...\n"
                     "count = %d, size: %d, tempFormat = %s\n",
                     mFormat.c_str(),count, mFormat.size(),tempFormat.c_str());
#endif

   if (tempFormat == "Epoch")
      return true;

   for (UnsignedInt i=0; i < DateFormatCount; ++i)
   {
       if (FORMAT[i] == tempFormat)
          return true;
   }


   return false;
}

//---------------------------------------------------------------------------
//  std::string GetLabel() const
//---------------------------------------------------------------------------
/**
 * Get the label for the parameter text.
 *
 * @return label of the parameter text.
 */
std::string Epoch::GetLabel() const
{
   return "Epoch." + GetFormat();
}

//-------------------------------------
// private methods
//-------------------------------------

//------------------------------------------------------------------------------
// void DefineDefault()
//------------------------------------------------------------------------------
/**
 * Initialize data method as default. 
 *
 */
void Epoch::DefineDefault()
{
   format = FORMAT[TAI_MJD];
   std::stringstream epochString("");
   epochString << GmatTimeConstants::MJD_OF_J2000;
   value = epochString.str();
}

//---------------------------------------------------------------------------
//  std::string GetFormatTrim(const std::string &mFormat) const
//---------------------------------------------------------------------------
/**
 * Trim out the text from the string with '.' as subparameters
 *
 * @param <s> The original that is being copied.
 *
 * @return same format if it has been parsed out.
 */
std::string Epoch::GetFormatTrim(const std::string &mFormat) const
{
   std::string tempFormat = mFormat;

   StringTokenizer token(mFormat,".");

   if (token.CountTokens() == 2)
      tempFormat = token.GetToken(1);
   else if (mFormat == "Epoch")
      tempFormat = FORMAT[TAI_MJD]; 

   return tempFormat;
}
