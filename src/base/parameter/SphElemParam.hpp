//$Header$
//------------------------------------------------------------------------------
//                              SphElemParam
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
 * Declares Spacecraft Spherical Elements parameter class.
 */
//------------------------------------------------------------------------------
#ifndef SphElemParam_hpp
#define SphElemParam_hpp

#include "gmatdefs.hpp"
#include "Rvector6Parameter.hpp"
#include "Rvector6.hpp"
#include "OrbitData.hpp"

class GMAT_API SphElemParam : public Rvector6Parameter, OrbitData
{
public:

    SphElemParam(const std::string &name = "",
                 GmatBase *obj = NULL,
                 const std::string &desc = "Spacecraft Spherical Elements",
                 const std::string &unit = "");
    SphElemParam(const SphElemParam &copy);
    const SphElemParam& operator=(const SphElemParam &right);
    virtual ~SphElemParam();

    // The inherited methods from Rvector6Parameter
    virtual Rvector6 EvaluateRvector6();
    
    // The inherited methods from Parameter
    virtual Integer GetNumObjects() const;
    virtual bool AddObject(GmatBase *obj);
    virtual bool Validate();
    virtual bool Evaluate();

protected:

};

#endif // SphElemParam_hpp
