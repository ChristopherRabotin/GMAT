//$Header$
//------------------------------------------------------------------------------
//                              KepTaParam
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
 * Declares Keplerian True Anomaly parameter class.
 */
//------------------------------------------------------------------------------
#ifndef KepTaParam_hpp
#define KepTaParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API KepTaParam : public RealParameter, OrbitData
{
public:

    KepTaParam(const std::string &name = "",
               GmatBase *obj = NULL,
               const std::string &desc = "Spacecraft Kep True Anomaly",
               const std::string &unit = "Deg");
    KepTaParam(const KepTaParam &copy);
    const KepTaParam& operator=(const KepTaParam &right);
    virtual ~KepTaParam();

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

#endif // KepTaParam_hpp
