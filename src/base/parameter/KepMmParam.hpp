//$Header$
//------------------------------------------------------------------------------
//                              KepMmParam
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
 * Declares Keplerian Mean Motion parameter class.
 */
//------------------------------------------------------------------------------
#ifndef KepMmParam_hpp
#define KepMmParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API KepMmParam : public RealParameter, OrbitData
{
public:

    KepMmParam(const std::string &name = "",
               GmatBase *obj = NULL,
               const std::string &desc = "Spacecraft Kep Mean Motion",
               const std::string &unit = "Deg");
    KepMmParam(const KepMmParam &copy);
    const KepMmParam& operator=(const KepMmParam &right);
    virtual ~KepMmParam();

    // The inherited methods from RealParameter
    virtual Real EvaluateReal();

    // The inherited methods from Parameter
    virtual Integer GetNumObjects() const;
    virtual bool AddObject(GmatBase *obj);
    virtual bool Validate();
    virtual bool Evaluate();

protected:

};

#endif // KepMmParam_hpp
