//$Header$
//------------------------------------------------------------------------------
//                                EnvReal
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/12/10
//
/**
 * Declares EnvReal class which provides base class for environment realated
 * Real Parameters
 */
//------------------------------------------------------------------------------
#ifndef EnvReal_hpp
#define EnvReal_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "EnvData.hpp"


class GMAT_API EnvReal : public RealVar, public EnvData
{
public:

   EnvReal(const std::string &name, const std::string &typeStr, 
           GmatBase *obj, const std::string &desc,
           const std::string &unit, Gmat::ObjectType ownerType,
           GmatParam::DepObject depObj);
   EnvReal(const EnvReal &copy);
   EnvReal& operator=(const EnvReal &right);
   virtual ~EnvReal();

   // methods inherited from Parameter
   virtual Real EvaluateReal();
   
   virtual Integer GetNumRefObjects() const;
   virtual bool AddRefObject(GmatBase *obj);
   virtual void SetSolarSystem(SolarSystem *ss);
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

#endif // EnvReal_hpp
