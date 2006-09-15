//$Header$
//------------------------------------------------------------------------------
//                                Rvector
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: M. Weippert, T. McRoberts, L. Jun, E. Corderman
// Created: 1995/10/10 for GSS project (originally Vector)
// Modified:
//   2003/09/15 Linda Jun - Replaced GSSString with std::string
//
/**
 * Declarations for the Rvector class, providing linear algebra operations
 * for the general n-element Real vector.
 */
//------------------------------------------------------------------------------
#include <stdarg.h>
#include <sstream>
#include "ArrayTemplate.hpp"
#include "TableTemplate.hpp"
#include "Rmatrix.hpp"
#include "RealUtilities.hpp"
#include "Rvector.hpp"
#include "Linear.hpp"         // for operator<<, operator >>

//---------------------------------
//  public
//---------------------------------

//------------------------------------------------------------------------------
//  Rvector()
//------------------------------------------------------------------------------
Rvector::Rvector()
   : ArrayTemplate<Real>()
{
}

//------------------------------------------------------------------------------
//  Rvector(int size)
//------------------------------------------------------------------------------
Rvector::Rvector(int size)
   : ArrayTemplate<Real> (size)
{
    int i;
    for (i = 0; i < sizeD; i++)
    {
        elementD[i] = 0.0;
    }
}

//------------------------------------------------------------------------------
//  Rvector(int size, Real a1, ...)
//------------------------------------------------------------------------------
/**
 * @note . is required for variable length Real values. eg) 123., 100.
 */
//------------------------------------------------------------------------------
Rvector::Rvector(int size, Real a1, ...)
  : ArrayTemplate<Real> (size)
{
    int i,a;
    va_list ap;
    a = 0;

    elementD[0] = a1;
    va_start(ap, a1);
    for (i = 1; i < sizeD; i++)
    {
        elementD[i] = va_arg(ap, Real);
    }
 
    va_end(ap);
}

//------------------------------------------------------------------------------
//  Rvector(const Rvector &v)
//------------------------------------------------------------------------------
Rvector::Rvector(const Rvector &v)
   : ArrayTemplate<Real>(v)
{
}

//------------------------------------------------------------------------------
//  ~Rvector()
//------------------------------------------------------------------------------
Rvector::~Rvector()
{
}


//------------------------------------------------------------------------------
//  Set(int numElem, Real a1, ...)
//------------------------------------------------------------------------------
/**
 * @note . is required for variable length Real values. eg) 123., 100.
 */
//------------------------------------------------------------------------------
void Rvector::Set(int numElem, Real a1, ...)
{
   int i,a;
   va_list ap;
   a = 0;

   elementD[0] = a1;
   va_start(ap, a1);
   for (i = 1; i < numElem; i++)
   {
      elementD[i] = va_arg(ap, Real);
   }
 
   va_end(ap);
}


//------------------------------------------------------------------------------
//  Real GetMagnitude() const
//------------------------------------------------------------------------------
Real Rvector::GetMagnitude() const
{
    Real sum = 0.0;
    int  i;

    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
   
    for (i = 0; i < sizeD; i++)
    {
        sum += elementD[i]*elementD[i];
    }
    return(GmatMathUtil::Sqrt(sum));
}


//------------------------------------------------------------------------------
//  Rvector GetUnitRvector() const
//------------------------------------------------------------------------------
Rvector Rvector::GetUnitRvector() const
{
   int i;

   if (isSizedD == false)
      throw ArrayTemplateExceptions::UnsizedArray();

   Real mag = GetMagnitude();

   if (GmatMathUtil::IsZero(mag))
      throw IsZeroVector(" from Rvector::GetUnitRvector()\n");
   
   Rvector vect(*this);
   
   for (i = 0; i < sizeD; i++)
      vect.elementD[i] /= mag;

   return vect;
}


//------------------------------------------------------------------------------
//  const Rvector& Normalize()
//------------------------------------------------------------------------------
const Rvector& Rvector::Normalize()
{
   int i;
   
   if (isSizedD == false)
      throw ArrayTemplateExceptions::UnsizedArray();
   
   Real mag = GetMagnitude();
   if (GmatMathUtil::IsZero(mag))
      throw IsZeroVector(" from Rvector::Normalize()\n");
   
   for (i = 0; i < sizeD; i++)
      elementD[i] /=  mag;
   
   return *this;
}


//------------------------------------------------------------------------------
//  const Rvector& operator=(const Rvector &v)
//------------------------------------------------------------------------------
const Rvector& Rvector::operator=(const Rvector &v)
{
    ArrayTemplate<Real>::operator=(v);
    return *this;
}

