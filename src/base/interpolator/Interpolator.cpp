//$Header$
//------------------------------------------------------------------------------
//                               Interpolator
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/23
//
/**
 * Implementation for the Interpolator base class
 */
//------------------------------------------------------------------------------


#include "Interpolator.hpp"


Interpolator::Interpolator(const std::string &typestr, Integer dim) :
    GmatBase        (Gmat::INTERPOLATOR, typestr),
    independent     (NULL),
    dependent       (NULL),
    dimension       (dim),
    requiredPoints  (2),
    bufferSize      (2),
    pointCount      (0),
    latestPoint     (-1),
    rangeCalculated (false)
{
    range[0] = range[1] = 0.0;
}


Interpolator::~Interpolator(void)
{
    CleanupArrays();
}


Interpolator::Interpolator(const Interpolator &i) :
    GmatBase        (i),
    dimension       (i.dimension),
    requiredPoints  (i.requiredPoints),
    bufferSize      (i.bufferSize),
    pointCount      (i.pointCount),
    latestPoint     (i.latestPoint),
    rangeCalculated (i.rangeCalculated)
{
    if (i.independent)
        CopyArrays(i);
    else
    {
        independent = NULL;
        dependent = NULL;
    }

    if (rangeCalculated) {
        range[0] = i.range[0];
        range[1] = i.range[1];
    }
}

Interpolator& Interpolator::operator=(const Interpolator &i)
{
    if (&i == this)
        return *this;
        
    GmatBase::operator=(i);

    // Free any allocated memory
    if (independent)
        CleanupArrays();
        
    // Now set the member data to the new values
    dimension = i.dimension;
    requiredPoints = i.requiredPoints;
    bufferSize = i.bufferSize;
    pointCount = i.pointCount;
    latestPoint = i.latestPoint;

    // Copy over the memory from i
    if (i.independent)
        CopyArrays(i);
        
    rangeCalculated = i.rangeCalculated;
    if (rangeCalculated) {
        range[0] = i.range[0];
        range[1] = i.range[1];
    }

    return *this;
}

bool Interpolator::AddPoint(const Real ind, const Real *data)
{
    Integer i;
    
    if (!independent) 
        AllocateArrays();

    // Reset the index into the array if at the end
    if (latestPoint == bufferSize-1)
        latestPoint = -1;

    independent[++latestPoint] = ind;
    for (i = 0; i < dimension; ++i)
        dependent[latestPoint][i] = data[i];
        
    ++pointCount;
    rangeCalculated = false;
    
    return true;
}


void Interpolator::Clear(void)
{
    latestPoint = -1;
    pointCount = 0;
}


void Interpolator::AllocateArrays(void)
{
    Integer i;
    
    independent = new Real[bufferSize];
    dependent = new Real*[bufferSize];
    for (i = 0; i < bufferSize; ++i)
        dependent[i] = new Real[dimension];
    latestPoint = -1;
}


void Interpolator::CleanupArrays(void)
{
    Integer i;
    
    if (independent)
        delete [] independent;
        
    if (dependent) {
        for (i = 0; i < bufferSize; ++i)
            delete [] dependent[i];
        delete [] dependent;
    }
    
    independent = NULL;
    dependent = NULL;
    latestPoint = -1;
    pointCount = 0;
}


void Interpolator::CopyArrays(const Interpolator &i)
{
    Integer j;
    
    AllocateArrays();
    memcpy(independent, i.independent, bufferSize*sizeof(Real));
    for (j = 0; j < bufferSize; ++j)
        memcpy(dependent[j], i.dependent[j], dimension*sizeof(Real));
}


void Interpolator::SetRange(void)
{
    if (rangeCalculated)
        return;
        
    // Not enough points -- just continue
    if (requiredPoints > pointCount)
        return;
        
    range[0] = range[1] = independent[0];
    
    Integer i, max = (pointCount > bufferSize ? bufferSize : pointCount);

    for (i = 1; i < max; ++i) {
        if (independent[i] < range[0])
            range[0] = independent[i];
        if (independent[i] > range[1])
            range[1] = independent[i];
    }
}

