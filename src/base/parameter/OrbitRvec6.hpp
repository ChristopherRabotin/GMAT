//$Header$
//------------------------------------------------------------------------------
//                                OrbitRvec6
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/09/08
//
/**
 * Declares OrbitRvec6 class which provides base class for orbit realated
 * Rvector6 Parameters.
 */
//------------------------------------------------------------------------------
#ifndef OrbitRvec6_hpp
#define OrbitRvec6_hpp

#include "gmatdefs.hpp"
#include "Rvec6Var.hpp"
#include "OrbitData.hpp"


class GMAT_API OrbitRvec6 : public Rvec6Var, public OrbitData
{
public:

   OrbitRvec6(const std::string &name, const std::string &typeStr, 
             ParameterKey key, GmatBase *obj, const std::string &desc,
             const std::string &unit, bool isTimeParam);
   OrbitRvec6(const OrbitRvec6 &copy);
   OrbitRvec6& operator=(const OrbitRvec6 &right);
   virtual ~OrbitRvec6();

   // The inherited methods from Rvec6Var
   virtual Rvector6 GetRvector6() const;
   virtual Rvector6 EvaluateRvector6();

   // The inherited methods from Parameter
   virtual void SetSolarSystem(SolarSystem *ss);
   virtual Integer GetNumObjects() const;
   virtual GmatBase* GetObject(const std::string &objTypeName);
   virtual bool SetObject(Gmat::ObjectType objType,
                          const std::string &objName,
                          GmatBase *obj);
   virtual bool AddObject(GmatBase *obj);
   virtual bool Validate();
   virtual void Initialize();

protected:

};

#endif // OrbitRvec6_hpp
