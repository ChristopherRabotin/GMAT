//$Header$
//------------------------------------------------------------------------------
//                              CurrentA1MjdParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/12
//
/**
 * Declares Current A1Mjd parameter class.
 */
//------------------------------------------------------------------------------
#ifndef CurrentA1MjdParam_hpp
#define CurrentA1MjdParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "TimeData.hpp"

class GMAT_API CurrentA1MjdParam : public RealParameter, TimeData
{
public:

    CurrentA1MjdParam(const std::string &name = "",
                      GmatBase *obj = NULL,
                      const std::string &desc = "Current A1Mjd",
                      const std::string &unit = "A1 Mod. Julian days");
    CurrentA1MjdParam(const CurrentA1MjdParam &copy);
    CurrentA1MjdParam& operator= (const CurrentA1MjdParam &right); 
    virtual ~CurrentA1MjdParam();

    // The inherited methods from RealParameter
    virtual Real EvaluateReal();

    // The inherited methods from Parameter
    virtual Integer GetNumObjects() const;
    virtual bool AddObject(GmatBase *obj);
    virtual bool Validate();
    virtual bool Evaluate();

protected:
    
private:

};

#endif // CurrentA1MjdParam_hpp
