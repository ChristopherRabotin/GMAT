//$Header$
//------------------------------------------------------------------------------
//                              KepElemParam
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
 * Declares Spacecraft Keplerian Elements parameter class.
 */
//------------------------------------------------------------------------------
#ifndef KepElemParam_hpp
#define KepElemParam_hpp

#include "gmatdefs.hpp"
#include "Rvector6Parameter.hpp"
#include "Rvector6.hpp"
#include "OrbitData.hpp"

class GMAT_API KepElemParam : public Rvector6Parameter, OrbitData
{
public:

    KepElemParam(const std::string &name = "",
                 GmatBase *obj = NULL,
                 const std::string &desc = "Spacecraft Keplerian Elements",
                 const std::string &unit = "");
    KepElemParam(const KepElemParam &copy);
    const KepElemParam& operator=(const KepElemParam &right);
    virtual ~KepElemParam();

    // The inherited methods from Rvector6Parameter
    virtual Rvector6 EvaluateRvector6();
    
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

#endif // KepElemParam_hpp
