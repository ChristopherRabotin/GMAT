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
// Author: M. Weippert, T. McRoberts, L. Jun
// Created: 1995/10/10 for GSS project (originally Vector3)
// Modified: 2003/09/16 Linda Jun - Added new methods and member data to return
//           data descriptions and values.
//
/**
 * Provides linear algebra operations for 3-element Real vectors.
 */
//------------------------------------------------------------------------------
#ifndef Rvector3_hpp
#define Rvector3_hpp

#include "gmatdefs.hpp"
#include "Rvector.hpp"

//forward declaration for matrix operations
class Rmatrix33;

class Rvector3 : public Rvector
{
public:

   Rvector3();
   Rvector3(const Real e1, const Real e2, const Real e3);
   Rvector3(const Rvector3 &v);
   virtual ~Rvector3();

   Real Get(Integer index) const;
   void Set(const Real e1, const Real e2, const Real e3);

   virtual Real GetMagnitude() const; 
   Rvector3 GetUnitVector() const;
   const Rvector3& Normalize();

   Rvector3& operator=(const Rvector3 &v);
   bool operator==(const Rvector3 &v)const;
   bool operator!=(const Rvector3 &v)const;
   Rvector3 operator-() const;                    // negation 
   Rvector3 operator+(const Rvector3 &v) const;
   const Rvector3& operator+=(const Rvector3 &v);
   Rvector3 operator-(const Rvector3 &v) const; 
   const Rvector3& operator-=(const Rvector3 &v);
   Rvector3 operator*(Real s) const;
   const Rvector3& operator*=(Real s);
   Real operator*(const Rvector3 &v) const;       // dot product
   Rvector3 operator/(Real s) const;
   const Rvector3& operator/=(Real s);
   
   friend class Rmatrix33;
   Rvector3 operator*(const Rmatrix33 &m) const; 
   const Rvector3& operator*=(const Rmatrix33 &m);
   Rvector3 operator/(const Rmatrix33 &m) const;
   const Rvector3& operator/=(const Rmatrix33 &m);

   friend Rvector3 operator*(Real s, const Rvector3 &v);
   friend Rmatrix33 Outerproduct(const Rvector3 &v1, const Rvector3 &v2);
   friend Rvector3 Cross(const Rvector3 &v1, const Rvector3 &v2);

   Integer GetNumData() const;
   const std::string* GetDataDescriptions() const;
   std::string* ToValueStrings();
   std::string ToString(); //loj: 4/7/05 Added

protected:
private:
   static const Integer NUM_DATA = 3;
   static const std::string DATA_DESCRIPTIONS[NUM_DATA];
   std::string stringValues[NUM_DATA];
};
#endif // Rvector3_hpp
