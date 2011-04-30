//$Id$
//------------------------------------------------------------------------------
//                                  GravityFile
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2007/08/15
//
/**
 * Implementation of the GravityFile class.  This is the code that reads the
 * different types of gravity files, such as .cof, .dat, .grv files.
 *
 */
//------------------------------------------------------------------------------
#include "GravityFile.hpp"
#include "StringUtil.hpp"       // for Trim()
#include "FileTypes.hpp"        // for MAX_LINE_LEN
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdlib>                      // Required for GCC 4.3

//#define DEBUG_GRAVITY_FILE
//#define DEBUG_GRAVITY_COF_FILE
//#define DEBUG_GRAVITY_DAT_FILE
//#define DEBUG_GRAVITY_GRV_FILE


//------------------------------------------------------------------------------
// GravityFile()
//------------------------------------------------------------------------------
GravityFile::GravityFile()
{
}


//------------------------------------------------------------------------------
// GravityFile(const GravityFile &copy)
//------------------------------------------------------------------------------
GravityFile::GravityFile(const GravityFile &copy)
{
}


//------------------------------------------------------------------------------
// const GravityFile& operator=(const GravityFile &right)
//------------------------------------------------------------------------------
const GravityFile& GravityFile::operator=(const GravityFile &right)
{
   return *this;
}


//------------------------------------------------------------------------------
// ~GravityFile()
//------------------------------------------------------------------------------
GravityFile::~GravityFile()
{
}


//------------------------------------------------------------------------------
// GmatFM::GravityFileType GetFileType(const std::string &filename)
//------------------------------------------------------------------------------
/*
 * Returns reconized gravity file type by reading first non-comment line.
 *
 * Assumption:
 *    GFT_COF contains "POTFILED"
 *    GFT_DAT contains valid Real number
 *    GFT_GRV contains "stk.v."
 *
 * @param  filename  Input file name
 * @return  Gravity file type
 *
 */
//------------------------------------------------------------------------------
GmatFM::GravityFileType GravityFile::GetFileType(const std::string &filename)
{
   #ifdef DEBUG_GRAVITY_FILE
   MessageInterface::ShowMessage
      ("GravityFile::GetFileType() entered\n   filename = \"%s\"\n",
       filename.c_str());
   #endif

   std::ifstream inStream(filename.c_str());
   if (!inStream)
      throw GravityFileException("Cannot open gravity file \"" + filename + "\"");

   std::string line;
   GmatFM::GravityFileType gft = GmatFM::GFT_UNKNOWN;

   while (!inStream.eof())
   {
      getline(inStream, line);

      // Make upper case, so we can check for certain keyword
      line = GmatStringUtil::ToUpper(line);

      #ifdef DEBUG_GRAVITY_FILE
      MessageInterface::ShowMessage("   => line=<%s>\n", line.c_str());
      #endif

      // Get first non-comment line
      if (line[0] != 'C' && line[0] != '#')
      {
         if (line.find("POTFIELD") != line.npos)
         {
            gft = GmatFM::GFT_COF;
            break;
         }
         else if (line.find("STK.V.") != line.npos)
         {
            gft = GmatFM::GFT_GRV;
            break;
         }
         else
         {
            Real rval;
            if (GmatStringUtil::ToReal(line, rval))
               gft = GmatFM::GFT_DAT;

            break;
         }
      }
   }

   inStream.close();

   #ifdef DEBUG_GRAVITY_FILE
   MessageInterface::ShowMessage("GravityFile::GetFileType() returning %d\n", gft);
   #endif

   if (gft == GmatFM::GFT_UNKNOWN)
      throw GravityFileException
         ("Gravity file \"" + filename + " is of unknown format");

   return gft;
}


//------------------------------------------------------------------------------
// bool GetFileInfo(const std::string &filename, Integer& degree, Integer& order,
//                  Real &mu, Real &radius)
//------------------------------------------------------------------------------
/*
 * Reads degree, order, gravity constant, and equatorial radius from the file.
 *
 * @param  filename  Input file name
 * @param  degree  Degree from the file
 * @param  order  Order from the file
 * @param  mu  Gravity constant from the file
 * @param  radius  Equatorial radius from the file
 *
 * @exception GravityFileException thrown if unreconized file type found
 */
//------------------------------------------------------------------------------
bool GravityFile::GetFileInfo(const std::string &filename, Integer& degree,
                              Integer& order, Real &mu, Real &radius)
{
   ReadFile(filename, degree, order, mu, radius, false);

   return true;
}