//------------------------------------------------------------------------------
//  bool operator==(const Rvector &v) const
//------------------------------------------------------------------------------
bool Rvector::operator==(const Rvector &v) const
{
    int i;
   
    if ((isSizedD == false) || (v.IsSized() == false))
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
   
    if(this!= &v)
    {
        if (sizeD == v.sizeD)
        {
            for(i = 0; i<sizeD; i++)
            {
                if(elementD[i] != v[i])
                {
                    return false;
                }
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
//  bool operator!=(const Rvector &v) const
//------------------------------------------------------------------------------
bool Rvector::operator!=(const Rvector &v) const
{
    return (bool)!(*this == v);
}

//------------------------------------------------------------------------------
//  Rvector operator-() const                      // negation
//------------------------------------------------------------------------------
Rvector Rvector::operator-() const
{
    int i;

    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
   
    Rvector vector(sizeD);
    for (i = 0; i < sizeD; i++)
    {
        vector.elementD[i] = -elementD[i];
    }
    return vector;
}

//------------------------------------------------------------------------------
//  Rvector operator+(const Rvector &v) const
//------------------------------------------------------------------------------
Rvector Rvector::operator+(const Rvector &v) const
{
    int i;
   
    if ((isSizedD == false) || (v.IsSized() == false))
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (sizeD != v.sizeD)
    {
        throw ArrayTemplateExceptions::DimensionError();
    }
    
    Rvector vect(*this);
    for (i = 0; i < sizeD; i++)
    {
        vect.elementD[i] += v.elementD[i];
    }
    return vect;
}

//------------------------------------------------------------------------------
//  const Rvector& operator+=(const Rvector &v)
//------------------------------------------------------------------------------
const Rvector& Rvector::operator+=(const Rvector &v)
{
    int i;
   
    if ((isSizedD == false) || (v.IsSized() == false))
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (sizeD != v.sizeD)
    {
        throw ArrayTemplateExceptions::DimensionError();
    }

    for (i = 0; i < sizeD; i++)
    {
        elementD[i] += v.elementD[i];
    }
    return *this;
}

//------------------------------------------------------------------------------
//  Rvector operator-(const Rvector &v) const
//------------------------------------------------------------------------------
Rvector Rvector::operator-(const Rvector &v) const
{
    int i;
   
    if ((isSizedD == false) || (v.IsSized() == false))
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
   
    if (sizeD != v.sizeD)
    {
        throw ArrayTemplateExceptions::DimensionError();
    }
    
    Rvector vector(*this);
    for (i = 0; i < sizeD; i++)
    {
        vector.elementD[i] -= v.elementD[i];
    }
    return vector;
} 

//------------------------------------------------------------------------------
//  const Rvector& operator-=(const Rvector &v)
//------------------------------------------------------------------------------
const Rvector& Rvector::operator-=(const Rvector &v)
{
    int i;
   
    if ((isSizedD == false) || (v.IsSized() == false))
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (sizeD != v.sizeD)
    {
        throw ArrayTemplateExceptions::DimensionError();
    }
   
    for (i = 0; i < sizeD; i++)
    {
        elementD[i] -= v.elementD[i];
    }
    return *this;
}

//------------------------------------------------------------------------------
//  Rvector operator*(Real s) const
//------------------------------------------------------------------------------
Rvector Rvector::operator*(Real s) const
{
    int i;

    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }

    Rvector vector(*this);
    for (i = 0; i < sizeD; i++)
    {
        vector.elementD[i] *= s;
    }
    return vector;
}

//------------------------------------------------------------------------------
//  const Rvector& operator*=(Real s)
//------------------------------------------------------------------------------
const Rvector& Rvector::operator*=(Real s)
{
    int i;

    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
   
    for (i = 0; i < sizeD; i++)
    {
        elementD[i] *= s;
    }
    return *this;
}

//------------------------------------------------------------------------------
//  Real operator*(const Rvector &v) const         // dot product
//------------------------------------------------------------------------------
Real Rvector::operator*(const Rvector &v) const
{
    if ((isSizedD == false) || (v.IsSized() == false))
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (sizeD != v.sizeD)
    {
        throw ArrayTemplateExceptions::DimensionError();
    }
   
    Real sum = 0.0;
    int  i;
    for (i = 0; i < sizeD; i++)
    {
        sum += elementD[i] * v.elementD[i];
    }
    return sum;    
}

//------------------------------------------------------------------------------
//  Rvector operator/(Real s) const
//------------------------------------------------------------------------------
Rvector Rvector::operator/(Real s) const
{
    int i;

    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (GmatMathUtil::IsZero(s))
    {
        throw RealUtilitiesExceptions::ArgumentError();
    }
   
    Rvector vector(*this);
    for (i = 0; i < sizeD; i++)
    {
        vector.elementD[i] /= s;
    }
    return vector;
}

//------------------------------------------------------------------------------
//  const Rvector& operator/=(Real s)
//------------------------------------------------------------------------------
const Rvector& Rvector::operator/=(Real s)
{
    int i;
   
    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (GmatMathUtil::IsZero(s))
    {
        throw RealUtilitiesExceptions::ArgumentError();
    }
    for (i = 0; i < sizeD; i++)
    {
        elementD[i] /= s;
    }
    return *this;
}

//------------------------------------------------------------------------------
//  Rvector operator*(const Rmatrix &m) const
//------------------------------------------------------------------------------
/**
 * @note Here vector is treated as a row matrix 1 x N.
 */
//------------------------------------------------------------------------------
Rvector Rvector::operator*(const Rmatrix &m) const
{
    int i,j;

    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (m.IsSized() == false)
    {
        throw TableTemplateExceptions::UnsizedTable();
    }
    if (m.rowsD != sizeD)
    {
        throw ArrayTemplateExceptions::DimensionError();
    }

    Rvector v(m.colsD);
    for (i = 0; i < m.colsD; i++)
    {
        v(i) = 0;
        for (j = 0; j < m.rowsD; j++)  
        {
            v(i) = v(i) + m(j,i)*elementD[j];
        }
    }
    return v;
}

//------------------------------------------------------------------------------
//  const Rvector& operator*=(const Rmatrix &m)
//------------------------------------------------------------------------------
/**
 * @note Here vector is treated as a row matrix 1 x N.
 */
//------------------------------------------------------------------------------
const Rvector& Rvector::operator*=(const Rmatrix &m)
{
    *this *= m;
    return *this;
}

//------------------------------------------------------------------------------
//  Rvector operator/(const Rmatrix &m) const
//------------------------------------------------------------------------------
Rvector Rvector::operator/(const Rmatrix &m) const
{
    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (m.IsSized() == false)
    {
        throw TableTemplateExceptions::UnsizedTable();
    }

    Rmatrix invM(m);
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
//  const Rvector& operator/=(const Rmatrix &m)
//------------------------------------------------------------------------------
const Rvector& Rvector::operator/=(const Rmatrix &m)
{
    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (m.IsSized() == false)
    {
        throw TableTemplateExceptions::UnsizedTable();
    }

    Rmatrix invM(m);
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


//loj: 4/19/05 Added
//------------------------------------------------------------------------------
//  std::string ToString()
//------------------------------------------------------------------------------
std::string Rvector::ToString()
{
   std::stringstream ss("");
   ss.precision(12);
   //ss.setf(std::ios::showpoint);
   
   for (Integer i=0; i<sizeD; i++) 
   {
      ss << elementD[i] << " ";
   }

   return ss.str();
}

bool Rvector::MakeZeroVector()
{
   for (Integer i=0; i<sizeD; i++)
      elementD[i] = 0.0;
   return true;
}

//arg: 4/24/06 Added
//------------------------------------------------------------------------------
//  Real Norm()
//------------------------------------------------------------------------------
/**
 * This method calls returns the square root of the sum of the squares.
 */
//------------------------------------------------------------------------------
Real Rvector::Norm()
{
   Real sum = 0;
       
   for (Integer i=0; i<sizeD; i++)
      sum += GmatMathUtil::Pow(elementD[i], 2);
      
   return GmatMathUtil::Sqrt(sum);
}

//---------------------------------
// friend function
//---------------------------------

//------------------------------------------------------------------------------
//  <friend>
//  Rvector operator*(Real s, const Rvector &v)
//------------------------------------------------------------------------------
Rvector operator*(Real s, const Rvector &v)
{
    int i;

    if (v.isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    Rvector returnRvector(v.sizeD);
    for (i = 0; i < v.sizeD; i++)
    {
        returnRvector.elementD[i] = s * v.elementD[i];
    }
    return returnRvector;
}

//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix Outerproduct(const Rvector &v1, const Rvector &v2)
//------------------------------------------------------------------------------
/**
 * @note Rmatrix multiplication, with first vector considered a Nx1
 *       matrix and a second vector considered a 1xM matrix.
 */
//------------------------------------------------------------------------------
Rmatrix Outerproduct(const Rvector &v1, const Rvector &v2)
{
    int i,j;

    if ((v1.isSizedD == false) || (v2.isSizedD == false))
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }

    Rmatrix prod(v1.sizeD, v2.sizeD);
    for (i = 0; i < v1.sizeD; i++)
    {
        for (j = 0; j < v2.sizeD;j++)
        {
            prod(i,j) = v1.elementD[i] * v2.elementD[j];
        }
    }
    return prod;
}

//------------------------------------------------------------------------------
// friend std::istream& operator>> (std::istream &input, Rvector &a)
//------------------------------------------------------------------------------
std::istream& operator>> (std::istream &input, Rvector &a)
{
   return GmatRealUtil::operator>> (input, a);
}

//------------------------------------------------------------------------------
// friend std::ostream& operator<< (std::ostream &output, const Rvector &a)
//------------------------------------------------------------------------------
std::ostream& operator<< (std::ostream &output, const Rvector &a)
{
   return GmatRealUtil::operator<< (output, a);
}
