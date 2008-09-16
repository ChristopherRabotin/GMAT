//$Id$
//------------------------------------------------------------------------------
//                                  Linear
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Matthew Weippert, Chu-chi Li, E. Corderman
// Created: 1995/07/21 for GSS project
// Modified: 2003/09/16 Linda Jun - See Linear.hpp
//
/**
 * Defines Linear Algebra conversion, Linear I/O, and Linear Math operations.
 */
//------------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "RealTypes.hpp"
#include "RealUtilities.hpp"     // for PI, TWO_PI, Acos(), Atan()
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "Rmatrix.hpp"
#include "Linear.hpp"
#include "GmatGlobal.hpp"        // for Global settings
#include "MessageInterface.hpp"

using namespace GmatRealUtil;
using namespace GmatMathUtil;

//---------------------------------
//  pubic
//---------------------------------

//------------------------------------------------------------------------------
//  GmatRealUtil::RaCodec GmatRealUtil::CartesianToRaCodec(const Rvector3 &r)
//------------------------------------------------------------------------------
/**
 * @exception throws RealUtilitiesExceptions::ArgumentError if all three
 *            Cartesian coordinates to be converted are 0
 */
//------------------------------------------------------------------------------
GmatRealUtil::RaCodec GmatRealUtil::CartesianToRaCodec(const Rvector3 &r)
{
   RaCodec s;
   if ( (r[0] == 0.0) && (r[1] == 0.0) ) 
   {
      if (r[2] == 0.0) 
      {
         throw RealUtilitiesExceptions::ArgumentError();
      } 
      else if( r[2]<0.0 ) 
      {
         s.radiusD = -r[2];
         s.coDeclinationD = GmatMathUtil::PI;
         s.rightAscensionD = 0.0;
      } 
      else if( r[2]>0.0) 
      {
         s.radiusD = r[2];
         s.coDeclinationD = 0.0;
         s.rightAscensionD =0.0;
      }
   } 
   else
   {
      s.radiusD = r.GetMagnitude();
      s.coDeclinationD = ACos(r[2] / s.radiusD);
      s.rightAscensionD = ATan(r[1],r[0]);
   }
   return s;
}


//------------------------------------------------------------------------------
//  GmatRealUtil::RaDec GmatRealUtil::CartesianToRaDec(const Rvector3 &r)
//------------------------------------------------------------------------------
/**
 * @exception throws RealUtilitiesExceptions::ArgumentError if all three
 *            Cartesian coordinates to be converted are 0
 */
//------------------------------------------------------------------------------
GmatRealUtil::RaDec GmatRealUtil::CartesianToRaDec(const Rvector3 &r)
{
   RaDec RD;

   if ( r[0] == 0.0 && r[1] == 0.0) 
   {
      if(r[2] == 0.0) 
      {
         throw RealUtilitiesExceptions::ArgumentError();
      } 
      else if(r[2]<0.0) 
      {
         RD.radiusD = -r[2];
         RD.rightAscensionD = 0.0;
         RD.declinationD = -GmatMathUtil::PI_OVER_TWO;
      } 
      else if(r[2]>0.0) 
      {
         RD.radiusD = r[2];
         RD.rightAscensionD = 0.0;
         RD.declinationD  = GmatMathUtil::PI_OVER_TWO;
      }
   } 
   else 
   {
      RD.radiusD = r.GetMagnitude();
      RD.rightAscensionD = ATan(r[1],r[0]);
      RD.declinationD = ASin(r[2]/RD.radiusD);
   }
   return RD;
}


//------------------------------------------------------------------------------
//  Rvector3 GmatRealUtil::RaCodecToCartesian(const RaCodec &r)
//------------------------------------------------------------------------------
Rvector3 GmatRealUtil::RaCodecToCartesian(const RaCodec &r)
{
   Rvector3 v;
   v[0] = r.radiusD * Sin(r.coDeclinationD) * Cos(r.rightAscensionD);
   v[1] = r.radiusD * Sin(r.coDeclinationD) * Sin(r.rightAscensionD);
   v[2] = r.radiusD * Cos(r.coDeclinationD);
   return v;
}


//------------------------------------------------------------------------------
//  GmatRealUtil::RaDec GmatRealUtil::RaCodecToRaDec(const RaCodec &r)
//------------------------------------------------------------------------------
GmatRealUtil::RaDec GmatRealUtil::RaCodecToRaDec(const RaCodec &r)
{
   RaDec rD;
   rD.radiusD = r.radiusD;
   rD.rightAscensionD = r.rightAscensionD;
   rD.declinationD = GmatMathUtil::PI_OVER_TWO - r.coDeclinationD;
   return rD;
}


