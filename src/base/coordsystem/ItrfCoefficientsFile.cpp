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
// Author: Wendy C. Shoan
// Created: 2005/01/31
//
/**
 * Implementation of the ItrfCoefficientsFile class.  This is the code that reads 
 * the nutation and planetary coefficients from the file.
 *
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "gmatdefs.hpp"
#include "ItrfCoefficientsFile.hpp"
#include "RealUtilities.hpp"   // for GmatMathUtil
#include "UtilityException.hpp"


using namespace GmatMathUtil; // for trig functions, angle conversions


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string ItrfCoefficientsFile::FIRST_NUT_PHRASE_1996  = "1996 IAU";
const std::string ItrfCoefficientsFile::FIRST_NUT_PHRASE_2000  = "2000 IAU";
const std::string ItrfCoefficientsFile::FIRST_PLAN_PHRASE_1996 = "1996 IAU";
const std::string ItrfCoefficientsFile::FIRST_PLAN_PHRASE_2000 = "unknown"; // ????

const Integer ItrfCoefficientsFile::MAX_1996_NUT_TERMS         = 263;
const Real    ItrfCoefficientsFile::MULT_1996_NUT              = 1.0e-06;
const Integer ItrfCoefficientsFile::MAX_2000_NUT_TERMS         = 106;
const Real    ItrfCoefficientsFile::MULT_2000_NUT              = 1.0e-04;
const Integer ItrfCoefficientsFile::MAX_1996_PLANET_TERMS      = 112;
const Real    ItrfCoefficientsFile::MULT_1996_PLANET           = 1.0e-04;
const Integer ItrfCoefficientsFile::MAX_2000_PLANET_TERMS = 112;            // ????
const Real    ItrfCoefficientsFile::MULT_2000_PLANET      = 1.0e-04;        // ????

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  ItrfCoefficientsFile(const std::string &fileName 
//                       Integer nutTerms  = REDEX96_MAX_NUT_TERMS,
//                       Integer planTerms = MAX_PLANET_TERMS);
//---------------------------------------------------------------------------
/**
 * Constructs base ItrfCoefficientsFile structures used in derived classes
 * (default constructor).
 *
 * @param fileNme  EOP file name.
 */
//---------------------------------------------------------------------------
ItrfCoefficientsFile::ItrfCoefficientsFile(const std::string &nutFileName,
                      const std::string planFileName,
                      GmatItrf::NutationTerms  nutTerms,
                      GmatItrf::PlanetaryTerms planTerms) :
nutation              (nutTerms),
planetary             (planTerms),
nutationFileName      (nutFileName),
planetaryFileName     (planFileName),
filesAreInitialized   (false),
A                     (NULL),
B                     (NULL),
C                     (NULL),
D                     (NULL),
E                     (NULL),
F                     (NULL),
Ap                    (NULL),
Bp                    (NULL),
Cp                    (NULL),
Dp                    (NULL)
{
   InitializeArrays(nutTerms, planTerms);      
}

//---------------------------------------------------------------------------
//  ItrfCoefficientsFile(const ItrfCoefficientsFile &itrfF);
//---------------------------------------------------------------------------
/**
 * Constructs base ItrfCoefficientsFile structures, by copying 
 * the input instance (copy constructor).
 *
 * @param itrfF   ItrfCoefficientsFile instance to copy to create "this" 
 *                instance.
 */
//---------------------------------------------------------------------------
ItrfCoefficientsFile::ItrfCoefficientsFile(const ItrfCoefficientsFile &itrfF) :
nut                   (itrfF.nut),
nutpl                 (itrfF.nutpl),
nutation              (itrfF.nutation),
planetary             (itrfF.planetary),
nutationFileName      (itrfF.nutationFileName),
planetaryFileName     (itrfF.planetaryFileName),
filesAreInitialized   (itrfF.filesAreInitialized),
A                     (NULL),
B                     (NULL),
C                     (NULL),
D                     (NULL),
E                     (NULL),
F                     (NULL),
Ap                    (NULL),
Bp                    (NULL),
Cp                    (NULL),
Dp                    (NULL)
{
   InitializeArrays(nutation, planetary);
   Integer i, j;
   for (i = 0; i < nut; i++)
      for (j = 0; j < 5; j++)  
         (a.at(j)).at(i) = ((itrfF.a).at(j)).at(i);
   (*A) = *(itrfF.A);
   (*B) = *(itrfF.B);
   (*C) = *(itrfF.C);
   (*D) = *(itrfF.D);
   (*E) = *(itrfF.E);
   (*F) = *(itrfF.F);
   for (i = 0; i < nutpl; i++)
      for (j = 0; j < 10; j++)  
         (ap.at(j)).at(i) = ((itrfF.ap).at(j)).at(i);
   (*Ap) = *(itrfF.Ap);
   (*Bp) = *(itrfF.Bp);
   (*Cp) = *(itrfF.Cp);
   (*Dp) = *(itrfF.Dp);
}

