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
polarMotion     (new Rmatrix(MAX_TABLE_SIZE,4)),
ut1UtcOffsets   (new Rmatrix(MAX_TABLE_SIZE,2)),
isInitialized   (false)
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
ut1UtcOffsets   (new Rmatrix(*(eopF.ut1UtcOffsets))),
isInitialized   (eopF.isInitialized)
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
   isInitialized = eopF.isInitialized;
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
   if (isInitialized) return;
   
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
      Real        x, y, ut1_utc, lod; // ignore lod, dPsi, dEpsilon
      while (!eopFile.eof())
      {
         getline(eopFile, line);
         if (!IsBlank(line.c_str()))
         {
            std::istringstream lineS;
            lineS.str(line);
            lineS >> year >> month >> day >> mjd >> x >> y >> ut1_utc >> lod;
            ut1UtcOffsets->SetElement(tableSz,0,mjd + GmatTimeUtil::JD_NOV_17_1858);
            ut1UtcOffsets->SetElement(tableSz,1,ut1_utc);
            polarMotion->SetElement(tableSz,0,mjd + GmatTimeUtil::JD_NOV_17_1858);
            polarMotion->SetElement(tableSz,1,x);
            polarMotion->SetElement(tableSz,2,y);
            polarMotion->SetElement(tableSz,3,lod);
            tableSz++;
         }
      }
   }
   else if (eopFType == GmatEop::FINALS)
   {
      char        ipFlag1, ipFlag2;
      Real        mjd, x, y, ut1_utc, lod;
      // ignore dutc, lod, dlod, I/P, dPsi ddPsi, dEpsilon, ddEpsilon, Bull. B data?
      Real        dx, dy, dut1_utc;
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
               >> x >> dx >> y >> dy >> ipFlag2 >> ut1_utc >> dut1_utc >> lod;
            // We're done when we reach the end of the predicted values
            if ((ipFlag1 != 'I') && (ipFlag1 != 'P')) 
            {
               done = true;
            }
            else
            {
               ut1UtcOffsets->SetElement(tableSz,0,mjd + GmatTimeUtil::JD_NOV_17_1858);
               ut1UtcOffsets->SetElement(tableSz,1,ut1_utc);
               polarMotion->SetElement(tableSz,0,mjd + GmatTimeUtil::JD_NOV_17_1858);
               polarMotion->SetElement(tableSz,1,x);
               polarMotion->SetElement(tableSz,2,y);
               polarMotion->SetElement(tableSz,3,lod*1.0e-03); // convert to seconds
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
   isInitialized = true;
}

//---------------------------------------------------------------------------
//  std::string GetFileName() const
//---------------------------------------------------------------------------
/**
 * Returns the name of the EOP file.
 *
 * @return name of the EOp file.  
 */
//---------------------------------------------------------------------------
std::string EopFile::GetFileName() const
{
   return eopFileName;
}


//---------------------------------------------------------------------------
//  Real GetUt1UtcOffset(const Real utcMjd)
//---------------------------------------------------------------------------
 /**
 * Returns the UT1-UTC offset for the given utc mjd.
 *
 * @param utcMjd  The utc mjd for which to return the offset.
 *
 * @return UT1-UTC offset for the given time; values between table entries
 *         are interpolated linearly.  
 */
//---------------------------------------------------------------------------
Real EopFile::GetUt1UtcOffset(const Real utcMjd)
{
   if (!isInitialized)  Initialize();
   
   Integer i = 0;
   Real    utcJD = utcMjd + GmatTimeUtil::JD_NOV_17_1858;
   for (i = (tableSz - 1); i >= 0; i--)
   {
      if (utcJD >= ut1UtcOffsets->GetElement(i,0))
      {
         // if it's greater than the last entry in the table, then return the 
         // last value
         if (i == (tableSz -1))  return ut1UtcOffsets->GetElement(i,1);
         // otherwise, interpolate between values
         Real diffJD  = ut1UtcOffsets->GetElement(i+1,0) - 
                        ut1UtcOffsets->GetElement(i,0);
         Real whereJD = utcJD - ut1UtcOffsets->GetElement(i,0);
         Real ratio   = whereJD / diffJD;
         Real diffOff = ut1UtcOffsets->GetElement(i+1,1) -
                        ut1UtcOffsets->GetElement(i,1);
         Real off     = ut1UtcOffsets->GetElement(i,1) + ratio * diffOff;
         return off;
      }
   }
   // if it's before the time on the file, return the first value
   return ut1UtcOffsets->GetElement(0,1);
}

//---------------------------------------------------------------------------
//  Rmatrix GetPolarMotionData()
//---------------------------------------------------------------------------
/**
 * Returns the polar motion data.
 * for each row:  mjd, x, y
 *
 * @return polar motion data.
 */
//---------------------------------------------------------------------------
Rmatrix EopFile::GetPolarMotionData()
{
   return Rmatrix(*polarMotion);
}

//---------------------------------------------------------------------------
//  bool GetPolarMotionAndLod(Real forUtcMjd, Real &xval, Real  &yval,
//                            Real &lodval)
//---------------------------------------------------------------------------
/**
 * Returns the polar motion data X, Y, and LOD, for the input UTC MJD time.
 * 
 * @param forUtcMjd time for which to return the data
 * @param xval      return X value of polar motion data (arcsec)
 * @param yval      return Y value of polar motion data (arcsec)
 * @param lodval    return LOD value (seconds)
 *
 */
//---------------------------------------------------------------------------
bool EopFile::GetPolarMotionAndLod(Real forUtcMjd, Real &xval, Real  &yval,
                                   Real &lodval)
{
   if (!isInitialized)  Initialize();
   
   Integer i = 0;
   Real    utcJD = forUtcMjd + GmatTimeUtil::JD_NOV_17_1858;
   // if it's before the time on the file, return the first values
   if (utcJD <= polarMotion->GetElement(0,0))
   {
      xval   = polarMotion->GetElement(0,1);
      yval   = polarMotion->GetElement(0,2);
      lodval = polarMotion->GetElement(0,3);
   }
   else
   {
      for (i = (tableSz - 1); i >= 0; i--)
      {
         if (utcJD >= polarMotion->GetElement(i,0))
         {
            // if it's greater than the last entry in the table, then return the 
            // last value
            if (i == (tableSz -1))
            {
               xval   = polarMotion->GetElement(i,1);
               yval   = polarMotion->GetElement(i,2);
               lodval = polarMotion->GetElement(i,3);
            }
            else
            {
               // otherwise, interpolate between values
               Real diffJD  = polarMotion->GetElement(i+1,0) - 
                              polarMotion->GetElement(i,0);
               Real whereJD = utcJD - polarMotion->GetElement(i,0);
               Real ratio   = whereJD / diffJD;
               Real diffX   = polarMotion->GetElement(i+1,1) -
                              polarMotion->GetElement(i,1);
               Real diffY   = polarMotion->GetElement(i+1,2) -
                              polarMotion->GetElement(i,2);
               //Real diffLOD = polarMotion->GetElement(i+1,3) -
               //               polarMotion->GetElement(i,3);
               xval   = polarMotion->GetElement(i,1) + ratio * diffX;
               yval   = polarMotion->GetElement(i,2) + ratio * diffY;
               // 2005.02.23 - Steve says not to interpolate lod
               //lodval = polarMotion->GetElement(i,3) + ratio * diffLOD;
               lodval = polarMotion->GetElement(i,3);
            }
            break;
         }
      }
   }
   return true;
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

