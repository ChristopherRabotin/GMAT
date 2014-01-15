//$Id: IncomingAsymptoteParameters.hpp 2013-05-24 Youngkwang Kim $
//------------------------------------------------------------------------------
//                            File: IncomingAsymptoteParameters.hpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Youngkwang Kim
// Created: 2013/09/15
//
/**
 * Declares Incoming Asymptote related parameter classes.
 * IncAsym_RadPer, IncAsym_C3, IncAsym_RHA, IncAsym_DHA, IncAsym_BVAZI, IncAsym_TA, IncAsym_State
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"


class GMAT_API IncAsymRHA : public OrbitReal
{
public:

   IncAsymRHA(const std::string &name = "", GmatBase *obj = NULL);
   IncAsymRHA(const IncAsymRHA &copy);
   IncAsymRHA& operator=(const IncAsymRHA &right);
   virtual ~IncAsymRHA();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};

class GMAT_API IncAsymDHA : public OrbitReal
{
public:

   IncAsymDHA(const std::string &name = "", GmatBase *obj = NULL);
   IncAsymDHA(const IncAsymDHA &copy);
   IncAsymDHA& operator=(const IncAsymDHA &right);
   virtual ~IncAsymDHA();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};
class GMAT_API IncAsymBVAZI : public OrbitReal
{
public:

   IncAsymBVAZI(const std::string &name = "", GmatBase *obj = NULL);
   IncAsymBVAZI(const IncAsymBVAZI &copy);
   IncAsymBVAZI& operator=(const IncAsymBVAZI &right);
   virtual ~IncAsymBVAZI();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};

class GMAT_API IncAsymState : public OrbitRvec6
{
public:

   IncAsymState(const std::string &name = "", GmatBase *obj = NULL);
   IncAsymState(const IncAsymState &copy);
   IncAsymState& operator=(const IncAsymState &right);
   virtual ~IncAsymState();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
    
};


