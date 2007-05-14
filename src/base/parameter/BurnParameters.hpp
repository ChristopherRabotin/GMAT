//$Header$
//------------------------------------------------------------------------------
//                            File: BurnParameters.hpp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/05/27
//
/**
 * Declares Burn related parameter classes.
 *   DeltaVDir1, DeltaVDir2, DeltaVDir3
 */
//------------------------------------------------------------------------------
#ifndef BurnParameters_hpp
#define BurnParameters_hpp

#include "gmatdefs.hpp"
#include "BurnReal.hpp"


//==============================================================================
//                               DeltaVDir1
//==============================================================================
/**
 * Declares Cartesian position X class.
 */
//------------------------------------------------------------------------------

class GMAT_API DeltaVDir1 : public BurnReal
{
public:

   DeltaVDir1(const std::string &name = "", const std::string &typeName = "V",
              GmatBase *obj = NULL);
   DeltaVDir1(const DeltaVDir1 &copy);
   DeltaVDir1& operator=(const DeltaVDir1 &right);
   virtual ~DeltaVDir1();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              DeltaVDir2
//==============================================================================
/**
 * Declares Cartesian position Y class.
 */
//------------------------------------------------------------------------------

class GMAT_API DeltaVDir2 : public BurnReal
{
public:
   
   DeltaVDir2(const std::string &name = "", const std::string &typeName = "N",
              GmatBase *obj = NULL);
   DeltaVDir2(const DeltaVDir2 &copy);
   DeltaVDir2& operator=(const DeltaVDir2 &right);
   virtual ~DeltaVDir2();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              DeltaVDir3
//==============================================================================
/**
 * Declares Cartesian position Z class.
 */
//------------------------------------------------------------------------------

class GMAT_API DeltaVDir3 : public BurnReal
{
public:
   
   DeltaVDir3(const std::string &name = "", const std::string &typeName = "B",
              GmatBase *obj = NULL);
   DeltaVDir3(const DeltaVDir3 &copy);
   DeltaVDir3& operator=(const DeltaVDir3 &right);
   virtual ~DeltaVDir3();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


#endif // BurnParameters_hpp
