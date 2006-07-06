
#include "AttitudeParameters.hpp"

Quat1::Quat1(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "Quat1", obj, "Quat1", "(units)", 0)
{
   mColor = GmatColor::RED32;
}

Quat1::Quat1(const Quat1 &copy)
   : AttitudeReal(copy)
{
}

Quat1::Quat1& operator=(const Quat1 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}

Quat1::~Quat1()
{
}
   
bool Quat1::Evaluate()
{
   mRealValue = AttitudeData::GetAttitudeReal("Quat1");
    
   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
      return false;
   else
      return true;
}
   
GmatBase* Quat1::Clone(void) const
{
   return new Quat1(*this);
}


Quat2::Quat2(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "Quat2", obj, "Quat2", "(units)", 0)
{
   mColor = GmatColor::YELLOW32;
}

Quat2::Quat2(const Quat2 &copy)
   : AttitudeReal(copy)
{
}

Quat2::Quat2& operator=(const Quat2 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}

Quat2::~Quat2()
{
}
   
bool Quat2::Evaluate()
{
   mRealValue = AttitudeData::GetAttitudeReal("Quat2");
    
   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
      return false;
   else
      return true;
}
   
GmatBase* Quat2::Clone(void) const
{
   return new Quat2(*this);
}


Quat3::Quat3(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "Quat3", obj, "Quat3", "(units)", 0)
{
   mColor = GmatColor::BLUE32;
}

Quat3::Quat3(const Quat3 &copy)
   : AttitudeReal(copy)
{
}

Quat3::Quat3& operator=(const Quat3 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}

Quat3::~Quat3()
{
}
   
bool Quat3::Evaluate()
{
   mRealValue = AttitudeData::GetAttitudeReal("Quat3");
    
   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
      return false;
   else
      return true;
}
   
GmatBase* Quat3::Clone(void) const
{
   return new Quat3(*this);
}


Quat4::Quat4(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "Quat4", obj, "Quat4", "(units)", 0)
{
   mColor = GmatColor::GREEN32;
}

Quat4::Quat4(const Quat4 &copy)
   : AttitudeReal(copy)
{
}

Quat4::Quat4& operator=(const Quat4 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}

Quat4::~Quat4()
{
}
   
bool Quat4::Evaluate()
{
   mRealValue = AttitudeData::GetAttitudeReal("Quat4");
    
   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
      return false;
   else
      return true;
}
   
GmatBase* Quat4::Clone(void) const
{
   return new Quat4(*this);
}
