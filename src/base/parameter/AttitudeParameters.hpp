//$Header$
//------------------------------------------------------------------------------
//                                  AttitudeParameters
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Daniel Hunter
// Created: 2006/6/26
//
/**
 * Implements Attitude related parameter classes.
 *    Quat1, Quat2, Quat3, Quat4
 */
//------------------------------------------------------------------------------
#ifndef AttitudeParameters_hpp
#define AttitudeParameters_hpp

#include "gmatdefs.hpp"
#include "AttitudeReal.hpp"

//------------------------------------------------------------------------------
//  Direction Cosine Matrix
//------------------------------------------------------------------------------
class GMAT_API DCM11 : public AttitudeReal
{
public:

   DCM11(const std::string &name = "", GmatBase *obj = NULL);
   DCM11(const DCM11 &copy);
   DCM11& operator=(const DCM11 &right);
   virtual ~DCM11();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API DCM12 : public AttitudeReal
{
public:

   DCM12(const std::string &name = "", GmatBase *obj = NULL);
   DCM12(const DCM12 &copy);
   DCM12& operator=(const DCM12 &right);
   virtual ~DCM12();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API DCM13 : public AttitudeReal
{
public:

   DCM13(const std::string &name = "", GmatBase *obj = NULL);
   DCM13(const DCM13 &copy);
   DCM13& operator=(const DCM13 &right);
   virtual ~DCM13();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API DCM21 : public AttitudeReal
{
public:

   DCM21(const std::string &name = "", GmatBase *obj = NULL);
   DCM21(const DCM21 &copy);
   DCM21& operator=(const DCM21 &right);
   virtual ~DCM21();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API DCM22 : public AttitudeReal
{
public:

   DCM22(const std::string &name = "", GmatBase *obj = NULL);
   DCM22(const DCM22 &copy);
   DCM22& operator=(const DCM22 &right);
   virtual ~DCM22();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API DCM23 : public AttitudeReal
{
public:

   DCM23(const std::string &name = "", GmatBase *obj = NULL);
   DCM23(const DCM23 &copy);
   DCM23& operator=(const DCM23 &right);
   virtual ~DCM23();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API DCM31 : public AttitudeReal
{
public:

   DCM31(const std::string &name = "", GmatBase *obj = NULL);
   DCM31(const DCM31 &copy);
   DCM31& operator=(const DCM31 &right);
   virtual ~DCM31();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API DCM32 : public AttitudeReal
{
public:

   DCM32(const std::string &name = "", GmatBase *obj = NULL);
   DCM32(const DCM32 &copy);
   DCM32& operator=(const DCM32 &right);
   virtual ~DCM32();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API DCM33 : public AttitudeReal
{
public:

   DCM33(const std::string &name = "", GmatBase *obj = NULL);
   DCM33(const DCM33 &copy);
   DCM33& operator=(const DCM33 &right);
   virtual ~DCM33();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//------------------------------------------------------------------------------
//  Quaternions
//------------------------------------------------------------------------------

class GMAT_API Quat1 : public AttitudeReal
{
public:

   Quat1(const std::string &name = "", GmatBase *obj = NULL);
   Quat1(const Quat1 &copy);
   Quat1& operator=(const Quat1 &right);
   virtual ~Quat1();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


class GMAT_API Quat2 : public AttitudeReal
{
public:

   Quat2(const std::string &name = "", GmatBase *obj = NULL);
   Quat2(const Quat2 &copy);
   Quat2& operator=(const Quat2 &right);
   virtual ~Quat2();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};



class GMAT_API Quat3 : public AttitudeReal
{
public:

   Quat3(const std::string &name = "", GmatBase *obj = NULL);
   Quat3(const Quat3 &copy);
   Quat3& operator=(const Quat3 &right);
   virtual ~Quat3();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API Quat4 : public AttitudeReal
{
public:

   Quat4(const std::string &name = "", GmatBase *obj = NULL);
   Quat4(const Quat4 &copy);
   Quat4& operator=(const Quat4 &right);
   virtual ~Quat4();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//------------------------------------------------------------------------------
//  Euler Angles
//------------------------------------------------------------------------------
class GMAT_API EulerAngle1 : public AttitudeReal
{
public:

   EulerAngle1(const std::string &name = "", GmatBase *obj = NULL);
   EulerAngle1(const EulerAngle1 &copy);
   EulerAngle1& operator=(const EulerAngle1 &right);
   virtual ~EulerAngle1();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API EulerAngle2 : public AttitudeReal
{
public:

   EulerAngle2(const std::string &name = "", GmatBase *obj = NULL);
   EulerAngle2(const EulerAngle2 &copy);
   EulerAngle2& operator=(const EulerAngle2 &right);
   virtual ~EulerAngle2();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API EulerAngle3 : public AttitudeReal
{
public:

   EulerAngle3(const std::string &name = "", GmatBase *obj = NULL);
   EulerAngle3(const EulerAngle3 &copy);
   EulerAngle3& operator=(const EulerAngle3 &right);
   virtual ~EulerAngle3();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//------------------------------------------------------------------------------
//  Angular Velocity
//------------------------------------------------------------------------------

class GMAT_API AngVelX : public AttitudeReal
{
public:

   AngVelX(const std::string &name = "", GmatBase *obj = NULL);
   AngVelX(const AngVelX &copy);
   AngVelX& operator=(const AngVelX &right);
   virtual ~AngVelX();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API AngVelY : public AttitudeReal
{
public:

   AngVelY(const std::string &name = "", GmatBase *obj = NULL);
   AngVelY(const AngVelY &copy);
   AngVelY& operator=(const AngVelY &right);
   virtual ~AngVelY();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API AngVelZ : public AttitudeReal
{
public:

   AngVelZ(const std::string &name = "", GmatBase *obj = NULL);
   AngVelZ(const AngVelZ &copy);
   AngVelZ& operator=(const AngVelZ &right);
   virtual ~AngVelZ();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//------------------------------------------------------------------------------
//  Euler Angle Rates
//------------------------------------------------------------------------------

class GMAT_API EulerAngleRate1 : public AttitudeReal
{
public:

   EulerAngleRate1(const std::string &name = "", GmatBase *obj = NULL);
   EulerAngleRate1(const EulerAngleRate1 &copy);
   EulerAngleRate1& operator=(const EulerAngleRate1 &right);
   virtual ~EulerAngleRate1();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API EulerAngleRate2 : public AttitudeReal
{
public:

   EulerAngleRate2(const std::string &name = "", GmatBase *obj = NULL);
   EulerAngleRate2(const EulerAngleRate2 &copy);
   EulerAngleRate2& operator=(const EulerAngleRate2 &right);
   virtual ~EulerAngleRate2();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API EulerAngleRate3 : public AttitudeReal
{
public:

   EulerAngleRate3(const std::string &name = "", GmatBase *obj = NULL);
   EulerAngleRate3(const EulerAngleRate3 &copy);
   EulerAngleRate3& operator=(const EulerAngleRate3 &right);
   virtual ~EulerAngleRate3();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};



#endif /*AttitudeParameters_hpp*/
