//$Id$
//------------------------------------------------------------------------------
//                           RootFinder
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
// Created: Sep 19, 2011
//
/**
 * Implementation of the RootFinder base class
 */
//------------------------------------------------------------------------------


#include "RootFinder.hpp"
#include "EventException.hpp"
#include "PropSetup.hpp"
#include "Spacecraft.hpp"
//#include "Formation.hpp"
#include "MessageInterface.hpp"



//------------------------------------------------------------------------------
// RootFinder(std::string finderType) :
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
RootFinder::RootFinder(std::string finderType) :
   typeName             (finderType),
   tolerance            (1.0e-5),
   bufferSize           (2),        // Default to 2 point buffers
   buffer               (NULL),
   epochBuffer          (NULL),
   propagator           (NULL),
   maxAttempts          (50),
   events               (NULL)
{
}

//------------------------------------------------------------------------------
// ~RootFinder()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RootFinder::~RootFinder()
{
   if (buffer != NULL)
      delete [] buffer;
   if (epochBuffer != NULL)
      delete [] epochBuffer;
}

//------------------------------------------------------------------------------
// RootFinder(const RootFinder & rf) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rf The RootFinder being copied
 */
//------------------------------------------------------------------------------
RootFinder::RootFinder(const RootFinder & rf) :
   typeName             (rf.typeName),
   tolerance            (rf.tolerance),
   bufferSize           (rf.bufferSize),
   buffer               (NULL),
   epochBuffer          (NULL),
   propagator           (NULL),
   maxAttempts          (rf.maxAttempts),
   events               (NULL)
{
}

//------------------------------------------------------------------------------
// RootFinder & operator=(const RootFinder & rf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
RootFinder & RootFinder::operator=(const RootFinder & rf)
{
   if (this != &rf)
   {
      typeName            = rf.typeName;
      tolerance           = rf.tolerance;
      bufferSize          = rf.bufferSize;
      if (buffer != NULL)
         delete [] buffer;
      buffer              = NULL;
      if (epochBuffer != NULL)
         delete [] epochBuffer;
      epochBuffer         = NULL;
      propagator          = NULL;
      maxAttempts         = rf.maxAttempts;

      satBuffer.clear();
      formBuffer.clear();
      events = NULL;
   }

   return *this;
}


//------------------------------------------------------------------------------
// bool Initialize(GmatEpoch t0, Real f0, GmatEpoch t1, Real f1)
//------------------------------------------------------------------------------
/**
 * Prepares the root finder for use
 *
 * This method removes stale buffers and allocates new ones for use by root
 * finding algorithms derived from this one.  The buffer and epochBuffer arrays
 * are loaded with the data passed into the call.
 *
 * The input parameters are preferred to be time ordered, with t0 before t1.  If
 * the time ordering is reversed, the method flips them to ensure time ordering.
 * The case of t0 == t1 is an error, and throws an exception.
 *
 * @param t0 The earlier epoch for the data.
 * @param f0 The function value at t0
 * @param t1 The later epoch for the data.
 * @param f1 The function value at t1
 *
 * @return true if initialization succeeds, false if it fails.
 */
