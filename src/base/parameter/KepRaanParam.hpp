//$Header$
//------------------------------------------------------------------------------
//                              KepRaanParam
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
 * Declares Keplerian Right Ascension of Ascending Node parameter class.
 */
//------------------------------------------------------------------------------
#ifndef KepRaanParam_hpp
#define KepRaanParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "OrbitData.hpp"

class GMAT_API KepRaanParam : public RealParameter, OrbitData
{
public:

    KepRaanParam(const std::string &name = "",
                 GmatBase *obj = NULL,
                 const std::string &desc = "Spacecraft Kep Right Ascension of Ascending Node",
                 const std::string &unit = "Deg");
    KepRaanParam(const KepRaanParam &copy);
    const KepRaanParam& operator=(const KepRaanParam &right);
    virtual ~KepRaanParam();

    // The inherited methods from RealParameter
    virtual Real EvaluateReal();

    // The inherited methods from Parameter
    virtual Integer GetNumObjects() const;
    virtual bool AddObject(GmatBase *obj);
    virtual bool Validate();
    virtual bool Evaluate();

protected:

};

#endif // KepRaanParam_hpp
