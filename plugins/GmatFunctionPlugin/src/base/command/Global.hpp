//$Id$
//------------------------------------------------------------------------------
//                                 Global
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan
// Created: 2008.03.14
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CCA54C
//
/**
 * Class declaration for the Global command
 */
//------------------------------------------------------------------------------


#ifndef Global_hpp
#define Global_hpp

#include "GmatFunction_defs.hpp"
#include "ManageObject.hpp"


/**
 * Declaration of the Global command
 */
class GMATFUNCTION_API Global : public ManageObject
{
public:
   Global();
   virtual          ~Global();
   Global(const Global &gl);
   Global&        operator=(const Global &gl);
   
   virtual GmatBase*    Clone() const;
   
   bool                 Initialize();
   bool                 Execute();
   
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);

   DEFAULT_TO_NO_CLONES

protected:
   enum
   {
      GlobalParamCount = ManageObjectParamCount, 
   };
   //static const std::string PARAMETER_TEXT[GlobalParamCount - ManageObjectParamCount];

   //static const Gmat::ParameterType PARAMETER_TYPE[GlobalParamCount - ManageObjectParamCount];

};

#endif /* Global_hpp */
