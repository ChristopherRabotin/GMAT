//$Header$
//------------------------------------------------------------------------------
//                             File: KeplerianParameters.cpp
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
 * Implements Keplerian related parameter classes.
 *   KepSMA, KepEcc, KepInc, KepAOP, KepRAAN, KepTA, KepMA, KepMM, KepElem
 */
//------------------------------------------------------------------------------
#include "KeplerianParameters.hpp"

//==============================================================================
//                              KepSMA
//==============================================================================
/**
 * Implements Keplerian Semimajor Axis class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepSMA(const std::string &name, GmatBase *obj,
//        const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
KepSMA::KepSMA(const std::string &name, GmatBase *obj,
               const std::string &desc, const std::string &unit)
   : OrbitReal(name, "SMA", SYSTEM_PARAM, obj, desc, unit, false)
{
   AddRefObject(obj);
}

//------------------------------------------------------------------------------
// KepSMA(const KepSMA &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepSMA::KepSMA(const KepSMA &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const KepSMA& operator=(const KepSMA &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepSMA&
KepSMA::operator=(const KepSMA &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~KepSMA()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepSMA::~KepSMA()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool KepSMA::Evaluate()
{
   mRealValue = OrbitData::GetKepReal("KepSMA");    
    
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* KepSMA::Clone(void) const
{
   return new KepSMA(*this);
}

//==============================================================================
//                              KepEcc
//==============================================================================
/**
 * Implements Keplerian Eccentricity class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepEcc(const std::string &name, GmatBase *obj,
//        const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
KepEcc::KepEcc(const std::string &name, GmatBase *obj,
               const std::string &desc, const std::string &unit)
   : OrbitReal(name, "ECC", SYSTEM_PARAM, obj, desc, unit, false)
{
   AddRefObject(obj);
}

//------------------------------------------------------------------------------
// KepEcc(const KepEcc &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepEcc::KepEcc(const KepEcc &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const KepEcc& operator=(const KepEcc &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepEcc&
KepEcc::operator=(const KepEcc &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~KepEcc()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepEcc::~KepEcc()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool KepEcc::Evaluate()
{
   mRealValue = OrbitData::GetKepReal("KepEcc");
    
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* KepEcc::Clone(void) const
{
   return new KepEcc(*this);
}


//==============================================================================
//                              KepInc
//==============================================================================
/**
 * Implements Keplerian Inclinatin class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepInc(const std::string &name, GmatBase *obj,
//        const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
KepInc::KepInc(const std::string &name, GmatBase *obj,
               const std::string &desc, const std::string &unit)
   : OrbitReal(name, "INC", SYSTEM_PARAM, obj, desc, unit, false)
{
   AddRefObject(obj);
}

//------------------------------------------------------------------------------
// KepInc(const KepInc &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepInc::KepInc(const KepInc &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const KepInc& operator=(const KepInc &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepInc&
KepInc::operator=(const KepInc &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~KepInc()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepInc::~KepInc()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool KepInc::Evaluate()
{
   mRealValue = OrbitData::GetKepReal("KepInc");    
    
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* KepInc::Clone(void) const
{
   return new KepInc(*this);
}

//==============================================================================
//                              KepAOP
//==============================================================================
/**
 * Implements Keplerian Argument of Periapsis class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepAOP(const std::string &name, GmatBase *obj,
//        const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
KepAOP::KepAOP(const std::string &name, GmatBase *obj,
               const std::string &desc, const std::string &unit)
   : OrbitReal(name, "AOP", SYSTEM_PARAM, obj, desc, unit, false)
{
   AddRefObject(obj);
}

//------------------------------------------------------------------------------
// KepAOP(const KepAOP &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepAOP::KepAOP(const KepAOP &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const KepAOP& operator=(const KepAOP &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepAOP&
KepAOP::operator=(const KepAOP &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~KepAOP()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepAOP::~KepAOP()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool KepAOP::Evaluate()
{
   mRealValue = OrbitData::GetKepReal("KepAOP");    
    
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* KepAOP::Clone(void) const
{
   return new KepAOP(*this);
}

//==============================================================================
//                              KepRAAN
//==============================================================================
/**
 * Implements Keplerian Right Ascention of Ascending Node class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepRAAN(const std::string &name, GmatBase *obj,
//              const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
KepRAAN::KepRAAN(const std::string &name, GmatBase *obj,
                 const std::string &desc, const std::string &unit)
   : OrbitReal(name, "RAAN", SYSTEM_PARAM, obj, desc, unit, false)
{
   AddRefObject(obj);
}

//------------------------------------------------------------------------------
// KepRAAN(const KepRAAN &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepRAAN::KepRAAN(const KepRAAN &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const KepRAAN& operator=(const KepRAAN &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepRAAN&
KepRAAN::operator=(const KepRAAN &right)
{
   if (this != &right)
      OrbitReal::operator=(right);
    
   return *this;
}

//------------------------------------------------------------------------------
// ~KepRAAN()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepRAAN::~KepRAAN()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool KepRAAN::Evaluate()
{
   mRealValue = OrbitData::GetKepReal("KepRAAN");    
    
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* KepRAAN::Clone(void) const
{
   return new KepRAAN(*this);
}

//==============================================================================
//                              KepTA
//==============================================================================
/**
 * Implements Keplerian True Anomaly class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepTA(const std::string &name, GmatBase *obj,
//       const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
KepTA::KepTA(const std::string &name, GmatBase *obj,
             const std::string &desc, const std::string &unit)
   : OrbitReal(name, "TA", SYSTEM_PARAM, obj, desc, unit, false)
{
   AddRefObject(obj);
}

//------------------------------------------------------------------------------
// KepTA(const KepTA &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepTA::KepTA(const KepTA &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const KepTA& operator=(const KepTA &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepTA&
KepTA::operator=(const KepTA &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~KepTA()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepTA::~KepTA()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool KepTA::Evaluate()
{
   mRealValue = OrbitData::GetKepReal("KepTA");    
    
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* KepTA::Clone(void) const
{
   return new KepTA(*this);
}

//==============================================================================
//                              KepMA
//==============================================================================
/**
 * Implements Keplerian Mean Anomaly class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepMA(const std::string &name, GmatBase *obj,
//       const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
KepMA::KepMA(const std::string &name, GmatBase *obj,
             const std::string &desc, const std::string &unit)
   : OrbitReal(name, "MA", SYSTEM_PARAM, obj, desc, unit, false)
{
   AddRefObject(obj);
}

//------------------------------------------------------------------------------
// KepMA(const KepMA &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepMA::KepMA(const KepMA &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const KepMA& operator=(const KepMA &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepMA&
KepMA::operator=(const KepMA &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~KepMA()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepMA::~KepMA()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool KepMA::Evaluate()
{
   mRealValue = OrbitData::GetKepReal("KepMA");    
    
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* KepMA::Clone(void) const
{
   return new KepMA(*this);
}

//==============================================================================
//                              KepMM
//==============================================================================
/**
 * Implements Keplerian Mean Motion class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepMM(const std::string &name, GmatBase *obj,
//       const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
KepMM::KepMM(const std::string &name, GmatBase *obj,
             const std::string &desc, const std::string &unit)
   : OrbitReal(name, "MM", SYSTEM_PARAM, obj, desc, unit, false)
{
   AddRefObject(obj);
}

//------------------------------------------------------------------------------
// KepMM(const KepMM &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepMM::KepMM(const KepMM &copy)
   : OrbitReal(copy)
{
}

//------------------------------------------------------------------------------
// const KepMM& operator=(const KepMM &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepMM&
KepMM::operator=(const KepMM &right)
{
   if (this != &right)
      OrbitReal::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~KepMM()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepMM::~KepMM()
{
}

//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
//------------------------------------------------------------------------------
bool KepMM::Evaluate()
{
   mRealValue = OrbitData::GetOtherKepReal("KepMM");  //loj: 4/28/04 changed from KepMm
    
   if (mRealValue == OrbitData::ORBIT_REAL_UNDEFINED)
      return false;
   else
      return true;
}

//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* KepMM::Clone(void) const
{
   return new KepMM(*this);
}


//==============================================================================
//                              KepElem
//==============================================================================
/**
 * Implements Keplerian Elements class.
 *   6 elements: KepSMA, KepEcc, KepInc, KepRAAN, KepAOP, KepTA
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KepElem(const std::string &name, GmatBase *obj,
//         const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
KepElem::KepElem(const std::string &name, GmatBase *obj,
                 const std::string &desc, const std::string &unit)
   : OrbitRvec6(name, "KepElem", SYSTEM_PARAM, obj, desc, unit, false)
{
   // Parameter member data
   mIsPlottable = false; //loj: 9/8/04 need more work in Plot to make this plottable
   
   AddRefObject(obj);
}

//------------------------------------------------------------------------------
// KepElem(const KepElem &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
KepElem::KepElem(const KepElem &copy)
   : OrbitRvec6(copy)
{
}

//------------------------------------------------------------------------------
// const KepElem& operator=(const KepElem &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
const KepElem&
KepElem::operator=(const KepElem &right)
{
   if (this != &right)
      OrbitRvec6::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~KepElem()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
KepElem::~KepElem()
{
}


//--------------------------------------
// Inherited methods from Parameter
//--------------------------------------

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated.
 */
//------------------------------------------------------------------------------
bool KepElem::Evaluate()
{
   mRvec6Value.Set(OrbitData::GetKepReal("KepSMA"),
                   GetKepReal("KepEcc"),
                   GetKepReal("KepInc"),
                   GetKepReal("KepRAAN"),
                   GetKepReal("KepAOP"),
                   GetKepReal("KepTA"));

   return mRvec6Value.IsValid(ORBIT_REAL_UNDEFINED);
}

//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* KepElem::Clone(void) const
{
   return new KepElem(*this);
}
