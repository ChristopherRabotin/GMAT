//$Header$
//------------------------------------------------------------------------------
//                                 Linear
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
// Modified: 2003/09/16 Linda Jun - Fixed deprecated OMANIP.
//
/**
 * Declares Linear Algebra conversion, Linear I/O, and Linear Math operations.
 */
//------------------------------------------------------------------------------
#ifndef Linear_hpp
#define Linear_hpp

#include <iostream>
#include <iomanip>
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "Rmatrix.hpp"
#include "Rmatrix33.hpp"

namespace GmatRealUtil
{
   
   // spherical coordinate types and operations 
   struct RaCodec
   {
      Real radiusD;
      Real rightAscensionD;
      Real coDeclinationD;
   };

   struct RaDec
   {
      Real radiusD;
      Real rightAscensionD;
      Real declinationD;
   };

   // Conversion Functions for Spherical coordinates
   RaCodec  CartesianToRaCodec(const Rvector3 &r);
   RaDec    CartesianToRaDec(const Rvector3 &r);
   Rvector3 RaCodecToCartesian(const RaCodec &r);
   RaDec    RaCodecToRaDec(const RaCodec &r);
   Rvector3 RaDecToCartesian(const RaDec &r);
   RaCodec  RaDecToRaCodec(const RaDec &r);

   // I/O formatting   
   struct IoFormat
   {
      IoFormat(int width = 10, int precision = 9, int spacing = 2,
               bool horizontal = false, bool binaryIn = false,
               bool binaryOut = false)
      {
         mWidth = width;
         mPrecision = precision;
         mSpacing = spacing;
         mHorizontal = horizontal;
         mBinaryIn = binaryIn;
         mBinaryOut = binaryOut;
      };
      
      int  mWidth;      /// the field width
      int  mPrecision;  /// the number of digits of precision
      int  mSpacing;    /// determines number of spaces in between each element
      bool mHorizontal; /// print horizontally if true. Default is false
      bool mBinaryIn;   /// read in binary if true. Default is false
      bool mBinaryOut;  /// print in binary if true. Default is false

   };

   static IoFormat format;
   const static IoFormat DEFAULT_FORMAT;

   void SetWidth(int w);
   void SetPrecision(int p);
   void SetSpacing(int s);
   void SetHorizontal(bool h);
   void SetBinaryIn(bool b);
   void SetBinaryOut(bool b);
   
   int  GetWidth();
   int  GetPrecision();
   int  GetSpacing();
   bool IsHorizontal();
   bool IsBinaryIn();
   bool IsBinaryOut();

   // Math Utilities
   Real Min(const Rvector &numbers);
   Real Max(const Rvector &numbers);

   //  I/O stream operations
   std::istream& operator>> (std::istream &input, Rvector &a);
   std::ostream& operator<< (std::ostream &output, const Rvector &a); 
   std::istream& operator>> (std::istream &input, Rmatrix &a);
   std::ostream& operator<< (std::ostream &output, const Rmatrix &a); 

   //loj: 9/21/04 added width and precision
   std::string ToString(const Real &val, Integer width=10, Integer precision=9);
   std::string ToString(const Integer &val);
}
#endif // Linear_hpp

