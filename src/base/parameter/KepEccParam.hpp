//$Header$
//------------------------------------------------------------------------------
//                              KepEccParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/06
//
/**
 * Declares Keplerian Eccentricity parameter class.
 */
//------------------------------------------------------------------------------
#ifndef KepEccParam_hpp
#define KepEccParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API KepEccParam : public RealParameter, OrbitData
{
public:

    KepEccParam(const std::string &name = "",
                GmatBase *obj = NULL,
                const std::string &desc = "Spacecraft Kep Eccentricity",
                const std::string &unit = " ");
    KepEccParam(const KepEccParam &param);
    const KepEccParam& operator=(const KepEccParam &right);
    virtual ~KepEccParam();

    // The inherited methods from RealParameter
    virtual Real EvaluateReal();

    // The inherited methods from Parameter
    virtual Integer GetNumObjects() const;
    virtual bool AddObject(GmatBase *obj);
    virtual bool Validate();
    virtual bool Evaluate();

protected:

};

#endif // KepEccParam_hpp