//------------------------------------------------------------------------------
//  Rvector3 GmatRealUtil::RaDecToCartesian(const RaDec &r)
//------------------------------------------------------------------------------
Rvector3 GmatRealUtil::RaDecToCartesian(const RaDec &r)
{
   Rvector3 v;
   v[0] = r.radiusD * Cos(r.rightAscensionD) * Cos(r.declinationD);
   v[1] = r.radiusD * Sin(r.rightAscensionD) * Cos(r.declinationD);
   v[2] = r.radiusD * Sin(r.declinationD);
   return v;
}


//------------------------------------------------------------------------------
//  GmatRealUtil::RaCodec GmatRealUtil::RaDecToRaCodec(const RaDec &r)
//------------------------------------------------------------------------------
GmatRealUtil::RaCodec GmatRealUtil::RaDecToRaCodec(const RaDec &r)
{
   RaCodec s;
   s.radiusD = r.radiusD;
   s.rightAscensionD = r.rightAscensionD;
   s.coDeclinationD = GmatMathUtil::PI_OVER_TWO - r.declinationD;
   return s;
}


//------------------------------------------------------------------------------
//  Real GmatRealUtil::Min(const Rvector &numbers)
//------------------------------------------------------------------------------
Real GmatRealUtil::Min(const Rvector &numbers)
{
   int i;
   int end = numbers.GetSize();
   Real smallest = numbers[0];
    
   for ( i = 1; i<end;i++) 
   {
      if (numbers[i]<smallest) 
         smallest = numbers[i];
   }
   
   return smallest;
}


//------------------------------------------------------------------------------
//  Real GmatRealUtil::Max(const Rvector &numbers)
//------------------------------------------------------------------------------
Real GmatRealUtil::Max(const Rvector &numbers)
{
   int i;
   int end = numbers.GetSize();
   Real biggest = numbers[0];

   for ( i = 1; i<end; i++) 
   {
      if (numbers[i]>biggest) 
         biggest = numbers[i];
   }
   
   return biggest;
}


//------------------------------------------------------------------------------
//  std::istream& GmatRealUtil::operator>> (std::istream &input, Rvector &a) 
//------------------------------------------------------------------------------
std::istream& GmatRealUtil::operator>> (std::istream &input, Rvector &a) 
{
   GmatGlobal *global = GmatGlobal::Instance();
   int size = a.GetSize();
   
   if (global->IsBinaryIn())
   {
      for (int i=0; i<size; i++)  
         input.read((char*)&a[i], sizeof(Real));
   }
   else
   {
      for (int i=0; i<size; i++)  
         input>>a[i];
   }
   
   global->SetBinaryIn(false);
   
   return input;
}


//------------------------------------------------------------------------------
// std::ostream& GmatRealUtil::operator<< (std::ostream &output, const Rvector &a)
//------------------------------------------------------------------------------
/**
 * Formats Rvector value using global format and sends to output stream.
 * Once global format is set, it remains the same format until it is reset by
 * global->SetActualFormat().
 *
 * @param  output  Output stream
 * @param  a       Rvector to write out
 *
 * return  Output stream
 */
//------------------------------------------------------------------------------
std::ostream& GmatRealUtil::operator<< (std::ostream &output, const Rvector &a) 
{
   using namespace std;
   
   GmatGlobal *global = GmatGlobal::Instance();
   Integer size = a.GetSize();
   Integer p, w, spacing;
   bool scientific, showPoint, horizontal, appendEol;
   std::string prefix;
   global->GetActualFormat(scientific, showPoint, p, w, horizontal, spacing, prefix,
                           appendEol);
   
   if (showPoint)
      output.setf(std::ios::showpoint);
   
   if (scientific)
      output.setf(std::ios::scientific);
   
   if (global->IsBinaryOut())
   {
      for (int i=0; i<size; i++)  
         output.write((char*)&a[i], sizeof(Real));
   }
   else
   {      
      if (horizontal)
      {
         std::string spaces;
         spaces.append(spacing, ' ');
         
         for (int i=0; i<size; i++)
         {
            output << setw(w) << setprecision(p) << a[i];
            if (i < size-1)
               output << spaces;
         }
         
         if (appendEol)
            output << std::endl;
      } 
      else 
      {
         for (int i=0; i<size; i++)
         {
            output << setw(w) << setprecision(p) << prefix << a[i];
            if (i < size-1)
               output << std::endl;
         }
      }
   }
   
   // Reset to current format
   #ifdef __RESET_TO_CURRENT_FORMAT__
   global->SetToCurrentFormat();
   #endif
   
   return output;
}


//------------------------------------------------------------------------------
//  std::istream& GmatRealUtil::operator>> (std::istream &input, Rmatrix &a) 
//------------------------------------------------------------------------------
std::istream& GmatRealUtil::operator>> (std::istream &input, Rmatrix &a) 
{
   GmatGlobal *global = GmatGlobal::Instance();
   int row = a.GetNumRows();
   int column = a.GetNumColumns();
   
   if (global->IsBinaryIn())
   {
      for (int i=0; i<row; i++) 
         for (int j=0; j<column; j++) 
            input.read((char*)&a(i,j), sizeof(Real));
   }
   else
   {
      for (int i=0; i<row; i++) 
         for (int j=0; j<column; j++) 
            input >> a(i,j);
   }
   
   global->SetBinaryIn(false);
   
   return input;
}


