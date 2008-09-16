//$Id$
//------------------------------------------------------------------------------
//                                Rmatrix
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Created: 2003/09/15 Linda Jun, NASA/GSFC
//
/**
 * Defines Matrix operations.
 */
//------------------------------------------------------------------------------
#include "TableTemplate.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "RealUtilities.hpp"
#include "Linear.hpp"         // for operator<<, operator >>
#include <stdarg.h>
#include <sstream>

//---------------------------------
//  public
//---------------------------------

//---------------------------------
//  public
//---------------------------------

//------------------------------------------------------------------------------
//  Rmatrix()
//------------------------------------------------------------------------------
Rmatrix::Rmatrix()
   : TableTemplate<Real>()
{
}


//------------------------------------------------------------------------------
//  Rmatrix(int r, int c)
//------------------------------------------------------------------------------
Rmatrix::Rmatrix(int r, int c)
   : TableTemplate<Real>(r, c) 
{
   int i;
   for (i = 0; i < rowsD*colsD; i++)
      elementD[i] = 0.0;

}


//------------------------------------------------------------------------------
//  Rmatrix(int r, int c, Real a1,...)
//------------------------------------------------------------------------------
Rmatrix::Rmatrix(int r, int c, Real a1,...)
   : TableTemplate<Real>(r, c) 
{
   va_list ap;
   int i;
   elementD[0] = a1;
   va_start(ap, a1);
    
   for (i = 1; i < rowsD*colsD; i++)
      elementD[i] = va_arg(ap, Real);

   va_end(ap);
}


//------------------------------------------------------------------------------
//  Rmatrix(const Rmatrix &m)
//------------------------------------------------------------------------------
Rmatrix::Rmatrix(const Rmatrix &m)
   : TableTemplate<Real>(m) 
{
}


//------------------------------------------------------------------------------
//  ~Rmatrix()
//------------------------------------------------------------------------------
Rmatrix::~Rmatrix() 
{
}


//------------------------------------------------------------------------------
//  virtual bool IsOrthogonal(Real accuracyRequired)= 
//                            GmatRealConst::REAL_EPSILON) const
//------------------------------------------------------------------------------
bool Rmatrix::IsOrthogonal(Real accuracyRequired) const 
{
   bool orthogonal = true;  // assume it's orthogonal and try to prove it's not
   int i, j;

   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   // create an array of pointers to column vectors
   ArrayTemplate< Rvector* > columnVect(colsD);

   // initialize the array
   for (i = 0; i < colsD; i++)
   {
      columnVect[i] = new Rvector(rowsD);
   }

   // copy from matrix
   for (i = 0; i < colsD; i++)
   {
      for (j = 0; j < rowsD; j++)
      {
         (*columnVect(i))(j) = elementD[j*colsD + i];
      }
   }

   // are they mutually orthogonal
   for (i = 0; i < colsD && orthogonal; i++)
   {
      for (j = i + 1; j < colsD; j++)
      {
         if (!GmatMathUtil::IsZero((*columnVect(i))*(*columnVect(j)),accuracyRequired)) 
            orthogonal = false;
      }
   }

   for (i = 0; i < colsD; i++)
   {
      delete columnVect[i];
   }

   return orthogonal;
}


//------------------------------------------------------------------------------
//  virtual bool IsOrthonormal(Real accuracyRequired) = 
//                             GmatRealConst::REAL_EPSILON)const
//------------------------------------------------------------------------------
bool Rmatrix::IsOrthonormal (Real accuracyRequired) const 
{
   bool normal = true;  // assume it's normal, try to prove it's not
   int i, j;

   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }
    
   // create an array of pointers to column vectors
   ArrayTemplate< Rvector* > columnVect(colsD);

   // initialize the array
   for (i = 0; i < colsD; i++)
   {
      columnVect[i] = new Rvector(rowsD);
   }

   // copy from matrix
   for (i = 0; i < colsD; i++)
   {
      for (j = 0; j < rowsD; j++)
      {
         (*columnVect(i))(j) = elementD[j*colsD + i];
      }
   }

   // see if each magnitude of each columnVect is equal to one
   for (i = 0; i < colsD && normal; i++)
   {
      if (!GmatMathUtil::IsZero(columnVect(i)->GetMagnitude() - 1, accuracyRequired))
         normal = false;
   }

   for (i = 0; i < colsD; i++)
   {
      delete columnVect[i];
   }
   return ((bool) (normal && IsOrthogonal(accuracyRequired)));
}


