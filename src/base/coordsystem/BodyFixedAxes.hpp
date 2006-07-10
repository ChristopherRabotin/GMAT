//$Header$
//------------------------------------------------------------------------------
//                                  BodyFixedAxes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2005/01/25
//
/**
 * Definition of the BodyFixedAxes class.
 *
 * @note There are three data files currently needed:
 *    EOP file containing polar motion (x,y) and UT1-UTC offset
 *    coefficient file containing nutation and planetary coeffifients
 */
//------------------------------------------------------------------------------

#ifndef BodyFixedAxes_hpp
#define BodyFixedAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "DynamicAxes.hpp"
#include "EopFile.hpp"
#include "DeFile.hpp"
#include "ItrfCoefficientsFile.hpp"

class GMAT_API BodyFixedAxes : public DynamicAxes
{
public:

   // default constructor
   BodyFixedAxes(const std::string &itsName = "");
   // copy constructor
   BodyFixedAxes(const BodyFixedAxes &bfAxes);
   // operator = for assignment
   const BodyFixedAxes& operator=(const BodyFixedAxes &bfAxes);
   // destructor
   virtual ~BodyFixedAxes();
   
   // methods to set files for reading
   // 3 are needed:
   //    leap second file (NOTE - this should be done in the utiltities!!)
   //    EOP file containing polar motion (x,y) and UT1-UTC offset
   //    coefficient file containing nutation and planetary coeffifients
   
   virtual GmatCoordinate::ParameterUsage UsesEopFile() const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;
   virtual GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const;
   
// method to initialize the data
   virtual bool Initialize();
   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone(void) const;

   // Parameter access methods - overridden from GmatBase
   /*
   virtual std::string     GetParameterText(const Integer id) const;     
   virtual Integer         GetParameterID(const std::string &str) const; 
   virtual Gmat::ParameterType
                           GetParameterType(const Integer id) const;
   virtual std::string     GetParameterTypeString(const Integer id) const;
   virtual std::string     GetStringParameter(const Integer id) const;
   virtual bool            SetStringParameter(const Integer id, 
                                              const std::string &value);
   virtual std::string     GetStringParameter(const std::string &label) const;
   virtual bool            SetStringParameter(const std::string &label, 
                                              const std::string &value);
    */
   
   
protected:

   enum
   {
      BodyFixedAxesParamCount = DynamicAxesParamCount,
   };
   
   //static const std::string PARAMETER_TEXT[BodyFixedAxesParamCount - 
   //                                        DynamicAxesParamCount];
   
   //static const Gmat::ParameterType PARAMETER_TYPE[BodyFixedAxesParamCount - 
   //                                                DynamicAxesParamCount];
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);
   
   //Rmatrix33      precT, nutT, stT, stDerivT, pmT;
   
   //const Real *precData;  // moved to AxisSystem
   //const Real *nutData;
   //const Real *stData;
   //const Real *stDerivData;
   //const Real *pmData;
   
   DeFile     *de;
};
#endif // BodyFixedAxes_hpp
