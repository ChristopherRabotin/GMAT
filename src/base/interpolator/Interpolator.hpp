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


/**
 * Base class for the GMAT Interpolators
 */
class Interpolator : public GmatBase
{
public:
    Interpolator(const std::string &name, const std::string &typestr,
                 Integer dim = 1);
    virtual ~Interpolator(void);

    Interpolator(const Interpolator &i);
    Interpolator&   operator=(const Interpolator &i);

    virtual bool    AddPoint(const Real ind, const Real *data);
    virtual bool    Interpolate(const Real ind, Real *results) = 0;
    virtual void    Clear(void);
    virtual Integer GetBufferSize(void);

protected:
    /// Data array used for the independent variable
    Real *independent;
    /// The data that gets interpolated
    Real **dependent;
    /// Previous independent value, used to determine direction data is going
    Real previousX; 

    // Parameters
    /// Number of dependent points to be interpolated
    Integer dimension;
    /// Number of points required to interpolate
    Integer requiredPoints;
    /// Number of points managed by the interpolator
    Integer bufferSize;
    /// Number of points fed to the interpolator
    Integer pointCount;
    /// Pointer to most recent point, for the ring buffer implementation
    Integer latestPoint;
    /// Valid range for the data points
    Real range[2];
    /// Flag used to detect if range has already been calculated
    bool rangeCalculated;
    /// Flag used to determine if independent variable increases or decreases
    bool dataIncreases;
    
    virtual void AllocateArrays(void);
    virtual void CleanupArrays(void);
    virtual void CopyArrays(const Interpolator &i);
    void SetRange(void);
};


#endif // Interpolator_hpp

