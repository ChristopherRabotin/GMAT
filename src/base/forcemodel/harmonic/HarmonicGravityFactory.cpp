//$Id$
//------------------------------------------------------------------------------
//                           HarmonicGravityFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: John P. Downing/GSFC/595
// Created: 2010.10.28
// (modified for style, etc. by Wendy Shoan/GSFC/583 2011.05.31)
//
/**
 * This is the class that creates and returns a pointer to the
 * appropriate HarmonicGravity sub-class.
 * NOTE: this is a static class.
 */
//------------------------------------------------------------------------------
#include "HarmonicGravityFactory.hpp"
#include "HarmonicGravityCof.hpp"
#include "HarmonicGravityGrv.hpp"
//#include "HarmonicGravityEgm96dat.hpp"
#include "HarmonicGravityBody.hpp"
#include "ODEModelException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_CREATE_DELETE
//#define DEBUG_CACHE

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
std::vector<HarmonicGravity*> HarmonicGravityFactory::cache;
GravityFile*                  HarmonicGravityFactory::gravFile = NULL;

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

HarmonicGravityFactory::HarmonicGravityFactory()
{
}

HarmonicGravityFactory::~HarmonicGravityFactory()
{
   EmptyCache();
}


//------------------------------------------------------------------------------
HarmonicGravity* HarmonicGravityFactory::Create(const std::string& filename,
                                                const Real& radius, const Real& mukm)
{
   if (!gravFile) gravFile = new GravityFile();

   for (std::vector<HarmonicGravity*>::iterator objptPos = cache.begin();
        objptPos != cache.end(); ++objptPos)
   {
      if ((*objptPos)->GetFilename() == filename)
      {
         return (*objptPos); // When are the pointers in cache deleted?
      }
   }
   HarmonicGravity         *newOne  = NULL;
   GmatFM::GravityFileType fileType = gravFile->GetFileType(filename);
   switch (fileType)
   {
      case GmatFM::GFT_COF:
         newOne = new HarmonicGravityCof(filename,radius,mukm);
         break;
//      case GmatFM::GFT_DAT:
//         newOne = new HarmonicGravityEgm96dat(filename,radius,mukm);
//         break;
      case GmatFM::GFT_GRV:
         newOne = new HarmonicGravityGrv(filename,radius,mukm);
         break;
      case GmatFM::GFT_UNKNOWN:
         throw ODEModelException
            ("HarmonicGravityFactory::Create file not found or incorrect type\n");
      default:
         return NULL;
   }

   if (!newOne)
   {
      std::string errmsg = "HarmonicGravity object for gravity file ";
      errmsg += filename + " cannot be created.\n";
      throw ODEModelException
         (errmsg);
   }
   cache.push_back(newOne);
   #ifdef DEBUG_CREATE_DELETE
      MessageInterface::ShowMessage("Just created a HarmonicGravity file object for filename %s\n",
            filename.c_str());
   #endif
   return newOne;  // When are the pointers in cache deleted?
}

//------------------------------------------------------------------------------
HarmonicGravity* HarmonicGravityFactory::Create(CelestialBody* body)
{
   if (body != NULL)
   {
      #ifdef DEBUG_CREATE_DELETE
         MessageInterface::ShowMessage("Just created a HarmonicGravity body object for body %s\n",
               (body->GetName()).c_str());
      #endif
      return new HarmonicGravityBody(body);
   }
   return NULL;
}


void HarmonicGravityFactory::EmptyCache()
{
   if (cache.empty()) return;

   Integer cacheSize = cache.size();
   #ifdef DEBUG_CACHE
      MessageInterface::ShowMessage("Emptying HarmonicGravityFactory::cache (size = %d) ...................\n", cacheSize);
   #endif
   for (Integer ii = cacheSize - 1; ii >= 0; ii--)
   {
      if (cache.at(ii) != NULL)
      {
         #ifdef DEBUG_CACHE
            MessageInterface::ShowMessage("Deleting HarmonicGravityFactory::cache[%d] for filename %s ...................\n",
                  ii, (cache[ii]->GetFilename()).c_str());
         #endif
         delete cache[ii];
         cache[ii] = NULL;
      }
   }
   cache.clear();
}

