//$Header$
//------------------------------------------------------------------------------
//                              MeanJ2000Equatorial
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/09/15
//
/**
 * Declares Mean of Reference J2000 Equator frame.
 */
//------------------------------------------------------------------------------
#ifndef MeanJ2000Equatorial_hpp
#define MeanJ2000Equatorial_hpp

#include "gmatdefs.hpp"
#include "A1Mjd.hpp"
#include "RefFrame.hpp"
#include "CelestialBody.hpp"

class GMAT_API MeanJ2000Equatorial : public RefFrame
{
public:
    //MeanJ2000Equatorial(const CelestialBody &centralBody);
    MeanJ2000Equatorial(const std::string &name = "",
                        CelestialBody *centralBody = NULL);
    MeanJ2000Equatorial(const MeanJ2000Equatorial &mj2000eq);
    MeanJ2000Equatorial& operator= (const MeanJ2000Equatorial &right);
    ~MeanJ2000Equatorial();

    // conversion to other frames (future build implementation)

    // inherited from GmatBase
    virtual GmatBase* Clone(void) const;

protected:

private:
};

#endif // MeanJ2000Equatorial_hpp
