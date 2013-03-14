//------------------------------------------------------------------------------
//                                  CoordinateConverter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2004/12/27
//
/**
 * Definition of the CoordinateConverter class.
 *
 */
//------------------------------------------------------------------------------

#ifndef CoordinateConverter_hpp
#define CoordinateConverter_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "SpacePoint.hpp"
#include "A1Mjd.hpp"
#include "CoordinateSystem.hpp"
#include "Rvector.hpp"
#include "SolarSystem.hpp"

class GMAT_API CoordinateConverter
{
public:

   // default constructor
   CoordinateConverter();
   // copy constructor
   CoordinateConverter(const CoordinateConverter &coordCvt);
   // operator = 
   const CoordinateConverter& operator=(const CoordinateConverter &coordCvt);
   // destructor
   virtual ~CoordinateConverter();
   
   // initializes the CoordinateConverter
   virtual void Initialize(); 
   
   bool Convert(const A1Mjd &epoch, const Rvector &inState,
                CoordinateSystem *inCoord, Rvector &outState,
                CoordinateSystem *outCoord, 
                bool forceNutationComputation = false, bool omitTranslation = false);
   bool Convert(const A1Mjd &epoch, const Real *inState,
                CoordinateSystem *inCoord, Real *outState,
                CoordinateSystem *outCoord, 
                bool forceNutationComputation = false, bool omitTranslation = false);
   // method to return the rotation matrix used to do the last conversion
   Rmatrix33    GetLastRotationMatrix() const;
   Rmatrix33    GetLastRotationDotMatrix() const;
   Rmatrix33    GetRotationMatrixFromICRFToFK5(const A1Mjd &atEpoch);

protected:
   Rmatrix33 lastRotMatrix;
   Rmatrix33 lastRotDotMatrix;

   bool         ConvertFromBaseToBase(const A1Mjd &epoch, SolarSystem *solarSystem,
                                      const std::string &inBase,  const std::string &outBase,
                                      const Rvector &inBaseState, Rvector &outBaseState);
   bool         ConvertFromBaseToBase(const A1Mjd &epoch, SolarSystem *solarSystem,
                                      const std::string &inBase,  const std::string &outBase,
                                      const Real *inBaseState,    Real *outBaseState);
   
private:
   void         RotationMatrixFromICRFToFK5(const A1Mjd &atEpoch);
   Rmatrix33 icrfToFK5;
   Rmatrix33 icrfToFK5Dot;

};
#endif // CoordinateConverter_hpp
