//------------------------------------------------------------------------------
//                                  SPADFileReader
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
// Author: Wendy Shoan / NASA
// Created: 2014.01.07
//
/**
 * Reads a SPAD file, and stores the data.
 * NOTE: Currently only reads azimuth, elevation, and X, Y, Z
 * @todo Add assumptions and general file requirements here
 */
//------------------------------------------------------------------------------
#ifndef SPADFileReader_hpp
#define SPADFileReader_hpp

#include "Rvector3.hpp"
#include "gmatdefs.hpp"

class GMAT_API SPADFileReader
{
public:
   /// class methods
   SPADFileReader();
   SPADFileReader(const SPADFileReader &copy);
   SPADFileReader& operator=(const SPADFileReader &copy);

   virtual ~SPADFileReader();

   virtual SPADFileReader* Clone() const;

   /// Initialize will parse the file, and validate the data
   virtual void            Initialize();

  /// Set the SPAD file name
   virtual bool            SetFile(const std::string &theSPADFile);

   /// Get the SRP area given the input sun vector; interpolate data
   /// if necessary
   virtual Rvector3        GetSRPArea(Rvector3 sunVector);

   /// Other methods should be added here, to return requested data
   /// TBD

protected:

   struct SPADDataRecord
   {
      Real     azimuth;     // degrees
      Real     elevation;   // degrees
      Rvector3 force;       // km^2
      // add to this as necessary

      SPADDataRecord(Real az = -999, Real el = -999, Rvector3 f = Rvector3())
      {
         azimuth   = az;
         elevation = el;
         force     = f;
      }
      SPADDataRecord(const SPADDataRecord &copy)
      {
         azimuth   = copy.azimuth;
         elevation = copy.elevation;
         force     = copy.force;
      }
   };

   struct SPADMotionRecord
   {
      Integer     id;
      std::string nombre;
      std::string method;
      Real        itsMin;
      Real        itsMax;
      Real        itsStep;

      SPADMotionRecord(Integer theId = -999, const std::string &theName = "",
                       const std::string &theMethod = "", Real theMin = -999.0,
                       Real theMax = -999.0, Real theStep = -999.0)
      {
         id       = theId;
         nombre   = theName;
         method   = theMethod;
         itsMin   = theMin;
         itsMax   = theMax;
         itsStep  = theStep;
      }
      SPADMotionRecord(const SPADMotionRecord &copy)
      {
         id       = copy.id;
         nombre   = copy.nombre;
         method   = copy.method;
         itsMin   = copy.itsMin;
         itsMax   = copy.itsMax;
         itsStep  = copy.itsStep;
      }
   };

   // Required header fields
   std::string    analysisType;
   Real           pressure;   // scale factor

   // Required meta fields
   // See SPADMotion struct above
   // Meta data Record Count
   Integer        recordCount;

   // The file name
   std::string spadFile;
   /// Has the file been read and the data stored and validated?
   bool        isInitialized;
   /// The number of data records
   Integer     numData;
   /// How many different azimuth values are there?
   Integer     azCount;
   /// How many different elevation values are there?
   Integer     elCount;
   /// Azimuth step size
   Integer     azStepSize;
   /// Elevation step size
   Integer     elStepSize;

   // Store a vector of record pointers
   std::vector<SPADDataRecord*>   spadData;

   /// Store a vector of meta "Motion" data
   std::vector<SPADMotionRecord*> spadMotion;

   /// the meta data

   /// Create a new record and add it to the data store
   virtual bool     AddDataRecord(Real az, Real el, Real x, Real y, Real z);
   /// Create a new record and add it to the data store
   virtual bool     AddDataRecord(Real az, Real el, Rvector3 f);
   /// Parse the file, validate, and store the data
   virtual bool     ParseFile();
   /// Check to make sure the meta data is valid
   virtual void     ValidateMetaData();
   /// Check to make sure the data is valid and as expected
   virtual void     ValidateData();
   /// Is the current line a dashed line?
   virtual bool     IsDashedLine(const std::string &theLine);
   /// Interpolate the data
   virtual Real     Interpolate(Real x, Real x1, Real x2, Real y1, Real y2);
   /// Get the force data at the record with the specified azimuth and Elevation
   virtual Rvector3 GetForceAt(Real azVal, Real elVal);
};

#endif // SPADFileReader_hpp
