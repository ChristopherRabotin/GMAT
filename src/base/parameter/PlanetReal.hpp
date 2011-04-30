//$Id$
//------------------------------------------------------------------------------
//                                PlanetReal
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/12/13
//
/**
 * Declares PlanetReal class which provides base class for the planet
 * realated Real Parameters
 */
//------------------------------------------------------------------------------
#ifndef PlanetReal_hpp
#define PlanetReal_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "PlanetData.hpp"


class GMAT_API PlanetReal : public RealVar, public PlanetData
{
public:

   PlanetReal(const std::string &name, const std::string &typeStr, 
              GmatBase *obj, const std::string &desc,
              const std::string &unit, Gmat::ObjectType ownerType,
              GmatParam::DepObject depObj);
   PlanetReal(const PlanetReal &copy);
   PlanetReal& operator=(const PlanetReal &right);
   virtual ~PlanetReal();

   // methods inherited from Parameter
   virtual Real EvaluateReal();

   virtual Integer GetNumRefObjects() const;
   virtual bool AddRefObject(GmatBase *obj, bool replaceName = false);
   virtual void SetSolarSystem(SolarSystem *ss);
   virtual void SetInternalCoordSystem(CoordinateSystem *ss);
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

#endif // PlanetReal_hpp
