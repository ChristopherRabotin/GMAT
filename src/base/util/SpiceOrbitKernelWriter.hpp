//$Id:$
//------------------------------------------------------------------------------
//                              SpiceOrbitKernelWriter
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
// Created: 2009.11.25
//
/**
 * Definition of the SpiceOrbitKernelWriter, which writes SPICE data (kernel) files.
 * This class calls the JPL-supplied CSPICE routines to write the specified
 * SPICE file.  Currently, this class write SPK files of Data Type 13 (Hermite
 * Interpolation with Unequal time steps; geometric - no aberration).  Currently,
 * each SPK file written by GMAT contains data for one and only one object
 * (currently only spacecraft objects are handled).
 *
 * Calling code must create a SpiceOrbitKernelWriter for each file that is to be
 * written.
 *
 * The output SPK file name takes the form
 *       <objName>-<yyyymmdd>-<data-type>-<n>.bsp
 * where <objName>  is the name of the object for which the SPK file is created
 *       <yyyymmdd> is the date of file creation
 *       <data-type>is the two-digit identifier for the SPK data type (see
 *                  SPK documentation at NAIF website)
 *       <n>        is the incremental file counter
 *       .bsp       is the standard file extension for binary SPK files
 *
 * This code creates a temporary text file, required in order to include META-Data
 * (commentary) in the SPK file.  The file is deleted from the system after the
 * commentary is added to the SPK file.  The name of this temporary text file
 * takes the form
 *       GMATtmpSPKcmmnt<objName>.txt
 * where <objName>  is the name of the object for which the SPK file is created
 *
 */
//------------------------------------------------------------------------------

#ifndef SpiceOrbitKernelWriter_hpp
#define SpiceOrbitKernelWriter_hpp

#include <stdio.h>
#include "gmatdefs.hpp"
#include "A1Mjd.hpp"
#include "Rvector6.hpp"
#include "Rmatrix33.hpp"
#include "FileManager.hpp"
#include "SpiceKernelWriter.hpp"
// include the appropriate SPICE C header(s)
extern "C"
{
#include "SpiceZfc.h"    // for CSPICE routines to add meta data
}

class GMAT_API SpiceOrbitKernelWriter :public SpiceKernelWriter
{
public:
   SpiceOrbitKernelWriter(const std::string       &objName,   const std::string &centerName,
                     Integer                 objNAIFId,  Integer            centerNAIFId,
                     const std::string       &fileName,  Integer            deg = 7,
                     const std::string       &frame = "J2000");
   SpiceOrbitKernelWriter(const SpiceOrbitKernelWriter &copy);
   SpiceOrbitKernelWriter& operator=(const SpiceOrbitKernelWriter &copy);
   ~SpiceOrbitKernelWriter();

   virtual SpiceOrbitKernelWriter* Clone() const;

   void        WriteSegment(const A1Mjd &start, const A1Mjd &end,
                            const StateArray &states, const EpochArray &epochs);
   void        AddMetaData(const std::string &line,  bool done = false);
   void        AddMetaData(const StringArray &lines, bool done = false);
   void        FinalizeKernel();

protected:
   /// the name of the spacecraft or body for which the SPK is created
   std::string     objectName;
   /// the name of the central body
   std::string     centralBodyName;
   /// the file (kernel) name
   std::string     kernelFileName;
   // the reference frame
   std::string     frameName;

   // data converted to SPICE types, to pass into SPICE methods
   /// the target body or spacecraft NAIF Id (SPICE)
   SpiceInt        objectNAIFId;
   /// the central body NAIF Id (SPICE)
   SpiceInt        centralBodyNAIFId;
   /// the degree of interpolating polynomials to pass to SPICE
   SpiceInt        degree;
   /// the reference frame (SPICE)
   ConstSpiceChar  *referenceFrame;
   /// handle to the SPK file to which to write the data
   SpiceInt        handle;
   /// "Basic" metadata
   StringArray     basicMetaData;
   /// Added metadata
   StringArray     addedMetaData;
   /// flag indicating whether or not a file handle has been obtained, and the file is
   /// open for writing
   bool            fileOpen;
   /// the name of the temporary text file
   std::string     tmpTxtFileName;
   /// the temporary text file
   FILE            *tmpTxtFile;
   /// flag indicating whether or not the temporary file can be written
   bool            tmpFileOK;
   /// an instance of the FileManager to handle renaming, etc.
   FileManager     *fm;

   static       std::string TMP_TXT_FILE_NAME;
   static const Integer     MAX_FILE_RENAMES;

   void     SetBasicMetaData();
   /// method used to create the temporary text file, to use to set metadata (comments)
   /// on the SPK file
   void     WriteMetaData();

private:
   // default constructor
   SpiceOrbitKernelWriter();

};

#endif // SpiceOrbitKernelWriter_hpp
