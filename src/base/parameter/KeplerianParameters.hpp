//$Header$
//------------------------------------------------------------------------------
//                             File: KeplerianParameters.hpp
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
 * Declares Keplerian related parameter classes.
 *   KepSMA, KepEcc, KepInc, KepAOP, KepRAAN, KepTA, KepMA, 
 *   KepMM, VelApoapsis, VelPeriapsis
 */
//------------------------------------------------------------------------------
#ifndef KeplerianParameters_hpp
#define KeplerianParameters_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "Rvec6Var.hpp"
#include "Rvector6.hpp"
#include "OrbitData.hpp"


//==============================================================================
//                              KepSMA
//==============================================================================
/**
 * Declares Keplerian Semimajor Axis class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepSMA : public RealVar, OrbitData
{
public:

    KepSMA(const std::string &name = "",
                GmatBase *obj = NULL,
                const std::string &desc = "",
                const std::string &unit = "Km");
    KepSMA(const KepSMA &copy);
    const KepSMA& operator=(const KepSMA &right);
    virtual ~KepSMA();

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

//==============================================================================
//                              KepEcc
//==============================================================================
/**
 * Declares Keplerian Eccentricity class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepEcc : public RealVar, OrbitData
{
public:

    KepEcc(const std::string &name = "",
           GmatBase *obj = NULL,
           const std::string &desc = "",
           const std::string &unit = " ");
    KepEcc(const KepEcc &param);
    const KepEcc& operator=(const KepEcc &right);
    virtual ~KepEcc();

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


//==============================================================================
//                              KepInc
//==============================================================================
/**
 * Declares Keplerian Inclinatin class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepInc : public RealVar, OrbitData
{
public:

    KepInc(const std::string &name = "",
           GmatBase *obj = NULL,
           const std::string &desc = "",
           const std::string &unit = "Deg");
    KepInc(const KepInc &copy);
    const KepInc& operator=(const KepInc &right);
    virtual ~KepInc();

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

//==============================================================================
//                              KepAOP
//==============================================================================
/**
 * Declares Keplerian Argument of Periapsis class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepAOP : public RealVar, OrbitData
{
public:

    KepAOP(const std::string &name = "",
           GmatBase *obj = NULL,
           const std::string &desc = "",
           const std::string &unit = "Deg");
    KepAOP(const KepAOP &copy);
    const KepAOP& operator=(const KepAOP &right);
    virtual ~KepAOP();

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

//==============================================================================
//                              KepRAAN
//==============================================================================
/**
 * Declares Keplerian Right Ascention of Ascending Node class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepRAAN : public RealVar, OrbitData
{
public:

    KepRAAN(const std::string &name = "",
            GmatBase *obj = NULL,
            const std::string &desc = "",
            const std::string &unit = "Deg");
    KepRAAN(const KepRAAN &copy);
    const KepRAAN& operator=(const KepRAAN &right);
    virtual ~KepRAAN();

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


//==============================================================================
//                              KepTA
//==============================================================================
/**
 * Declares Keplerian True Anomaly class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepTA : public RealVar, OrbitData
{
public:

    KepTA(const std::string &name = "",
          GmatBase *obj = NULL,
          const std::string &desc = "",
          const std::string &unit = "Deg");
    KepTA(const KepTA &copy);
    const KepTA& operator=(const KepTA &right);
    virtual ~KepTA();

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

//==============================================================================
//                              KepMA
//==============================================================================
/**
 * Declares Keplerian Mean Anomaly class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepMA : public RealVar, OrbitData
{
public:

    KepMA(const std::string &name = "",
          GmatBase *obj = NULL,
          const std::string &desc = "",
          const std::string &unit = "Deg");
    KepMA(const KepMA &copy);
    const KepMA& operator=(const KepMA &right);
    virtual ~KepMA();

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

//==============================================================================
//                              KepElem
//==============================================================================
/**
 * Declares Keplerian Elements class.
 *   6 elements: KepSMA, KepEcc, KepInc, KepRAAN, KepAOP, KepTA
 */
//------------------------------------------------------------------------------

class GMAT_API KepElem : public Rvec6Var, OrbitData
{
public:

    KepElem(const std::string &name = "",
            GmatBase *obj = NULL,
            const std::string &desc = "",
            const std::string &unit = "");
    KepElem(const KepElem &copy);
    const KepElem& operator=(const KepElem &right);
    virtual ~KepElem();

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

//==============================================================================
//                              KepMM
//==============================================================================
/**
 * Declares Keplerian Mean Motion class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepMM : public RealVar, OrbitData
{
public:

    KepMM(const std::string &name = "",
          GmatBase *obj = NULL,
          const std::string &desc = "Spacecraft Kep Mean Motion",
          const std::string &unit = "Deg");
    KepMM(const KepMM &copy);
    const KepMM& operator=(const KepMM &right);
    virtual ~KepMM();

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

//==============================================================================
//                              VelApoapsis
//==============================================================================
/**
 * Declares Velocity at Apoapsis class.
 */
//------------------------------------------------------------------------------

class GMAT_API VelApoapsis : public RealVar, OrbitData
{
public:

    VelApoapsis(const std::string &name = "",
                GmatBase *obj = NULL,
                const std::string &desc = "",
                const std::string &unit = "Km/Sec");
    VelApoapsis(const VelApoapsis &copy);
    const VelApoapsis& operator=(const VelApoapsis &right);
    virtual ~VelApoapsis();

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

//==============================================================================
//                              VelPeriapsis
//==============================================================================
/**
 * Declares Velocity at Periapsis class.
 */
//------------------------------------------------------------------------------

class GMAT_API VelPeriapsis : public RealVar, OrbitData
{
public:

    VelPeriapsis(const std::string &name = "",
                 GmatBase *obj = NULL,
                 const std::string &desc = "",
                 const std::string &unit = "Km/Sec");
    VelPeriapsis(const VelPeriapsis &copy);
    const VelPeriapsis& operator=(const VelPeriapsis &right);
    virtual ~VelPeriapsis();

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

#endif // KeplerianParameters_hpp
