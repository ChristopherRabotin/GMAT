//$Id$
//------------------------------------------------------------------------------
//                              SolarFluxFileReader
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Waka A. Waktola
// Created: 2004/06/18
//
/**
 * File reader for solar flux binary files.
 * Load() from Windows Swingby code.
 */
//------------------------------------------------------------------------------

#include "SolarFluxFileReader.hpp"
#include "MessageInterface.hpp"
#include <fstream>                   // for fseek(), fread()

//------------------------------------------------------------------------------
//  SolarFluxFileReader()
//------------------------------------------------------------------------------
/**
 *  Constructor.
 */
//------------------------------------------------------------------------------
SolarFluxFileReader::SolarFluxFileReader()
{
}

//------------------------------------------------------------------------------
//  ~SolarFluxFileReader()
//------------------------------------------------------------------------------
/**
 *  Destructor.
 */
//------------------------------------------------------------------------------
SolarFluxFileReader::~SolarFluxFileReader()
{
}

//------------------------------------------------------------------------------
//  SolarFluxFileReader(const SolarFluxFileReader& am)
//------------------------------------------------------------------------------
/**
 *  Copy constructor.
 */
//------------------------------------------------------------------------------
SolarFluxFileReader::SolarFluxFileReader(const SolarFluxFileReader& sf)
{
   
}

//------------------------------------------------------------------------------
//  Integer LoadSolarFluxFile
//                 (Real a1_time, FILE *tkptr, bool new_file, 
//                  GEOPARMS *geoParams)
//------------------------------------------------------------------------------
/**
 *  To access the Jacchia-Roberts binary data file using a time index value.  
 *  Unpacks the 3-hour magnetic indices array and retrieves the values for 20 
 *  days of the night- time global exospheric temperatures.  Returns a magnetic
 *  index value and temperature.
 * 
 *  @param <a1_time>  A1 time value which serves as an index into the Jacchia-
 *                    Roberts file.
 *  @param <*tkptr>   File pointer to the Jacchia-Roberts file.
 *  @param <new_file> If true, flush static data for file
 * 
 *  @param <*geoParams> Pointer to a data structure containing values for a
 *                    minimum global exospheric temperature and an unpacked
 *                    3-hour geomagnetic index value retrieved from the Jacchia
 *                    -Roberts file.
 *  @return status
 *
 * Modification -
 * Name              Date           Description
 * ----              ----           -----------
 * M. Zwick          12/14/92       Original C code converted from
 *                                  preexisting Fortran code.
 *                                  (Release 1 SMR 23)
 * D. Ginn           1/26/94        DSPSE OPS:  Added new_file argument, logic
 * W. Waktola        06/08/04       Code 583: Renamed from jaccwf() to 
 *                                            LoadSolarFluxFile()
 * 
 * Notes:  This function was converted from preexisting Fortran code
 *         written by Ken Aronson of CSC on 09/25/86.
 */ 
