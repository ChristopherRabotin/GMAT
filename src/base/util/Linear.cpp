//$Header$
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
#include "RealUtilities.hpp" // for PI, TWO_PI, Acos(), Atan()
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "Rmatrix.hpp"
#include "Linear.hpp"

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
// void GmatRealUtil::SetWidth(int w)
//------------------------------------------------------------------------------
void GmatRealUtil::SetWidth(int w)
{
   format.mWidth = w;
}

//------------------------------------------------------------------------------
// void GmatRealUtil::SetPrecision(int p)
//------------------------------------------------------------------------------
void GmatRealUtil::SetPrecision(int p)
{
   format.mPrecision = p;
}

//------------------------------------------------------------------------------
// void GmatRealUtil::SetSpacing(int s)
//------------------------------------------------------------------------------
void GmatRealUtil::SetSpacing(int s)
{
   format.mSpacing = s;
}

//------------------------------------------------------------------------------
// void GmatRealUtil::SetHorizontal(bool h)
//------------------------------------------------------------------------------
void GmatRealUtil::SetHorizontal(bool h)
{
   format.mHorizontal = h;
}

//------------------------------------------------------------------------------
// void GmatRealUtil::SetBinaryIn(bool b)
//------------------------------------------------------------------------------
void GmatRealUtil::SetBinaryIn(bool b)
{
   format.mBinaryIn = b;
}

//------------------------------------------------------------------------------
// void GmatRealUtil::SetBinaryOut(bool b)
//------------------------------------------------------------------------------
void GmatRealUtil::SetBinaryOut(bool b)
{
   format.mBinaryOut = b;
}

//------------------------------------------------------------------------------
//  int GmatRealUtil::GetWidth() 
//------------------------------------------------------------------------------
int GmatRealUtil::GetWidth()
{
   return format.mWidth;
}

//------------------------------------------------------------------------------
//  int GmatRealUtil::GetPrecision() 
//------------------------------------------------------------------------------
int GmatRealUtil::GetPrecision()
{
   return format.mPrecision;
}

//------------------------------------------------------------------------------
//  int GmatRealUtil::GetSpacing() 
//------------------------------------------------------------------------------
int GmatRealUtil::GetSpacing()
{
   return format.mSpacing;
}

//------------------------------------------------------------------------------
// bool GmatRealUtil::IsHorizontal() 
//------------------------------------------------------------------------------
bool GmatRealUtil::IsHorizontal() 
{
   return format.mHorizontal;
}

//------------------------------------------------------------------------------
//  bool GmatRealUtil::IsBinaryIn() 
//------------------------------------------------------------------------------
bool GmatRealUtil::IsBinaryIn() 
{
   return format.mBinaryIn;
}

//------------------------------------------------------------------------------
//  bool GmatRealUtil::IsBinaryOut() 
//------------------------------------------------------------------------------
bool GmatRealUtil::IsBinaryOut() 
{
   return format.mBinaryOut;
}

//------------------------------------------------------------------------------
//  std::istream& GmatRealUtil::operator>> (std::istream &input, Rvector &a) 
//------------------------------------------------------------------------------
std::istream& GmatRealUtil::operator>> (std::istream &input, Rvector &a) 
{
   int size = a.GetSize();
   int i;
   
   if (format.mBinaryIn)
   {
      for (i=0; i<size; i++)  
         input.read((char*)&a[i], sizeof(Real));
   }
   else
   {
      for (i=0; i<size; i++)  
         input>>a[i];
   }
   
   format.mBinaryIn = DEFAULT_FORMAT.mBinaryIn;
   return input;
}

//------------------------------------------------------------------------------
//  std::ostream& GmatRealUtil::operator<< (std::ostream &output, const Rvector &a)
//------------------------------------------------------------------------------
/**
 * @note Resets format to default.
 */
