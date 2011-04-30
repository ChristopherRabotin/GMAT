//$Id$
//------------------------------------------------------------------------------
//                            LagrangeInterpolator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number #####
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2009/09/28
//
/**
 * Implements LagrangeInterpolator class as specified in the GMAT Math Spec.
 */
//------------------------------------------------------------------------------

#include "LagrangeInterpolator.hpp"
#include "InterpolatorException.hpp"
#include "RealUtilities.hpp"         // for GmatMathUtil::Abs()
#include "MessageInterface.hpp"

//#define DEBUG_LAGRANGE_FEASIBLE
//#define DEBUG_LAGRANGE_BUILD
//#define DEBUG_LAGRANGE_INTERPOLATE

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  LagrangeInterpolator(const std::string &name, Integer dim, Integer ord)
//------------------------------------------------------------------------------
/**
 * Constructs lagrange interpolator (default constructor).
 * 
 * @param <name>  Name for this interpolator ("")
 * @param <dim>   Dimension of data that gets interpolated (1).
 * @param <order> The order of interpolation (7)
 */
//------------------------------------------------------------------------------
LagrangeInterpolator::LagrangeInterpolator(const std::string &name, Integer dim,
                                           Integer ord) :
   Interpolator  (name, "LagrangeInterpolator", dim),
   order         (ord),
   actualSize    (0),
   beginIndex    (0),
   startPoint    (0),
   lastX         (-9.9999e75)
{
   // Made bufferSize 10 times bigger than order, so that we can collect more
   // data to place requested ind parameter in the near to the center of the
   // interpolation range.
   requiredPoints = order + 1;
   bufferSize = requiredPoints * 10;
   if (bufferSize > MAX_BUFFER_SIZE)
      bufferSize = MAX_BUFFER_SIZE;
   
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage
      ("LagrangeInterpolator() order=%d, requiredPoints=%d, bufferSize=%d\n",
       order, requiredPoints, bufferSize);
   #endif
   
   for (Integer i = 0; i < bufferSize; ++i)
   {
      x[i]  = -9.9999e75;
      y[i]  = NULL;
   }
}


//------------------------------------------------------------------------------
//  ~LagrangeInterpolator()
//------------------------------------------------------------------------------
/**
 * Destroys lagrange interpolator (destructor).
 */
//------------------------------------------------------------------------------
LagrangeInterpolator::~LagrangeInterpolator()
{
   CleanupArrays();
}


//------------------------------------------------------------------------------
//  LagrangeInterpolator(const LagrangeInterpolator &li)
//------------------------------------------------------------------------------
/**
 * Constructs lagrange interpolator, based on another (copy constructor).
 * 
 * @param li The original that is being copied.
 */
//------------------------------------------------------------------------------
LagrangeInterpolator::LagrangeInterpolator(const LagrangeInterpolator &li) :
   Interpolator   (li),
   order          (li.order),
   actualSize     (li.actualSize),
   beginIndex     (li.beginIndex),
   startPoint     (li.startPoint),
   lastX          (li.lastX)
{
   Integer i;
   for (i = 0; i < bufferSize; ++i)
   {
      x[i]  = -9.9999e75;
      y[i]  = NULL;
   }
}


//------------------------------------------------------------------------------
//  LagrangeInterpolator& operator=(const LagrangeInterpolator &li)
//------------------------------------------------------------------------------
/**
 * Sets this lagrange interpolator to match another (assignment operator).
 * 
 * @param li The original that is being copied.
 * 
 * @return A reference to the copy (aka *this).
 */
//------------------------------------------------------------------------------
LagrangeInterpolator&
LagrangeInterpolator::operator=(const LagrangeInterpolator &li)
{
   if (&li == this)
      return *this;
   
   CleanupArrays();
   CopyArrays(li);
   
   order      = li.order;
   actualSize = li.actualSize;
   beginIndex = li.beginIndex;
   startPoint = li.startPoint;
   lastX      = li.lastX;
   
   return *this;
}


//------------------------------------------------------------------------------
//  Integer IsInterpolationFeasible(Real ind)
//------------------------------------------------------------------------------
/**
 * Checks if interpolation is feasible.
 * (Should I also check if ind is in the center of the interpolation range?)
 *
 * @param ind The value of the independent parameter.
 * @return  1 if feasible
 *         -1 if there is not enough data to interpolate
 *         -2 if requested data is before the first data
 *         -3 if requested data is after the last data
 */
