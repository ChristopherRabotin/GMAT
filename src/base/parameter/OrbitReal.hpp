//$Header$
//------------------------------------------------------------------------------
//                                OrbitReal
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/03/29
//
/**
 * Declares OrbitReal class which provides base class for orbit realated Real
 * Parameters
 */
//------------------------------------------------------------------------------
#ifndef OrbitReal_hpp
#define OrbitReal_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "OrbitData.hpp"


class GMAT_API OrbitReal : public RealVar, public OrbitData
{
public:

   OrbitReal(const std::string &name, const std::string &typeStr, 
             GmatBase *obj, const std::string &desc,
             const std::string &unit,  GmatParam::DepObject depObj);
   OrbitReal(const OrbitReal &copy);
   OrbitReal& operator=(const OrbitReal &right);
   virtual ~OrbitReal();

   // methods inherited from RealVar
   virtual Real GetReal();
   virtual Real EvaluateReal();

   // methods inherited from Parameter
   virtual Integer GetNumRefObjects() const;
   virtual void SetSolarSystem(SolarSystem *ss);
   virtual bool AddRefObject(GmatBase*obj);
   virtual bool Validate();
   virtual void Initialize();
   
   // methods inherited from GmatBase
   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);
   
   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;
   virtual bool SetRefObjectName(const Gmat::ObjectType type,
                                 const std::string &name);
   virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name);
   virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name = "");

protected:

};

#endif // OrbitReal_hpp
