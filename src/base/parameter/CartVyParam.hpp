//$Header$
//------------------------------------------------------------------------------
//                              CartVyParam
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
 * Declares Cartesian Velocity Y parameter class.
 */
//------------------------------------------------------------------------------
#ifndef CartVyParam_hpp
#define CartVyParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API CartVyParam : public RealParameter, OrbitData
{
public:

    CartVyParam(const std::string &name = "",
                GmatBase *obj = NULL,
                const std::string &desc = "Spacecraft Cartesian Velocity Y",
                const std::string &unit = "Km/Sec");
    CartVyParam(const CartVyParam &copy);
    const CartVyParam& operator=(const CartVyParam &right);
    virtual ~CartVyParam();

    // The inherited methods from Parameter
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

#endif // CartVyParam_hpp