//---------------------------------------------------------------------------
//  ItrfCoefficientsFile& operator=(const ItrfCoefficientsFile &itrfF)
//---------------------------------------------------------------------------
/**
 * Assignment operator for ItrfCoefficientsFile structures.
 *
 * @param eopF  The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const ItrfCoefficientsFile& ItrfCoefficientsFile::operator=(
                            const ItrfCoefficientsFile &itrfF)
{
   if (&itrfF == this)
      return *this;
   nut                 = itrfF.nut;
   nutpl               = itrfF.nutpl;
   nutation            = itrfF.nutation;
   planetary           = itrfF.planetary;
   nutationFileName    = itrfF.nutationFileName;
   planetaryFileName   = itrfF.planetaryFileName;
   filesAreInitialized = itrfF.filesAreInitialized;  
   InitializeArrays(nutation, planetary);
   Integer i, j;
   for (i = 0; i < nut; i++)
      for (j = 0; j < 5; j++)  
         (a.at(j)).at(i) = ((itrfF.a).at(j)).at(i);
   (*A) = *(itrfF.A);
   (*B) = *(itrfF.B);
   (*C) = *(itrfF.C);
   (*D) = *(itrfF.D);
   (*E) = *(itrfF.E);
   (*F) = *(itrfF.F);
   for (i = 0; i < nutpl; i++)
      for (j = 0; j < 10; j++)  
         (ap.at(j)).at(i) = ((itrfF.ap).at(j)).at(i);
   (*Ap) = *(itrfF.Ap);
   (*Bp) = *(itrfF.Bp);
   (*Cp) = *(itrfF.Cp);
   (*Dp) = *(itrfF.Dp);
   return *this;
}
//---------------------------------------------------------------------------
//  ~ItrfCoefficientsFile()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
ItrfCoefficientsFile::~ItrfCoefficientsFile()
{
   delete A;
   delete B;
   delete C;
   delete D;
   delete E;
   delete F;
   delete Ap;
   delete Bp;
   delete Cp;
   delete Dp;
   
   // bye bye
}


//------------------------------------------------------------------------------
//  void  Initialize()
//------------------------------------------------------------------------------
/**
 * This method initializes the ItrfCoefficientsFile class, by reading the file and 
 * storing the polar motion data.
 *
 */
