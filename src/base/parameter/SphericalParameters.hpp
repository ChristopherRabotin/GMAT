//$Header$
//------------------------------------------------------------------------------
//                              SphericalParameters
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
 * Declares Spacecraft Spehrical parameter classes.
 *   SphRA, SphDec, SphElem
 */
//------------------------------------------------------------------------------
#ifndef SphericalParameters_hpp
#define SphericalParameters_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "Rvec6Var.hpp"
#include "Rvector6.hpp"
#include "OrbitData.hpp"
#include "OrbitReal.hpp"

//==============================================================================
//                              SphRA
//==============================================================================
/**
 * Declares Spherical Right Ascension parameter class.
 */
//------------------------------------------------------------------------------
class GMAT_API SphRA : public OrbitReal
{
public:

    SphRA(const std::string &name = "",
          GmatBase *obj = NULL,
          const std::string &desc = "",
          const std::string &unit = "Deg");
    SphRA(const SphRA &copy);
    const SphRA& operator=(const SphRA &right);
    virtual ~SphRA();

    // The inherited methods from Parameter
    virtual bool Evaluate();

protected:

};


//==============================================================================
//                              SphDec
//==============================================================================
/**
 * Declares Spherical Declination parameter class.
 */
//------------------------------------------------------------------------------
class GMAT_API SphDec : public OrbitReal
{
public:

    SphDec(const std::string &name = "",
           GmatBase *obj = NULL,
           const std::string &desc = "",
           const std::string &unit = "Deg");
    SphDec(const SphDec &copy);
    const SphDec& operator=(const SphDec &right);
    virtual ~SphDec();

    // The inherited methods from Parameter
    virtual bool Evaluate();

protected:

};


//==============================================================================
//                              SphElem
//==============================================================================
/**
 * Declares Spherical Elements class.
 *   Elements are SphRMag, SphRA, SphDec, SphVMag, SphVRA, SphVDec
*/
//------------------------------------------------------------------------------
class GMAT_API SphElem : public Rvec6Var, OrbitData
{
public:

    SphElem(const std::string &name = "",
            GmatBase *obj = NULL,
            const std::string &desc = "",
            const std::string &unit = "");
    SphElem(const SphElem &copy);
    const SphElem& operator=(const SphElem &right);
    virtual ~SphElem();

    // The inherited methods from Rvec6Var
    virtual Rvector6 EvaluateRvector6();
    
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


#endif // SphericalParameters_hpp
