//$Id: OutgoingAsymptoteParameters.hpp 2013-05-24 Youngkwang Kim $
//------------------------------------------------------------------------------
//                            File: OutgoingAsymptoteParameters.hpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Out. under contract
// number S-67573-G
//
// Author: Youngkwang Kim
// Created: 2013/09/15
//
/**
 * Declares Outgoing Asymptote related parameter classes.
 * OutAsym_RadPer, OutAsym_C3, OutAsym_RHA, OutAsym_DHA, OutAsym_BVAZI, OutAsym_TA, OutAsym_State
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"

class GMAT_API OutAsymRHA : public OrbitReal
{
public:

   OutAsymRHA(const std::string &name = "", GmatBase *obj = NULL);
   OutAsymRHA(const OutAsymRHA &copy);
   OutAsymRHA& operator=(const OutAsymRHA &right);
   virtual ~OutAsymRHA();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};

class GMAT_API OutAsymDHA : public OrbitReal
{
public:

   OutAsymDHA(const std::string &name = "", GmatBase *obj = NULL);
   OutAsymDHA(const OutAsymDHA &copy);
   OutAsymDHA& operator=(const OutAsymDHA &right);
   virtual ~OutAsymDHA();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};
class GMAT_API OutAsymBVAZI : public OrbitReal
{
public:

   OutAsymBVAZI(const std::string &name = "", GmatBase *obj = NULL);
   OutAsymBVAZI(const OutAsymBVAZI &copy);
   OutAsymBVAZI& operator=(const OutAsymBVAZI &right);
   virtual ~OutAsymBVAZI();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};

class GMAT_API OutAsymState : public OrbitRvec6
{
public:

   OutAsymState(const std::string &name = "", GmatBase *obj = NULL);
   OutAsymState(const OutAsymState &copy);
   OutAsymState& operator=(const OutAsymState &right);
   virtual ~OutAsymState();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
    
};


