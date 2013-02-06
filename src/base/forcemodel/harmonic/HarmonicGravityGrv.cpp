//$Id$
//------------------------------------------------------------------------------
//                           HarmonicGravityGrv
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: John P. Downing/GSFC/595
// Created: 2010.10.28
// (modified for style, etc. by Wendy Shoan/GSFC/583 2011.06.04)
//
/**
 * This is the class that loads data from a GRV type gravity file.
 */
//------------------------------------------------------------------------------
#include "HarmonicGravityGrv.hpp"
#include "ODEModelException.hpp"
#include "UtilityException.hpp"
#include "StringUtil.hpp"       
#include "MessageInterface.hpp"
#include <fstream>
#include <sstream>
#include <stdlib.h>           // For atoi

//#define DEBUG_GRAVITY_GRV_FILE

#ifdef DEBUG_GRAVITY_GRV_FILE
#include "MessageInterface.hpp"
#endif

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// n/a

//------------------------------------------------------------------------------
HarmonicGravityGrv::HarmonicGravityGrv(const std::string& filename,
                                       const Real& radius, const Real& mukm) :
   HarmonicGravity (filename)
{
   bodyRadius = radius;
   factor     = -mukm;
   try
   {
      Load();
   }
   catch (BaseException &e)
   {
      throw;
	}
   catch (const std::exception& ex) 
   {
      MessageInterface::ShowMessage("Caught ex in HarmonicGravityGrv::"
         "HarmonicGravityGrv() during Load()\n");
      throw;
   }
}

//------------------------------------------------------------------------------
HarmonicGravityGrv::~HarmonicGravityGrv()
{
}

//------------------------------------------------------------------------------
void HarmonicGravityGrv::Load()
{
   std::ifstream inStream(gravityFilename.c_str());
   if (!inStream)
      throw GravityFileException("Cannot open GRV gravity file \"" + gravityFilename + "\"");

   #ifdef DEBUG_GRAVITY_GRV_FILE
   MessageInterface::ShowMessage("Entered GravityFile::ReadGrvFile\n");
   #endif

   std::string isNormalized = "";
   std::string line;
   std::string firstStr;
   bool allocated = false;
   bool degreeSet = false;
   bool orderSet  = false;

   // Read header line
   getline(inStream, line);

   while (!inStream.eof())
   {
      getline(inStream, line);

      if (line == "")
         continue;
      #ifdef DEBUG_GRAVITY_GRV_FILE
         else
            MessageInterface::ShowMessage("%s\n", line);
      #endif
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
            lineStream >> NN;
            degreeSet = true;
         }
         else if (upperString == "ORDER")
         {
            lineStream >> MM;
            orderSet = true;
         }
         else if (upperString == "GM")
         {
            Real tmpMu = 0.0;
            lineStream >> tmpMu;
            if (tmpMu != 0.0)
               factor = -tmpMu / 1.0e09;     // -> Km^3/sec^2
         }
         else if (upperString == "REFDISTANCE")
         {
            Real tmpA  = 0.0;
            lineStream >> tmpA;
            if (tmpA != 0.0)
               bodyRadius = tmpA / 1000.0;  // -> Km
         }
         else if (upperString == "NORMALIZED")
         {
            lineStream >> isNormalized;
         }
         else
         {
            if (!allocated && degreeSet && orderSet)
            {
               Allocate (); 
               allocated = true;
            }
            Integer n = -1;
            Integer m = -1;
            Real cnm = 0.0;
            Real snm = 0.0;
            // Ensure that m and n fall in the allowed ranges
            n = (Integer) atoi(firstStr.c_str());
            if ((n > 0) && (n < NN))
            {
               lineStream >> m;
               if ((m >= 0) && (m <= n))
               {
                  lineStream >> cnm >> snm;
                  if (isNormalized == "No")
                     {
                     cnm *= V[n][m];
                     snm *= V[n][m];
                     }
                  C[n][m] = (Real)cnm;
                  S[n][m] = (Real)snm;
               }
            }
         }
      }
   }

   inStream.close();
   #ifdef DEBUG_GRAVITY_GRV_FILE
      MessageInterface::ShowMessage("Leaving GravityFile::ReadGrvFile\n");
   #endif
}
