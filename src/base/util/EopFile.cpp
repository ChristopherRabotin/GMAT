//$Header$
//------------------------------------------------------------------------------
//                                  EopFile
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2005/01/26
//
/**
 * Implementation of the EopFile class.  This is the code that reads the polar
 * motion information from EOP file.
 *
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "gmatdefs.hpp"
#include "EopFile.hpp"
#include "TimeTypes.hpp"
#include "UtilityException.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  EopFile(const std::string &fileName, 
//          GmatEop::EopFileType eop = GmatEop::EOP_C04);
//---------------------------------------------------------------------------
/**
 * Constructs base EopFile structures used in derived classes
 * (default constructor).
 *
 * @param fileNme  EOP file name.
 */
//---------------------------------------------------------------------------
EopFile::EopFile(const std::string &fileName, GmatEop::EopFileType eop) :
eopFType        (eop),
eopFileName     (fileName),
tableSz         (0),
polarMotion     (new Rmatrix(MAX_TABLE_SIZE,3)),
ut1UtcOffsets   (new Rmatrix(MAX_TABLE_SIZE,2))
{
}

//---------------------------------------------------------------------------
//  EopFile(const EopFile &eopF);
//---------------------------------------------------------------------------
/**
 * Constructs base EopFile structures, by copying 
 * the input instance (copy constructor).
 *
 * @param eopF   EopFile instance to copy to create "this" 
 *               instance.
 */
//---------------------------------------------------------------------------
EopFile::EopFile(const EopFile &eopF) :
eopFType        (eopF.eopFType),
eopFileName     (eopF.eopFileName),
tableSz         (eopF.tableSz),
polarMotion     (new Rmatrix(*(eopF.polarMotion))),
ut1UtcOffsets   (new Rmatrix(*(eopF.ut1UtcOffsets)))
{
}

//---------------------------------------------------------------------------
//  EopFile& operator=(const EopFile &eopF)
//---------------------------------------------------------------------------
/**
 * Assignment operator for EopFile structures.
 *
 * @param eopF  The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const EopFile& EopFile::operator=(const EopFile &eopF)
{
   if (&eopF == this)
      return *this;
   eopFType      = eopF.eopFType;
   eopFileName   = eopF.eopFileName;
   tableSz       = eopF.tableSz;
   delete polarMotion;
   delete ut1UtcOffsets;
   polarMotion   = new Rmatrix(*(eopF.polarMotion));
   ut1UtcOffsets = new Rmatrix(*(eopF.ut1UtcOffsets));
   return *this;
}
//---------------------------------------------------------------------------
//  ~EopFile()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
EopFile::~EopFile()
{
   delete polarMotion;
   delete ut1UtcOffsets;
}


//------------------------------------------------------------------------------
//  void  Initialize()
//------------------------------------------------------------------------------
/**
 * This method initializes the EopFile class, by reading the file and 
 * storing the UT1-UTC offset and polar motion data.
 *
 */
//------------------------------------------------------------------------------
void EopFile::Initialize()
{
   std::string   line;
   std::ifstream eopFile(eopFileName.c_str());
   if (!eopFile)
      throw UtilityException("Error opening EopFile " + 
                             eopFileName);
   eopFile.setf(std::ios::skipws);
   if (eopFType == GmatEop::EOP_C04)
   {
      // read past the 'YEAR' line
      bool startNow = false;
      std::string   firstWord;
      while ((!startNow) && (!eopFile.eof()))
      {
         getline(eopFile,line);
         std::istringstream lineStr;
         lineStr.str(line);
         lineStr >> firstWord;
         if (firstWord == "YEAR")   startNow = true;
      }
      if (startNow == false)
         throw UtilityException("Unable to read EopFile.");
      // now start reading the data
      Integer     year, day, mjd;
      std::string month;
      Real        x, y, ut1_utc; // ignore lod, dPsi, dEpsilon
      while (!eopFile.eof())
      {
         getline(eopFile, line);
         if (!IsBlank(line.c_str()))
         {
            std::istringstream lineS;
            lineS.str(line);
            lineS >> year >> month >> day >> mjd >> x >> y >> ut1_utc;
            ut1UtcOffsets->SetElement(tableSz,0,mjd);
            ut1UtcOffsets->SetElement(tableSz,1,ut1_utc);
            polarMotion->SetElement(tableSz,0,mjd);
            polarMotion->SetElement(tableSz,1,x);
            polarMotion->SetElement(tableSz,2,y);
            tableSz++;
         }
      }
   }
   else if (eopFType == GmatEop::FINALS)
   {
      char        ipFlag1, ipFlag2;
      Real        mjd, x, y, ut1_utc;
      // ignore dutc, lod, dlod, I/P, dPsi ddPsi, dEpsilon, ddEpsilon, Bull. B data?
      Real        dx, dy;
      bool        done = false;
      while (!done && (!eopFile.eof()))
      {
         getline(eopFile, line);
         if (!IsBlank(line.c_str()))
         {
            std::istringstream lineS;
            lineS.str(line);
            lineS.ignore(6);
            lineS >> mjd >> ipFlag1
               >> x >> dx >> y >> dy >> ipFlag2 >> ut1_utc;
            // We're done when we reach teh end of the predicted values
            if ((ipFlag1 != 'I') && (ipFlag1 != 'P')) 
            {
               done = true;
            }
            else
            {
               ut1UtcOffsets->SetElement(tableSz,0,mjd);
               ut1UtcOffsets->SetElement(tableSz,1,ut1_utc);
               polarMotion->SetElement(tableSz,0,mjd);
               polarMotion->SetElement(tableSz,1,x);
               polarMotion->SetElement(tableSz,2,y);
               tableSz++;
            }
         }
      }
   }
   else
   {
      throw UtilityException("Error In EopFile - file type unknown.");
   }
   if (eopFile.is_open())  eopFile.close();
}

// method to return the UT1-UTC offset for the given UTCMjd - use UtcMjd???
Real EopFile::GetUt1UtcOffset(const Real utcMjd)
{
   Integer i = 0;
   for (i = (tableSz - 1); i >= 0; i--)
   {
      if (utcMjd >= ut1UtcOffsets->GetElement(i,0))
         return ut1UtcOffsets->GetElement(i,1);
   }
   return ut1UtcOffsets->GetElement(0,1);
}

// method to return JD, X, Y data (for use by coordinate systems)
Rmatrix EopFile::GetPolarMotionData()
{
   return Rmatrix(*polarMotion);
}


//------------------------------------------------------------------------------
//  bool IsBlank(char* aLine)
//------------------------------------------------------------------------------
/**
 * This method returns true if the string is empty or is all white space.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool EopFile::IsBlank(const char* aLine)
{
   Integer i;
   for (i=0;i<(int)strlen(aLine);i++)
   {
      //loj: 5/18/04 if (!isblank(aLine[i])) return false;
      if (!isspace(aLine[i])) return false;
   }
   return true;
}

