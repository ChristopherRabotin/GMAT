//$Header$
//------------------------------------------------------------------------------
//                             File: BplaneParameters.hpp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/06/15
//
/**
 * Declares B-Plane related parameter classes.
 *   BdotT, BdotR
 *
 */
//------------------------------------------------------------------------------
#ifndef BplaneParameters_hpp
#define BplaneParameters_hpp

#include "BplaneReal.hpp"


//==============================================================================
//                              BdotT
//==============================================================================
/**
 * Declares B dot T class.
 */
//------------------------------------------------------------------------------

class GMAT_API BdotT : public BplaneReal
{
public:

   BdotT(const std::string &name = "", GmatBase *obj = NULL);
   BdotT(const BdotT &copy);
   const BdotT& operator=(const BdotT &right);
   virtual ~BdotT();
   
   // The inherited methods from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              BdotR
//==============================================================================
/**
 * Declares BdotR class.
 */
//------------------------------------------------------------------------------

class GMAT_API BdotR : public BplaneReal
{
public:

   BdotR(const std::string &name = "", GmatBase *obj = NULL);
   BdotR(const BdotR &copy);
   const BdotR& operator=(const BdotR &right);
   virtual ~BdotR();
   
   // The inherited methods from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

#endif // BplaneParameters_hpp
