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

//#include <sstream>
//#include <fstream>
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
   //loj: Do this later.
   //OMANIP(int) SetSpacing(int i);
   //OMANIP(int) SetHorizontal(bool h);
   //IMANIP(int) SetBinaryIn(bool b);
   //OMANIP(int) SetBinaryOut(bool b);
   bool        IsHorizontal();
   bool        IsBinaryIn();
   bool        IsBinaryOut();
   int         GetSpacing();

   // Math Utilities
   Real Min(const Rvector &numbers);
   Real Max(const Rvector &numbers);

   //  I/O stream operations
   std::istream& operator>> (std::istream &input, Rvector &a);
   std::ostream& operator<< (std::ostream &output, const Rvector &a);

   std::istream& operator>> (std::istream &input, Rmatrix &a);
   std::ostream& operator<< (std::ostream &output, const Rmatrix &a);
}
#endif // Linear_hpp

