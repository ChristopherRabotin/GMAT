//$Header$
//------------------------------------------------------------------------------
//                              SemilatRecParam
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
 * Declares Semilatus Rectum parameter class.
 */
//------------------------------------------------------------------------------
#ifndef SemilatRecParam_hpp
#define SemilatRecParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API SemilatRecParam : public RealParameter, OrbitData
{
public:

    SemilatRecParam(const std::string &name = "",
                    GmatBase *obj = NULL,
                    const std::string &desc = "Semilatus Rectum (Semiparameter)",
                    const std::string &unit = "Km");
    SemilatRecParam(const SemilatRecParam &copy);
    const SemilatRecParam& operator=(const SemilatRecParam &right);
    virtual ~SemilatRecParam();

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

#endif // SemilatRecParam_hpp
