//$Header$
//------------------------------------------------------------------------------
//                              CartYParam
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
 * Declares Cartesian Position Y parameter class.
 */
//------------------------------------------------------------------------------
#ifndef CartYParam_hpp
#define CartYParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API CartYParam : public RealParameter, OrbitData
{
public:

    CartYParam(const std::string &name = "",
               GmatBase *obj = NULL,
               const std::string &desc = "Spacecraft Cartesian Position Y",
               const std::string &unit = "Km");
    CartYParam(const CartYParam &copy);
    const CartYParam& operator=(const CartYParam &right);
    virtual ~CartYParam();

    // The inherited methods from RealParameter
    virtual Real EvaluateReal();

    // The inherited methods from Parameter
    virtual Integer GetNumObjects() const;
    virtual bool AddObject(GmatBase *obj);
    virtual bool Validate();
    virtual bool Evaluate();
    
protected:

};

#endif // CartYParam_hpp
