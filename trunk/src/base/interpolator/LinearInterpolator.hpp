//$Header$
//------------------------------------------------------------------------------
//                             LinearInterpolator
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/10/15
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * A basic linear interpolator.
 */
//------------------------------------------------------------------------------


#ifndef LinearInterpolator_hpp
#define LinearInterpolator_hpp

#include "Interpolator.hpp"

/**
 * A linear interpolator for quick calculation of interpolated data
 */
class GMAT_API LinearInterpolator : public Interpolator
{
public:
    LinearInterpolator(const std::string &name = "", Integer dim = 1); 
    ~LinearInterpolator();
    LinearInterpolator(const LinearInterpolator &l);
    LinearInterpolator& operator=(const LinearInterpolator &li);

    virtual bool Interpolate(const Real ind, Real *results);
    
    // inherited from GmatBase
    virtual GmatBase* Clone() const;

};


#endif // LinearInterpolator_hpp

