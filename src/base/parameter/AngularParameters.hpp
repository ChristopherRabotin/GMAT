//$Header$
//------------------------------------------------------------------------------
//                             File: AngularParameters.hpp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/03/12
//
/**
 * Declares Angular related parameter classes.
 *   SemilatusRectum
 */
//------------------------------------------------------------------------------
#ifndef AngularParameters_hpp
#define AngularParameters_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "OrbitData.hpp"

//==============================================================================
//                              SemilatusRectum
//==============================================================================
/**
 * Declares Semilatus Rectum parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API SemilatusRectum : public RealVar, OrbitData
{
public:

    SemilatusRectum(const std::string &name = "",
                    GmatBase *obj = NULL,
                    const std::string &desc = "",
                    const std::string &unit = "Km");
    SemilatusRectum(const SemilatusRectum &copy);
    const SemilatusRectum& operator=(const SemilatusRectum &right);
    virtual ~SemilatusRectum();

    // The inherited methods from RealVar
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

#endif //AngularParameters_hpp
