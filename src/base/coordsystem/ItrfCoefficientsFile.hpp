//$Header$
//------------------------------------------------------------------------------
//                                  ItrfCoefficientsFile
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2005/01/31
//
/**
 * Definition of the ItrfCoefficientsFile class.  This is the code that reads 
 * the nutation and planetary coefficients from the file.
 *
 * The source of the nutation and planetary coefficient data is 
 * http://www.celestrak.com/software/vallado-sw.asp 
 *
 */
//------------------------------------------------------------------------------

#ifndef ItrfCoefficientsFile_hpp
#define ItrfCoefficientsFile_hpp

#include "gmatdefs.hpp"
#include "Rvector.hpp"

namespace GmatItrf
{
   enum NutationTerms
   {
      NUTATION_1996,
      NUTATION_2000
   };
   enum PlanetaryTerms
   {
      PLANETARY_1996
   };
};

class GMAT_API ItrfCoefficientsFile
{
public:

   // default constructor
   ItrfCoefficientsFile(const std::string &nutFileName = "NUTATION.DAT", 
               const std::string planFileName = "NUT85.DAT",
               GmatItrf::NutationTerms  nutTerms  = GmatItrf::NUTATION_1996,
               GmatItrf::PlanetaryTerms planTerms = GmatItrf::PLANETARY_1996);
   // copy constructor
   ItrfCoefficientsFile(const ItrfCoefficientsFile &itrfF);
   // operator = 
   const ItrfCoefficientsFile& operator=(const ItrfCoefficientsFile &itrfF);
   // destructor
   virtual ~ItrfCoefficientsFile();
   
   
   // initializes the ItrfCoefficientsFile
   virtual void Initialize();
   
   virtual Integer GetNumberOfNutationTerms();
   virtual Integer GetNumberOfPlanetaryTerms();
   virtual bool GetNutationTerms(std::vector<IntegerArray> &a5, Rvector &Aval, 
                                 Rvector &Bval, Rvector &Cval, Rvector &Dval, 
                                 Rvector &Eval, Rvector &Fval);
   virtual bool GetPlanetaryTerms(std::vector<IntegerArray> &ap10, 
                                  Rvector &Apval, Rvector &Bpval, 
                                  Rvector &Cpval, Rvector &Dpval);
   
  
protected:

   // additional protected data
   // (NOTE - static const strings are initialized in source file)
   static const Integer MAX_1996_NUT_TERMS         = 263;
   static const Real    MULT_1996_NUT              = 1.0e-06;
   static const std::string FIRST_NUT_PHRASE_1996;
   static const Integer MAX_2000_NUT_TERMS         = 106;
   static const Real    MULT_2000_NUT              = 1.0e-04;
   static const std::string FIRST_NUT_PHRASE_2000;
   static const Integer MAX_1996_PLANET_TERMS      = 112;
   static const Real    MULT_1996_PLANET           = 1.0e-04;
   static const std::string FIRST_PLAN_PHRASE_1996;
   static const Integer MAX_2000_PLANET_TERMS = 112;            // ????
   static const Real    MULT_2000_PLANET      = 1.0e-04;        // ????
   static const std::string FIRST_PLAN_PHRASE_2000; // ????
   
   
   /// number of terms in nutation logitude series <=MAX_NUT_TERMS
   Integer    nut;  
   /// number of terms in nutation planetary series <=MAX_PLANET_TERMS
   Integer    nutpl;
   
   /// multipliers for the nutation and planetary coefficients
   Real       nutMult;
   Real       planMult;
   
   std::string firstNutPhrase;
   std::string firstPlanPhrase;
   
   GmatItrf::NutationTerms  nutation;
   GmatItrf::PlanetaryTerms planetary;

   std::string nutationFileName;
   std::string planetaryFileName;
   bool filesAreInitialized;

   std::vector<IntegerArray> a;
   /// reduction terms
   Rvector*    A;
   Rvector*    B;
   Rvector*    C;
   Rvector*    D;
   Rvector*    E;
   Rvector*    F;
   /// planetary terms (F0-F9) multipliers
   std::vector<IntegerArray> ap;
   Rvector*    Ap;
   Rvector*    Bp;
   Rvector*    Cp;
   Rvector*    Dp;

   bool InitializeArrays(GmatItrf::NutationTerms nutT,
                         GmatItrf::PlanetaryTerms planT);
   
   bool IsBlank(const char* aLine);
   
};
#endif // ItrfCoefficientsFile_hpp
