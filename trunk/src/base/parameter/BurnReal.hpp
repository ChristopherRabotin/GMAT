//$Header$
//------------------------------------------------------------------------------
//                                BurnReal
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
 * Declares BurnReal class which provides base class for orbit realated Real
 * Parameters
 */
//------------------------------------------------------------------------------
#ifndef BurnReal_hpp
#define BurnReal_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "BurnData.hpp"


class GMAT_API BurnReal : public RealVar, public BurnData
{
public:

   BurnReal(const std::string &name, const std::string &typeStr,
            Gmat::ObjectType ownerType, GmatBase *obj,
            const std::string &desc, const std::string &unit,
            GmatParam::DepObject depObj, bool isSettable);
   BurnReal(const BurnReal &copy);
   BurnReal& operator=(const BurnReal &right);
   virtual ~BurnReal();

   // methods inherited from Parameter
   virtual Real EvaluateReal();
   
   virtual Integer GetNumRefObjects() const;
   virtual CoordinateSystem* GetInternalCoordSystem();
   virtual void SetSolarSystem(SolarSystem *ss);
   virtual void SetInternalCoordSystem(CoordinateSystem *ss);
   virtual bool AddRefObject(GmatBase*obj, bool replaceName = false);
   virtual bool Validate();
   virtual bool Initialize();
   
   // methods inherited from GmatBase
   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);
   
   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool SetRefObjectName(const Gmat::ObjectType type,
                                 const std::string &name);
   virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name);
   virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name = "");

protected:

};

#endif // BurnReal_hpp
