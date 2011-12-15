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

#ifndef SpiceOrbitKernelReader_hpp
#define SpiceOrbitKernelReader_hpp

#include "gmatdefs.hpp"
#include "A1Mjd.hpp"
#include "Rvector6.hpp"
#include "Rmatrix33.hpp"
#include "SpiceKernelReader.hpp"

class GMAT_API SpiceOrbitKernelReader : public SpiceKernelReader
{
public:

   SpiceOrbitKernelReader();
   SpiceOrbitKernelReader(const SpiceOrbitKernelReader &reader);
   SpiceOrbitKernelReader& operator=(const SpiceOrbitKernelReader &reader);

   virtual ~SpiceOrbitKernelReader();

   SpiceOrbitKernelReader* Clone() const;

   /// method to return the earliest and latest times of orbit data
   /// coverage for the specified object, over the specified list of
   /// SPK kernels (non-SPK kernels in the list will be ignored).
   void      GetCoverageStartAndEnd(StringArray       &kernels,
                                    Integer           forNaifId,
                                    Real              &start,
                                    Real              &end);

   /// method to return the state (position, velocity) of a planetary body or
   /// spacecraft at the specified time, with respect to the specified frame
   Rvector6  GetTargetState(const std::string &targetName,
                            const Integer     targetNAIFId,
                            const A1Mjd       &atTime,
                            const std::string &observingBodyName,
                            const Integer     observingBodyNAIFId,
                            const std::string &referenceFrame = "J2000",
                            const std::string &aberration = "NONE");



protected:

   // data converted to SPICE types, to pass into SPICE methods
   /// the observing body name (SPICE)
   ConstSpiceChar  *observingBodyNameSPICE;
   /// the aberration correction flag (SPICE)
   ConstSpiceChar  *aberrationSPICE;

};

#endif // SpiceOrbitKernelReader_hpp