//------------------------------------------------------------------------------
Integer LagrangeInterpolator::IsInterpolationFeasible(Real ind)
{
   #ifdef DEBUG_LAGRANGE_FEASIBLE
   MessageInterface::ShowMessage
      ("LagrangeInterpolator::IsInterpolationFeasible() ind=%f, pointCount = %d, "
       "requiredPoints = %d\n", ind, pointCount, requiredPoints);
   #endif
   
   // If not enough data points, throw an exception
   if (pointCount < requiredPoints)
   {
      #if 0
      InterpolatorException ie;
      ie.SetDetails
         ("There is not enough data to interpolate %f, number of "
          "required points is %d, received %d points", ind, requiredPoints,
          pointCount);
      throw ie;
      #else
      #ifdef DEBUG_LAGRANGE_FEASIBLE
      MessageInterface::ShowMessage
         ("   *** There is not enough data to interpolate %f, number of "
          "required points is %d, received %d points\n", ind, requiredPoints,
          pointCount);
      MessageInterface::ShowMessage
         ("LagrangeInterpolator::IsInterpolationFeasible() returning -1\n");
      #endif
      return -1;
      #endif
   }
   
   SetRange();
   
   #ifdef DEBUG_LAGRANGE_FEASIBLE
   MessageInterface::ShowMessage
      ("   range1 = %f, range2 = %f\n", range[0], range[1]);
   #endif
   
   if (ind < range[0])
   {
      #if 0
      InterpolatorException ie;
      ie.SetDetails
         ("The requested data %f is not within the data range of [%f : %f]",
          ind, range[0],  range[1]);
      throw ie;
      #else
      #ifdef DEBUG_LAGRANGE_FEASIBLE
      MessageInterface::ShowMessage
         ("   *** The requested data %f is not within the data range of "
          "[%f : %f]. It is before the first data\n", ind, range[0],  range[1]);
      MessageInterface::ShowMessage
         ("LagrangeInterpolator::IsInterpolationFeasible() returning -2\n");
      #endif
      return -2;
      #endif
   }
   
   // If independent data not within the range, throw an exception
   if (ind > range[1])
   {
      #if 0
      InterpolatorException ie;
      ie.SetDetails
         ("The requested data %f is not within the data range of [%f : %f]",
          ind, range[0],  range[1]);
      throw ie;
      #else
      #ifdef DEBUG_LAGRANGE_FEASIBLE
      MessageInterface::ShowMessage
         ("   *** The requested data %f is not within the data range of "
          "[%f : %f]. It is after the last data\n", ind, range[0],  range[1]);
      MessageInterface::ShowMessage
         ("LagrangeInterpolator::IsInterpolationFeasible() returning -3\n");
      #endif
      return -3;
      #endif
   }
   
   #ifdef DEBUG_LAGRANGE_FEASIBLE
   MessageInterface::ShowMessage
      ("LagrangeInterpolator::IsInterpolationFeasible() returning 1\n");
   #endif
   
   return 1;
}


//------------------------------------------------------------------------------
//  void Clear()
//------------------------------------------------------------------------------
/**
 * @see Interpolator
 */
//------------------------------------------------------------------------------
void LagrangeInterpolator::Clear()
{
   Interpolator::Clear();
   previousX = -9.9999e75;
   actualSize = 0;
   beginIndex = 0;
   startPoint = 0;
   
   for (Integer i = 0; i < bufferSize; ++i)
      x[i] = -9.9999e75;
}


//------------------------------------------------------------------------------
//  bool AddPoint(const Real ind, const Real *data)
//------------------------------------------------------------------------------
/**
 * See Interpolator
 *
 * @exception thrown when independent data direction changes
 */
//------------------------------------------------------------------------------
bool LagrangeInterpolator::AddPoint(const Real ind, const Real *data)
{
   #ifdef DEBUG_LAGRANGE_ADD
   MessageInterface::ShowMessage
      ("LagrangeInterpolator::AddPoint() entered, ind=%f, previousX=%f\n",
       ind, previousX);
   #endif
   
   if (ind < previousX)
   {
      InterpolatorException ie;
      ie.SetDetails("The independent data provided is not monotonic, current data is %f, "
                    "previous data is %f", ind, previousX);
      throw ie;
   }
   
   return Interpolator::AddPoint(ind, data);
}


