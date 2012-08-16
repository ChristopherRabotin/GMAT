//$Id$
//------------------------------------------------------------------------------
//                                  BallisticMassParameters
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. 
//
// Author: Linda Jun
// Created: 2009.03.20
//
/**
 * Implements BallisticMass related parameter classes.
 *    DryMass, DragCoeff, ReflectCoeff, DragArea, SRPArea, TotalMass
 */
//------------------------------------------------------------------------------
#ifndef BallisticMassParameters_hpp
#define BallisticMassParameters_hpp

#include "gmatdefs.hpp"
#include "BallisticMassReal.hpp"


//------------------------------------------------------------------------------
//  DryMass, DragCoeff, ReflectCoeff, DragArea, SRPArea, TotalMass
//------------------------------------------------------------------------------

class GMAT_API DryMass : public BallisticMassReal
{
public:
   
   DryMass(const std::string &name = "", GmatBase *obj = NULL);
   DryMass(const DryMass &copy);
   DryMass& operator=(const DryMass &right);
   virtual ~DryMass();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   
protected:

};

class GMAT_API DragCoeff : public BallisticMassReal
{
public:

   DragCoeff(const std::string &name = "", GmatBase *obj = NULL);
   DragCoeff(const DragCoeff &copy);
   DragCoeff& operator=(const DragCoeff &right);
   virtual ~DragCoeff();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   
protected:

};

class GMAT_API ReflectCoeff : public BallisticMassReal
{
public:

   ReflectCoeff(const std::string &name = "", GmatBase *obj = NULL);
   ReflectCoeff(const ReflectCoeff &copy);
   ReflectCoeff& operator=(const ReflectCoeff &right);
   virtual ~ReflectCoeff();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   
protected:

};

class GMAT_API DragArea : public BallisticMassReal
{
public:

   DragArea(const std::string &name = "", GmatBase *obj = NULL);
   DragArea(const DragArea &copy);
   DragArea& operator=(const DragArea &right);
   virtual ~DragArea();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   
protected:

};

class GMAT_API SRPArea : public BallisticMassReal
{
public:

   SRPArea(const std::string &name = "", GmatBase *obj = NULL);
   SRPArea(const SRPArea &copy);
   SRPArea& operator=(const SRPArea &right);
   virtual ~SRPArea();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   
protected:

};

class GMAT_API TotalMass : public BallisticMassReal
{
public:

   TotalMass(const std::string &name = "", GmatBase *obj = NULL);
   TotalMass(const TotalMass &copy);
   TotalMass& operator=(const TotalMass &right);
   virtual ~TotalMass();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   
protected:

};

#endif //BallisticMassParameters_hpp
