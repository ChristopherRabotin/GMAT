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
      CREATIONS,
      CreateParamCount
   };
   static const std::string PARAMETER_TEXT[CreateParamCount - ManageObjectParamCount];

   static const Gmat::ParameterType PARAMETER_TYPE[CreateParamCount - ManageObjectParamCount];

   std::string objectType;
   ObjectArray creations; 
};

#endif /* Create_hpp */
