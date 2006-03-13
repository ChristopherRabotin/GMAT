//$Header$
//------------------------------------------------------------------------------
//                                TimeString
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2006/03/09
//
/**
 * Declares TimeString class which provides base class for time realated String
 * Parameters
 */
//------------------------------------------------------------------------------
#ifndef TimeString_hpp
#define TimeString_hpp

#include "gmatdefs.hpp"
#include "StringVar.hpp"
#include "TimeData.hpp"


class GMAT_API TimeString : public StringVar, public TimeData
{
public:

   TimeString(const std::string &name, const std::string &typeStr, 
            GmatBase *obj, const std::string &desc, const std::string &unit);
   TimeString(const TimeString &copy);
   TimeString& operator=(const TimeString &right);
   virtual ~TimeString();

   // methods inherited from Parameter
   virtual std::string EvaluateString();
   
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

#endif // TimeString_hpp
