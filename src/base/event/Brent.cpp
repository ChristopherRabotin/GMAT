//$Id$
//------------------------------------------------------------------------------
//                           Brent
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Sep 20, 2011
//
/**
 * Implementation of Brent's Method used in event location and, eventually,
 * stopping conditions
 */
//------------------------------------------------------------------------------


#include "Brent.hpp"
#include "EventException.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// Brent()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
Brent::Brent() :
   RootFinder        ("BrentsMethod"),
   bisectionUsed     (true),
   epochOfStep       (-1.0),
   step              (0.0),
   oldCValue         (-1.0)
{
   bufferSize = 3;
}

//------------------------------------------------------------------------------
// ~Brent()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Brent::~Brent()
{
}

//------------------------------------------------------------------------------
// Brent(const Brent & b) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param b The original being copied here
 */
//------------------------------------------------------------------------------
Brent::Brent(const Brent & b) :
   RootFinder        (b),
   bisectionUsed     (true),
   epochOfStep       (-1.0),
   step              (0.0),
   oldCValue         (-1.0)
{
}



//------------------------------------------------------------------------------
// Brent &operator =(const Brent & b)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param b The original being copied here
 *
 * @return this instance, set to match b.
 */
//------------------------------------------------------------------------------
Brent & Brent::operator =(const Brent & b)
{
   if (this != &b)
   {
      RootFinder::operator =(b);
      bisectionUsed = true;
      epochOfStep   = -1.0;
      step          =  0.0;
      oldCValue     = -1.0;
   }

   return *this;
}



//------------------------------------------------------------------------------
// bool Brent::Initialize(GmatEpoch t0, Real f0, GmatEpoch t1, Real f1)
//------------------------------------------------------------------------------
/**
 * Prepares Brent's Method for use
 *
 * This method calls the RootFinder initialization to prepare the buffers for
 * use, then rearranges the buffers as needed and sets the third data point to
 * the first as needed by the algorithm.
 *
 * @param t0 The earlier epoch for the data.
 * @param f0 The function value at t0
 * @param t1 The later epoch for the data.
 * @param f1 The function value at t1
 *
 * @return true if initialization succeeds, false if it fails.
 */
//------------------------------------------------------------------------------
bool Brent::Initialize(GmatEpoch t0, Real f0, GmatEpoch t1, Real f1)
{
   MessageInterface::ShowMessage("Brent::Initialize(%15.9lf, %12lf, %15.9lf, "
         "%12lf) called\n", t0, f0, t1, f1);

   if (f0 * f1 >= 0.0)
      throw EventException("Error initializing Brent's method; the solution is "
            "not bracketed");

   bool retval = RootFinder::Initialize(t0, f0, t1, f1);

   if (retval)
   {
      if (buffer[0] < buffer[1])
      {
         Swap(0, 1);
      }
      epochBuffer[2] = epochBuffer[0];
      buffer[2]      = buffer[0];

      bisectionUsed  = true;        // Act as if bisection was used previously
   }

   MessageInterface::ShowMessage("Brent::Buffer Data:\n   %15.9lf  %12lf\n   "
         "%15.9lf  %12lf\n   %15.9lf  %12lf\n", epochBuffer[0], buffer[0],
         epochBuffer[1], buffer[1], epochBuffer[2], buffer[2]);


   return retval;
}


//------------------------------------------------------------------------------
// void SetValue(GmatEpoch forEpoch, Real withValue)
//------------------------------------------------------------------------------
/**
 * Adds a new data point to the algorithm, discarding the stale data
 *
 * @param forEpoch The epoch of the new data
 * @param withValue The new data value
 *
 * @return true on success.  (This Brent's method override always returns true)
 */
//------------------------------------------------------------------------------
bool Brent::SetValue(GmatEpoch forEpoch, Real withValue)
{
   bool retval = true;

   oldCValue = epochBuffer[2];
   epochBuffer[2] = epochBuffer[1];
   buffer[2] = buffer[1];

   if (buffer[0] * withValue < 0.0)
   {
      epochBuffer[1] = forEpoch;
      buffer[1] = withValue;
   }
   else
   {
      epochBuffer[0] = forEpoch;
      buffer[0] = withValue;
   }

   if (GmatMathUtil::Abs(buffer[0]) < GmatMathUtil::Abs(buffer[1]))
      Swap(0,1);

   return retval;
}

//------------------------------------------------------------------------------
// Real FindStep()
//------------------------------------------------------------------------------
/**
 * Finds the next step to take, given the data in the buffers.
 *
 * @param currentEpoch The epoch of the latest data in the buffers.  If not set,
 *        the return value is treated as absolute and the step is not converted
 *        from days to seconds from the current epoch.
 *
 * @return The next step
 */
//------------------------------------------------------------------------------
Real Brent::FindStep(const GmatEpoch currentEpoch)
{
   Real diffAB, diffAC, diffBC;

   diffAB = buffer[0] - buffer[1];
   if ((buffer[0] != buffer[2]) && (buffer[1] != buffer[2]))
   {
      diffAC = buffer[0] - buffer[2];
      diffBC = buffer[1] - buffer[2];
      // Inverse quadratic interpolation
      epochOfStep = epochBuffer[0] * buffer[1] * buffer[2] / ((diffAB*diffAC)) +
            epochBuffer[1] * buffer[0] * buffer[2] / ((-diffAB*diffBC)) +
            epochBuffer[2] * buffer[0] * buffer[1] / ((diffAC*diffBC));
   }
   else
   {
      // Secant method
      epochOfStep = epochBuffer[1] - buffer[1] *
            (epochBuffer[0] - epochBuffer[1])/diffAB;
   }

   // Figure out if we need to drop back to bisection
   Real delta = 1.0e-8;    // Numerical tolerance for epochs; set to ~1 msec
   Real deltaC, bMinusC, sMinusB;

   deltaC  = GmatMathUtil::Abs(epochBuffer[2] - oldCValue);
   bMinusC = GmatMathUtil::Abs(epochBuffer[1]-epochBuffer[2]);
   sMinusB = GmatMathUtil::Abs(epochOfStep - epochBuffer[1]);

   if ( ((epochOfStep >= (3.0 * epochBuffer[0] + epochBuffer[1]) / 4.0) &&
         (epochOfStep <= epochBuffer[1])) ||
        (bisectionUsed && (sMinusB >= bMinusC / 2.0)) ||
        (!bisectionUsed && (sMinusB >= deltaC / 2.0)) ||
        (bisectionUsed && (bMinusC < delta)) ||
        (!bisectionUsed && deltaC < delta) )
   {
      // Drop back to bisection. Sigh.
      epochOfStep = 0.5 * (epochBuffer[0] + epochBuffer[1]);
      bisectionUsed = true;
   }
   else
      bisectionUsed = false;

   // Get the step in seconds to the epochOfStep if input in days was set
   if (currentEpoch != -1.0)
      step = (epochOfStep - currentEpoch) * GmatTimeConstants::SECS_PER_DAY;
   else
      step = epochOfStep;

   MessageInterface::ShowMessage("Brent's Method: Current Epoch: %15.9lf, "
         "Epoch of Step: %15.9lf, step: %15.9lf\n", currentEpoch, epochOfStep,
         step);

   return step;
}



//bool Brent::FindRoot(Integer whichOne, GmatEpoch roots[2])
//{
//}