//------------------------------------------------------------------------------
//  bool Interpolate(const Real ind, Real *results)
//------------------------------------------------------------------------------
/**
 * Perform the interpolation.
 * 
 * This method is the core interface for the lagrange interpolation.
 * See the GMAT math spec for the algorithm.
 * 
 * @param ind       The value of the independent parameter.
 * @param results   Data structure for the estimates.
 * 
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool LagrangeInterpolator::Interpolate(const Real ind, Real *results)
{
   #ifdef DEBUG_LAGRANGE_INTERPOLATE
   MessageInterface::ShowMessage
      ("Lagrange::Interpolate() entered, ind=%f, dimension=%d, forceInterpolation=%d\n",
       ind, dimension, forceInterpolation);
   #endif
   
   // Check for interpolation feasibility
   if (IsInterpolationFeasible(ind) != 1)
   {
      #ifdef DEBUG_LAGRANGE_INTERPOLATE
      MessageInterface::ShowMessage
         ("Lagrange::Interpolate() returnnig false, NOT feasible to interpolate at %f\n",
          ind);
      #endif
      return false;
   }
   
   // Build data points
   BuildDataPoints(ind);
   
   // If not forcing interpolation, perfome more checking
   if (!forceInterpolation)
   {
      if (!IsDataNearCenter(ind))
      {
         #ifdef DEBUG_LAGRANGE_INTERPOLATE
         MessageInterface::ShowMessage
            ("Lagrange::Interpolate() returnnig false, %f is NOT in near center\n",
             ind);
         #endif
         return false;
      }
   }
   
   // Find starting point that will put ind in the center
   FindStartingPoint(ind);
   
   // Now interpolate using the alorithm in the Math Spec.
   Real *products = new Real[dimension];
   Real *estimates = new Real[dimension];
   
   for (Integer dim = 0; dim < dimension; dim++)
      estimates[dim] = 0.0;
   
   for (Integer i = startPoint; i < startPoint + order; i++)
   {
      for (Integer dim = 0; dim < dimension; dim++)
      {
         products[dim] = y[i][dim];
         
         #ifdef DEBUG_LAGRANGE_INTERPOLATE_MORE
         MessageInterface::ShowMessage
            ("  i=%d, products[%d]=%f\n", i, dim, products[dim]);
         #endif
      }

      for (Integer j = startPoint; j < startPoint + order; j++)
      {
         if (i != j)
         {
            for (Integer dim = 0; dim < dimension; dim++)
            {
               // Should I check for divide by zero?
               if ((x[i]- x[j] == 0.0))
               {
                  #ifdef DEBUG_LAGRANGE_INTERPOLATE_MORE
                  MessageInterface::ShowMessage("   x[%d]- x[%d]=%f\n", i, j, (x[i]- x[j]));
                  #endif
               }
               products[dim] = products[dim] * (ind - x[j]) / (x[i]- x[j]);
            }
         }
      }
      
      for (Integer dim = 0; dim < dimension; dim++)
      {
         #ifdef DEBUG_LAGRANGE_INTERPOLATE_MORE
         MessageInterface::ShowMessage
            ("  i=%d, products[%d]=%f\n", i, dim, products[dim]);
         #endif
         estimates[dim] = estimates[dim] + products[dim];
      }
   }
   
   // copy results
   for (Integer dim = 0; dim < dimension; dim++)
   {
      results[dim] = estimates[dim];
   }
   
   delete [] products;
   delete [] estimates;
   
   #ifdef DEBUG_LAGRANGE_INTERPOLATE
   MessageInterface::ShowMessage
      ("Lagrange::Interpolate() returning true, first data = %f\n", results[0]);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the LagrangeInterpolator.
 *
 * @return clone of the LagrangeInterpolator.
 */
//------------------------------------------------------------------------------
GmatBase* LagrangeInterpolator::Clone() const
{
   return (new LagrangeInterpolator(*this));
}


//---------------------------------
//  protected methods
//---------------------------------

//------------------------------------------------------------------------------
//  void AllocateArrays()
//------------------------------------------------------------------------------
/**
 * Allocates lagrange buffers and calls the base method to build the ring buffer.  
 */
//------------------------------------------------------------------------------
void LagrangeInterpolator::AllocateArrays()
{
   Interpolator::AllocateArrays();
   
   Integer i;
   for (i = 0; i < bufferSize; ++i)
   {
      x[i] = -9.9999e75;
      y[i]  = new Real[dimension];
   }
   
   latestPoint = -1;
}


//------------------------------------------------------------------------------
//  void CleanupArrays()
//------------------------------------------------------------------------------
/**
 * Frees the memory used by the lagrange buffer and calls the base method to 
 * manage the ring buffer.
 */
//------------------------------------------------------------------------------
void LagrangeInterpolator::CleanupArrays()
{
   Integer i = 0;
   if (y[i])
   {
      for (i = 0; i < bufferSize; ++i)
      {
         x[i] = -9.9999e75;
         delete [] y[i];
         y[i] = NULL;
      }
   }
   
   Interpolator::CleanupArrays();
}


//------------------------------------------------------------------------------
//  void CopyArrays(const Interpolator &i)
//------------------------------------------------------------------------------
/**
 * Copies the ring buffer from one Interpolator to this one.  
 * 
 * @param i The Interpolator that supplies the data copied to this one.
 */
//------------------------------------------------------------------------------
void LagrangeInterpolator::CopyArrays(const LagrangeInterpolator &i)
{
   Interpolator::CopyArrays(i);
   Integer j;
   for (j = 0; j < bufferSize; ++j)
   {
      x[j] = i.x[j];
      memcpy( y[j],  i.y[j], dimension*sizeof(Real));
   }
}


