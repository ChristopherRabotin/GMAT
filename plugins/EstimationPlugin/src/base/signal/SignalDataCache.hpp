//$Id$
//------------------------------------------------------------------------------
//                           SignalDataCache
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: 
// Created: July 10, 2019
/**
 * Data Cache used in the measurement modeling subsystem
 */
 //------------------------------------------------------------------------------

#ifndef SignalDataCache_hpp
#define SignalDataCache_hpp

#include "estimation_defs.hpp"
#include "Rvector3.hpp"
#include "Rmatrix.hpp"
#include "Rvector6.hpp"
#include "GmatTime.hpp"

#include <unordered_map>

// Forward references
class SignalData;

/**
 * The SignalDataCache class is a structure for caching SignalData 
 */
class ESTIMATION_API SignalDataCache {

public:

   /// Helper participant list hasher
   static unsigned long StrandToHash(const StringArray* participants);

   /// Cache key for signal data
   struct ESTIMATION_API CacheKey {
      const unsigned long strand;
      const Real          freq; 
      const Real          epoch1;
      const Real          epoch2;

      CacheKey(unsigned long strandId, Real aFreq, Real aEpoch1, Real aEpoch2);

      bool operator==(const CacheKey& k) const;
   };

   /// Cache value for the light time solution
   struct ESTIMATION_API CacheValue {

      const GmatTime tPrecTime;
      const GmatTime rPrecTime;
      const Rvector3 tLoc;
      const Rvector6 tOStateSSB;
      const Rvector3 rLoc;
      const Rvector6 rOStateSSB;
      const Rvector3 tVel;
      const Rvector3 rVel;
      const Rmatrix tSTM;
      const Rmatrix rSTM;
      const RealArray ionoCorrection;

      CacheValue(const SignalData& sd, const RealArray & ic);

   };

   /// Cache hasher based on simple xor accumulator and bit shifting 
   struct ESTIMATION_API CacheKeyHasher
   { 
      size_t operator()(const CacheKey& k) const;
   };

   typedef std::unordered_map<CacheKey, CacheValue, CacheKeyHasher> SimpleSignalDataCache;

   typedef std::unordered_map<SignalDataCache::CacheKey, SignalDataCache::CacheValue, SignalDataCache::CacheKeyHasher>::iterator SimpleSignalDataCacheIter;

};

#endif /* SignalDataCache_hpp */