//------------------------------------------------------------------------------
std::ostream& GmatRealUtil::operator<< (std::ostream &output, const Rvector &a) 
{
   int size = a.GetSize();
   int i;
   
   if (format.mBinaryOut)
   {
      for (i=0; i<size; i++)  
      {
         output.write((char*)&a[i], sizeof(Real));
      }
   }
   else
   {      
      if (format.mHorizontal) 
      {
         char *spaces = new char[format.mSpacing + 1];
         for (i=0; i<format.mSpacing; i++) 
         {
            spaces[i] = ' ';
         }
         spaces[format.mSpacing] = '\0';
         for (i=0; i<size; i++) 
         {
            output.width(format.mWidth);
            output.precision(format.mPrecision);
            output << a[i] << spaces;
         }
         output << std::endl;
      } 
      else 
      {
         for (i=0; i<size; i++) 
         {
            output.width(format.mWidth);
            output.precision(format.mPrecision);
            output << a[i] << std::endl;
         }
      }
   }

   format.mWidth = DEFAULT_FORMAT.mWidth;
   format.mPrecision = DEFAULT_FORMAT.mPrecision;
   format.mSpacing = DEFAULT_FORMAT.mSpacing;
   format.mHorizontal = DEFAULT_FORMAT.mHorizontal;
   format.mBinaryOut = DEFAULT_FORMAT.mBinaryOut;
   return output;
}

//------------------------------------------------------------------------------
//  std::istream& GmatRealUtil::operator>> (std::istream &input, Rmatrix &a) 
//------------------------------------------------------------------------------
std::istream& GmatRealUtil::operator>> (std::istream &input, Rmatrix &a) 
{
   int row = a.GetNumRows();
   int column = a.GetNumColumns();
   int i,j;

   if (format.mBinaryIn)
   {
      for (i=0; i<row; i++) 
         for (j=0; j<column; j++) 
            input.read((char*)&a(i,j), sizeof(Real));
   }
   else
   {
      for (i=0; i<row; i++) 
         for (j=0; j<column; j++) 
            input >> a(i,j);
   }
   
   format.mBinaryIn = DEFAULT_FORMAT.mBinaryIn;
   return input;
}

//------------------------------------------------------------------------------
//  std::ostream& GmatRealUtil::operator<< (std::ostream &output, const Rmatrix &)
//------------------------------------------------------------------------------
/**
 * @note Resets format to default.
 */
//------------------------------------------------------------------------------
std::ostream& GmatRealUtil::operator<< (std::ostream &output, const Rmatrix &a) 
{
   int row = a.GetNumRows();
   int column = a.GetNumColumns();
   int i,j;
   char *spaces = new char[format.mSpacing + 1];

   if (format.mBinaryOut)
   {
      for (i=0; i<row; i++) 
      {
         for (j=0; j<column; j++) 
            output.write((char*)&a(i,j), sizeof(Real));
      }
   }
   else
   {
      for(i=0; i<format.mSpacing; i++) 
      {
         spaces[i] = ' ';
      }
      
      spaces[format.mSpacing] = '\0';
      
      if (format.mHorizontal) 
      {
         for (i=0; i<row; i++) 
         {
            for (j=0; j<column; j++) 
            {
               output.width(format.mWidth);
               output.precision(format.mPrecision);
               output  << a.GetElement(i,j) << spaces;
            }
         }
      }   
      else 
      {
         for (i=0; i<row; i++) 
         {
            for (j=0; j<column; j++) 
            {
               output.width(format.mWidth);
               output.precision(format.mPrecision);
               output << a.GetElement(i,j) << spaces;
            }
            output << std::endl;
         }
      }
   }
   
   format.mWidth = DEFAULT_FORMAT.mWidth;
   format.mPrecision = DEFAULT_FORMAT.mPrecision;
   format.mSpacing = DEFAULT_FORMAT.mSpacing;
   format.mHorizontal = DEFAULT_FORMAT.mHorizontal;
   format.mBinaryOut = DEFAULT_FORMAT.mBinaryOut;
   return output;
}

//loj: 9/21/04 added width and precision
//------------------------------------------------------------------------------
// std::string ToString(const Real &val, Integer width=10, Integer precision=9)
//------------------------------------------------------------------------------
std::string GmatRealUtil::ToString(const Real &val, Integer width,
                                   Integer precision)
{
   std::stringstream ss("");
   ss.width(width);
   ss.precision(precision);
   ss << val;
   return std::string(ss.str());
}

//------------------------------------------------------------------------------
// std::string ToString(const Integer &val)
//------------------------------------------------------------------------------
std::string GmatRealUtil::ToString(const Integer &val)
{
   std::stringstream ss("");
   ss << val;
   return std::string(ss.str());
}