//------------------------------------------------------------------------------
//  void BuildDataPoints(Real ind)
//------------------------------------------------------------------------------
/**
 * Use the ring buffer to load the arrays used to build the lagrange buffer.
 * Lagrange buffer should be constructed so that requested data sits in the
 * middle of the buffer as possible.
 */
//------------------------------------------------------------------------------
void LagrangeInterpolator::BuildDataPoints(Real ind)
{
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage("Lagrange::BuildDataPoints() entered, ind=%f\n", ind);
   #endif
   
   Integer i, j, start = 0;
   Real sign = (dataIncreases ? 1.0 : -1.0);
   Real indData = sign * independent[0];
   
   // Compute actual size to use since bufferSize is twice of order
   actualSize = bufferSize;
   if (actualSize > pointCount)
      actualSize = pointCount;
   
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage("   indData = %f, actualSize=%d\n", indData, actualSize);
   #endif
   
   for (i = 1; i < actualSize; ++i)
   {
      if (sign*independent[i] < indData)
      {
         start = i;
         indData = sign*independent[i];
      }
   }
   
   for (i = 0; i < actualSize; ++i, ++start)
   {
      if (start == bufferSize)
         start = 0;
      x[i] = independent[start];
      
      for (j = 0; j < dimension; j++)
         y[i][j] = dependent[start][j];
      
      #ifdef DEBUG_LAGRANGE_BUILD
      MessageInterface::ShowMessage
         ("   start = %2d, x[%2d] = %f, y[%2d][0] = %f\n", start, i, x[i],
          i, y[i][0]);
      #endif
   }
   
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage
      ("LagrangeInterpolator::BuildDataPoints() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// bool IsDataNearCenter(Real ind)
//------------------------------------------------------------------------------
bool LagrangeInterpolator::IsDataNearCenter(Real ind)
{
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage
      ("Lagrange::IsDataNearCenter() entered, ind=%f, actualSize=%d\n",
       ind, actualSize);
   #endif
   
   Integer dataIndex = 0;
   
   for (Integer i = 0; i < actualSize; i++)
   {
      if (x[i] >= ind)
      {
         dataIndex = i;
         break;
      }
   }
   
   dataIndex = dataIndex - 1;
   bool retval = false;
   Integer begin = dataIndex - (order/2);
   Integer end = dataIndex + (order/2);
   
   beginIndex = begin;
   if ((beginIndex + order) > actualSize)
      beginIndex = actualSize - order;
   
   // Check if there are enough points before and after data requested
   if (begin >= 0 && end < actualSize)
   {
      retval = true;
   }
   
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage
      ("   dataIndex=%d, begin=%d, end=%d, beginIndex=%d\n",
       dataIndex, begin, end, beginIndex);
   MessageInterface::ShowMessage
      ("Lagrange::IsDataNearCenter() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// Integer FindStartingPoint(Real ind)
//------------------------------------------------------------------------------
/*
 * Finds index that places ind in the center of the interpolation range using
 * algorithm given in the math spec.
 */
//------------------------------------------------------------------------------
Integer LagrangeInterpolator::FindStartingPoint(Real ind)
{
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage("Lagrange::FindStartingPoint() entered, ind=%f\n", ind);
   #endif
   
   // Find the data point near the requested value
   Real minDiff = 1.0e30, diff, meanX;
   Integer qMin = 0;
   Integer qEnd = beginIndex + order;
   
   for (Integer q = beginIndex; q < qEnd; ++q)
   {
      meanX = ( x[q + order] + x[q] ) / 2;
      diff = GmatMathUtil::Abs( meanX - ind );
      if (diff < minDiff)
      {
         qMin = q;
         minDiff = diff;
      }
   }
   
   // Assign to starting point
   startPoint = qMin;
   
   // We don't want start point to pass the actual data size, so adjust
   if (qMin + order > actualSize - 1)
   {
      startPoint = actualSize - order;
      #ifdef DEBUG_LAGRANGE_BUILD
      MessageInterface::ShowMessage
         ("   qMin + order = %d passed the actualSize %d, so adjusted "
          "startPoint to %d\n", qMin+order, actualSize, startPoint);
      #endif
   }
   
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage
      ("   beginIndex=%d, qEnd=%d, qMin=%d, startPoint=%d\n", beginIndex,
       qEnd, qMin, startPoint);
   #endif
   
   if (startPoint < 0)
      startPoint = 0;
   
   if (beginIndex > 0)
      startPoint = beginIndex;
   
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage
      ("LagrangeInterpolator::FindStartingPoint() returning startPoint = %d\n",
       startPoint);
   #endif
   
   return startPoint;
}


