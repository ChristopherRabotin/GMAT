//$Header$
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
// Author: M. Weippert, T. McRoberts, E. Corderman
// Created: 1996/06/30 for GSS project (originally Matrix)
// Modified:
//   2003/09/15 Linda Jun - Replaced GSSString with std::string.
//
/**
 * Declares Matrix operations.
 */
//------------------------------------------------------------------------------
#ifndef h_Rmatrix_hpp
#define h_Rmatrix_hpp

#include "RealUtilities.hpp"
#include "TableTemplate.hpp"
#include "BaseException.hpp"

// Forward declarations
class Rvector;

class Rmatrix :  public TableTemplate<Real>
{
public:

   // exceptions
   class NotSquare : public BaseException 
   { public:  NotSquare(const std::string &message =
                        "Rmatrix error: matrix not square")
        : BaseException(message) {}; };
   class IsSingular : public BaseException
   { public:  IsSingular(const std::string &message =
                         "Rmatrix error: matrix is singular")
        : BaseException(message) {}; };
   class DivideByZero : public BaseException
   { public:  DivideByZero(const std::string &message =
                           "Rmatrix error: attempt to divide by zero")
        : BaseException(message) {}; };
    
   Rmatrix();
   Rmatrix(int r, int c);
   Rmatrix(int r, int c, Real a1, ...);
   Rmatrix(const Rmatrix &m);
   virtual ~Rmatrix();
    
   virtual bool 
   IsOrthogonal(Real accuracyRequired = GmatRealConst::REAL_EPSILON) const;
   virtual bool 
   IsOrthonormal(Real accuracyRequired = GmatRealConst::REAL_EPSILON)
      const;
    
   const Rmatrix& operator=(const Rmatrix &m);
   bool operator==(const Rmatrix &m)const;
   bool operator!=(const Rmatrix &m)const;

   //  operations with other matrices
   Rmatrix operator+(const Rmatrix& RHSRmatrix) const;
   const Rmatrix& operator+=(const Rmatrix &RHSRmatrix);

   Rmatrix operator-(const Rmatrix &RHSRmatrix) const;
   const Rmatrix& operator-=(const Rmatrix &RHSRmatrix);

   Rmatrix operator*(const Rmatrix &RHSRmatrix) const;
   const Rmatrix& operator*=(const Rmatrix &RHSRmatrix);

   Rmatrix operator/(const Rmatrix &RHSRmatrix) const;
   const Rmatrix& operator/=(const Rmatrix &RHSRmatrix);
    
   // operations with scalars
   Rmatrix operator*(Real scalar) const;
   const Rmatrix& operator*=(Real scalar);
   Rmatrix operator/(Real scalar) const;
   const Rmatrix& operator/=(Real scalar);
   // negate
   Rmatrix operator-() const;

   friend class Rvector;
   friend class Rvector3;
   Rvector operator*(const Rvector &v) const;

   friend Rmatrix operator*(Real scalar, const Rmatrix &m);
    
   virtual Real   Trace() const ;                            
   virtual Real   Determinant() const;            
   virtual Real   Cofactor(int r, int c) const;            
   Rmatrix Transpose() const;                        
   Rmatrix Inverse() const;                    
   virtual Rmatrix Pseudoinverse() const;            
   Rmatrix Symmetric() const;            
   Rmatrix AntiSymmetric() const;    

   // friends
   friend Rmatrix SkewSymmetric4by4(const Rvector3 &v);
   friend Rmatrix TransposeTimesRmatrix(const Rmatrix &m1, const Rmatrix &m2);
   friend Rmatrix RmatrixTimesTranspose(const Rmatrix &m1, const Rmatrix &m2); 
   friend Rmatrix TransposeTimesTranspose(const Rmatrix &m1, const Rmatrix &m2);

   friend std::istream& operator>> (std::istream &input, Rmatrix &a);
   friend std::ostream& operator<< (std::ostream &output, const Rmatrix &a);

protected:   

private:
};
#endif // Rmatrix_hpp
