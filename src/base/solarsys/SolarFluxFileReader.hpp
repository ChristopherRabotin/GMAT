//$Header$
//------------------------------------------------------------------------------
//                              SolarFluxFileReader
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Author: Waka A. Waktola
// Created: 2004/06/18
//
/**
 * File reader for solar flux binary files.
 */
//------------------------------------------------------------------------------

#ifndef SolarFluxFileReader_hpp
#define SolarFluxFileReader_hpp

#include "gmatdefs.hpp"
#include "A1Mjd.hpp"  
   
class SolarFluxFileReader
{
public:
   SolarFluxFileReader();
   virtual ~SolarFluxFileReader();
   SolarFluxFileReader(const SolarFluxFileReader& sf);
   SolarFluxFileReader& operator=(const SolarFluxFileReader& sf);
        
   Integer Load(Real a1_time, FILE *tkptr, bool new_file, GEOPARMS *tkparms);
protected:

private:    
};

#endif // SolarFluxFileReader_hpp
