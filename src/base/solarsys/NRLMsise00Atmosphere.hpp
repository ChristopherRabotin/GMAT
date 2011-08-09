//$Id: NRLMsise00Atmosphere.hpp 9485 2011-07-18 00:11:14Z tdnguye2 $
//------------------------------------------------------------------------------
//                              NRLMsise00Atmosphere
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Tuan Dang Nguyen
// Created: 2011/07/18
//
/**
 * The NRLMSISE00 atmosphere
 */
//------------------------------------------------------------------------------


#ifndef NRLMsise00Atmosphere_hpp
#define NRLMsise00Atmosphere_hpp

#include "AtmosphereModel.hpp"
#include "TimeTypes.hpp"

class GMAT_API NRLMsise00Atmosphere : public AtmosphereModel
{
public:
   NRLMsise00Atmosphere(const std::string &name = "");
   virtual ~NRLMsise00Atmosphere();

   NRLMsise00Atmosphere(const NRLMsise00Atmosphere& msise);
   NRLMsise00Atmosphere& operator=(const NRLMsise00Atmosphere& msise);

   bool  Density(Real *position, Real *density,
	             Real epoch = GmatTimeConstants::MJD_OF_J2000,
	             Integer count = 1);

   virtual GmatBase* Clone() const; // inherited from GmatBase

protected:
   /// Flag to indicate if data comes from a file
   bool                    fileData;
   /// Name of the file
   std::string             fluxfilename;
   /// Second of day
   Real                    sod;
   /// Year + Day of year, in the form YYYYDDD
   Integer                 yd;
   /// Value of F10.7 to use
   Real                    f107;
   /// 3 month average of the F10.7 data
   Real                    f107a;
   /// Geomagnetic index (Ap, not Kp)
   Real                    ap[7];

   Integer                 mass;

   void                    GetInputs(Real epoch);

};

#endif /* NRLMsise00Atmosphere_hpp */
