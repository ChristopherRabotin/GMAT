//$Header$
//------------------------------------------------------------------------------
//                              VelPeriapsisParam
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
 * Declares Velocity at Periapsis parameter class.
 */
//------------------------------------------------------------------------------
#ifndef VelPeriapsisParam_hpp
#define VelPeriapsisParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API VelPeriapsisParam : public RealParameter, OrbitData
{
public:

    VelPeriapsisParam(const std::string &name = "",
                      GmatBase *obj = NULL,
                      const std::string &desc = "Spacecraft Velocity at Periapsis",
                      const std::string &unit = "Km/Sec");
    VelPeriapsisParam(const VelPeriapsisParam &copy);
    const VelPeriapsisParam& operator=(const VelPeriapsisParam &right);
    virtual ~VelPeriapsisParam();

    // The inherited methods from RealParameter
    virtual Real EvaluateReal();

    // The inherited methods from Parameter
    virtual Integer GetNumObjects() const;
    virtual bool AddObject(GmatBase *obj);
    virtual bool Validate();
    virtual bool Evaluate();

protected:

};

#endif // VelPeriapsisParam_hpp