//------------------------------------------------------------------------------
// bool ReadFile(const std::string &filename, Integer& degree, Integer& order,
//               Real &mu, Real &radius, bool readCoeff, Real cbar[][361],
//               Real sbar[][DEG_DIM], Real dcbar[][DRF_DIM], Real dsbar[][DRF_DIM],
//               Integer maxDegree, Integer maxOrder, Integer maxDriftDegree = 0)
//------------------------------------------------------------------------------
bool GravityFile::ReadFile(const std::string &filename, Integer& degree,
                           Integer& order, Real &mu, Real &radius,
                           bool readCoeff, Real cbar[][361], Real sbar[][DEG_DIM],
                           Real dcbar[][DRF_DIM], Real dsbar[][DRF_DIM],
                           Integer maxDegree, Integer maxOrder, Integer maxDriftDegree)
{
   GmatFM::GravityFileType gft = GetFileType(filename);

   switch (gft)
   {
   case GmatFM::GFT_COF:
      return ReadCofFile(filename, degree, order, mu, radius, readCoeff, cbar, sbar,
                         maxDegree, maxOrder, maxDriftDegree);
   case GmatFM::GFT_DAT:
      return ReadDatFile(filename, degree, order, mu, radius, readCoeff, cbar, sbar,
                         dcbar, dsbar, maxDegree, maxOrder, maxDriftDegree);
   case GmatFM::GFT_GRV:
      return ReadGrvFile(filename, degree, order, mu, radius, readCoeff, cbar, sbar,
                         maxDegree, maxOrder, maxDriftDegree);
   default:
      return false;
   }
}


//------------------------------------------------------------------------------
// bool ReadCofFile(const std::string &filename, Integer& degree, Integer& order,
//                  Real &mu, Real &radius, bool readCoeff, Real cbar[][361],
//                  Real sbar[][DEG_DIM], Integer maxDegree, Integer maxOrder,
//                  Integer maxDriftDegree)
//------------------------------------------------------------------------------
bool GravityFile::ReadCofFile(const std::string &filename, Integer& degree,
                              Integer& order, Real &mu, Real &radius, bool readCoeff,
                              Real cbar[][361], Real sbar[][DEG_DIM], Integer maxDegree,
                              Integer maxOrder, Integer maxDriftDegree)
{
   std::ifstream inStream(filename.c_str());
   if (!inStream)
      throw GravityFileException("Cannot open COF gravity file \"" + filename + "\"");

   Integer       n = -1, m = -1;
   Integer       fileOrder = -1, fileDegree = -1;
   Real          Cnm=0.0, Snm=0.0;
   Integer       noIdea;
   Real          noClue;
   Real          tmpMu;
   Real          tmpA;

   #ifdef DEBUG_GRAVITY_COF_FILE
   MessageInterface::ShowMessage("Entered GravityFile::ReadCofFile\n");
   #endif

   std::string line;
   std::string firstStr;
   std::string degStr, ordStr;
   std::string nStr, mStr, cnmStr, snmStr;

   while (!inStream.eof())
   {
      getline(inStream, line);
      std::istringstream lineStream;

      // ignore comment lines
      if (line[0] != 'C')
      {
         firstStr = line.substr(0, 8);
         firstStr = GmatStringUtil::Trim(firstStr);

         if (firstStr == "END") break;
         if (firstStr == "POTFIELD")
         {
            degStr = line.substr(8, 3);
            ordStr = line.substr(11, 3);

            if ((GmatStringUtil::ToInteger(degStr, fileDegree)) &&
                (GmatStringUtil::ToInteger(ordStr, fileOrder)))
            {
               lineStream.str(line.substr(14));
               lineStream >> noIdea >> tmpMu >> tmpA >> noClue;
               if (tmpMu != 0.0)
                  mu = tmpMu / 1.0e09;     // -> km^3/sec^2
               if (tmpA  != 0.0)
                  radius = tmpA / GmatMathConstants::KM_TO_M;  // -> km

               // if not reading coeffiencts, stop after reading the mu and a
               if (!readCoeff)
                  break;
            }
            else
            {
               throw GravityFileException
                  ("File \"" + filename + "\" has error in \n   \"" + line + "\"");
            }
         }
         else if (firstStr == "RECOEF")
         {
            nStr = line.substr(8, 3);
            mStr = line.substr(11, 3);
            cnmStr = line.substr(17, 21);
            lineStream.str(line.substr(38, 21));
            lineStream >> snmStr;
            snmStr = GmatStringUtil::Trim(snmStr);
            //MessageInterface::ShowMessage
            //   ("===> nStr=%s, mStr=%s, snmStr=<%s>\n",
            //    nStr.c_str(), mStr.c_str(), snmStr.c_str());
            if ((GmatStringUtil::ToInteger(nStr, n)) &&
                (GmatStringUtil::ToInteger(mStr, m)) &&
                (GmatStringUtil::ToReal(cnmStr, Cnm)) &&
                ((snmStr == "") ||
                 (GmatStringUtil::ToReal(snmStr, Snm))))
            {
               if ( n <= maxDegree && m <= maxOrder )
               {
                  cbar[n][m] = Cnm;
                  sbar[n][m] = Snm;
                  //MessageInterface::ShowMessage
                  //   ("   cbar[%d][%d]=% .12e\n   sbar[%d][%d]=% .12e\n",
                  //    n, m, cbar[n][m], n, m, sbar[n][m]);
               }
            }
            else
            {
               throw GravityFileException
                  ("File \"" + filename + "\" has error in \n   \"" + line + "\"");
            }

            snmStr = "";
            Snm = 0.0;
         }
      }
   }

   degree = fileDegree;
   order = fileOrder;

   // make sure mu and a are in KM and Km^3/sec^2 (they are in meters on the files)

   #ifdef DEBUG_GRAVITY_COF_FILE
   MessageInterface::ShowMessage("   \"%s\" successfully read\n", filename.c_str());
   MessageInterface::ShowMessage
      ("   degree=%d, order=%d, mu=%f, radius=%f\n", degree, order,
       mu, radius);
   MessageInterface::ShowMessage
      ("   last n=%d, m=%d, Cnm=%le, Snm=%le\n", n, m, Cnm, Snm);
   MessageInterface::ShowMessage
      ("   last nStr=%s, mStr=%s, cnmStr=%s, snmStr=%s\n",
       nStr.c_str(), mStr.c_str(), cnmStr.c_str(), snmStr.c_str());
   #endif

   inStream.close();
   return true;
}


