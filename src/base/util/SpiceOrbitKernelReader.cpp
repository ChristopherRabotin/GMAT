//$Id:$
//------------------------------------------------------------------------------
//                              SpiceOrbitKernelReader
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under
// FDSS Task order 28.
//
// Author: Wendy C. Shoan
// Created: 2010.04.05
//
/**
 * Definition of the SpiceOrbitKernelReader, which reads SPICE SPK (kernel) files.
 * This class calls the JPL-supplied CSPICE routines to read the specified
 * SPICE file(s) and return the requested data.
 */
//------------------------------------------------------------------------------
#include "SpiceOrbitKernelReader.hpp"
#include "gmatdefs.hpp"
#include "Rvector6.hpp"
#include "Rmatrix33.hpp"
#include "A1Mjd.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include "TimeTypes.hpp"
#include "TimeSystemConverter.hpp"
#include "UtilityException.hpp"


//#define DEBUG_SPK_READING
//#define DEBUG_SPK_COVERAGE
//#define DEBUG_SPK_PLANETS

// -----------------------------------------------------------------------------
// static data
// -----------------------------------------------------------------------------
// none


// -----------------------------------------------------------------------------
// public methods
// -----------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  SpiceOrbitKernelReader()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SpiceOrbitKernelReader class
 * (default constructor).
 *
 */
//------------------------------------------------------------------------------
SpiceOrbitKernelReader::SpiceOrbitKernelReader() :
   SpiceKernelReader(),
   observingBodyNameSPICE  (NULL),
   aberrationSPICE         (NULL)
{
}

//------------------------------------------------------------------------------
//  SpiceOrbitKernelReader(const SpiceKernelReader &reader)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SpiceOrbitKernelReader class, by copying
 * the input object.
 * (copy constructor).
 *
* @param <reader> SpiceOrbitKernelReader object to copy.
  */
//------------------------------------------------------------------------------
SpiceOrbitKernelReader::SpiceOrbitKernelReader(const SpiceOrbitKernelReader &reader) :
   SpiceKernelReader(reader),
   observingBodyNameSPICE  (NULL),
   aberrationSPICE         (NULL)
{

}

//------------------------------------------------------------------------------
//  SpiceOrbitKernelReader& operator=(const SpiceOrbitKernelReader &reader)
//------------------------------------------------------------------------------
/**
 * This method copies the data from the input object to the object.
 *
 * @param <reader> the SpiceOrbitKernelReader object whose data to assign to "this"
 *                 SpiceOrbitKernelReader.
 *
 * @return "this" SpiceOrbitKernelReader with data of input SpiceOrbitKernelReader
 *         reader.
 */
//------------------------------------------------------------------------------
SpiceOrbitKernelReader& SpiceOrbitKernelReader::operator=(const SpiceOrbitKernelReader &reader)
{
   if (&reader == this)
      return *this;

   SpiceKernelReader::operator=(reader);

   observingBodyNameSPICE   = NULL;
   aberrationSPICE          = NULL;

   return *this;
}

//------------------------------------------------------------------------------
// ~SpiceOrbitKernelReader()
//------------------------------------------------------------------------------
/**
 * This method is the destructor for the SpiceOrbitKernelReader.
 *
 */
//------------------------------------------------------------------------------
SpiceOrbitKernelReader::~SpiceOrbitKernelReader()
{
}

//------------------------------------------------------------------------------
//  SpiceOrbitKernelReader* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method clones the object.
 *
 * @return new object, cloned from "this" object.
 *
 */
//------------------------------------------------------------------------------
SpiceOrbitKernelReader* SpiceOrbitKernelReader::Clone(void) const
{
   SpiceOrbitKernelReader * clonedSKR = new SpiceOrbitKernelReader(*this);

   return clonedSKR;
}

