//$Header$
//------------------------------------------------------------------------------
//                              KepAopParam
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
 * Declares Keplerian Argument of Periapsis parameter class.
 */
//------------------------------------------------------------------------------
#ifndef KepAopParam_hpp
#define KepAopParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API KepAopParam : public RealParameter, OrbitData
{
public:

    KepAopParam(const std::string &name = "",
                GmatBase *obj = NULL,
                const std::string &desc = "Spacecraft Kep Argument of Periapsis",
                const std::string &unit = "Deg");
    KepAopParam(const KepAopParam &copy);
    const KepAopParam& operator=(const KepAopParam &right);
    virtual ~KepAopParam();

    // The inherited methods from RealParameter
    virtual Real EvaluateReal();

    // The inherited methods from Parameter
    virtual Integer GetNumObjects() const;
    virtual bool AddObject(GmatBase *obj);
    virtual bool Validate();
    virtual bool Evaluate();

protected:

};

#endif // KepAopParam_hpp