//------------------------------------------------------------------------------
// bool ReadDatFile(const std::string &filename, Integer& degree, Integer& order,
//                  Real &mu, Real &radius, bool readCoeff, Real cbar[][361],
//                  Real sbar[][DEG_DIM], Integer maxDegree, Integer maxOrder,
//                  Integer maxDriftDegree)
//------------------------------------------------------------------------------
bool GravityFile::ReadDatFile(const std::string &filename, Integer& degree,
                              Integer& order, Real &mu, Real &radius, bool readCoeff,
                              Real cbar[][361], Real sbar[][DEG_DIM],
                              Real dcbar[][DRF_DIM], Real dsbar[][DRF_DIM],
                              Integer maxDegree, Integer maxOrder, Integer maxDriftDegree)
{
   #ifdef DEBUG_GRAVITY_DAT_FILE
   MessageInterface::ShowMessage
      ("GravityFile::ReadDatFile() filename=%s\n   maxDegree=%d, maxOrder=%d, "
       "maxDriftDegree=%d, readCoeff=%d\n", filename.c_str(), maxDegree, maxOrder,
       maxDriftDegree, readCoeff);
   #endif

   if (!readCoeff)
   {
      std::ifstream inStream(filename.c_str());
      if (!inStream)
         throw GravityFileException("Cannot open DAT gravity file \"" + filename + "\"");

      std::string line;
      while (!inStream.eof())
      {
         getline(inStream, line);

         // ignore comment lines
         if (line[0] != '#')
            break;
      }

      std::istringstream muStream(line);
      muStream >> mu;

      getline(inStream, line);
      std::istringstream raStream(line);
      raStream >> radius;

      mu = mu / 1.0e09;           // -> Km^3/sec^2
      radius  = radius / GmatMathConstants::KM_TO_M;  // -> Km

      while (!inStream.eof())
      {
         getline(inStream, line);
         if (line[0] != '#')
         {
            std::istringstream coefStream(line);
            coefStream >> degree >> order;
         }
      }

      #ifdef DEBUG_GRAVITY_DAT_FILE
      MessageInterface::ShowMessage
         ("GravityFile::ReadDatFile() returning degree=%d, order=%d, mu=%le, "
          "radius=%le\n", degree, order, mu, radius);
      #endif

      inStream.close();
      return true;
   }


   //------------------------------------------------------------
   // read all information from file
   //------------------------------------------------------------
   FILE *fp = fopen( filename.c_str(), "r");
   if (!fp)
      throw GravityFileException("Cannot open DAT gravity file \"" + filename + "\"");

   char buf[GmatFile::MAX_LINE_LEN];
   Integer maxLen = GmatFile::MAX_LINE_LEN;
   Integer cc, dd, sz=0;
   Integer iscomment, rtn;
   Integer n=0, m=0;
   Integer fileDegree, fileOrder;
   Real    Cnm=0.0, Snm=0.0, dCnm=0.0, dSnm=0.0;

   //-------------------------------------------------------
   // read mu and radius
   //-------------------------------------------------------
   iscomment = 1;
   while ( iscomment )
   {
      rtn = fgetc( fp );
      if ( (char)rtn == '#' )
      {
         // Intentionally get the return and then ignore it to move warning from
         // system libraries to GMAT code base.  The "unused variable" warning
         // here can be safely ignored.
         char* ch = fgets( buf, maxLen, fp );
      }
      else
      {
         ungetc( rtn, fp );
         iscomment = 0;
      }
   }

   int len = fscanf(fp, "%lg\n", &mu ); mu = (Real)mu;
   len += fscanf(fp, "%lg\n", &radius ); radius = (Real)radius;
   radius  = radius / GmatMathConstants::KM_TO_M;  // -> Km
   mu = mu / 1.0e09;           // -> Km^3/sec^2

   fileDegree = 0;
   fileOrder  = 0;
   cc=0; n=0; m=0;

   // Check maxDegree, maxOrder, maxDriftDegree
   if (maxDegree <= 0 || maxDegree > 360)
   {
      GravityFileException ue;
      ue.SetDetails("Invalid MAX Degree passed %d", maxDegree);
      MessageInterface::ShowMessage
         ("**** ERROR **** Invalid MAX Degree passed %d\n", maxDegree);
      throw ue;
   }

   if (maxOrder <= 0 || maxOrder > 360)
   {
      fclose(fp);
      
      GravityFileException ue;
      ue.SetDetails("Invalid MAX Order passed %d", maxOrder);
      MessageInterface::ShowMessage
         ("**** ERROR **** Invalid MAX Order passed %d\n", maxOrder);
      throw ue;
   }

   if (maxDriftDegree <= 0 || maxDriftDegree > 2)
   {
      fclose(fp);
      
      GravityFileException ue;
      ue.SetDetails("Invalid MAX Drift Degree passed %d", maxDriftDegree);
      MessageInterface::ShowMessage
         ("**** ERROR **** Invalid MAX Drift Order passed %d\n", maxDriftDegree);
      throw ue;
   }


   for (cc = 2; cc <= maxDegree; ++cc)
      for (dd = 0; dd <= cc; ++dd)
         sz++;

   //-------------------------------------------------------
   // read coefficient drift rate
   //-------------------------------------------------------
   char* ch = fgets( buf, maxLen, fp );
   while ( ( (char)(rtn=fgetc(fp)) != '#' ) && (rtn != EOF) )
   {
      ungetc( rtn, fp );
      // Intentionally get the return and then ignore it to move warning from
      // system libraries to GMAT code base.  The "unused variable" warning
      // here can be safely ignored.
      int len = fscanf( fp, "%i %i %le %le\n", &n, &m, &dCnm, &dSnm );
      if ( n <= maxDriftDegree  && m <= n )
      {
         dcbar[n][m] = (Real)dCnm;
         dsbar[n][m] = (Real)dSnm;
      }
   }

   //-------------------------------------------------------
   // read coefficients and store
   //-------------------------------------------------------
   ch = fgets( buf, maxLen, fp );

   fileDegree = 0;
   fileOrder  = 0;
   cc=0; n=0; m=0;
   do
   {
      if ( n <= maxDegree && m <= maxOrder )
      {
         cbar[n][m] = (Real)Cnm;
         sbar[n][m] = (Real)Snm;
      }
      if (n > fileDegree) fileDegree = n;
      if (m > fileOrder)  fileOrder  = m;

      cc++;
   } while ( ( cc<=sz ) && ( fscanf( fp, "%i %i %le %le\n", &n, &m, &Cnm, &Snm ) > 0 ));

   degree = fileDegree;
   order = fileOrder;

   #ifdef DEBUG_GRAVITY_DAT_FILE
   MessageInterface::ShowMessage("Leaving GravityFile::ReadDatFile\n");
   MessageInterface::ShowMessage
      ("    degree = %d, order = %d, mu = %.4f, radius = %.4f\n", degree, order,
       mu, radius);
   MessageInterface::ShowMessage
      ("    cbar[2][0] = % .12e    sbar[2][0] = %.12e\n", cbar[2][0], sbar[2][0]);
   MessageInterface::ShowMessage
      ("    cbar[2][1] = % .12e    sbar[2][1] = %.12e\n", cbar[2][1], sbar[2][1]);
   MessageInterface::ShowMessage
      ("   dcbar[2][0] = % .12e   dsbar[2][0] = %.12e\n", dcbar[2][0], dsbar[2][0]);
   MessageInterface::ShowMessage
      ("   dcbar[2][1] = % .12e   dsbar[2][1] = %.12e\n", dcbar[2][1], dsbar[2][1]);
   #endif

   fclose(fp);
   return true;
}


