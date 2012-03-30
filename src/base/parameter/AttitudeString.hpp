//$Id$
//------------------------------------------------------------------------------
//                                AttitudeString
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
// Created: 2006/03/09
//
/**
 * Declares AttitudeString class which provides base class for time realated String
 * Parameters
 */
//------------------------------------------------------------------------------
#ifndef AttitudeString_hpp
#define AttitudeString_hpp

#include "gmatdefs.hpp"
#include "StringVar.hpp"
#include "AttitudeData.hpp"


class GMAT_API AttitudeString : public StringVar, public AttitudeData
{
public:

   AttitudeString(const std::string &name, const std::string &typeStr, 
              GmatBase *obj, const std::string &desc, const std::string &unit,
              bool isSettable = false);
   AttitudeString(const AttitudeString &copy);
   AttitudeString& operator=(const AttitudeString &right);
   virtual ~AttitudeString();

   // methods inherited from Parameter
   virtual const std::string& EvaluateString();
   
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
   virtual const std::string&
                    GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                        const std::string &prefix = "",
                                        const std::string &useName = "");
protected:
   

};

#endif // AttitudeString_hpp
