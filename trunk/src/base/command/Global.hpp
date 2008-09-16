//$Header$
//------------------------------------------------------------------------------
//                                 Global
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
 * Class declaration for the Global command
 */
//------------------------------------------------------------------------------


#ifndef Global_hpp
#define Global_hpp

#include "ManageObject.hpp"


/**
 * Declaration of the Global command
 */
class Global : public ManageObject
{
public:
   Global();
   virtual          ~Global();
   Global(const Global &gl);
   Global&        operator=(const Global &gl);
   
   virtual GmatBase*    Clone() const;
   
   bool                 Initialize();
   bool                 Execute();

protected:
   enum
   {
      GlobalParamCount = ManageObjectParamCount, 
   };
   //static const std::string PARAMETER_TEXT[GlobalParamCount - ManageObjectParamCount];

   //static const Gmat::ParameterType PARAMETER_TYPE[GlobalParamCount - ManageObjectParamCount];

};

#endif /* Global_hpp */
