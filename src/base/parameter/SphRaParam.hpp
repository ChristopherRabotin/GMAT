//$Header$
//------------------------------------------------------------------------------
//                              SphRaParam
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
 * Declares Spacecraft Spehrical Right Ascension parameter class.
 */
//------------------------------------------------------------------------------
#ifndef SphRaParam_hpp
#define SphRaParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API SphRaParam : public RealParameter, OrbitData
{
public:

    SphRaParam(const std::string &name = "",
               GmatBase *obj = NULL,
               const std::string &desc = "Spacecraft Sphrical Right Ascension",
               const std::string &unit = "Deg");
    SphRaParam(const SphRaParam &copy);
    const SphRaParam& operator=(const SphRaParam &right);
    virtual ~SphRaParam();

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

#endif // SphRaParam_hpp