//------------------------------------------------------------------------------
//  std::ostream& GmatRealUtil::operator<< (std::ostream &output, const Rmatrix &)
//------------------------------------------------------------------------------
/**
 * Formats Rmatrix value using global format and sends to output stream.
 * Once global format is set, it remains the same format until it is reset by
 * global->SetActualFormat().
 *
 * @param  output  Output stream
 * @param  a       Rmatrix to write out
 *
 * return  Output stream
 */
//------------------------------------------------------------------------------
std::ostream& GmatRealUtil::operator<< (std::ostream &output, const Rmatrix &a) 
{
   using namespace std;
   
   GmatGlobal *global = GmatGlobal::Instance();
   int row = a.GetNumRows();
   int column = a.GetNumColumns();
   Integer p, w, spacing;
   bool scientific, showPoint, horizontal, appendEol;
   std::string prefix;
   global->GetActualFormat(scientific, showPoint, p, w, horizontal, spacing, prefix,
                           appendEol);
   
   if (showPoint)
      output.setf(std::ios::showpoint);
   
   if (scientific)
      output.setf(std::ios::scientific);
   
   if (global->IsBinaryOut())
   {
      for (int i=0; i<row; i++) 
      {
         for (int j=0; j<column; j++) 
            output.write((char*)&a(i,j), sizeof(Real));
      }
   }
   else
   {      
      std::string spaces;
      spaces.append(spacing, ' ');
      
      if (horizontal) 
      {
         for (int i=0; i<row; i++) 
            for (int j=0; j<column; j++) 
               output << setw(w) << setprecision(p) << a.GetElement(i,j) << spaces;
         
         if (appendEol)
            output << std::endl;
      }
      else 
      {
         for (int i=0; i<row; i++)
         {
            output << prefix;
            
            for (int j=0; j<column; j++)
               output << setw(w) << setprecision(p) << a.GetElement(i,j) << spaces;
            
            output << std::endl;
         }
      }
   }
   
   // Reset to current format
   #ifdef __RESET_TO_CURRENT_FORMAT__
   global->SetToCurrentFormat();
   #endif
   
   return output;
}


//------------------------------------------------------------------------------
// std::string ToString(const Real &rval, bool useCurrentFormat = true,
//                      bool scientific = false, bool showPoint = false,
//                      Integer precision = GmatGlobal::DATA_PRECISION,
//                      Integer width = GmatGlobal::INTEGER_WIDTH)
//------------------------------------------------------------------------------
/*
 * Formats Real value to String.
 *
 * @param  rval  Real value
 * @param  useCurrentFormat  Uses precision and width from GmatGlobal
 * @param  scientific  if true, formats using scientific notation
 * @param  showPoint if true, shows decimal point and trailing zeros
 * @param  precision  Precision to be used in formatting
 * @param  width  Width to be used in formatting
 */
//------------------------------------------------------------------------------
std::string GmatRealUtil::ToString(const Real &rval, bool useCurrentFormat,
                                   bool scientific, bool showPoint,
                                   Integer precision, Integer width)
{
   Integer p = precision;
   Integer w = width;
   bool isScientific = scientific;
   bool isShowPointSet = showPoint;
   
   if (useCurrentFormat)
   {
      GmatGlobal *global = GmatGlobal::Instance();
      p = global->GetDataPrecision();
      w = global->GetDataWidth();
      isScientific = global->IsScientific();
      isShowPointSet = global->ShowPoint();
   }
   
   std::stringstream ss("");
   ss.width(w);
   ss.precision(p);
   
   if (isShowPointSet)
      ss.setf(std::ios::showpoint);
   
   if (isScientific)
      ss.setf(std::ios::scientific);
   
   ss << rval;
   return ss.str();
}


//------------------------------------------------------------------------------
// std::string ToString(const Integer &ival, bool useCurrentFormat = true,
//                      Integer width = GmatGlobal::INTEGER_WIDTH)
//------------------------------------------------------------------------------
/*
 * Formats Integer value to String.
 *
 * @param  ival  Integer value
 * @param  useCurrentFormat  Uses width from GmatGlobal if true
 * @param  width  Width to be used in formatting
 */
//------------------------------------------------------------------------------
std::string GmatRealUtil::ToString(const Integer &ival, bool useCurrentFormat,
                                   Integer width)
{
   Integer w = width;
   
   if (useCurrentFormat)
      w = GmatGlobal::Instance()->GetIntegerWidth();
   
   std::stringstream ss("");
   ss.width(w);
   ss << ival;
   return ss.str();
}
