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
 *   KepSMA, KepEcc, KepInc, KepAOP, KepRAAN, KepTA, KepMA, KepMM, KepElem
 */
//------------------------------------------------------------------------------
#ifndef KeplerianParameters_hpp
#define KeplerianParameters_hpp

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"

//==============================================================================
//                              KepSMA
//==============================================================================
/**
 * Declares Keplerian Semimajor Axis class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepSMA : public OrbitReal
{
public:

   KepSMA(const std::string &name = "", GmatBase *obj = NULL);
   KepSMA(const KepSMA &copy);
   const KepSMA& operator=(const KepSMA &right);
   virtual ~KepSMA();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//==============================================================================
//                              KepEcc
//==============================================================================
/**
 * Declares Keplerian Eccentricity class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepEcc : public OrbitReal
{
public:

   KepEcc(const std::string &name = "", GmatBase *obj = NULL);
   KepEcc(const KepEcc &param);
   const KepEcc& operator=(const KepEcc &right);
   virtual ~KepEcc();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//==============================================================================
//                              KepInc
//==============================================================================
/**
 * Declares Keplerian Inclinatin class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepInc : public OrbitReal
{
public:

   KepInc(const std::string &name = "", GmatBase *obj = NULL);
   KepInc(const KepInc &copy);
   const KepInc& operator=(const KepInc &right);
   virtual ~KepInc();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//==============================================================================
//                              KepAOP
//==============================================================================
/**
 * Declares Keplerian Argument of Periapsis class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepAOP : public OrbitReal
{
public:

   KepAOP(const std::string &name = "", GmatBase *obj = NULL);
   KepAOP(const KepAOP &copy);
   const KepAOP& operator=(const KepAOP &right);
   virtual ~KepAOP();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//==============================================================================
//                              KepRAAN
//==============================================================================
/**
 * Declares Keplerian Right Ascention of Ascending Node class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepRAAN : public OrbitReal
{
public:

   KepRAAN(const std::string &name = "", GmatBase *obj = NULL);
   KepRAAN(const KepRAAN &copy);
   const KepRAAN& operator=(const KepRAAN &right);
   virtual ~KepRAAN();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//==============================================================================
//                              KepRADN
//==============================================================================
/**
 * Declares Keplerian Right Ascention of Ascending Node class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepRADN : public OrbitReal
{
public:

   KepRADN(const std::string &name = "", GmatBase *obj = NULL);
   KepRADN(const KepRADN &copy);
   const KepRADN& operator=(const KepRADN &right);
   virtual ~KepRADN();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//==============================================================================
//                              KepTA
//==============================================================================
/**
 * Declares Keplerian True Anomaly class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepTA : public OrbitReal
{
public:

   KepTA(const std::string &name = "", GmatBase *obj = NULL);
   KepTA(const KepTA &copy);
   const KepTA& operator=(const KepTA &right);
   virtual ~KepTA();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//==============================================================================
//                              KepMA
//==============================================================================
/**
 * Declares Keplerian Mean Anomaly class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepMA : public OrbitReal
{
public:

   KepMA(const std::string &name = "", GmatBase *obj = NULL);
   KepMA(const KepMA &copy);
   const KepMA& operator=(const KepMA &right);
   virtual ~KepMA();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//==============================================================================
//                              KepMM
//==============================================================================
/**
 * Declares Keplerian Mean Motion class.
 */
//------------------------------------------------------------------------------

class GMAT_API KepMM : public OrbitReal
{
public:

   KepMM(const std::string &name = "", GmatBase *obj = NULL);
   KepMM(const KepMM &copy);
   const KepMM& operator=(const KepMM &right);
   virtual ~KepMM();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
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

class GMAT_API KepElem : public OrbitRvec6
{
public:

   KepElem(const std::string &name = "", GmatBase *obj = NULL);
   KepElem(const KepElem &copy);
   const KepElem& operator=(const KepElem &right);
   virtual ~KepElem();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:
    
};

#endif // KeplerianParameters_hpp
