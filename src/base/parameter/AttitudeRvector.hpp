//$Id$
//------------------------------------------------------------------------------
//                                  AttitudeRvector
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2014/02/10
//
/**
 * Declares Attitude real data class.
 */
//------------------------------------------------------------------------------
#ifndef ATTITUDERVECTOR_HPP_
#define ATTITUDERVECTOR_HPP_

#include "gmatdefs.hpp"
#include "RvectorVar.hpp"
#include "AttitudeData.hpp"


class GMAT_API AttitudeRvector : public RvectorVar, public AttitudeData
{
public:

   AttitudeRvector(const std::string &name, const std::string &typeStr, 
                   GmatBase *obj, const std::string &desc, const std::string &unit,
                   bool isSettable, Integer size);
   AttitudeRvector(const AttitudeRvector &copy);
   AttitudeRvector& operator=(const AttitudeRvector &right);
   virtual ~AttitudeRvector();
   
   // methods inherited from Parameter
   virtual const Rvector& EvaluateRvector();
   
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


#endif /*ATTITUDERVECTOR_HPP_*/
