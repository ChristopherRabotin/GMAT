//$Id$
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
   
   //void        SetJ2000BodyName(const std::string &toName);
   //std::string GetJ2000BodyName() const;
   //void        SetJ2000Body(SpacePoint *toBody);
   //SpacePoint* GetJ2000Body();
   
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

protected:
   Rmatrix33 lastRotMatrix;
   Rmatrix33 lastRotDotMatrix;
   /*
   Rvector internalState;
   Rmatrix33 toMJ2000RotMatrix;
   Rmatrix33 fromMJ2000Matrix;
   const Real *toData;
   const Real *fromData;
   */
   
   /// Origin for the return coordinate system (aligned with the MJ2000 Earth
   /// Equatorial coordinate system)
   //SpacePoint                     *j2000Body;  
   /// Name for the J2000 body
   //std::string                    j2000BodyName;
   
};
#endif // CoordinateConverter_hpp
