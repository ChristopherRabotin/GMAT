//$Header$
//------------------------------------------------------------------------------
//                              KepIncParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/06
//
/**
 * Declares Keplerian Inclination parameter class.
 */
//------------------------------------------------------------------------------
#ifndef KepIncParam_hpp
#define KepIncParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API KepIncParam : public RealParameter, OrbitData
{
public:

    KepIncParam(const std::string &name = "",
                GmatBase *obj = NULL,
                const std::string &desc = "Spacecraft Kep Inclination",
                const std::string &unit = "Deg");
    KepIncParam(const KepIncParam &copy);
    const KepIncParam& operator=(const KepIncParam &right);
    virtual ~KepIncParam();

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

#endif // KepIncParam_hpp
