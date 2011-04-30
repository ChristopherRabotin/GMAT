//$Id$
//------------------------------------------------------------------------------
//                                  InertialAxes
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
// Created: 2004/12/28
//
/**
 * Definition of the InertialAxes class.  This is the base class for those
 * AxisSystem classes that implement inertial systems.
 *
 */
//------------------------------------------------------------------------------

#ifndef InertialAxes_hpp
#define InertialAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"

class GMAT_API InertialAxes : public AxisSystem
{
public:

   // default constructor
   InertialAxes(const std::string &itsType,
                const std::string &itsName = "");
   // copy constructor
   InertialAxes(const InertialAxes &inertial);
   // operator = for assignment
   const InertialAxes& operator=(const InertialAxes &inertial);
   // destructor
   virtual ~InertialAxes();
   
   virtual bool Initialize();
      
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   //virtual GmatBase*       Clone(void) const;

   // Parameter access methods - overridden from GmatBase
   /* placeholder - may be needed later
   virtual std::string     GetParameterText(const Integer id) const;     
   virtual Integer         GetParameterID(const std::string &str) const; 
   virtual Gmat::ParameterType
                           GetParameterType(const Integer id) const;
   virtual std::string     GetParameterTypeString(const Integer id) const;
    */
   
protected:

   enum
   {
      InertialAxesParamCount = AxisSystemParamCount,
   };
   
   //static const std::string PARAMETER_TEXT[InertialAxesParamCount - 
   //                                        AxisSystemParamCount];
   
   //static const Gmat::ParameterType PARAMETER_TYPE[InertialAxesParamCount - 
   //                                                AxisSystemParamCount];
   
   // no additional data at this time
};
#endif // InertialAxes_hpp