//------------------------------------------------------------------------------
//  const Rmatrix& operator=(const Rmatrix &m)
//------------------------------------------------------------------------------
const Rmatrix& Rmatrix::operator=(const Rmatrix &m) 
{
   TableTemplate<Real>::operator=(m);
   return *this;
}


//------------------------------------------------------------------------------
//  bool operator==(const Rmatrix &m)const
//------------------------------------------------------------------------------
bool Rmatrix::operator==(const Rmatrix &m)const
{
   int ii,jj;

   if ((isSizedD == false) || (m.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if ( this != &m)
   {
      if ((rowsD == m.rowsD) && (colsD == m.colsD))
      {
         for (ii=0; ii<rowsD; ii++)
         {
            for (jj=0; jj<colsD; jj++)
            {
               //loj: 5/5/06 used epsilon
               //if (elementD[ii*colsD+jj] != m(ii,jj))
               if (GmatMathUtil::Abs(elementD[ii*colsD+jj] - m(ii,jj)) >
                   GmatRealConst::REAL_TOL)
               {
                  return false;
               }
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
//  bool operator!=(const Rmatrix &m)const
//------------------------------------------------------------------------------
bool Rmatrix::operator!=(const Rmatrix &m)const
{
   return (bool) (!( *this== m));
}


//------------------------------------------------------------------------------
//  Rmatrix operator+(const Rmatrix &m)const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::operator+(const Rmatrix &m) const 
{
   if ((isSizedD == false) || (m.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (rowsD != m.rowsD || colsD != m.colsD)
      throw TableTemplateExceptions::DimensionError();
   Rmatrix sum(rowsD, colsD);
    
   int i;
   for (i = 0; i < rowsD*colsD; i++)
   {
      sum.elementD[i] = elementD[i] + m.elementD[i];
   }
   return sum;
}


//------------------------------------------------------------------------------
//  const Rmatrix& operator+=(const Rmatrix &m)
//------------------------------------------------------------------------------
const Rmatrix& Rmatrix::operator+=(const Rmatrix &m) 
{
   if ((isSizedD == false) || (m.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (rowsD != m.rowsD || colsD != m.colsD)
      throw TableTemplateExceptions::DimensionError();

   int i;
   for (i = 0; i < rowsD*colsD; i++)
   {
      elementD[i] = elementD[i] + m.elementD[i];
   }

   return *this;
}


//------------------------------------------------------------------------------
//  Rmatrix operator-(const Rmatrix &m) const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::operator-(const Rmatrix &m) const 
{
   if ((isSizedD == false) || (m.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (rowsD != m.rowsD || colsD != m.colsD)
      throw TableTemplateExceptions::DimensionError();
   Rmatrix diff(rowsD, colsD);

   int i;
   for (i = 0; i < rowsD*colsD; i++)
   {
      diff.elementD[i] = elementD[i] - m.elementD[i];
   }

   return diff;
}


//------------------------------------------------------------------------------
//  const Rmatrix& operator-=(const Rmatrix &m)
//------------------------------------------------------------------------------
const Rmatrix& Rmatrix::operator-=(const Rmatrix &m) 
{
   if ((isSizedD == false) || (m.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (rowsD != m.rowsD || colsD != m.colsD)
      throw TableTemplateExceptions::DimensionError();

   int i;
   for (i = 0; i < rowsD*colsD; i++)
   {
      elementD[i] = elementD[i] - m.elementD[i];
   }
   return *this;
}


//------------------------------------------------------------------------------
//  const Rmatrix operator*(const Rmatrix &m) const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::operator*(const Rmatrix &m) const 
{
   if ((isSizedD == false) || (m.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (colsD != m.rowsD)
      throw TableTemplateExceptions::DimensionError();
   Rmatrix prod(rowsD, m.colsD);  // declare a zero matrix
    
   int i, j, k;
   for (i = 0; i < rowsD; i++)
   {
      for (j = 0; j < m.colsD; j++)
      {
         for (k = 0; k < colsD; k++)
         {
            prod(i, j) += elementD[i*colsD + k]*m(k, j);
         }
      }
   }
   
   return prod;
}


//------------------------------------------------------------------------------
//  const Rmatrix& operator*=(const Rmatrix &m)
//------------------------------------------------------------------------------
const Rmatrix& Rmatrix::operator*=(const Rmatrix &m) 
{
   if ((isSizedD == false) || (m.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   *this = *this * m;  
   return *this;
}


//------------------------------------------------------------------------------
//  const Rmatrix operator/(const Rmatrix &m) const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::operator/( const Rmatrix &m) const
{ 
   if ((isSizedD == false) || (m.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   return (*this)*m.Inverse();
}


//------------------------------------------------------------------------------
//  const Rmatrix & operator/=(const Rmatrix &m)
//------------------------------------------------------------------------------
const Rmatrix& 
Rmatrix::operator/=(const Rmatrix &m) 
{
   if ((isSizedD == false) || (m.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   return (*this) *= m.Inverse();
}


//------------------------------------------------------------------------------
//  Rmatrix operator*(Real scalar) const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::operator*(Real scalar) const 
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   Rmatrix prod(rowsD, colsD);
    
   int i;
   for (i = 0; i < rowsD*colsD; i++)
   {
      prod.elementD[i] = elementD[i]*scalar;
   }
   return prod;
}


//------------------------------------------------------------------------------
//  const Rmatrix& operator*=(Real scalar)
//------------------------------------------------------------------------------
const Rmatrix& Rmatrix::operator*=(Real scalar) 
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   int i;
   for (i = 0; i < rowsD*colsD; i++)
   {
      elementD[i] = elementD[i]*scalar;
   }
   return *this;
}


//------------------------------------------------------------------------------
//  Rmatrix operator/(Real scalar)const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::operator/(Real scalar) const 
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   Rmatrix quot(rowsD, colsD);
   
   if (GmatMathUtil::IsZero(scalar))
      throw Rmatrix::DivideByZero();
   
   int i;
   for (i = 0; i < rowsD*colsD; i++)
      quot.elementD[i] = elementD[i]/scalar;

   return quot;
}


//------------------------------------------------------------------------------
//  const Rmatrix& operator/=(Real scalar)
//------------------------------------------------------------------------------
const Rmatrix& Rmatrix::operator/=(Real scalar) 
{
   if (isSizedD == false)
      throw TableTemplateExceptions::UnsizedTable();

   if (GmatMathUtil::IsZero(scalar))
      throw Rmatrix::DivideByZero();
   
   int i;
   for (i = 0; i < rowsD*colsD; i++)
      elementD[i] = elementD[i]/scalar;

   return *this;
}


//------------------------------------------------------------------------------
//  Rmatrix operator-() const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::operator-() const 
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   Rmatrix neg(rowsD, colsD);
    
   int i;
   for (i = 0; i < rowsD*colsD; i++)
   {
      neg.elementD[i] = -elementD[i];
   }
   return neg;
}


//------------------------------------------------------------------------------
//  Rvector operator*(const Rvector &v) const
//------------------------------------------------------------------------------
Rvector Rmatrix::operator*(const Rvector &v) const 
{
   if (isSizedD == false)
      throw TableTemplateExceptions::UnsizedTable();

   if (v.IsSized() == false)
      throw ArrayTemplateExceptions::UnsizedArray();

   if (colsD != v.sizeD)
      throw TableTemplateExceptions::DimensionError();
   
   Rvector prod(rowsD);

   int i, j;
   Real var;
   for (i = 0; i < rowsD; i++)
   { 
      var = 0.0;
      for (j = 0; j < colsD; j++)
      {
         prod.elementD[i] = elementD[i*colsD + j]*v.elementD[j];
         var = var + prod.elementD[i];
      }
      prod.elementD[i] = var;
   }
   
   return prod;
}


//---------------------------------
// friend functions
//---------------------------------

//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix operator*(Real scalar, const Rmatrix &m)
//------------------------------------------------------------------------------
Rmatrix operator*(Real scalar, const Rmatrix &m) 
{
   if (m.IsSized() == false)
      throw TableTemplateExceptions::UnsizedTable();

   Rmatrix prod(m);
    
   int i;
   for (i = 0; i < m.rowsD*m.colsD; i++)
      prod.elementD[i] *= scalar;

   return prod;
}


//------------------------------------------------------------------------------
//  virtual real Trace() const
//------------------------------------------------------------------------------
Real Rmatrix::Trace() const
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (rowsD != colsD)
      throw Rmatrix::NotSquare();
   Real sum = 0;

   int i;
   for (i = 0; i < rowsD; i++)
   {
      sum += elementD[i*colsD + i];
   }

   return sum;
}


//------------------------------------------------------------------------------
//  virtual Real Determinant() const
//------------------------------------------------------------------------------
Real Rmatrix::Determinant() const
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (rowsD != colsD)
      throw Rmatrix::NotSquare();
   Real D;

   if (rowsD == 1)
      D = elementD[0];
   else if (rowsD == 2)
      D = elementD[0]*elementD[3] - elementD[1]*elementD[2];
   else if (rowsD == 3)
      D = elementD[0]*elementD[4]*elementD[8] + 
         elementD[1]*elementD[5]*elementD[6] + 
         elementD[2]*elementD[3]*elementD[7] - 
         elementD[0]*elementD[5]*elementD[7] - 
         elementD[1]*elementD[3]*elementD[8] -
         elementD[2]*elementD[4]*elementD[6];
   else {
      D = 0.0;
      int i;
      for (i = 0; i < colsD; i++)
      {
         D += elementD[i]*Cofactor(0,i);
      }
   }
   
   return D;
}


//------------------------------------------------------------------------------
//  virtual Real Cofactor(int r, int c) const
//------------------------------------------------------------------------------
Real Rmatrix::Cofactor(int r, int c)const 
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (rowsD != colsD)
      throw Rmatrix::NotSquare();
   Rmatrix Minor(rowsD - 1, colsD - 1);
   Real Cof;
  
   // build the minor matrix
   int i, j, minorI, minorJ;
   for (i = 0, minorI = -1; i < rowsD; i++) 
   {
      if (i != r) 
      {
         minorI++;
         for ( j = 0, minorJ = -1; j < colsD; j++) 
         { 
            if (j != c) 
            {
               minorJ++;
               Minor(minorI, minorJ) = elementD[i*colsD + j]; 
            } // if (j != c) 
         } // for (j = ...
      } // if (i != r)
   } // for (i = ...

   Cof = Minor.Determinant();

   // if r+c is odd Cof is negative
   if ((r+c)%2 == 1)
      Cof = - Cof;
   return Cof;
}


//------------------------------------------------------------------------------
//  Rmatrix Transpose() const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::Transpose() const
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   Rmatrix tran(colsD, rowsD);
    
   int i, j;
   for (i = 0; i < rowsD; i++)
   {
      for (j = 0; j < colsD; j++)
      {
         tran(j, i) = elementD[i*colsD + j]; 
      }
   }

   return tran;
}


//------------------------------------------------------------------------------
//  Rmatrix Inverse() const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::Inverse() const
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (rowsD != colsD)
      throw Rmatrix::NotSquare();
   //int dummy_marker = 4;
   int IndexRange = rowsD;
   Rmatrix A = *this;
   ArrayTemplate<bool> PivotAllowed(IndexRange);
   ArrayTemplate<int> PivotRowList(IndexRange), PivotColumnList(IndexRange);
   Real PivotElement;
   int PivotRow = -1, PivotColumn = -1;
   Real tmp;
   int k;
   for (k = 0; k < IndexRange; k++)
   {
      PivotAllowed(k) = true;
   }
   // Outer Loop
   int i, n, j;
   for (n = 0; n < IndexRange; n++) 
   {
      PivotElement = GmatRealConst::REAL_EPSILON;

      // find pivot element
      for (i = 0; i < IndexRange; i++) 
      {
         if (PivotAllowed(i))          // row I not yet used to pivot
         {
            for (j = 0; j < IndexRange; j++) 
            {
               if (PivotAllowed(j))  // column J not yet used to pivot
               {
                  if (GmatMathUtil::Abs(PivotElement) < GmatMathUtil::Abs(A(i,j))) 
                  {
                     PivotRow = i;
                     PivotColumn = j;
                     PivotElement = A(i,j);
                  }
               }
            }
         } 
      }
        
      if (GmatMathUtil::IsZero(PivotElement,0.000000000001)){
         throw Rmatrix::IsSingular(); }

      PivotRowList(n) = PivotRow;
      PivotColumnList(n) = PivotColumn;
      PivotAllowed(PivotColumn) = false;

      A(PivotRow, PivotColumn) = 1.0;

    
      // normalize and exchange rows
      for (j = 0; j < IndexRange; j++) 
      {
         tmp = A(PivotRow, j);
         A(PivotRow, j) = A(PivotColumn, j);
         A(PivotColumn, j) = tmp / PivotElement;
      }

      // Perform elimination
      for (i = 0; i < IndexRange; i++) 
      {
         if (i != PivotColumn)         // except for pivoted row.
         {
            tmp = A(i, PivotColumn);
            A(i, PivotColumn) = 0.0;
            for (j = 0; j < IndexRange; j++) 
            {
               A(i, j) = A(i, j) - A(PivotColumn, j)*tmp;
            }
         } // if (i != PivotColumn)
      } // for (i = 0...
   } // for (n = 0...

   // Reorder matrix by exchanging columns
   for (n = (IndexRange - 1); n > -1; n--) 
   {
      // exchange two columns
      for (i = 0; i < IndexRange; i++) 
      {
         tmp = A(i, PivotRowList(n));
         A(i, PivotRowList(n)) = A(i, PivotColumnList(n));
         A(i, PivotColumnList(n)) = tmp;
      }
   }
   return A;
}


//----------------------------------------------------------------------------- 
//  virtual Rmatrix Pseudoinverse() const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::Pseudoinverse() const
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   Rmatrix InverseM(colsD, rowsD), TransposeM(colsD, rowsD);
   Rmatrix m1(rowsD, rowsD), m2(colsD, colsD);
   Real accuracyRequired = 0.005;
   if (rowsD < colsD) 
   {
      m1 = MatrixTimesTranspose(*this, *this);
      if (!GmatMathUtil::IsZero(m1.Determinant(),accuracyRequired))
         InverseM = Transpose()*m1.Inverse();
      else
         throw Rmatrix::IsSingular();
   } 
   else if (rowsD > colsD) 
   {
      m2 = TransposeTimesRmatrix(*this, *this);
      if (!GmatMathUtil::IsZero(m2.Determinant(),accuracyRequired))
         InverseM = m2.Inverse()*Transpose();
      else 
         throw Rmatrix::IsSingular();
   }
   else
      InverseM = Inverse();
   return InverseM;
}


//------------------------------------------------------------------------------
//  Rmatrix Symmetric() const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::Symmetric() const 
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (rowsD != colsD)
      throw Rmatrix::NotSquare();

   return (*this + Transpose())/2;
}


//------------------------------------------------------------------------------
//  Rmatrix AntiSymmetric() const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::AntiSymmetric() const
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (rowsD != colsD)
      throw Rmatrix::NotSquare();

   return (*this - Transpose())/2;
}


//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix SkewSymmetric4x4(const Rvector3 &v)
//----------------------------------------------------------------------------- 
Rmatrix SkewSymmetric4by4(const Rvector3 &v)
{
   Rmatrix skew(4,4);
   skew(0,0) = 0.0;
   skew(0,1) = v[2];
   skew(0,2) = -v[1];
   skew(0,3) = v[0];

   skew(1,0) = -v[2];
   skew(1,1) = -0.0;
   skew(1,2) = v[0];
   skew(1,3) = v[1];

   skew(2,0) = v[1];
   skew(2,1) = -v[0];
   skew(2,2) = 0.0;
   skew(2,3) = v[2];

   skew(3,0) = -v[0];
   skew(3,1) = -v[1];
   skew(3,2) = -v[2];
   skew(3,3) = 0.0;
   return skew;
}


//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix TransposeTimesRmatrix(const Rmatrix &m1, const Rmatrix &m2)
//------------------------------------------------------------------------------
Rmatrix TransposeTimesRmatrix(const Rmatrix &m1, const Rmatrix &m2) 
{
   if ((m1.IsSized() == false) || (m2.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (m1.rowsD != m2.rowsD)
      throw TableTemplateExceptions::DimensionError();
    
   Rmatrix m(m1.colsD, m2.colsD);

   int i, j, k;
   for (i = 0; i < m1.colsD; i++)
   {
      for (j = 0; j < m2.colsD; j++)
      {
         for (k = 0; k < m1.rowsD; k++)
         {
            m(i, j) += m1(k, i)*m2(k, j);
         }
      }
   }

   return m;
}


//------------------------------------------------------------------------------
//  <friend>
//  MatrixTimesTranspose(const Rmatrix &m1, const Rmatrix &m2)
//------------------------------------------------------------------------------
Rmatrix MatrixTimesTranspose(const Rmatrix &m1, const Rmatrix &m2) 
{
   if ((m1.IsSized() == false) || (m2.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (m1.rowsD != m2.rowsD)
      throw TableTemplateExceptions::DimensionError();
    
   Rmatrix m(m1.rowsD, m2.rowsD);

   int i, j, k;
   for (i = 0; i < m1.rowsD; i++)
   {
      for (j = 0; j < m2.rowsD; j++)
      {
         for (k = 0; k < m1.colsD; k++)
         {
            m(i, j) += m1(i, k)*m2(j, k);
         }
      }
   }

   return m;
}


//------------------------------------------------------------------------------
//  <friend>
//  TransposeTimesTranspose(const Rmatrix &m1, const Rmatrix &m2)
//------------------------------------------------------------------------------
Rmatrix TransposeTimesTranspose(const Rmatrix &m1, const Rmatrix &m2) 
{
   if ((m1.IsSized() == false) || (m2.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (m1.rowsD != m2.rowsD)
      throw TableTemplateExceptions::DimensionError();
    
   Rmatrix m(m1.colsD, m2.rowsD);

   int i, j, k;
   for (i = 0; i < m1.colsD; i++)
   {
      for (j = 0; j < m2.rowsD; j++)
      {
         for (k = 0; k < m1.rowsD; k++)
         {
            m(i, j) += m1(k, i)*m2(j, k);
         }
      }
   }

   return m;
}


//------------------------------------------------------------------------------
// friend std::istream& operator>> (std::istream &input, Rmatrix &a)
//------------------------------------------------------------------------------
std::istream& operator>> (std::istream &input, Rmatrix &a)
{
   return GmatRealUtil::operator>> (input, a);
}


//------------------------------------------------------------------------------
// friend std::ostream& operator<< (std::ostream &output, const Rmatrix &a)
//------------------------------------------------------------------------------
std::ostream& operator<< (std::ostream &output, const Rmatrix &a)
{
   return GmatRealUtil::operator<< (output, a);
}

//------------------------------------------------------------------------------
// Rvector GetRow(int r) const
//------------------------------------------------------------------------------
Rvector Rmatrix::GetRow(int r) const
{
   Rvector rvec(colsD);
  
   for (int i=0; i<colsD; i++)
      rvec.SetElement(i, GetElement(r, i));

   return rvec;
}


//------------------------------------------------------------------------------
// Rvector GetColumn(int c) const
//------------------------------------------------------------------------------
Rvector Rmatrix::GetColumn(int c) const
{
   Rvector rvec(rowsD);

   for (int i=0; i<rowsD; i++)
      rvec.SetElement(i, GetElement(i, c));

   return rvec;
}


//------------------------------------------------------------------------------
// const StringArray& GetStringVals(Integer p = GmatGlobal::DATA_PRECISION,
//                                  Integer w = GmatGlobal::DATA_WIDTH)
//------------------------------------------------------------------------------
const StringArray& Rmatrix::GetStringVals(Integer p, Integer w)
{
   stringVals.clear();
   std::stringstream ss("");
   
   ss.setf(std::ios::right);
   
   for (int i=0; i<rowsD*colsD; i++)
   {
      ss.str("");
      ss << std::setw(w) << std::setprecision(p) << elementD[i];
      stringVals.push_back(ss.str());
   }

   return stringVals;
}


//------------------------------------------------------------------------------
// std::string ToString(Integer precision, bool horizontal,
//                      const std::string &prefix = "") const
//------------------------------------------------------------------------------
/*
 * Formats Rmatrix value to String.
 *
 * @param  precision  Precision to be used in formatting
 * @param  horizontal  Format horizontally if true (false)
 * @param  prefix  Prefix to be used in vertical formatting ("")
 *
 * @return Formatted Rmatrix value string
 */
//------------------------------------------------------------------------------
std::string Rmatrix::ToString(Integer precision, bool horizontal,
                              const std::string &prefix) const
{
   GmatGlobal *global = GmatGlobal::Instance();
   global->SetActualFormat(false, false, precision, 0, horizontal, 1, prefix);
   
   std::stringstream ss("");
   ss << *this;
   return ss.str();
}


//------------------------------------------------------------------------------
// std::string ToString(bool useCurrentFormat, bool scientific,
//                      bool showPoint, Integer precision, Integer width,
//                      bool horizontal, Integer spacing,
//                      const std::string &prefix, bool appendEol) const
//------------------------------------------------------------------------------
/*
 * Formats Rmatrix value to String.
 *
 * @param  useCurrentFormat  Uses precision and width from GmatGlobal (true)
 * @param  scientific  Formats using scientific notation if true (false)
 * @param  showPoint  Formats using ios::showpoint if true (false)
 * @param  precision  Precision to be used in formatting (GmatGlobal::DATA_PRECISION)
 * @param  width  Width to be used in formatting (GmatGlobal::DATA_WIDTH)
 * @param  horizontal  Format horizontally if true (false)
 * @param  spacing  Spacing to be used in formatting (1)
 * @param  appendEol  Appends eol if true (true)
 *
 * @return Formatted Rmatrix value string
 */
//------------------------------------------------------------------------------
std::string Rmatrix::ToString(bool useCurrentFormat, bool scientific,
                              bool showPoint, Integer precision, Integer width,
                              bool horizontal, Integer spacing,
                              const std::string &prefix, bool appendEol) const
{
   GmatGlobal *global = GmatGlobal::Instance();
   
   if (!useCurrentFormat)
      global->SetActualFormat(scientific, showPoint, precision, width, horizontal,
                              spacing, prefix, appendEol);
   
   std::stringstream ss("");
   ss << *this;
   return ss.str();
}

