//$Header$
//------------------------------------------------------------------------------
//                                  Parameter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/09/16
//
/**
 * Declares base class of parameters.
 */
//------------------------------------------------------------------------------
#ifndef Parameter_hpp
#define Parameter_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"

class GMAT_API Parameter : public GmatBase
{
public:

   enum ParameterKey
   {
      SYSTEM_PARAM, USER_PARAM, KeyCount
   };

   Parameter(const std::string &name, const std::string &typeStr,
             ParameterKey key, const std::string &desc);
   Parameter(const Parameter &param);
   Parameter& operator= (const Parameter& right);
   virtual ~Parameter();

   ParameterKey GetParameterKey() const;
   std::string GetParameterDesc() const;

   void SetDesc(const std::string &desc);

   bool operator==(const Parameter &right) const;
   bool operator!=(const Parameter &right) const;

   virtual const std::string* GetParameterList() const;

protected:
   static const std::string PARAMETER_KEY_STRING[KeyCount];

   ParameterKey  mKey;
   std::string   mDesc;

private:

};
#endif // Parameter_hpp

