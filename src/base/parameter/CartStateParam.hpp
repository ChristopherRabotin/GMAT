//$Header$
//------------------------------------------------------------------------------
//                              CartStateParam
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
 * Declares Spacecraft Cartesian State parameter class.
 */
//------------------------------------------------------------------------------
#ifndef CartStateParam_hpp
#define CartStateParam_hpp

#include "gmatdefs.hpp"
#include "Rvector6Parameter.hpp"
#include "Rvector6.hpp"
#include "OrbitData.hpp"

class GMAT_API CartStateParam : public Rvector6Parameter, OrbitData
{
public:

    CartStateParam(const std::string &name = "",
                   GmatBase *obj = NULL,
                   const std::string &desc = "Spacecraft Cartesian State",
                   const std::string &unit = "");
    CartStateParam(const CartStateParam &copy);
    const CartStateParam& operator=(const CartStateParam &right);
    virtual ~CartStateParam();

    // The inherited methods from Rvector6Parameter
    virtual Rvector6 EvaluateRvector6();
    
    // The inherited methods from Parameter
    virtual Integer GetNumObjects() const;
    virtual bool AddObject(GmatBase *obj);
    virtual bool Validate();
    virtual bool Evaluate();

protected:
    
};

#endif // CartStateParam_hpp
