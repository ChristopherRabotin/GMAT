//------------------------------------------------------------------------------
//                             TrajectorySegment
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Jeremy Knittel / NASA
// Created: 2017.02.23
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include "TrajectorySegment.hpp"
#include "LowThrustException.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
TrajectorySegment::TrajectorySegment() :
   numDataPoints   (0),
   numStates       (0),
   numControls     (0),
   numIntegrals    (0)
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
TrajectorySegment::TrajectorySegment(const TrajectorySegment &copy) :
   numDataPoints   (copy.numDataPoints),
   numStates       (copy.numStates),
   numControls     (copy.numControls),
   numIntegrals    (copy.numIntegrals)
{
   CopyArrays(copy);
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
TrajectorySegment& TrajectorySegment::operator=(const TrajectorySegment &copy)
{
   if (&copy == this)
      return *this;

   numDataPoints = copy.numDataPoints;
   numStates     = copy.numStates;
   numControls   = copy.numControls;
   numIntegrals  = copy.numIntegrals;

   CopyArrays(copy);

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
TrajectorySegment::~TrajectorySegment()
{
   // Nothing to do here
}

//------------------------------------------------------------------------------
// Get/Set Methods
//------------------------------------------------------------------------------
void TrajectorySegment::SetNumControlParams(Integer input)
{
   numControls = input;
}
void TrajectorySegment::SetNumStateParams(Integer input)
{
   numStates = input;
}
void TrajectorySegment::SetNumIntegralParams(Integer input)
{
   numIntegrals = input;
}
Integer TrajectorySegment::GetNumControls()
{
   return numControls;
}
Integer TrajectorySegment::GetNumStates()
{
   return numStates;
}
Integer TrajectorySegment::GetNumIntegrals()
{
   return numIntegrals;
}
Integer TrajectorySegment::GetNumDataPoints()
{
   return numDataPoints;
}
Real TrajectorySegment::GetTime(Integer index)
{
   return trajData.at(index).time;
}
Real TrajectorySegment::GetState(Integer index, Integer dimension)
{
   return trajData.at(index).states(dimension);
}
Real TrajectorySegment::GetControl(Integer index, Integer dimension)
{
   return trajData.at(index).controls(dimension);
}
Real TrajectorySegment::GetIntegral(Integer index, Integer dimension)
{
   return trajData.at(index).integrals(dimension);
}
Integer TrajectorySegment::GetStateSize(Integer index)
{
   return trajData.at(index).states.GetSize();
}
Integer TrajectorySegment::GetControlSize(Integer index)
{
   return trajData.at(index).controls.GetSize();
}
Integer TrajectorySegment::GetIntegralSize(Integer index)
{
   return trajData.at(index).integrals.GetSize();
}

//------------------------------------------------------------------------------
// void AddDataPoint(TrajectoryDataStructure inputData)
//------------------------------------------------------------------------------
/**
* This method adds a data point to the vector of data
*
* @param <inputData> a structure of new data
*
*/
//------------------------------------------------------------------------------
void TrajectorySegment::AddDataPoint(TrajectoryDataStructure inputData)
{
   trajData.push_back(inputData);
   numDataPoints++;
   if (numDataPoints > 1)
   {
     if (trajData.back().time < trajData[numDataPoints-2].time)
         throw LowThrustException("ERROR - TrajectoryData: New data point" 
                 " is not in the correct temporal order\n");    
   }
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void CopyArrays(const TrajectorySegment &copy)
//------------------------------------------------------------------------------
/**
 * This method copies the array values to those of the input TrajectorySegment
 *
 * @param <copy>   trajectory segment to copy
 *
 */
//------------------------------------------------------------------------------
void TrajectorySegment::CopyArrays(const TrajectorySegment &copy)
{
   Integer sz = copy.trajData.size();
   trajData.resize(sz);
   trajData = copy.trajData;
}