//------------------------------------------------------------------------------
bool RootFinder::Initialize(GmatEpoch t0, Real f0, GmatEpoch t1, Real f1)
{
   bool retval = false;

   if (t0 == t1)
      throw EventException("Error initializing the " + typeName +
            " root finder; the bounding epochs are identical.");
   if (bufferSize > 0)
   {
      if (buffer != NULL)
         delete [] buffer;

      if (epochBuffer != NULL)
         delete [] epochBuffer;

      buffer = new Real[bufferSize];
      epochBuffer = new GmatEpoch[bufferSize];

      if (t0 > t1)
      {
         GmatEpoch tempT;
         Real tempF;

         tempT = t0;
         t0 = t1;
         t1 = tempT;
         tempF = f0;
         f0 = f1;
         f1 = tempF;
      }

      epochBuffer[0] = t0;
      buffer[0]      = f0;
      epochBuffer[1] = t1;
      buffer[1]      = f1;

      // Fill the remaining data with default values
      for (Integer i = 2; i < bufferSize; ++i)
      {
         buffer[i] = 0.0;
         epochBuffer[i] = -1.0;
      }

      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// Real GetStep()
//------------------------------------------------------------------------------
/**
 * Retrieves the next time interval to step given the current buffer data.
 *
 * The default implementation calls FindStep() and returns the result
 *
 * @return The time step, in seconds
 */
//------------------------------------------------------------------------------
Real RootFinder::GetStep(const GmatEpoch currentEpoch)
{
   return FindStep(currentEpoch);
}



//------------------------------------------------------------------------------
// bool SetValue(GmatEpoch forEpoch, Real withValue)
//------------------------------------------------------------------------------
/**
 * Adds data to the buffers
 *
 * The default implementation rolling out the [0] values if the buffer is full
 *
 * @param forEpoch The epoch of the new data value
 * @param withValue The value at the epoch forEpoch
 *
 * @return true if the data was added, false if not
 */
//------------------------------------------------------------------------------
bool RootFinder::SetValue(GmatEpoch forEpoch, Real withValue)
{
   bool retval = false, bufferFull = true;
   Integer index = bufferSize - 1;

   if (epochBuffer[index] == -1.0)
   {
      bufferFull = false;
      for (UnsignedInt i = bufferSize-1; i >= 0; --i)
      {
         if (epochBuffer[i] == -1.0)
         {
            index = i;
         }
      }
   }

   if (bufferFull)
   {
      for (Integer i = 1; i < bufferSize; ++i)
      {
         epochBuffer[i-1] = epochBuffer[i];
         buffer[i-1]      = buffer[i];
      }
      epochBuffer[bufferSize-1] = forEpoch;
      buffer[bufferSize-1]      = withValue;
      retval = true;
   }
   else
   {
      epochBuffer[index] = forEpoch;
      buffer[index]      = withValue;
      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// void Swap(Integer i1, Integer i2)
//------------------------------------------------------------------------------
/**
 * Exchanges two members of the buffers
 *
 * @param i1 Index of the first member
 * @param i2 Index of the second member
 */
//------------------------------------------------------------------------------
void RootFinder::Swap(Integer i1, Integer i2)
{
   if ((i1 < 0) || (i2 < 0))
      return;

   if ((i1 < bufferSize) && (i2 < bufferSize))
   {
      GmatEpoch tt;
      Real tf;

      tt = epochBuffer[i1];  epochBuffer[i1] = epochBuffer[i2];
            epochBuffer[i2] = tt;
      tf = buffer[i1];  buffer[i1] = buffer[i2]; buffer[i2] = tf;
   }
}


//------------------------------------------------------------------------------
// Real GetStepMeasure()
//------------------------------------------------------------------------------
/**
 * Returns the time difference, in seconds, used as a measure of convergence
 *
 * The default returns the difference between elements 0 an 1 in the epochBuffer
 *
 * @return The time measure, in seconds
 */
//------------------------------------------------------------------------------
Real RootFinder::GetStepMeasure()
{
      MessageInterface::ShowMessage("Returning epoch measure %.12lf, from "
            "epochs %.12lf and %.12lf\n", 
            GmatMathUtil::Abs(epochBuffer[1] - epochBuffer[0]) * 
            GmatTimeConstants::SECS_PER_DAY, epochBuffer[0], epochBuffer[1]);

   return GmatMathUtil::Abs(epochBuffer[1] - epochBuffer[0]) * 
      GmatTimeConstants::SECS_PER_DAY;
}


//------------------------------------------------------------------------------
// void GetBrackets(GmatEpoch &start, GmatEpoch &end)
//------------------------------------------------------------------------------
/**
 * Retrieves the bracketing epochs from the epoch buffer.
 *
 * @param start The epoch earlier than the zero value
 * @param end The epoch later than the zero value
 */
//------------------------------------------------------------------------------
void RootFinder::GetBrackets(GmatEpoch &start, GmatEpoch &end)
{
   start = (epochBuffer[0] < epochBuffer[1] ? epochBuffer[0] : epochBuffer[1]);
   end   = (epochBuffer[0] > epochBuffer[1] ? epochBuffer[0] : epochBuffer[1]);
}

//void RootFinder::SetEventVector(std::vector<Event*> *evs)
//{
//   events = evs;
//}
//
//void RootFinder::SetPropSetup(PropSetup *ps)
//{
//   if (!ps->IsOfType(Gmat::PROP_SETUP))
//      throw EventException("Error setting the Propagator for the " + typeName +
//            "rootfinder; it must be a PropSetup object");
//   propagator = ps;
//}

//std::vector<GmatEpoch> RootFinder::LocateEvents(const IntegerArray &whichOnes)
//{
//   std::vector<GmatEpoch> retval;
//
//   // Save the satellite state data
//   BufferSatelliteStates(true);
//
//   GmatEpoch roots[2];
//
//   //Now find the event locations, one at a time
//   for (UnsignedInt i = 0; i < whichOnes.size(); ++i)
//   {
//      roots[0] = roots[1] = -1.0;
//      FindRoot(whichOnes[i], roots);
//
//      retval.push_back(roots[0]);
//      retval.push_back(roots[1]);
//   }
//
//   BufferSatelliteStates(false);
//
//   return retval;
//}



//void RootFinder::FixState(Event *thisOne)
//{
//}

//bool FindRoot(Integer whichOne, GmatEpoch roots[2])
//{
//   bool retval = false;
//
//   return retval;
//}
//
//void RootFinder::BufferSatelliteStates(bool fillingBuffer)
//{
//   Spacecraft *fromSat = NULL, *toSat = NULL;
//   Formation *fromForm = NULL, *toForm = NULL;
//   std::string soName;
//
//   for (std::vector<Spacecraft *>::iterator i = satBuffer.begin();
//        i != satBuffer.end(); ++i)
//   {
//      soName = (*i)->GetName();
//      if (fillingBuffer)
//      {
////         fromSat = (Spacecraft *)FindObject(soName);
//         toSat = *i;
//      }
//      else
//      {
//         fromSat = *i;
////         toSat = (Spacecraft *)FindObject(soName);
//      }
//
//      #ifdef DEBUG_STOPPING_CONDITIONS
//         MessageInterface::ShowMessage(
//            "   Sat is %s, fill direction is %s; fromSat epoch = %.12lf   "
//            "toSat epoch = %.12lf\n",
//            fromSat->GetName().c_str(),
//            (fillingBuffer ? "from propagator" : "from buffer"),
//            fromSat->GetRealParameter("A1Epoch"),
//            toSat->GetRealParameter("A1Epoch"));
//
//         MessageInterface::ShowMessage(
//            "   '%s' Satellite state:\n", fromSat->GetName().c_str());
//         Real *satrv = fromSat->GetState().GetState();
//         MessageInterface::ShowMessage(
//            "      %.12lf  %.12lf  %.12lf\n      %.12lf  %.12lf  %.12lf\n",
//            satrv[0], satrv[1], satrv[2], satrv[3], satrv[4], satrv[5]);
//      #endif
//
//      (*toSat) = (*fromSat);
//
//      #ifdef DEBUG_STOPPING_CONDITIONS
//         MessageInterface::ShowMessage(
//            "After copy, From epoch %.12lf to epoch %.12lf\n",
//            fromSat->GetRealParameter("A1Epoch"),
//            toSat->GetRealParameter("A1Epoch"));
//      #endif
//   }
//
//   for (std::vector<Formation *>::iterator i = formBuffer.begin();
//        i != formBuffer.end(); ++i)
//   {
//      soName = (*i)->GetName();
//      #ifdef DEBUG_STOPPING_CONDITIONS
//         MessageInterface::ShowMessage("Buffering formation %s, filling = %s\n",
//            soName.c_str(), (fillingBuffer?"true":"false"));
//      #endif
//      if (fillingBuffer)
//      {
////         fromForm = (Formation *)FindObject(soName);
//         toForm = *i;
//      }
//      else
//      {
//         fromForm = *i;
////         toForm = (Formation *)FindObject(soName);
//      }
//
//      #ifdef DEBUG_STOPPING_CONDITIONS
//         MessageInterface::ShowMessage(
//            "   Formation is %s, fill direction is %s; fromForm epoch = %.12lf"
//            "   toForm epoch = %.12lf\n",
//            fromForm->GetName().c_str(),
//            (fillingBuffer ? "from propagator" : "from buffer"),
//            fromForm->GetRealParameter("A1Epoch"),
//            toForm->GetRealParameter("A1Epoch"));
//      #endif
//
//      (*toForm) = (*fromForm);
//
//      toForm->UpdateState();
//
//      #ifdef DEBUG_STOPPING_CONDITIONS
//         Integer count = fromForm->GetStringArrayParameter("Add").size();
//
//         MessageInterface::ShowMessage(
//            "After copy, From epoch %.12lf to epoch %.12lf\n",
//            fromForm->GetRealParameter("A1Epoch"),
//            toForm->GetRealParameter("A1Epoch"));
//
//         MessageInterface::ShowMessage(
//            "   %s for '%s' Formation state:\n",
//            (fillingBuffer ? "Filling buffer" : "Restoring states"),
//            fromForm->GetName().c_str());
//
//         Real *satrv = fromForm->GetState().GetState();
//
//         for (Integer i = 0; i < count; ++i)
//            MessageInterface::ShowMessage(
//               "      %d:  %.12lf  %.12lf  %.12lf  %.12lf  %.12lf  %.12lf\n",
//               i, satrv[i*6], satrv[i*6+1], satrv[i*6+2], satrv[i*6+3],
//               satrv[i*6+4], satrv[i*6+5]);
//      #endif
//   }
//
//   #ifdef DEBUG_STOPPING_CONDITIONS
//      for (std::vector<Spacecraft *>::iterator i = satBuffer.begin();
//           i != satBuffer.end(); ++i)
//         MessageInterface::ShowMessage(
//            "   Epoch of '%s' is %.12lf\n", (*i)->GetName().c_str(),
//            (*i)->GetRealParameter("A1Epoch"));
//   #endif
//}