//------------------------------------------------------------------------------
// bool ReadGrvFile(const std::string &filename, Integer& degree, Integer& order,
//                  Real &mu, Real &radius, bool readCoeff, Real cbar[][361],
//                  Real sbar[][DEG_DIM], Integer maxDegree, Integer maxOrder,
//                  Integer maxDriftDegree = 0)
//------------------------------------------------------------------------------
bool GravityFile::ReadGrvFile(const std::string &filename, Integer& degree,
                              Integer& order, Real &mu, Real &radius, bool readCoeff,
                              Real cbar[][361], Real sbar[][DEG_DIM], Integer maxDegree,
                              Integer maxOrder, Integer maxDriftDegree)
{
   std::ifstream inStream(filename.c_str());
   if (!inStream)
      throw GravityFileException("Cannot open GRV gravity file \"" + filename + "\"");

   Integer       n, m;
   Integer       fileOrder = -1, fileDegree = -1;
   Real          Cnm = 0.0, Snm = 0.0;
   Real          tmpMu = 0.0;
   Real          tmpA  = 0.0;
   std::string   isNormalized;

   #ifdef DEBUG_GRAVITY_GRV_FILE
   MessageInterface::ShowMessage("Entered GravityFile::ReadGrvFile\n");
   #endif

   std::string line;
   std::string firstStr;

   // Read header line
   getline(inStream, line);

   while (!inStream.eof())
   {
      getline(inStream, line);

      //MessageInterface::ShowMessage("=> line=<%s>\n", line.c_str());
      if (line == "")
         continue;

      std::istringstream lineStream;
      lineStream.str(line);

      // ignore comment lines
      if (line[0] != '#')
      {
         lineStream >> firstStr;
         if (firstStr == "END") break;

         std::string upperString = GmatStringUtil::ToUpper(firstStr);

         // ignore the stk version and blank lines
         if ((upperString == "MODEL") ||
             (upperString =="BEGIN"))
         {
            // do nothing - we don't need to know this
         }
         else if (upperString == "DEGREE")
         {
            lineStream >> fileDegree;
         }
         else if (upperString == "ORDER")
         {
            lineStream >> fileOrder;
         }
         else if (upperString == "GM")
         {
            lineStream >> tmpMu;
            if (tmpMu != 0.0)
               mu = tmpMu / 1.0e09;     // -> Km^3/sec^2
         }
         else if (upperString == "REFDISTANCE")
         {
            lineStream >> tmpA;
            if (tmpA != 0.0)
               radius = tmpA / GmatMathConstants::KM_TO_M;  // -> Km
         }
         else if (upperString == "NORMALIZED")
         {
            lineStream >> isNormalized;
            if (isNormalized == "No")
               throw GravityFileException("File " + filename + " is not normalized.");
         }
         else
         {
            // We can stop here if we dont read coefficients
            if (!readCoeff)
               break;

            // Ensure that m and n fall in the allowed ranges
            n = (Integer) atoi(firstStr.c_str());
            if ((n > 0) && (n < maxDegree))
            {
               lineStream >> m;
               if ((m >= 0) && (m <= n))
               {
                  lineStream >> Cnm >> Snm;
                  cbar[n][m] = (Real)Cnm;
                  sbar[n][m] = (Real)Snm;
               }
            }
         }
      }
   }

   degree = fileDegree;
   order = fileOrder;

   inStream.close();

   #ifdef DEBUG_GRAVITY_GRV_FILE
   if (readCoeff)
   {
      MessageInterface::ShowMessage("Leaving GravityFile::ReadGrvFile\n");
      MessageInterface::ShowMessage("   cbar[ 2][ 0] = %le   sbar[ 2][ 0] = %le   \n",
                                    cbar[2][0], sbar[2][0]);
      MessageInterface::ShowMessage("   cbar[20][20] = %le   sbar[20][20] = %le   \n",
                                    cbar[20][20], sbar[20][20]);
   }
   #endif

   return true;
}