//------------------------------------------------------------------------------
// void GetCoverageStartAndEnd(const StringArray &kernels, Integer forNaifId.
//                             Real              &start,   Real    &end)
//------------------------------------------------------------------------------
/**
 * This (pure virtual) method determines the coverage for the specified object
 * over the specified kernels.
 *
 * @param kernels   the array of kernels over which to check the coverage
 * @param forNaifId the NAIF ID of the object for which coverage should
 *                  be determined
 * @param start     (output) the earliest time of coverage for the object
 *                  included in the specified kernels
 * @param end       (output) the latest time of coverage for the object
 *                  included in the specified kernels
 *
 * @note An exception is thrown if any of the kernels listed are not currently
 *       loaded into the kernel pool, and an attempt to load it fails.
 * @note The interval between the returned start and end times is not
 *       necessarily continuous.  The method checks all intervals over which
 *       there is coverage for the specified object and returns the earliest
 *       time and the latest time of coverage.  There could be gaps in
 *       coverage over that span.
 * @note The method will ignore kernels of types not containing the type of
 *       data requested, i.e. this method will ignore kernels in the list that
 *       are not of type 'spk'.
 * @note Celestial bodies must remember to include the main Solar System SPK
 *       kernel in the list of kernels passed in to this method.
 */
//------------------------------------------------------------------------------
void  SpiceOrbitKernelReader::GetCoverageStartAndEnd(StringArray       &kernels,
                                                     Integer           forNaifId,
                                                     Real              &start,
                                                     Real              &end)
{
   // first check to see if a kernel specified is not loaded; if not,
   // try to load it
   for (unsigned int ii = 0; ii < kernels.size(); ii++)
      if (!IsLoaded(kernels.at(ii)))   LoadKernel(kernels.at(ii));

   SpiceInt         idSpice     = forNaifId;
   SpiceInt         arclen      = 4;
   SpiceInt         typlen      = 5;
   bool             firstInt    = true;
   bool             idOnKernel  = false;
   ConstSpiceChar   *kernelName = NULL;
   SpiceInt         objId       = 0;
   SpiceInt         numInt      = 0;
   SpiceChar        *kernelType;
   SpiceChar        *arch;
   SpiceDouble      b;
   SpiceDouble      e;
   Real             bA1;
   Real             eA1;
   SPICEINT_CELL(ids, 200);
   SPICEDOUBLE_CELL(cover, 200000);
   char             kStr[5] = "    ";
   char             aStr[4] = "   ";

   // look through each kernel
   for (unsigned int ii = 0; ii < kernels.size(); ii++)
   {
      #ifdef DEBUG_SPK_COVERAGE
         MessageInterface::ShowMessage("Checking coverage for ID %d on kernel %s\n",
               forNaifId, (kernels.at(ii)).c_str());
      #endif
      kernelName = kernels[ii].c_str();
      // check the type of kernel
      arch        = aStr;
      kernelType  = kStr;
      getfat_c(kernelName, arclen, typlen, arch, kernelType);
      if (failed_c())
      {
         ConstSpiceChar option[] = "LONG";
         SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
         //SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
         SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
         getmsg_c(option, numChar, err);
         std::string errStr(err);
         std::string errmsg = "Error determining type of kernel \"";
         errmsg += kernels.at(ii) + "\".  Message received from CSPICE is: ";
         errmsg += errStr + "\n";
         reset_c();
         delete [] err;
         throw UtilityException(errmsg);
      }
      #ifdef DEBUG_SPK_COVERAGE
         MessageInterface::ShowMessage("Kernel is of type %s\n",
               kernelType);
      #endif
      // only deal with SPK kernels
      if (eqstr_c( kernelType, "spk" ))
      {
         spkobj_c(kernelName, &ids);
         // get the list of objects (IDs) for which data exists in the SPK kernel
         for (SpiceInt jj = 0;  jj < card_c(&ids);  jj++)
         {
            objId = SPICE_CELL_ELEM_I(&ids,jj);
            #ifdef DEBUG_SPK_COVERAGE
               MessageInterface::ShowMessage("Kernel contains data for object %d\n",
                     (Integer) objId);
            #endif
            // look to see if this kernel contains data for the object we're interested in
            if (objId == idSpice)
            {
               idOnKernel = true;
               break;
            }
         }
         // only deal with kernels containing data for the object we're interested in
         if (idOnKernel)
         {
            #ifdef DEBUG_SPK_COVERAGE
               MessageInterface::ShowMessage("Checking kernel %s for data for object %d\n",
                     (kernels.at(ii)).c_str(), (Integer) objId);
            #endif
            scard_c(0, &cover);   // reset the coverage cell
            spkcov_c (kernelName, idSpice, &cover);
            if (failed_c())
            {
               ConstSpiceChar option[] = "LONG";
               SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
               //SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
               SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
               getmsg_c(option, numChar, err);
               std::string errStr(err);
               std::string errmsg = "Error determining coverage for SPK kernel \"";
               errmsg += kernels.at(ii) + "\".  Message received from CSPICE is: ";
               errmsg += errStr + "\n";
               reset_c();
               delete [] err;
               throw UtilityException(errmsg);
            }
            numInt = wncard_c(&cover);
            #ifdef DEBUG_SPK_COVERAGE
               MessageInterface::ShowMessage("Number of intervals found =  %d\n",
                     (Integer) numInt);
            #endif
            if ((firstInt) && (numInt > 0))
            {
               wnfetd_c(&cover, 0, &b, &e);
               if (failed_c())
               {
                  ConstSpiceChar option[] = "LONG";
                  SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
                  //SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
                  SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
                  getmsg_c(option, numChar, err);
                  std::string errStr(err);
                  std::string errmsg = "Error getting interval times for SPK kernel \"";
                  errmsg += kernels.at(ii) + "\".  Message received from CSPICE is: ";
                  errmsg += errStr + "\n";
                  reset_c();
                  delete [] err;
                  throw UtilityException(errmsg);
               }
               start    = SpiceTimeToA1(b);
               end      = SpiceTimeToA1(e);
               firstInt = false;
            }
            for (SpiceInt jj = 0; jj < numInt; jj++)
            {
               wnfetd_c(&cover, jj, &b, &e);
               bA1 = SpiceTimeToA1(b);
               eA1 = SpiceTimeToA1(e);
               if (bA1 < start)  start = bA1;
               if (eA1 > end)    end   = eA1;
            }
         }

      }
   }
   if (firstInt)
   {
      std::string errmsg = "Error - no data available for body on specified SPK kernels";
      throw UtilityException(errmsg);
   }
}


