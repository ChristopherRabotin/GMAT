//$Header$
//------------------------------------------------------------------------------
//                                Rvector3
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2003/09/16
//
/**
 * Provides definitions for the Rvector3 class, providing linear algebra
 * operations for the 3-element Real vector.
 */
//------------------------------------------------------------------------------
#include <sstream>
#include "gmatdefs.hpp"
#include "Rvector3.hpp"
#include "Rmatrix33.hpp"
#include "Linear.hpp"
#include "MessageInterface.hpp"

//---------------------------------
//  static data
//---------------------------------
const std::string Rvector3::DATA_DESCRIPTIONS[NUM_DATA] =
{
   "Element 1",
   "Element 2",
   "Element 3"
};

//---------------------------------
//  public
//---------------------------------

//------------------------------------------------------------------------------
//  Rvector3()
//------------------------------------------------------------------------------
Rvector3::Rvector3()
   : Rvector(3)
{
}

//------------------------------------------------------------------------------
//  Rvector3(const Real e1, const Real e2, const Real e3)
//------------------------------------------------------------------------------
Rvector3::Rvector3(const Real e1, const Real e2, const Real e3)
   : Rvector(3, e1, e2, e3) 
{
}

//------------------------------------------------------------------------------
//  Rvector3(const Rvector3 &v)
//------------------------------------------------------------------------------
Rvector3::Rvector3(const Rvector3 &v)
   : Rvector(v)
{
}

//------------------------------------------------------------------------------
//  ~Rvector3()
//------------------------------------------------------------------------------
Rvector3::~Rvector3()
{
}

//------------------------------------------------------------------------------
// Real Get(const Integer index) const
//------------------------------------------------------------------------------
Real Rvector3::Get(const Integer index) const
{
   return elementD[index];
}

//------------------------------------------------------------------------------
// void Set(const Real e1, const Real e2, const Real e3)
//------------------------------------------------------------------------------
void Rvector3::Set(const Real e1, const Real e2, const Real e3)
{
   elementD[0] = e1;
   elementD[1] = e2;
   elementD[2] = e3;
}

//------------------------------------------------------------------------------
//  Real GetMagnitude() const
//------------------------------------------------------------------------------
Real Rvector3::GetMagnitude() const
{
    return GmatMathUtil::Sqrt(elementD[0]*elementD[0] +
                              elementD[1]*elementD[1] +
                              elementD[2]*elementD[2]);
}


//------------------------------------------------------------------------------
//  Rvector3 GetUnitVector() const
//------------------------------------------------------------------------------
Rvector3 Rvector3::GetUnitVector() const
{
   Real mag = GetMagnitude();

   if (GmatMathUtil::IsZero(mag))
      throw IsZeroVector(" from Rvector3::GetUnitVector()\n");
   
   return Rvector3(elementD[0]/mag, elementD[1]/mag, elementD[2]/mag);
}


//------------------------------------------------------------------------------
//  const Rvector3& Normalize()
//------------------------------------------------------------------------------
const Rvector3& Rvector3::Normalize()
{
   Real mag = GetMagnitude();
   
   if (GmatMathUtil::IsZero(mag))
      throw IsZeroVector(" from Rvector3::Normalize()\n");
   
   elementD[0] /= mag;
   elementD[1] /= mag;
   elementD[2] /= mag;
   return *this;
}

//------------------------------------------------------------------------------
//  Rvector3& operator=(const Rvector3 &v)
//------------------------------------------------------------------------------
Rvector3& Rvector3::operator=(const Rvector3 &v)
{
    elementD[0] = v.elementD[0];
    elementD[1] = v.elementD[1];
    elementD[2] = v.elementD[2];
    return *this;
}

