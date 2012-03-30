//$Id$
//------------------------------------------------------------------------------
//                                  AttitudeReal
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
// Author: Daniel Hunter
// Created: 2006/6/26
//
/**
 * Declares Attitude real data class.
 */
//------------------------------------------------------------------------------
#ifndef ATTITUDEREAL_HPP_
#define ATTITUDEREAL_HPP_

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "AttitudeData.hpp"


class GMAT_API AttitudeReal : public RealVar, public AttitudeData
{
public:

   AttitudeReal(const std::string &name, const std::string &typeStr, 
                GmatBase *obj, const std::string &desc, const std::string &unit,
                bool isSettable = true);
   AttitudeReal(const AttitudeReal &copy);
   AttitudeReal& operator=(const AttitudeReal &right);
   virtual ~AttitudeReal();
   
   // methods inherited from Parameter
   virtual Real EvaluateReal();
   
   virtual Integer GetNumRefObjects() const;
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


#endif /*ATTITUDEREAL_HPP_*/
