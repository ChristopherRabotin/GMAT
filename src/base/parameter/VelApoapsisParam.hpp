//$Header$
//------------------------------------------------------------------------------
//                              VelApoapsisParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/22
//
/**
 * Declares Velocity at Apoapsis parameter class.
 */
//------------------------------------------------------------------------------
#ifndef VelApoapsisParam_hpp
#define VelApoapsisParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API VelApoapsisParam : public RealParameter, OrbitData
{
public:

    VelApoapsisParam(const std::string &name = "",
                     GmatBase *obj = NULL,
                     const std::string &desc = "Spacecraft Velocity at Apoapsis",
                     const std::string &unit = "Km/Sec");
    VelApoapsisParam(const VelApoapsisParam &copy);
    const VelApoapsisParam& operator=(const VelApoapsisParam &right);
    virtual ~VelApoapsisParam();

    // The inherited methods from RealParameter
    virtual Real EvaluateReal();

    // The inherited methods from Parameter
    virtual Integer GetNumObjects() const;
    virtual bool AddObject(GmatBase *obj);
    virtual bool Validate();
    virtual bool Evaluate();

protected:

};

#endif // VelApoapsisParam_hpp
