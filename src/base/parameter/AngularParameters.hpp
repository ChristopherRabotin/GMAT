//$Header$
//------------------------------------------------------------------------------
//                             File: AngularParameters.hpp
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
 * Declares Angular related parameter classes.
 *   SemilatusRectum, AngularMomemtumMag, AngularMomentumX, AngularMomentumY,
 *   AngularMomentumZ
 */
//------------------------------------------------------------------------------
#ifndef AngularParameters_hpp
#define AngularParameters_hpp

#include "OrbitReal.hpp"

//==============================================================================
//                              SemilatusRectum
//==============================================================================
/**
 * Declares Semilatus Rectum parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API SemilatusRectum : public OrbitReal
{
public:

   SemilatusRectum(const std::string &name = "", GmatBase *obj = NULL);
   SemilatusRectum(const SemilatusRectum &copy);
   const SemilatusRectum& operator=(const SemilatusRectum &right);
   virtual ~SemilatusRectum();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//==============================================================================
//                              AngularMomentumMag
//==============================================================================
/**
 * Declares angular momentum magnitude parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API AngularMomentumMag : public OrbitReal
{
public:

   AngularMomentumMag(const std::string &name = "", GmatBase *obj = NULL);
   AngularMomentumMag(const AngularMomentumMag &copy);
   const AngularMomentumMag& operator=(const AngularMomentumMag &right);
   virtual ~AngularMomentumMag();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              AngularMomentumX
//==============================================================================
/**
 * Declares angular momentum magnitude parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API AngularMomentumX : public OrbitReal
{
public:

   AngularMomentumX(const std::string &name = "", GmatBase *obj = NULL);
   AngularMomentumX(const AngularMomentumX &copy);
   const AngularMomentumX& operator=(const AngularMomentumX &right);
   virtual ~AngularMomentumX();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              AngularMomentumY
//==============================================================================
/**
 * Declares angular momentum magnitude parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API AngularMomentumY : public OrbitReal
{
public:

   AngularMomentumY(const std::string &name = "", GmatBase *obj = NULL);
   AngularMomentumY(const AngularMomentumY &copy);
   const AngularMomentumY& operator=(const AngularMomentumY &right);
   virtual ~AngularMomentumY();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              AngularMomentumZ
//==============================================================================
/**
 * Declares angular momentum magnitude parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API AngularMomentumZ : public OrbitReal
{
public:

   AngularMomentumZ(const std::string &name = "", GmatBase *obj = NULL);
   AngularMomentumZ(const AngularMomentumZ &copy);
   const AngularMomentumZ& operator=(const AngularMomentumZ &right);
   virtual ~AngularMomentumZ();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

#endif //AngularParameters_hpp
