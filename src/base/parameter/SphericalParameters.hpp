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
 *   RMag, VMag, SphRA, SphDec, SphElem
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
//                              SphRMag
//==============================================================================
/**
 * Declares Magnitude of Position class.
 */
//------------------------------------------------------------------------------

class GMAT_API SphRMag : public OrbitReal
{
public:

   SphRMag(const std::string &name = "", GmatBase *obj = NULL,
           const std::string &desc = "", const std::string &unit = "Km/Sec");
   SphRMag(const SphRMag &copy);
   SphRMag& operator=(const SphRMag &right);
   virtual ~SphRMag();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
    
protected:

};

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
   SphRA& operator=(const SphRA &right);
   virtual ~SphRA();

    // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

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
   SphDec& operator=(const SphDec &right);
   virtual ~SphDec();
   
   // The inherited methods from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//==============================================================================
//                              SphVMag
//==============================================================================
/**
 * Declares Magnitude of Velocity class.
 */
//------------------------------------------------------------------------------

class GMAT_API SphVMag : public OrbitReal
{
public:

   SphVMag(const std::string &name = "", GmatBase *obj = NULL,
           const std::string &desc = "", const std::string &unit = "Km/Sec");
   SphVMag(const SphVMag &copy);
   SphVMag& operator=(const SphVMag &right);
   virtual ~SphVMag();
    
   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
    
protected:

};

//==============================================================================
//                              SphRAV
//==============================================================================
/**
 * Declares Spherical Right Ascension of velocity parameter class.
 */
//------------------------------------------------------------------------------
class GMAT_API SphRAV : public OrbitReal
{
public:

   SphRAV(const std::string &name = "", GmatBase *obj = NULL,
          const std::string &desc = "", const std::string &unit = "Deg");
   SphRAV(const SphRAV &copy);
   SphRAV& operator=(const SphRAV &right);
   virtual ~SphRAV();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//==============================================================================
//                              SphDecV
//==============================================================================
/**
 * Declares Spherical Declination of Velocity parameter class.
 */
//------------------------------------------------------------------------------
class GMAT_API SphDecV : public OrbitReal
{
public:

   SphDecV(const std::string &name = "", GmatBase *obj = NULL,
        const std::string &desc = "", const std::string &unit = "Deg");
   SphDecV(const SphDecV &copy);
   SphDecV& operator=(const SphDecV &right);
   virtual ~SphDecV();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//==============================================================================
//                              SphElem
//==============================================================================
/**
 * Declares Spherical Elements class.
 *   Elements are SphRMag, SphRA, SphDec, SphVMag, SphRAV, SphDecV
 */
//------------------------------------------------------------------------------
class GMAT_API SphElem : public Rvec6Var, public OrbitData
{
public:

   SphElem(const std::string &name = "",
           GmatBase *obj = NULL,
           const std::string &desc = "",
           const std::string &unit = "");
   SphElem(const SphElem &copy);
   SphElem& operator=(const SphElem &right);
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

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

#endif // SphericalParameters_hpp
