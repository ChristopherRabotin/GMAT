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
#include "RealVar.hpp"
#include "Rvec6Var.hpp"
#include "Rvector6.hpp"
#include "OrbitData.hpp"
#include "OrbitReal.hpp"

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

   KepSMA(const std::string &name = "", GmatBase *obj = NULL,
          const std::string &desc = "", const std::string &unit = "Km");
   KepSMA(const KepSMA &copy);
   const KepSMA& operator=(const KepSMA &right);
   virtual ~KepSMA();

   // The inherited methods from Parameter
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

class GMAT_API KepEcc : public OrbitReal
{
public:

   KepEcc(const std::string &name = "", GmatBase *obj = NULL,
          const std::string &desc = "", const std::string &unit = " ");
   KepEcc(const KepEcc &param);
   const KepEcc& operator=(const KepEcc &right);
   virtual ~KepEcc();

   // The inherited methods from Parameter
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

class GMAT_API KepInc : public OrbitReal
{
public:

   KepInc(const std::string &name = "", GmatBase *obj = NULL,
          const std::string &desc = "", const std::string &unit = "Deg");
   KepInc(const KepInc &copy);
   const KepInc& operator=(const KepInc &right);
   virtual ~KepInc();

   // The inherited methods from Parameter
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

class GMAT_API KepAOP : public OrbitReal
{
public:

   KepAOP(const std::string &name = "", GmatBase *obj = NULL,
          const std::string &desc = "", const std::string &unit = "Deg");
   KepAOP(const KepAOP &copy);
   const KepAOP& operator=(const KepAOP &right);
   virtual ~KepAOP();

   // The inherited methods from Parameter
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

class GMAT_API KepRAAN : public OrbitReal
{
public:

   KepRAAN(const std::string &name = "", GmatBase *obj = NULL,
           const std::string &desc = "", const std::string &unit = "Deg");
   KepRAAN(const KepRAAN &copy);
   const KepRAAN& operator=(const KepRAAN &right);
   virtual ~KepRAAN();

   // The inherited methods from Parameter
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

class GMAT_API KepTA : public OrbitReal
{
public:

   KepTA(const std::string &name = "", GmatBase *obj = NULL,
         const std::string &desc = "", const std::string &unit = "Deg");
   KepTA(const KepTA &copy);
   const KepTA& operator=(const KepTA &right);
   virtual ~KepTA();

   // The inherited methods from Parameter
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

class GMAT_API KepMA : public OrbitReal
{
public:

   KepMA(const std::string &name = "", GmatBase *obj = NULL,
         const std::string &desc = "", const std::string &unit = "Deg");
   KepMA(const KepMA &copy);
   const KepMA& operator=(const KepMA &right);
   virtual ~KepMA();

   // The inherited methods from Parameter
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

class GMAT_API KepMM : public OrbitReal
{
public:

   KepMM(const std::string &name = "", GmatBase *obj = NULL,
         const std::string &desc = "", const std::string &unit = "Deg");
   KepMM(const KepMM &copy);
   const KepMM& operator=(const KepMM &right);
   virtual ~KepMM();

   // The inherited methods from Parameter
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

   KepElem(const std::string &name = "", GmatBase *obj = NULL,
           const std::string &desc = "", const std::string &unit = "");
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

#endif // KeplerianParameters_hpp
