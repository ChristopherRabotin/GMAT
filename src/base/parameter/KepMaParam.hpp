//$Header$
//------------------------------------------------------------------------------
//                              KepMaParam
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
 * Declares Keplerian Mean Anomaly parameter class.
 */
//------------------------------------------------------------------------------
#ifndef KepMaParam_hpp
#define KepMaParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API KepMaParam : public RealParameter, OrbitData
{
public:

    KepMaParam(const std::string &name = "",
               GmatBase *obj = NULL,
               const std::string &desc = "Spacecraft Kep Mean Anomaly",
               const std::string &unit = "Deg");
    KepMaParam(const KepMaParam &copy);
    const KepMaParam& operator=(const KepMaParam &right);
    virtual ~KepMaParam();

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

#endif // KepMaParam_hpp
