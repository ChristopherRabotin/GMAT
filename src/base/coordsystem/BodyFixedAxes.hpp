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
 *    leap second file (NOTE - this should be done in the utiltities!!)
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
#include "ItrfCoefficientsFile.hpp"

class GMAT_API BodyFixedAxes : public DynamicAxes
{
public:

   // default constructor
   BodyFixedAxes(const std::string &itsName = "");
   // constructor that sets the filenames
   BodyFixedAxes(const std::string eopFile,
                 const std::string coefFile,
                 const std::string &itsName = "");
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
   
   // method to initialize the data
   virtual void Initialize(); 
   
   // methods to set the files to use
   virtual void SetEopFile(EopFile *eopF);
   virtual void SetCoefficientsFile(ItrfCoefficientsFile *itrfF);

   // methods to convert to/from the MJ2000 Equatorial axis system
   virtual bool RotateToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
                                 Rvector &outState); 
   virtual bool RotateFromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
                                   Rvector &outState); 

   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone(void) const;

   // Parameter access methods - overridden from GmatBase
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
   
   
protected:

   enum
   {
      EOP_FILENAME = DynamicAxesParamCount,
      COEFF_FILENAME,
      BodyFixedAxesParamCount,
   };
   
   static const std::string PARAMETER_TEXT[BodyFixedAxesParamCount - 
                                           DynamicAxesParamCount];
   
   static const Gmat::ParameterType PARAMETER_TYPE[BodyFixedAxesParamCount - 
                                                   DynamicAxesParamCount];
   
   static const Real  JD_OF_JANUARY_1_1997 = 2450449.5;  // correct????
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch);

   
   std::string               eopFileName;
   std::string               itrfFileName;
         
   EopFile                   *eop;
   ItrfCoefficientsFile      *itrf;
   
   //Rmatrix                   pm;
   std::vector<IntegerArray> a, ap;
   Rvector                   A, B, C, D, E, F, Ap, Bp, Cp, Dp;
   
   // methods from Steve Queen (C code - not yet fully converted to c++
   // and GMAT style)
   //Reduction redux96_init(const char *fname_coeff, const char *fname_eop, 
   //                       int nut, int nutpl, Real jday );
   //void redux96_rtq( Reduction *ITRF, Real Rj2000[], Real Ritrf[], Real jday );
   
};
#endif // BodyFixedAxes_hpp
