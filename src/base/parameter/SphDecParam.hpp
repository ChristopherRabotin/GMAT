//$Header$
//------------------------------------------------------------------------------
//                              SphDecParam
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
 * Declares Spacecraft Sphrical Declination parameter class.
 */
//------------------------------------------------------------------------------
#ifndef SphDecParam_hpp
#define SphDecParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API SphDecParam : public RealParameter, OrbitData
{
public:

    SphDecParam(const std::string &name = "",
                GmatBase *obj = NULL,
                const std::string &desc = "Spacecraft Spherical Declination",
                const std::string &unit = "Deg");
    SphDecParam(const SphDecParam &copy);
    const SphDecParam& operator=(const SphDecParam &right);
    virtual ~SphDecParam();

    // The inherited methods from RealParameter
    virtual Real EvaluateReal();

    // The inherited methods from Parameter
    virtual Integer GetNumObjects() const;
    virtual GmatBase* GetObject(const std::string &objTypeName);
    
    virtual bool SetObject(Gmat::ObjectType objType,
                           const std::string &objName,
                           GmatBase *obj);
    
    virtual bool AddObject(GmatBase *obj);
    virtual bool Validate();
    virtual bool Evaluate();

protected:

};

#endif // SphDecParam_hpp
