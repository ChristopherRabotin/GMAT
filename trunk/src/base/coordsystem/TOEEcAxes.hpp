//$Header$
//------------------------------------------------------------------------------
//                                  TOEEcAxes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2005/05/11
//
/**
 * Definition of the TOEEcAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef TOEEcAxes_hpp
#define TOEEcAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "InertialAxes.hpp"

class GMAT_API TOEEcAxes : public InertialAxes
{
public:

   // default constructor
   TOEEcAxes(const std::string &itsName = "");
   // copy constructor
   TOEEcAxes(const TOEEcAxes &toe);
   // operator = for assignment
   const TOEEcAxes& operator=(const TOEEcAxes &toe);
   // destructor
   virtual ~TOEEcAxes();
   
   // method to initialize the data
   virtual bool Initialize();

   virtual GmatCoordinate::ParameterUsage UsesEpoch() const;
   virtual GmatCoordinate::ParameterUsage UsesEopFile() const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;
   virtual GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const;
   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone(void) const;

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
      TOEEcAxesParamCount = InertialAxesParamCount,
   };
   
   //static const std::string PARAMETER_TEXT[TOEEcAxesParamCount - 
   //                                        InertialAxesParamCount];
   
   //static const Gmat::ParameterType PARAMETER_TYPE[TOEEcAxesParamCount - 
   //                                                InertialAxesParamCount];
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);

   // no additional data at this time
};
#endif // TOEEcAxes_hpp
