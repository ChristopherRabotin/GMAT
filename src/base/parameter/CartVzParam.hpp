//$Header$
//------------------------------------------------------------------------------
//                              CartVzParam
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
 * Declares Cartesian Velocity Z parameter class.
 */
//------------------------------------------------------------------------------
#ifndef CartVzParam_hpp
#define CartVzParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API CartVzParam : public RealParameter, OrbitData
{
public:

    CartVzParam(const std::string &name = "",
                GmatBase *obj = NULL,
                const std::string &desc = "Spacecraft Cartesian Velocity Z",
                const std::string &unit = "Km/Sec");
    CartVzParam(const CartVzParam &copy);
    const CartVzParam& operator=(const CartVzParam &right);
    virtual ~CartVzParam();

    // The inherited methods from Parameter
    virtual Real EvaluateReal();

    // The inherited methods from Parameter
    virtual Integer GetNumObjects() const;
    virtual bool AddObject(GmatBase *obj);
    virtual bool Validate();
    virtual bool Evaluate();
    
protected:

};

#endif // CartVzParam_hpp
