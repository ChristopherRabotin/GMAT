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
 * Definition for the Interpolator base class
 */
//------------------------------------------------------------------------------


#ifndef Interpolator_hpp
#define Interpolator_hpp


#include "GmatBase.hpp"


class Interpolator : public GmatBase
{
public:
    Interpolator(const std::string &typestr, Integer dim = 1);
    ~Interpolator(void);

    Interpolator(const Interpolator &i);
    Interpolator&			operator=(const Interpolator &i);

    virtual bool            AddPoint(const Real ind, const Real *data);
    virtual bool	        Interpolate(const Real ind, Real *results) = 0;
    virtual void            Clear(void);

protected:
    /// Data array used for the independent variable
    Real					*independent;
    /// The data that gets interpolated
    Real					**dependent;

    // Parameters
    /// Number of dependent points to be interpolated
    Integer                 dimension;
    /// Number of points required to interpolate
    Integer					requiredPoints;
    /// Number of points managed by the interpolator
    Integer					bufferSize;
    /// Number of points fed to the interpolator
    Integer					pointCount;
    /// Pointer to most recent point, for the ring buffer implementation
    Integer                 latestPoint;
    /// Valid range for the data points
    Real                    range[2];
    /// Flag used to detect if range has already been calculated
    bool                    rangeCalculated;
    
    void                    AllocateArrays(void);
    void                    CleanupArrays(void);
    void                    CopyArrays(const Interpolator &i);
    void                    SetRange(void);
};


#endif // Interpolator_hpp

