//$Id$
//------------------------------------------------------------------------------
//                           HarmonicGravityGrv
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
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
//#define DEBUG_GRAVITY_DEGREE_ORDER

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
   unsigned long lineno = 1;
   bool allocated = false;
   bool degreeSet = false;
   bool orderSet  = false;

   // Read header line
   getline(inStream, line);

   while (!inStream.eof())
   {
      getline(inStream, line);
      lineno++;

//      if (line == "")
      if (GmatStringUtil::IsBlank(line, true))
         continue;
      #ifdef DEBUG_GRAVITY_GRV_FILE
         else
            MessageInterface::ShowMessage("Line is \"%s\"\n", line.c_str());
      #endif
      std::istringstream lineStream;
      std::ostringstream linenoStream;
      linenoStream << lineno;
      lineStream.str(line);

      // ignore comment lines
      if (line[0] != '#')
      {
         lineStream >> firstStr;
         if (firstStr == "END") break;

         std::string upperString = GmatStringUtil::ToUpper(firstStr);
         #ifdef DEBUG_GRAVITY_DEGREE_ORDER
            MessageInterface::ShowMessage("----- firstStr = %s\n", firstStr.c_str());
         #endif

         // ignore the stk version and blank lines
         if ((upperString == "MODEL") ||
             (upperString =="BEGIN"))
         {
            // do nothing - we don't need to know this
         }
         else if (upperString == "DEGREE")
         {
            lineStream >> NN;
            if (lineStream.fail())
               throw GravityFileException("Error reading DEGREE in GRV gravity file \"" + gravityFilename + "\": line " + linenoStream.str());
               #ifdef DEBUG_GRAVITY_DEGREE_ORDER
                  MessageInterface::ShowMessage("--- Setting NN (degree) = %d\n", NN);
               #endif
            degreeSet = true;
         }
         else if (upperString == "ORDER")
         {
            lineStream >> MM;
            if (lineStream.fail())
               throw GravityFileException("Error reading ORDER in GRV gravity file \"" + gravityFilename + "\": line " + linenoStream.str());
               #ifdef DEBUG_GRAVITY_DEGREE_ORDER
                  MessageInterface::ShowMessage("--- Setting MM (order)  = %d\n", MM);
               #endif
            orderSet = true;
         }
         else if (upperString == "GM")
         {
            Real tmpMu = 0.0;
            lineStream >> tmpMu;
            if (lineStream.fail())
               throw GravityFileException("Error reading GM in GRV gravity file \"" + gravityFilename + "\": line " + linenoStream.str());
            if (tmpMu != 0.0)
               factor = -tmpMu / 1.0e09;     // -> Km^3/sec^2
         }
         else if (upperString == "REFDISTANCE")
         {
            Real tmpA  = 0.0;
            lineStream >> tmpA;
            if (lineStream.fail())
               throw GravityFileException("Error reading REFDISTANCE in GRV gravity file \"" + gravityFilename + "\": line " + linenoStream.str());
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
            #ifdef DEBUG_GRAVITY_DEGREE_ORDER
               MessageInterface::ShowMessage("--- n = %d\n", n);
            #endif
            if ((n > 0) && (n <= NN))    // change to <= NN  WCS 2015.09.01  GMT-5173
            {
               lineStream >> m;
               #ifdef DEBUG_GRAVITY_DEGREE_ORDER
                  MessageInterface::ShowMessage("--- and m = %d\n", m);
               #endif
               if (lineStream.fail())
                  throw GravityFileException("Error reading m in GRV gravity file \"" + gravityFilename + "\": line " + linenoStream.str());
               if ((m >= 0) && (m <= n))
               {
                  lineStream >> cnm;
                  if (lineStream.fail())
                     throw GravityFileException("Error reading cnm in GRV gravity file \"" + gravityFilename + "\": line " + linenoStream.str());
                  if (!lineStream.eof())
                  {
                     lineStream >> snm;
                     if (lineStream.fail())
                        throw GravityFileException("Error reading snm in GRV gravity file \"" + gravityFilename + "\": line " + linenoStream.str());
                  }
                  #ifdef DEBUG_GRAVITY_DEGREE_ORDER
                     MessageInterface::ShowMessage("------ cnm = %12.10f, snm = %12.10f, isNormalized = %s\n",
                           cnm, snm, isNormalized.c_str());
                  #endif
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
