//$Header$
//------------------------------------------------------------------------------
//                            File: CartesianParameters.hpp
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
 * Declares Cartesian related parameter classes.
 *   CartX, CartY, CartZ, CartVx, CartVy, CartVz, CartState
 */
//------------------------------------------------------------------------------
#ifndef CartesianParameters_hpp
#define CartesianParameters_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "Rvec6Var.hpp"
#include "Rvector6.hpp"
#include "OrbitData.hpp"

//==============================================================================
//                               CartX
//==============================================================================
/**
 * Declares Cartesian position X class.
 */
//------------------------------------------------------------------------------

class GMAT_API CartX : public RealVar, OrbitData
{
public:

    CartX(const std::string &name = "",
          GmatBase *obj = NULL,
          const std::string &desc = "",
          const std::string &unit = "Km");
    CartX(const CartX &copy);
    const CartX& operator=(const CartX &right);
    virtual ~CartX();

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
//                              CartY
//==============================================================================
/**
 * Declares Cartesian position Y class.
 */
//------------------------------------------------------------------------------

class GMAT_API CartY : public RealVar, OrbitData
{
public:

    CartY(const std::string &name = "",
          GmatBase *obj = NULL,
          const std::string &desc = "",
          const std::string &unit = "Km");
    CartY(const CartY &copy);
    const CartY& operator=(const CartY &right);
    virtual ~CartY();

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
//                              CartZ
//==============================================================================
/**
 * Declares Cartesian position Z class.
 */
//------------------------------------------------------------------------------

class GMAT_API CartZ : public RealVar, OrbitData
{
public:

    CartZ(const std::string &name = "",
          GmatBase *obj = NULL,
          const std::string &desc = "",
          const std::string &unit = "Km");
    CartZ(const CartZ &copy);
    const CartZ& operator=(const CartZ &right);
    virtual ~CartZ();

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
//                              CartVx
//==============================================================================
/**
 * Declares Cartesian velocity X class.
 */
//------------------------------------------------------------------------------

class GMAT_API CartVx : public RealVar, OrbitData
{
public:

    CartVx(const std::string &name = "",
           GmatBase *obj = NULL,
           const std::string &desc = "",
           const std::string &unit = "Km/Sec");
    CartVx(const CartVx &copy);
    const CartVx& operator=(const CartVx &right);
    virtual ~CartVx();

    // The inherited methods from Parameter
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
//                              CartVy
//==============================================================================
/**
 * Declares Cartesian velocity Y class.
 */
//------------------------------------------------------------------------------

class GMAT_API CartVy : public RealVar, OrbitData
{
public:

    CartVy(const std::string &name = "",
           GmatBase *obj = NULL,
           const std::string &desc = "",
           const std::string &unit = "Km/Sec");
    CartVy(const CartVy &copy);
    const CartVy& operator=(const CartVy &right);
    virtual ~CartVy();

    // The inherited methods from Parameter
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
//                              CartVz
//==============================================================================
/**
 * Declares Cartesian velocity Z class.
 */
//------------------------------------------------------------------------------

class GMAT_API CartVz : public RealVar, OrbitData
{
public:

    CartVz(const std::string &name = "",
           GmatBase *obj = NULL,
           const std::string &desc = "",
           const std::string &unit = "Km/Sec");
    CartVz(const CartVz &copy);
    const CartVz& operator=(const CartVz &right);
    virtual ~CartVz();

    // The inherited methods from Parameter
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
//                              CartState
//==============================================================================
/**
 * Declares Cartesian state class.
 *   6 elements: CartX, CartY, CartZ, CartVx, CartVy, CartVz
 */
//------------------------------------------------------------------------------

class GMAT_API CartState : public Rvec6Var, OrbitData
{
public:

    CartState(const std::string &name = "",
              GmatBase *obj = NULL,
              const std::string &desc = "",
              const std::string &unit = "");
    CartState(const CartState &copy);
    const CartState& operator=(const CartState &right);
    virtual ~CartState();

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

#endif // CartesianParameters_hpp