//------------------------------------------------------------------------------
//  Rvector6 GetTargetState(const std::string &targetName,
//                          const A1Mjd       &atTime,
//                          const std::string &observingBodyName,
//                          const std::string &referenceFrame,
//                          const std::string &aberration)
//------------------------------------------------------------------------------
/**
 * This method returns the state of the target with respect to the observing body
 * at the input time.
 *
 * @param <targetName>        name of the target object.
 * @param <atTime>            time at which the state is requested.
 * @param <observingBodyName> name of the observing body
 * @param <referenceFrame>    frame in which state should be returned
 * @param <aberration>        flag indicating aberration corrections, if any
 *
 * @return state at the input time
 *
 */
//------------------------------------------------------------------------------
Rvector6 SpiceOrbitKernelReader::GetTargetState(const std::string &targetName,
                                 const Integer     targetNAIFId,
                                 const A1Mjd       &atTime,
                                 const std::string &observingBodyName,
                                 const std::string &referenceFrame,
                                 const std::string &aberration)
{
   #ifdef DEBUG_SPK_READING
      MessageInterface::ShowMessage(
            "Entering SPKReader::GetTargetState with target = %s, naifId = %d, time = %12.10f, observer = %s\n",
            targetName.c_str(), targetNAIFId, atTime.Get(), observingBodyName.c_str());
      Real start, end;
      GetCoverageStartAndEnd(loadedKernels, targetNAIFId, start, end);
      MessageInterface::ShowMessage("   coverage for object %s : %12.10f --> %12.10f\n",
            targetName.c_str(), start, end);
   #endif
   std::string targetNameToUse = targetName;
   if ((targetName == "Luna") || (targetName == "LUNA"))  // Luna, instead of Moon, for GMAT
      targetNameToUse        = "Moon";
   targetNameToUse           = GmatStringUtil::ToUpper(targetNameToUse);
   objectNameSPICE           = targetNameToUse.c_str();
   observingBodyNameSPICE    = observingBodyName.c_str();
   referenceFrameSPICE       = referenceFrame.c_str();
   aberrationSPICE           = aberration.c_str();
   // convert time to Ephemeris Time (TDB)
   etSPICE                   = A1ToSpiceTime(atTime.Get());
   naifIDSPICE               = targetNAIFId;
   boddef_c(objectNameSPICE, naifIDSPICE);        // CSPICE method to set NAIF ID for an object

   #ifdef DEBUG_SPK_READING
      MessageInterface::ShowMessage("SET NAIF Id for object %s to %d\n",
            targetNameToUse.c_str(), targetNAIFId);
//      MessageInterface::ShowMessage(
//            "In SPKReader::Converted (to TBD) time = %12.10f\n", etMjdAtTime);
//      MessageInterface::ShowMessage("  then the full JD = %12.10f\n",
//            (etMjdAtTime + GmatTimeConstants::JD_JAN_5_1941));
      MessageInterface::ShowMessage("So time passed to SPICE is %12.14f\n", (Real) etSPICE);
   #endif
   SpiceDouble state[6];
   SpiceDouble oneWayLightTime;
   spkezr_c(objectNameSPICE, etSPICE, referenceFrameSPICE, aberrationSPICE,
            observingBodyNameSPICE, state, &oneWayLightTime);
#ifdef DEBUG_SPK_PLANETS
   Real        ttMjdAtTime   = TimeConverterUtil::Convert(atTime.Get(), TimeConverterUtil::A1MJD,
                               TimeConverterUtil::TTMJD, GmatTimeConstants::JD_JAN_5_1941);
//   Real etJd                 = etMjdAtTime + GmatTimeConstants::JD_JAN_5_1941;
   Real ttJd                 = ttMjdAtTime + GmatTimeConstants::JD_JAN_5_1941;
   MessageInterface::ShowMessage("Asking CSPICE for state of body %s, with observer %s, referenceFrame %s, and aberration correction %s\n",
         objectNameSPICE, observingBodyNameSPICE, referenceFrameSPICE, aberrationSPICE);
   MessageInterface::ShowMessage(
         "           Body: %s   TT Time:  %12.10f  TDB Time: %12.10f   state:  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
         targetName.c_str(), ttJd, /*etJd,*/ state[0], state[1], state[2], state[3], state[4], state[5]);
#endif
   if (failed_c())
   {
//      ConstSpiceChar option[] = "SHORT"; // retrieve short error message, for now
//      SpiceInt       numChar  = MAX_SHORT_MESSAGE;
//      SpiceChar      err[MAX_SHORT_MESSAGE];
      ConstSpiceChar option[] = "LONG"; // retrieve long error message, for now
      SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
      //SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
      SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
      getmsg_c(option, numChar, err);
      std::string errStr(err);
      std::string errmsg = "Error getting state for body \"";
      errmsg += targetName + "\".  Message received from CSPICE is: ";
      errmsg += errStr + "\n";
      reset_c();
      delete [] err;
      throw UtilityException(errmsg);
   }
   #ifdef DEBUG_SPK_READING
      MessageInterface::ShowMessage(
            "In SPKReader::Called spkezr_c and got state out\n");
   #endif


   Rvector6 r6(state[0],state[1],state[2],state[3],state[4],state[5]);
   return r6;
}
