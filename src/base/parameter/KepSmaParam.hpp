//$Header$
//------------------------------------------------------------------------------
//                              KepSmaParam
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
 * Declares Keplerian Semi-Major Axis parameter class.
 */
//------------------------------------------------------------------------------
#ifndef KepSmaParam_hpp
#define KepSmaParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API KepSmaParam : public RealParameter, OrbitData
{
public:

    KepSmaParam(const std::string &name = "",
                GmatBase *obj = NULL,
                const std::string &desc = "Spacecraft Kep Semi-Major Axis",
                const std::string &unit = "Km");
    KepSmaParam(const KepSmaParam &copy);
    const KepSmaParam& operator=(const KepSmaParam &right);
    virtual ~KepSmaParam();

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

#endif // KepSmaParam_hpp
