//$Header$
//------------------------------------------------------------------------------
//                                  EquatorAxes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2005/02/24
//
/**
 * Definition of the EquatorAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef EquatorAxes_hpp
#define EquatorAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "DynamicAxes.hpp"
#include "DeFile.hpp"

class GMAT_API EquatorAxes : public DynamicAxes
{
public:

   // default constructor
   EquatorAxes(const std::string &itsName = "");
   // copy constructor
   EquatorAxes(const EquatorAxes &eqAxes);
   // operator = for assignment
   const EquatorAxes& operator=(const EquatorAxes &eqAxes);
   // destructor
   virtual ~EquatorAxes();
      
   // method to initialize the data
   virtual bool Initialize();
   
   virtual GmatCoordinate::ParameterUsage UsesEopFile() const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;
   virtual GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const;

   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone(void) const;

   // Parameter access methods - overridden from GmatBase
   //virtual std::string     GetParameterText(const Integer id) const;     
   //virtual Integer         GetParameterID(const std::string &str) const; 
   //virtual Gmat::ParameterType
   //                        GetParameterType(const Integer id) const;
   //virtual std::string     GetParameterTypeString(const Integer id) const;
   
protected:

   enum
   {
      EquatorAxesParamCount = DynamicAxesParamCount,
   };
   
   //static const std::string PARAMETER_TEXT[EquatorAxesParamCount - 
   //                                        DynamicAxesParamCount];
   
   //static const Gmat::ParameterType PARAMETER_TYPE[EquatorAxesParamCount - 
   //                                                DynamicAxesParamCount];
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);
   
   DeFile *theDeFile;

};
#endif // EquatorAxes_hpp
