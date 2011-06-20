//$Id$
//------------------------------------------------------------------------------
//                           HarmonicGravityBody
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
 * This class returns handles data obtained form the celestial body.
 */
//------------------------------------------------------------------------------
#include "HarmonicGravityBody.hpp"
#include "ODEModelException.hpp"
#include "UtilityException.hpp"
#include "StringUtil.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// n/a

//#define DEBUG_HG_BODY
//#define DEBUG_HG_BODY_DESTRUCTION

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
HarmonicGravityBody::HarmonicGravityBody(CelestialBody* body) :
   HarmonicGravity (""),
   Body (body)
{
   #ifdef DEBUG_HG_BODY
      MessageInterface::ShowMessage("Entering constructor for HarmonicGravityBody\n");
      if (!body) MessageInterface::ShowMessage(" --- body is NULL!!!!\n");
      else       MessageInterface::ShowMessage(" --- body is %s\n", (body->GetName()).c_str());
   #endif
   if (!body)  throw ODEModelException("Cannot create HarmonicGravityBody class with a NULL body pointer.\n");
   Radius = body->GetEquatorialRadius();
   Factor = -body->GetGravitationalConstant();
   Load();
}

//------------------------------------------------------------------------------
HarmonicGravityBody::~HarmonicGravityBody()
{
   #ifdef DEBUG_HG_BODY_DESTRUCTION
      MessageInterface::ShowMessage("In HarmonicGravityBody destructor ...\n");
   #endif
}

//------------------------------------------------------------------------------
void HarmonicGravityBody::Load()
{
   #ifdef DEBUG_HG_BODY
      MessageInterface::ShowMessage("Entering Load for HarmonicGravityBody (%s)\n", (Body->GetName()).c_str());
   #endif
   Rmatrix sij(Body->GetHarmonicCoefficientsSij());
   Rmatrix cij(Body->GetHarmonicCoefficientsCij());

   Integer cn,cm,sn,sm;
   sij.GetSize(sn,sm);
   cij.GetSize(cn,cm);
   #ifdef DEBUG_HG_BODY
      MessageInterface::ShowMessage("sij = %s\n", (sij.ToString()).c_str());
      MessageInterface::ShowMessage("cij = %s\n", (cij.ToString()).c_str());
      MessageInterface::ShowMessage("sn = %d, sm = %d, cn = %d, cm = %d\n", sn, sm, cn, cm);
   #endif
   NN = GmatMathUtil::Max(sn,cn);
   MM = GmatMathUtil::Max(sm,cm);
   #ifdef DEBUG_HG_BODY
      MessageInterface::ShowMessage("NN = %d, MM = %d\n", NN, MM);
   #endif
   try
   {
      Allocate();
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("ERROR allocating arrays for HarmonicGravityBody.  Message is:\n");
      MessageInterface::ShowMessage(be.GetFullMessage());
   }
   #ifdef DEBUG_HG_BODY
      MessageInterface::ShowMessage("After call to Allocate ....\n");
   #endif
   for (Integer n=0; n<NN; n++)
   {
      for (Integer m=0; m<MM; m++)
      {
         #ifdef DEBUG_HG_BODY
            MessageInterface::ShowMessage("   attempting to set S[%d][%d] to %12.10f\n",
                  n, m, sij.GetElement(n,m));
         #endif
         S[n][m] = sij.GetElement(n,m);
         #ifdef DEBUG_HG_BODY
            MessageInterface::ShowMessage("   attempting to set C[%d][%d] to %12.10f\n",
                  n, m, cij.GetElement(n,m));
         #endif
         C[n][m] = cij.GetElement(n,m);
      }
   }
   #ifdef DEBUG_HG_BODY
      MessageInterface::ShowMessage("EXITing Load for HarmonicGravityBody\n");
   #endif
}