//------------------------------------------------------------------------------
void ItrfCoefficientsFile::Initialize()
{
   if (filesAreInitialized)  return;
   
   //Integer a1, a2, a3, a4, a5;
   //Integer ap1, ap2, ap3, ap4, ap5, ap6, ap7, ap8, ap9, ap10;
   std::string   line;
   // read the nutation file and put the coefficient data into the arrays
   std::ifstream itrfNutFile(nutationFileName.c_str());
   if (!itrfNutFile)
      throw UtilityException("Error opening ItrfCoefficientsFile (nutation) " + 
                             nutationFileName);
   itrfNutFile.setf(std::ios::skipws);
   // read until the requested data set is found
   bool startNow = false;
   while ((!startNow) && (!itrfNutFile.eof()))
   {
      getline(itrfNutFile,line);
      if (line.find(firstNutPhrase) != std::string::npos)   startNow = true;
   }
   if (startNow == false)
      throw UtilityException("Unable to read nutation ItrfCoefficientsFile.");
   // skip line with column headings
   getline(itrfNutFile, line);
   if (line.find("a2") == std::string::npos)
      throw UtilityException("Itrf nutation file not in expected format.");
   Integer i;
   for (i = 0; i < nut; i++)
   {
      if (itrfNutFile.eof())
          throw UtilityException(
               "Itrf nutation file does not contain all expected values.");
      getline(itrfNutFile,line);
      std::istringstream lineStr;
      lineStr.str(line);
      lineStr >> (a.at(0)).at(i) >> (a.at(1)).at(i) >> (a.at(2)).at(i) 
              >> (a.at(3)).at(i) >> (a.at(4)).at(i);
      lineStr >> (*A)(i) >> (*B)(i) >> (*C)(i) >> (*D)(i) 
              >> (*E)(i) >> (*F)(i);
   }
   (*A) *= nutMult;
   (*B) *= nutMult;
   (*C) *= nutMult;
   (*D) *= nutMult;
   (*E) *= nutMult;
   (*F) *= nutMult;
   
   // read the planetary file and put the coefficient data into the arrays
   std::ifstream itrfPlanFile(planetaryFileName.c_str());
   if (!itrfPlanFile)
      throw UtilityException("Error opening ItrfCoefficientsFile (planetary) " + 
                             planetaryFileName);
   itrfPlanFile.setf(std::ios::skipws);
   // read until the requested data set is found
   startNow = false;
   while ((!startNow) && (!itrfPlanFile.eof()))
   {
      getline(itrfPlanFile,line);
      if (line.find(firstPlanPhrase) != std::string::npos)   startNow = true;
   }
   if (startNow == false)
      throw UtilityException("Unable to read planetary ItrfCoefficientsFile.");
   // skip line with column headings
   getline(itrfPlanFile, line);
   if (line.find("a2") == std::string::npos)
      throw UtilityException("Itrf planetary file not in expected format.");
   for (i = 0; i < nutpl; i++)
   {
      if (itrfPlanFile.eof())
         throw UtilityException(
               "Itrf planetary file does not contain all expected values.");
      getline(itrfPlanFile,line);
      std::istringstream lineStr;
      lineStr.str(line);
      lineStr >> (ap.at(0)).at(i) >> (ap.at(1)).at(i) >> (ap.at(2)).at(i) 
              >> (ap.at(3)).at(i) >> (ap.at(4)).at(i) >> (ap.at(5)).at(i)
              >> (ap.at(6)).at(i) >> (ap.at(7)).at(i) >> (ap.at(8)).at(i) 
              >> (ap.at(9)).at(i);
      lineStr >> (*Ap)(i) >> (*Bp)(i) >> (*Cp)(i) >> (*Dp)(i);
   }
   (*Ap) *= planMult;
   (*Bp) *= planMult;
   (*Cp) *= planMult;
   (*Dp) *= planMult;
   
   if (itrfNutFile.is_open())  itrfNutFile.close();
   if (itrfPlanFile.is_open()) itrfPlanFile.close();
   filesAreInitialized = true;
}


std::string ItrfCoefficientsFile::GetNutationFileName() const
{
   return nutationFileName;
}

std::string ItrfCoefficientsFile::GetPlanetaryFileName() const
{
   return planetaryFileName;
}


Integer ItrfCoefficientsFile::GetNumberOfNutationTerms()
{
   return nut;
}

Integer ItrfCoefficientsFile::GetNumberOfPlanetaryTerms()
{
   return nutpl;
}

//------------------------------------------------------------------------------
//  bool  GetNutationTerms(std::vector<IntegerArray> &a5, Rvector &Aval, 
//                         Rvector &Bval, Rvector &Cval,
//                         Rvector &Dval, Rvector &Eval,Rvector &Fval)
//------------------------------------------------------------------------------
/**
 * This method returns the nutation terms read from the file.
 *
 * @param a5   nutation coefficients (???)
 * @param Aval A coefficients (longitude)
 * @param Bval B coefficients (longitude)
 * @param Cval C coefficients (obliquity)
 * @param Dval D coefficients (obliquity)
 * @param Eval E coefficients (longitude)
 * @param Fval F coefficients (obliquity)
 *
 * @return success of the method
 */
//------------------------------------------------------------------------------
bool ItrfCoefficientsFile::GetNutationTerms(
                           std::vector<IntegerArray> &a5, Rvector &Aval, 
                           Rvector &Bval, Rvector &Cval,
                           Rvector &Dval, Rvector &Eval,Rvector &Fval)
{
   if (!filesAreInitialized) Initialize();
   a5   = a;
   Aval = *A;
   Bval = *B;
   Cval = *C;
   Dval = *D;
   Eval = *E;
   Fval = *F;
   return true;
}

