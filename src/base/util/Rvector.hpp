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
#ifndef Rvector_hpp
#define Rvector_hpp

#include "gmatdefs.hpp"
#include "ArrayTemplate.hpp"
#include "BaseException.hpp"
#include "RealUtilities.hpp"

//forward declaration for matrix operations
class Rmatrix;

class Rvector : public ArrayTemplate<Real>
{
public:
   
    // exceptions
    class IsZeroVector : public BaseException
        { public:  IsZeroVector(const std::string &message =
           "Rvector error : vector is a zero vector")
           : BaseException(message) {}; };
    
    Rvector();
    Rvector(int size);
    Rvector(int size, Real a1, ... );  
    Rvector(const Rvector &v); 
    virtual ~Rvector();

    virtual Real GetMagnitude() const;          
    Rvector GetUnitRvector() const; 
    const Rvector& Normalize();
    const Rvector& operator=(const Rvector &v); 
    bool operator==(const Rvector &v)const;
    bool operator!=(const Rvector &v)const;
    Rvector operator-() const;                     // negation 
    Rvector operator+(const Rvector & v) const;
    const Rvector& operator+=(const Rvector &v);
    Rvector operator-(const Rvector &v) const; 
    const Rvector& operator-=(const Rvector &v);
    Rvector operator*(Real s) const;
    const Rvector& operator*=(Real s);
    Real operator*(const Rvector &v) const;        // dot product
    Rvector operator/(Real s) const;
    const Rvector& operator/=(Real s);

    friend class Rmatrix;
    Rvector operator*(const Rmatrix &m) const; 
    const Rvector& operator*=(const Rmatrix &m);
    Rvector operator/(const Rmatrix &m) const;
    const Rvector& operator/=(const Rmatrix &m);
    
    friend Rvector operator*(Real s, const Rvector &v);
    friend Rmatrix Outerproduct(const Rvector &v1, const Rvector &v2); 
   
protected:
private:
};
#endif // Rvector_hpp
