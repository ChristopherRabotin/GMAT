//$Header$
//------------------------------------------------------------------------------
//                              CartXParam
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
 * Declares Cartesian Position X parameter class.
 */
//------------------------------------------------------------------------------
#ifndef CartXParam_hpp
#define CartXParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API CartXParam : public RealParameter, OrbitData
{
public:

    CartXParam(const std::string &name = "",
               GmatBase *obj = NULL,
               const std::string &desc = "Spacecraft Cartesian Position X",
               const std::string &unit = "Km");
    CartXParam(const CartXParam &copy);
    const CartXParam& operator=(const CartXParam &right);
    virtual ~CartXParam();

    // The inherited methods from RealParameter
    virtual Real EvaluateReal();

    // The inherited methods from Parameter
    virtual bool AddObject(GmatBase *obj);
    virtual Integer GetNumObjects() const;
    virtual bool Validate();
    virtual bool Evaluate();

protected:

};

#endif // CartXParam_hpp