//------------------------------------------------------------------------------
Integer SolarFluxFileReader::LoadSolarFluxFile(Real a1_time, FILE *tkptr, 
                                               bool new_file, GEOPARMS *geoParams)
{
   //@todo set initial value of it
   //subtracting uninitialized it from day below, so initialize to 0
   //Integer it;
   Integer it = 0;      // Time of first day of TC data
   Integer kp[21][8];   // Magnetic activity, 3-hour indices
   Real tc[20];         // 20 days of nighttime minimum exospheric
                        // temperatures, starting with day IT.

   Integer inbuf[4], idrec1, ita, rec_num, nrec, lgd, byte_offset;
   Integer day, diff, i3_hour, i3_day;
   Integer status, i, j, n, kp1[21][4];
   Real hour, tp1;

   status = 0;               // Initialize status flag to zero
   day = (Integer)a1_time;   // Assign time as an Integer value
   diff = day - it;          // Compare input time to stored time
   
   // If input time out of range of stored time
   if (new_file || diff < 0 || diff > 19)
   {                          
      //   retrieve the requested time record
      new_file = false;
      
      // Go to 1st position on Record.
      if (fseek(tkptr, 0, SEEK_SET) != 0) // If error reading
      {
         status = -1;  // 1st record
      }
      else
      {   
          // Read the 1st record
         if(fread( (void *) inbuf, sizeof(Integer), (size_t)4, tkptr) == 4)
         {
            idrec1 = inbuf[0]; // Obtain 1st time point in file
            nrec = inbuf[2];   // Obtain # of records in file
            lgd = 100000;      // Obtain last good data point in file

            idrec1 /= 20;      // Compute record # in which calling time occurs
            idrec1 *= 20;

            // Assign time as an Integer for record computation.
            ita = (Integer)a1_time;
            
            if (ita < idrec1 || ita > lgd) // If input time out of bounds for
            {
               status = -5;                // record times, set error status
            }
            else
            {
               rec_num = (ita - idrec1) / 20 + 1;  // Compute record number

               if (rec_num > nrec)  // If input record # > maximum number of      
               {                    // records in file, set error status and exit
                  status = -6;      
               }
               else
               {
                  byte_offset = rec_num * 252; // 252 bytes per record
                  if (fseek (tkptr, byte_offset, SEEK_SET) != 0)
                  {
                     status = -2;
                  }
                  else
                  {
                     // First, get time of 1st day of TC data. 
                     // Second, get magnetic activity 3-hour indices.
                     // Last, get 20 days of nighttime global exospheric temperatures.
                     if(fread((void *)&it, sizeof(Integer), (size_t)1, tkptr) == 1)
                     {
                        for(j = 0; j < 4; ++j)      // Read words in column-major
                           for (i = 0; i < 21; ++i)
                              // Intentionally get the return and then ignore it
                              // to move warning from system libraries to GMAT
                              // code base.  The "unused variable" warning here
                              // can be safely ignored.
                              size_t len = fread((void *)&kp1[i][j], sizeof(Integer), (size_t)1, tkptr);
                        if(fread((void *)tc, sizeof(Real), (size_t)20, tkptr) == 20)
                        {
                           for (i = 0; i < 21; ++i)     // Copy into 21 by 8 buffer to
                              for (j = 0; j < 4; ++j)   // unpack Geomagnetic 3-hour
                                 kp[i][j] = kp1[i][j];  // indices array

                           for (i = 0; i < 21; ++i)
                           {
                              kp[i][6] = kp[i][3] / 100;
                              kp[i][7] = kp[i][3] - kp[i][6]*100;
                              kp[i][4] = kp[i][2] / 100;
                              kp[i][5] = kp[i][2] - kp[i][4]*100;
                              kp[i][2] = kp[i][1] / 100;
                              kp[i][3] = kp[i][1] - kp[i][2]*100;
                              n = kp[i][0];
                              kp[i][0] = kp[i][0] / 100;
                              kp[i][1] = n - kp[i][0] * 100;
                           }
                        }
                        else if(ferror(tkptr) != 0)       // Error reading 20-day
                        {
                           status = -20;                  // temperature array 
                        }
                     }
                     else if(ferror(tkptr) != 0)           // Error reading 1st day
                     {
                        status = -1;                       // of TC data 
                     }
                  }
               }
            }   
         }
         else if (ferror(tkptr) != 0)    // Error reading 1st record 
         {
            status = -4;
         }
      }
   }

   // If no errors to this point, retrieve geomagnetic index value and 
   // exothermic temperature for this day and 3-hour period 
   if(status == 0)
   {
      i = day - it;
      geoParams->xtemp = tc[i];
      day = (Integer)(a1_time - 0.28);
      hour = (Integer)((a1_time - 0.28 - day) * 24.0);
      i3_hour = (Integer)(hour / 3.0);
      i = day - it;
      i3_day  = i + 1;
      tp1 = (kp[i3_day][i3_hour] * 3 + 5) / 10;
      geoParams->tkp = tp1 / 3.0;
   }
   return status;
}

//------------------------------------------------------------------------------
//  OpenSolarFluxFile(std::string filename)
//------------------------------------------------------------------------------
/**
 *  Open solar flux file.
 *
 *  @return true if successful, false otherwise
 */
//------------------------------------------------------------------------------
FILE* SolarFluxFileReader::OpenSolarFluxFile(std::string filename)
{
   FILE *file;
   
   file = fopen(filename.c_str(), "rb");
   
   if (file != NULL)
      return file;
   else
      return NULL;    
}

//------------------------------------------------------------------------------
//  bool CloseSolarFluxFile()
//------------------------------------------------------------------------------
/**
 *  Close file correctly so that it becomes available again
 *
 *  @return true if successful, false otherwise
 */
//------------------------------------------------------------------------------
bool SolarFluxFileReader::CloseSolarFluxFile(FILE* tkptr)
{
   Integer status = fclose(tkptr);
   
   if (status == 0)
      return true;
   else
      return false;
}

