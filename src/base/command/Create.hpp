//$Header$
//------------------------------------------------------------------------------
//                                 Create
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Wendy C. Shoan
// Created: 2008.03.14
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CCA54C
//
/**
 * Class declaration for the Create command
 */
//------------------------------------------------------------------------------


#ifndef Create_hpp
#define Create_hpp

#include "ManageObject.hpp"


/**
 * Declaration of the Create command
 */
class Create : public ManageObject
{
public:
   Create();
   virtual          ~Create();
   Create(const Create &cr);
   Create&        operator=(const Create &cr);
   
   // Parameter access methods - overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   virtual GmatBase*    Clone() const;
   
   bool                 Initialize();
   bool                 Execute();

protected:
   enum
   {
      OBJECT_TYPE = ManageObjectParamCount, 
      CreateParamCount
   };
   static const std::string PARAMETER_TEXT[CreateParamCount - ManageObjectParamCount];

   static const Gmat::ParameterType PARAMETER_TYPE[CreateParamCount - ManageObjectParamCount];

   std::string objType;
   ObjectArray creations; 
};

#endif /* Create_hpp */
