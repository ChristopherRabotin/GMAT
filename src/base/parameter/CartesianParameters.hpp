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
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp" //loj: 9/8/04 added

//==============================================================================
//                               CartX
//==============================================================================
/**
 * Declares Cartesian position X class.
 */
//------------------------------------------------------------------------------

class GMAT_API CartX : public OrbitReal
{
public:

   CartX(const std::string &name = "",
         GmatBase *obj = NULL,
         const std::string &desc = "",
         const std::string &unit = "Km");
   CartX(const CartX &copy);
   CartX& operator=(const CartX &right);
   virtual ~CartX();

   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//==============================================================================
//                              CartY
//==============================================================================
/**
 * Declares Cartesian position Y class.
 */
//------------------------------------------------------------------------------

class GMAT_API CartY : public OrbitReal
{
public:

   CartY(const std::string &name = "",
         GmatBase *obj = NULL,
         const std::string &desc = "",
         const std::string &unit = "Km");
   CartY(const CartY &copy);
   CartY& operator=(const CartY &right);
   virtual ~CartY();

   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
    
protected:

};

//==============================================================================
//                              CartZ
//==============================================================================
/**
 * Declares Cartesian position Z class.
 */
//------------------------------------------------------------------------------

class GMAT_API CartZ : public OrbitReal
{
public:

   CartZ(const std::string &name = "",
         GmatBase *obj = NULL,
         const std::string &desc = "",
         const std::string &unit = "Km");
   CartZ(const CartZ &copy);
   CartZ& operator=(const CartZ &right);
   virtual ~CartZ();

   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//==============================================================================
//                              CartVx
//==============================================================================
/**
 * Declares Cartesian velocity X class.
 */
//------------------------------------------------------------------------------

class GMAT_API CartVx : public OrbitReal
{
public:

   CartVx(const std::string &name = "",
          GmatBase *obj = NULL,
          const std::string &desc = "",
          const std::string &unit = "Km/Sec");
   CartVx(const CartVx &copy);
   CartVx& operator=(const CartVx &right);
   virtual ~CartVx();

   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
    
protected:

};

//==============================================================================
//                              CartVy
//==============================================================================
/**
 * Declares Cartesian velocity Y class.
 */
//------------------------------------------------------------------------------

class GMAT_API CartVy : public OrbitReal
{
public:

   CartVy(const std::string &name = "",
          GmatBase *obj = NULL,
          const std::string &desc = "",
          const std::string &unit = "Km/Sec");
   CartVy(const CartVy &copy);
   CartVy& operator=(const CartVy &right);
   virtual ~CartVy();

   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//==============================================================================
//                              CartVz
//==============================================================================
/**
 * Declares Cartesian velocity Z class.
 */
//------------------------------------------------------------------------------

class GMAT_API CartVz : public OrbitReal
{
public:

   CartVz(const std::string &name = "",
          GmatBase *obj = NULL,
          const std::string &desc = "",
          const std::string &unit = "Km/Sec");
   CartVz(const CartVz &copy);
   CartVz& operator=(const CartVz &right);
   virtual ~CartVz();

   // methods inherited from Parameter
   virtual bool Evaluate();
    
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
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

class GMAT_API CartState : public OrbitRvec6
{
public:

   CartState(const std::string &name = "",
             GmatBase *obj = NULL,
             const std::string &desc = "",
             const std::string &unit = "");
   CartState(const CartState &copy);
   CartState& operator=(const CartState &right);
   virtual ~CartState();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
    
};

#endif // CartesianParameters_hpp
