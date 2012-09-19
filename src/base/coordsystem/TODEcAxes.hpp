//$Id$
//------------------------------------------------------------------------------
//                                  TODEcAxes
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2005/05/11
//
/**
 * Definition of the TODEcAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef TODEcAxes_hpp
#define TODEcAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "TrueOfDateAxes.hpp"

class GMAT_API TODEcAxes : public TrueOfDateAxes
{
public:

   // default constructor
   TODEcAxes(const std::string &itsName = "");
   // copy constructor
   TODEcAxes(const TODEcAxes &tod);
   // operator = for assignment
   const TODEcAxes& operator=(const TODEcAxes &tod);
   // destructor
   virtual ~TODEcAxes();
   
   // method to initialize the data
   virtual bool Initialize();

   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone() const;
   
protected:

   enum
   {
      TODEcAxesParamCount = TrueOfDateAxesParamCount,
   };
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);

};
#endif // TODEcAxes_hpp
