//$Id$
//------------------------------------------------------------------------------
//                                TimeReal
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
// Created: 2004/04/28
//
/**
 * Declares TimeReal class which provides base class for time realated Real
 * Parameters
 */
//------------------------------------------------------------------------------
#ifndef TimeReal_hpp
#define TimeReal_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "TimeData.hpp"


class GMAT_API TimeReal : public RealVar, public TimeData
{
public:

   TimeReal(const std::string &name, const std::string &typeStr, 
            GmatBase *obj, const std::string &desc, const std::string &unit,
            bool isSettable = false, Gmat::ObjectType paramOwnerType = Gmat::SPACECRAFT);
   TimeReal(const TimeReal &copy);
   TimeReal& operator=(const TimeReal &right);
   virtual ~TimeReal();

   // methods inherited from Parameter
   virtual Real EvaluateReal();
   
   virtual Integer GetNumRefObjects() const;
   virtual bool Validate();
   virtual bool Initialize();
   virtual bool AddRefObject(GmatBase *obj, bool replaceName = false);
   
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

#endif // TimeReal_hpp
