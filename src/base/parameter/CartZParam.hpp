//$Header$
//------------------------------------------------------------------------------
//                              CartZParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/08
//
/**
 * Declares Cartesian Position Z parameter class.
 */
//------------------------------------------------------------------------------
#ifndef CartZParam_hpp
#define CartZParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API CartZParam : public RealParameter, OrbitData
{
public:

    CartZParam(const std::string &name = "",
               GmatBase *obj = NULL,
               const std::string &desc = "Spacecraft Cartesian Position Z",
               const std::string &unit = "Km");
    CartZParam(const CartZParam &copy);
    const CartZParam& operator=(const CartZParam &right);
    virtual ~CartZParam();

    // The inherited methods from RealParameter
    virtual Real EvaluateReal();

    // The inherited methods from Parameter
    virtual Integer GetNumObjects() const;
    virtual bool AddObject(GmatBase *obj);
    virtual bool Validate();
    virtual bool Evaluate();

protected:

};

#endif // CartZParam_hpp
