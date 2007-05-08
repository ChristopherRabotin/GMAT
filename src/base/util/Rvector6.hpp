//$Header$
//------------------------------------------------------------------------------
//                                Rvector6
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: L. Jun
// Created: 2003/09/15
//
/**
 * Provides linear algebra operations for 6-element Real vectors.
 */
//------------------------------------------------------------------------------
#ifndef Rvector6_hpp
#define Rvector6_hpp

#include "gmatdefs.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "GmatGlobal.hpp"      // for DATA_PRECISION

class Rvector3;

#define NUM_DATA_INIT 6

class Rvector6 : public Rvector
{
public:

   Rvector6();
   Rvector6(const Real e1, const Real e2, const Real e3, const Real e4,
            const Real e5, const Real e6);
   Rvector6(const Rvector3 &r, const Rvector3 &v);
   Rvector6(Real vec[6]);
   Rvector6(const Rvector6 &v);
   Rvector6& operator=(const Rvector6 &v);
   virtual ~Rvector6();
   
   Real Get(Integer index) const;
   Rvector3 GetR() const;
   Rvector3 GetV() const;
   void GetR(Real *r) const;
   void GetV(Real *v) const;
   
   void Set(const Real e1, const Real e2, const Real e3,
            const Real e4, const Real e5, const Real e6);
   void Set(const Real v[6]);
   void SetR(const Rvector3 &v);
   void SetV(const Rvector3 &v);

   bool operator==(const Rvector6 &v)const;
   bool operator!=(const Rvector6 &v)const;
   Rvector6 operator-() const;                    // negation 
   Rvector6 operator+(const Rvector6 &v) const;
   const Rvector6& operator+=(const Rvector6 &v);
   Rvector6 operator-(const Rvector6 &v) const; 
   const Rvector6& operator-=(const Rvector6 &v);
   Rvector6 operator*(Real s) const;
   const Rvector6& operator*=(Real s);
   Rvector6 operator/(Real s) const;
   const Rvector6& operator/=(Real s);
   
   Integer GetNumData() const;
   const std::string* GetDataDescriptions() const;
   
   bool IsValid(const Real val);
   
   static const Real UTIL_REAL_UNDEFINED;
   static const Rvector6 RVECTOR6_UNDEFINED;
   
private:
   static const Integer NUM_DATA;
   static const std::string DATA_DESCRIPTIONS[NUM_DATA_INIT];
};
#endif // Rvector6_hpp
