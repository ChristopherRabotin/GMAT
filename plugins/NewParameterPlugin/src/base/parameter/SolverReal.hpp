//$Id$
//------------------------------------------------------------------------------
//                                SolverReal
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract task order 28
//
// Author: Darrel Conway
// Created: 2013/05/24
//
/**
 * Declares SolverReal class which provides base class for environment realated
 * Real Parameters
 */
//------------------------------------------------------------------------------
#ifndef SolverReal_hpp
#define SolverReal_hpp

#include "newparameter_defs.hpp"
#include "RealVar.hpp"
#include "SolverData.hpp"


class NEW_PARAMETER_API SolverReal : public RealVar, public SolverData
{
public:

   SolverReal(const std::string &name, const std::string &typeStr,
           GmatBase *obj, const std::string &desc,
           const std::string &unit, Gmat::ObjectType ownerType,
           GmatParam::DepObject depObj);
   SolverReal(const SolverReal &copy);
   SolverReal& operator=(const SolverReal &right);
   virtual ~SolverReal();

   // methods inherited from Parameter
   virtual Real EvaluateReal();
   
   virtual Integer GetNumRefObjects() const;
   virtual bool AddRefObject(GmatBase *obj, bool replaceName = false);
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

   virtual const std::string GetExternalCloneName(Integer whichOne);
   virtual void SetExternalClone(GmatBase *clone);

protected:
   
};

#endif // SolverReal_hpp
