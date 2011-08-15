//$Id$
//------------------------------------------------------------------------------
//                           HarmonicGravityCof
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: John P. Downing/GSFC/595
// Created: 2010.10.28
// (modified for style, etc. by Wendy Shoan/GSFC/583 2011.05.31)
//
/**
 * This is the class that loads data from a COF type gravity file.
 */
//------------------------------------------------------------------------------
#include "HarmonicGravityCof.hpp"
#include "ODEModelException.hpp"
#include "UtilityException.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include <fstream>
#include <sstream>

//#define DEBUG_GRAVITY_COF_FILE
//#define DEBUG_GRAVITY_DESTRUCT
//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// n/a

//------------------------------------------------------------------------------
HarmonicGravityCof::HarmonicGravityCof(const std::string& filename,
                                       const Real& radius, const Real& mukm) :
   HarmonicGravity (filename)
{
   bodyRadius = radius;
   factor     = -mukm;
   Load ();
}

//------------------------------------------------------------------------------
HarmonicGravityCof::~HarmonicGravityCof()
{
   #ifdef DEBUG_GRAVITY_DESTRUCT
      MessageInterface::ShowMessage("In HarmonicGravityCof destructor ...\n");
   #endif
}

//------------------------------------------------------------------------------
void HarmonicGravityCof::Load()
{
   std::ifstream inStream (gravityFilename.c_str());
   if (!inStream)
      throw GravityFileException("Cannot open COF gravity file \"" + gravityFilename + "\"");

   Integer       cbflag = -1;  // 0 = earth
   Real          normalizedflag = -1;

   #ifdef DEBUG_GRAVITY_COF_FILE
   MessageInterface::ShowMessage("Entered GravityFile::ReadCofFile for file %s\n", gravityFilename.c_str());
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
         if (firstStr == "99999") break;  // sometimes this is EOF marker
         if (firstStr == "POTFIELD")
         {
            degStr = line.substr(8, 3);
            ordStr = line.substr(11, 3);

            if ((GmatStringUtil::ToInteger(degStr, NN)) &&
                (GmatStringUtil::ToInteger(ordStr, MM)))
            {
               lineStream.str(line.substr(14));
               Real tmpMu = 0;
               Real tmpA = 0;
               lineStream >> cbflag >> tmpMu >> tmpA >> normalizedflag;
               if (tmpMu != 0.0)
                  factor = -tmpMu / 1.0e09;     // -> km^3/sec^2
               if (tmpA  != 0.0)
                  bodyRadius = tmpA / 1000.0;  // -> km
               
               // if not reading coefficients, stop after reading the mu and a
               Allocate ();
            }
            else
            {
               throw GravityFileException
                  ("File \"" + gravityFilename + "\" has error in \n   \"" + line + "\"");
            }
         }
         else if (firstStr == "RECOEF")
         {
            Integer n = -1;
            Integer m = -1;
            Real cnm = 0.0;
            Real snm = 0.0;
            nStr = line.substr(8, 3);
            mStr = line.substr(11, 3);
            cnmStr = line.substr(17, 21);
            lineStream.str(line.substr(38, 21));
            lineStream >> snmStr;
            snmStr = GmatStringUtil::Trim(snmStr);
            if ((GmatStringUtil::ToInteger(nStr, n)) &&
                (GmatStringUtil::ToInteger(mStr, m)) &&
                (GmatStringUtil::ToReal(cnmStr, cnm)) &&
                ((snmStr == "") ||
                 (GmatStringUtil::ToReal(snmStr, snm))))
            {
               if ( n <= NN && m <= MM )
               {
//                  if (normalizedflag < 0.5)
//                  {
//                     cnm /= V[n][m];
//                     snm /= V[n][m];
//                     #ifdef DEBUG_GRAVITY_COF_FILE
//                        MessageInterface::ShowMessage("NORMALIZING cnm and snm -----------\n");
//                     #endif
//                  }
                  C[n][m] = cnm;
                  S[n][m] = snm;
                  #ifdef DEBUG_GRAVITY_COF_FILE
                     if (cnm != 0.0) MessageInterface::ShowMessage("Cbar[%d][%d] = %12.10f\n", n, m, cnm);
                     if (snm != 0.0) MessageInterface::ShowMessage("Sbar[%d][%d] = %12.10f\n", n, m, snm);
                  #endif
               }
            }
            else
            {
               throw GravityFileException
                  ("File \"" + gravityFilename + "\" has error in \n   \"" + line + "\"");
            }
            snmStr = "";
            snm = 0.0;
         }
      }
   }

   #ifdef DEBUG_GRAVITY_COF_FILE
   MessageInterface::ShowMessage("   \"%s\" successfully read\n", gravityFilename.c_str());
   MessageInterface::ShowMessage
      ("   NN=%d, MM=%d, factor=%f, bodyRadius=%f\n", NN, MM,
            factor, bodyRadius);
   MessageInterface::ShowMessage
      ("   last nStr=%s, mStr=%s, cnmStr=%s, snmStr=%s\n",
       nStr.c_str(), mStr.c_str(), cnmStr.c_str(), snmStr.c_str());
   #endif

   inStream.close();
}