//------------------------------------------------------------------------------
//  bool operator==(const Rvector3 &v) const
//------------------------------------------------------------------------------
bool Rvector3::operator==(const Rvector3 &v) const
{
   if (elementD[0] == v[0] &&
       elementD[1] == v[1] &&
       elementD[2] == v[2])
   {
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
//  bool operator!=(const Rvector3 &v) const
//------------------------------------------------------------------------------
bool Rvector3::operator!=(const Rvector3 &v) const
{
   return !(operator==(v));
}

//------------------------------------------------------------------------------
//  Rvector3 operator-() const                     // negation
//------------------------------------------------------------------------------
Rvector3 Rvector3::operator-() const
{
    return Rvector3(-elementD[0], -elementD[1], -elementD[2]);
}

//------------------------------------------------------------------------------
//  Rvector3 operator+(const Rvector3& v) const
//------------------------------------------------------------------------------
Rvector3 Rvector3::operator+(const Rvector3& v) const
{
    return Rvector3(elementD[0] + v.elementD[0],
                   elementD[1] + v.elementD[1],
                   elementD[2] + v.elementD[2]);
}

//------------------------------------------------------------------------------
//  const Rvector3&  operator+=(const Rvector3& v)
//------------------------------------------------------------------------------
const Rvector3& Rvector3::operator+=(const Rvector3& v)
{
    elementD[0] += v.elementD[0];
    elementD[1] += v.elementD[1];
    elementD[2] += v.elementD[2];
    return *this;
}

//------------------------------------------------------------------------------
//  const Rvector3 operator-(const Rvector3& v) const
//------------------------------------------------------------------------------
Rvector3 Rvector3::operator-(const Rvector3& v) const
{
    return Rvector3(elementD[0] - v.elementD[0],
                    elementD[1] - v.elementD[1],
                    elementD[2] - v.elementD[2]);
}

//------------------------------------------------------------------------------
//  const Rvector3& operator-=(const Rvector3& v)
//------------------------------------------------------------------------------
const Rvector3& Rvector3::operator-=(const Rvector3& v)
{
    elementD[0] -= v.elementD[0];
    elementD[1] -= v.elementD[1];
    elementD[2] -= v.elementD[2];
    return *this;
}

//------------------------------------------------------------------------------
//  Rvector3 operator*(Real s) const
//------------------------------------------------------------------------------
Rvector3 Rvector3::operator*(Real s) const
{
    return Rvector3(elementD[0]*s, elementD[1]*s, elementD[2]*s);
}

//------------------------------------------------------------------------------
//  const Rvector3& operator*=(Real s)
//------------------------------------------------------------------------------
const Rvector3& Rvector3::operator*=(Real s)
{
    elementD[0] *= s;
    elementD[1] *= s;
    elementD[2] *= s;
    return *this;
}

//------------------------------------------------------------------------------
//  Real operator*(const Rvector3& v) const        // dot product
//------------------------------------------------------------------------------
Real Rvector3::operator*(const Rvector3& v) const 
{
    return (elementD[0] * v.elementD[0] +
            elementD[1] * v.elementD[1] +
            elementD[2] * v.elementD[2]);
}

//------------------------------------------------------------------------------
//  Rvector3 operator/(Real s) const
//------------------------------------------------------------------------------
Rvector3 Rvector3::operator/(Real s) const
{
    if (GmatMathUtil::IsZero(s))
    {
        throw RealUtilitiesExceptions::ArgumentError();
    }
    return Rvector3(elementD[0]/s, elementD[1]/s, elementD[2]/s);
}

//------------------------------------------------------------------------------
//  const Rvector3& operator/=(Real s)
//------------------------------------------------------------------------------
const Rvector3& Rvector3::operator/=(Real s)
{
    if (GmatMathUtil::IsZero(s))
    {
        throw RealUtilitiesExceptions::ArgumentError();
    }
    elementD[0] /= s;
    elementD[1] /= s;
    elementD[2] /= s;
    return *this;
}

//------------------------------------------------------------------------------
//  Rvector3 operator*(const Rmatrix33& m) const
//------------------------------------------------------------------------------
Rvector3 Rvector3::operator*(const Rmatrix33& m) const
{
    return Rvector3(elementD[0]*m(0,0) + elementD[1]*m(0,1) + elementD[2]*m(0,2),
           elementD[0]*m(1,0) + elementD[1]*m(1,1) + elementD[2]*m(1,2),
           elementD[0]*m(2,0) + elementD[1]*m(2,1) + elementD[2]*m(2,2));
}

//------------------------------------------------------------------------------
//  const Rvector3& operator*=(const Rmatrix33& m)
//------------------------------------------------------------------------------
const Rvector3& Rvector3::operator*=(const Rmatrix33& m) 
{
    *this = *this * m;
    return *this;
}

//------------------------------------------------------------------------------
//  Rvector3 operator/(const Rmatrix33& m) const
//------------------------------------------------------------------------------
Rvector3 Rvector3::operator/(const Rmatrix33& m) const
{
    Rmatrix33 invM;
    try
    {
        invM = m.Inverse();
    }
    catch (Rmatrix::IsSingular)
    {
        throw;
    }
    return (*this) * invM;
}

//------------------------------------------------------------------------------
//  const Rvector3& operator/=(const Rmatrix33& m)
//------------------------------------------------------------------------------
const Rvector3& Rvector3::operator/=(const Rmatrix33& m)
{
    Rmatrix33 invM;
    try
    {
        invM = m.Inverse();
    }
    catch (Rmatrix::IsSingular)
    {
        throw;
    }
    *this *= invM;
    return *this;
}

//------------------------------------------------------------------------------
//  <friend>
//  Rvector3 operator*(Real s, const Rvector3& v)
//------------------------------------------------------------------------------
Rvector3 operator*(Real s, const Rvector3& v)
{
    return Rvector3(s * v.elementD[0],
                   s * v.elementD[1],
                   s * v.elementD[2]);
}

//------------------------------------------------------------------------------
//  <friend>
//  Rvector3 Cross(const Rvector3 &v1, const Rvector3 &v2)
//------------------------------------------------------------------------------
Rvector3 Cross(const Rvector3 &v1, const Rvector3 &v2)
{
    return Rvector3(
       v1.elementD[1]*v2.elementD[2] - v1.elementD[2]*v2.elementD[1],
       v1.elementD[2]*v2.elementD[0] - v1.elementD[0]*v2.elementD[2],
       v1.elementD[0]*v2.elementD[1] - v1.elementD[1]*v2.elementD[0]);
}

//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix33 Outerproduct(const Rvector3 &v1, const Rvector3 &v2)
//------------------------------------------------------------------------------
Rmatrix33 Outerproduct(const Rvector3 &v1, const Rvector3 &v2)
{
    return Rmatrix33(v1.elementD[0] * v2.elementD[0],
                    v1.elementD[0] * v2.elementD[1],
                    v1.elementD[0] * v2.elementD[2],
                    v1.elementD[1] * v2.elementD[0],
                    v1.elementD[1] * v2.elementD[1],
                    v1.elementD[1] * v2.elementD[2],
                    v1.elementD[2] * v2.elementD[0],
                    v1.elementD[2] * v2.elementD[1],
                    v1.elementD[2] * v2.elementD[2]);
}

//------------------------------------------------------------------------------
// Integer GetNumData() const
//------------------------------------------------------------------------------
Integer Rvector3::GetNumData() const
{
   return NUM_DATA;
}

//------------------------------------------------------------------------------
// const std::string* GetDataDescriptions() const
//------------------------------------------------------------------------------
const std::string* Rvector3::GetDataDescriptions() const
{
   return DATA_DESCRIPTIONS;
}


//------------------------------------------------------------------------------
// std::string ToString(Integer precision) const
//------------------------------------------------------------------------------
/*
 * Formats Rvector3 value to String.
 *
 * @param  precision  Precision to be used in formatting
 *
 * @return Formatted Rvector3 value string
 */
//------------------------------------------------------------------------------
std::string Rvector3::ToString(Integer precision) const
{
   GmatGlobal *global = GmatGlobal::Instance();
   global->SetActualFormat(false, precision, 0, true, 1);
   
   std::stringstream ss("");
   ss << *this;
   return ss.str();
}


//------------------------------------------------------------------------------
// std::string ToString(bool useCurrentFormat = true, bool scientific = false,
//                      Integer precision = GmatGlobal::DATA_PRECISION,
//                      Integer width = GmatGlobal::DATA_WIDTH,
//                      bool horizontal = false, Integer spacing = 1) const
//------------------------------------------------------------------------------
/*
 * Formats Rvector3 value to String.
 *
 * @param  useCurrentFormat  Uses precision and width from GmatGlobal
 * @param  scientific  Formats using scientific notation if true
 * @param  precision  Precision to be used in formatting
 * @param  width  Width to be used in formatting
 * @param  horizontal  Format horizontally if true
 * @param  spacing  Spacing to be used in formatting
 *
 * @return Formatted Rvector3 value
 */
//------------------------------------------------------------------------------
std::string Rvector3::ToString(bool useCurrentFormat, bool scientific,
                               Integer precision, Integer width,
                               bool horizontal, Integer spacing) const
{
   GmatGlobal *global = GmatGlobal::Instance();
   
   if (!useCurrentFormat)
      global->SetActualFormat(scientific, precision, width, horizontal, spacing);
   
   std::stringstream ss("");
   ss << *this;
   return ss.str();
}


//------------------------------------------------------------------------------
// friend std::istream& operator>> (std::istream &input, Rvector3 &a)
//------------------------------------------------------------------------------
std::istream& operator>> (std::istream &input, Rvector3 &a)
{
   return GmatRealUtil::operator>> (input, a);
}


//------------------------------------------------------------------------------
// friend std::ostream& operator<< (std::ostream &output, const Rvector3 &a)
//------------------------------------------------------------------------------
std::ostream& operator<< (std::ostream &output, const Rvector3 &a)
{
   return GmatRealUtil::operator<< (output, a);
}
