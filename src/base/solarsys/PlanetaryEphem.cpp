//$Id$
//------------------------------------------------------------------------------
//                                  PlanetaryEphem
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan
// Created: 2004/02/18
//
/**
 * Implementation of the PlanetaryEphem class.
 *
 * @note This class was created (basically) from Swingby code.  Major mods were:
 * - changed #defines to static const parameters
 * - removed _MAX_PATH (not standard on platforms) - use new static
 *   const MAX_PATH_LEN instead
 */
//------------------------------------------------------------------------------

#include "PlanetaryEphem.hpp"
#include "TimeTypes.hpp"
#include <cstdio>  // Fix for header rearrangement in gcc 4.4

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  PlanetaryEphem(std::string withFileName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the PlanetaryEphem class
 * (default constructor).
 *
 * @param <name> parameter indicating the full path name of the File.
 */
//------------------------------------------------------------------------------
PlanetaryEphem::PlanetaryEphem(std::string withFileName) :
   jdMjdOffset (GmatTimeConstants::JD_JAN_5_1941),
   itsName     (withFileName)
{
   strcpy(g_pef_dcb.full_path,withFileName.c_str());
   g_pef_dcb.recl           = 0;
   g_pef_dcb.fptr           = NULL;
}

//------------------------------------------------------------------------------
//  PlanetaryEphem(const PlanetaryEphem& pef)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the PlanetaryEphem class
 * (constructor).
 *
 * @param <pef> PlanetaryEphem object whose values to copy to create a new
 *              PlanetaryEphem.
 */
//------------------------------------------------------------------------------
PlanetaryEphem::PlanetaryEphem(const PlanetaryEphem& pef) :
   jdMjdOffset   (pef.jdMjdOffset),
   itsName       (pef.itsName),
   g_pef_dcb     (pef.g_pef_dcb)
{
}

//------------------------------------------------------------------------------
//  PlanetaryEphem& operator= (const PlanetaryEphem& pef)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the PlanetaryEphem class.
 *
 * @param <pef> the PlanetaryEphem object whose data to assign to "this"
 *            File.
 *
 * @return "this" PlanetaryEphem with data of input PlanetaryEphem pef.
 */
//------------------------------------------------------------------------------
PlanetaryEphem& PlanetaryEphem::operator=(const PlanetaryEphem& pef)
{
   if (this == &pef) return *this;
   jdMjdOffset = pef.jdMjdOffset;
   itsName     = pef.itsName;
   g_pef_dcb   = pef.g_pef_dcb;
   return *this;
}

//------------------------------------------------------------------------------
//  ~PlanetaryEphem()
//------------------------------------------------------------------------------
/**
 * Destructor for the PlanetaryEphem class.
 */
//------------------------------------------------------------------------------
PlanetaryEphem::~PlanetaryEphem()
{
   if (g_pef_dcb.fptr)  fclose(g_pef_dcb.fptr);   
}

//------------------------------------------------------------------------------
//  std::string GetName()
//------------------------------------------------------------------------------
/**
 * Returns the name of the file (full path name).
 *
 * @return the full path name of the file.
 */
//------------------------------------------------------------------------------
std::string PlanetaryEphem::GetName() const
{
   return itsName;
}