//------------------------------------------------------------------------------
//  bool  GetPlanetaryTerms(std::vector<IntegerArray> &ap10, Rvector &Apval, 
//                          Rvector &Bpval, Rvector &Cpval,
//                          Rvector &Dpval)
//------------------------------------------------------------------------------
/**
 * This method returns the nutation terms read from the file.
 *
 * @param ap10  planetary coefficients (???)
 * @param Apval Ap coefficients (longitude)
 * @param Bpval Bp coefficients (obliquity)
 * @param Cpval Cp coefficients (longitude)
 * @param Dpval Dp coefficients (obliquity)
 *
 * @return success of the method
 */
//------------------------------------------------------------------------------
bool ItrfCoefficientsFile::GetPlanetaryTerms(
                           std::vector<IntegerArray> &ap10, Rvector &Apval, 
                           Rvector &Bpval, Rvector &Cpval, 
                           Rvector &Dpval)
{
   if (!filesAreInitialized) Initialize();
   ap10  = ap;
   Apval = *Ap;
   Bpval = *Bp;
   Cpval = *Cp;
   Dpval = *Dp;
   return true;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  bool InitializeArrays(GmatItrf::NutationTerms nutT,
//                        GmatItrf::PlanetaryTerms planT)
//------------------------------------------------------------------------------
/**
 * This method initilaizes the internal arrays that store the nutation and
 * planetary data.
 *
 * @param  nutT  nutation data type
 * @param  planT planetary data type
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool ItrfCoefficientsFile::InitializeArrays(GmatItrf::NutationTerms nutT,
                                            GmatItrf::PlanetaryTerms planT)
{
   if (nutT == GmatItrf::NUTATION_1996)  
   {
      nut            = MAX_1996_NUT_TERMS;
      nutMult        = MULT_1996_NUT;
      firstNutPhrase = FIRST_NUT_PHRASE_1996;
   }
   else
   {
      nut            = MAX_2000_NUT_TERMS;
      nutMult        = MULT_2000_NUT;
      firstNutPhrase = FIRST_NUT_PHRASE_2000;
   }
   
   if (planT == GmatItrf::PLANETARY_1996)  
   {
      nutpl           = MAX_1996_PLANET_TERMS;
      planMult        = MULT_1996_PLANET;
      firstPlanPhrase = FIRST_PLAN_PHRASE_1996;
   }
   else  // this is currently not possible
   {
      nutpl           = MAX_2000_PLANET_TERMS;
      planMult        = MULT_2000_PLANET;
      firstPlanPhrase = FIRST_PLAN_PHRASE_2000;
   }
      
   if (a.size() != 5)     a.resize(5);
   if (ap.size() != 10)   ap.resize(10);
   Integer i = 0;
   for (i = 0; i < 5 ; i++)
   { 
      (a.at(i)).clear();
      (a.at(i)).resize(nut);
      (ap.at(i)).clear();
      (ap.at(i+5)).clear();
      (ap.at(i)).resize(nutpl);
      (ap.at(i+5)).resize(nutpl);
   }
   
   if (A && (A->GetSize() != nut))      delete A;
   if (!A) A = new Rvector(nut);
   if (B && (B->GetSize() != nut))      delete B;
   if (!B) B = new Rvector(nut);
   if (C && (C->GetSize() != nut))      delete C;
   if (!C) C = new Rvector(nut);
   if (D && (D->GetSize() != nut))      delete D;
   if (!D) D = new Rvector(nut);
   if (E && (E->GetSize() != nut))      delete E;
   if (!E) E = new Rvector(nut);
   if (F && (F->GetSize() != nut))      delete F;
   if (!F) F = new Rvector(nut);
   
   if (Ap && (Ap->GetSize() != nutpl))  delete Ap;
   if (!Ap) Ap = new Rvector(nutpl);
   if (Bp && (Bp->GetSize() != nutpl))  delete Bp;
   if (!Bp) Bp = new Rvector(nutpl);
   if (Cp && (Cp->GetSize() != nutpl))  delete Cp;
   if (!Cp) Cp = new Rvector(nutpl);
   if (Dp && (Dp->GetSize() != nutpl))  delete Dp;
   if (!Dp) Dp = new Rvector(nutpl);

   return true;
}

//------------------------------------------------------------------------------
//  bool IsBlank(char* aLine)
//------------------------------------------------------------------------------
/**
 * This method returns true if the string is empty or is all white space.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool ItrfCoefficientsFile::IsBlank(const char* aLine)
{
   Integer i;
   for (i=0;i<(int)strlen(aLine);i++)
   {
      //loj: 5/18/04 if (!isblank(aLine[i])) return false;
      if (!isspace(aLine[i])) return false;
   }
   return true;
}


