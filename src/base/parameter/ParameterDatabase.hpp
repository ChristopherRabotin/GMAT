//$Header$
//------------------------------------------------------------------------------
//                                ParameterDatabase
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/09/18
//
/**
 * Declares operations for parameter database.
 */
//------------------------------------------------------------------------------
#ifndef ParameterDatabase_hpp
#define ParameterDatabase_hpp

#include "gmatdefs.hpp"
#include "paramdefs.hpp"
#include "Parameter.hpp"
#include "Rvector3.hpp"
#include "Rmatrix33.hpp"
#include "Cartesian.hpp"
#include "Keplerian.hpp"
#include "A1Mjd.hpp"
#include "Parameter.hpp"
//loj:#include "UtcDate.hpp"

class ParameterDatabase
{

public:

   ParameterDatabase();
   //loj: future build:
   //ParameterDatabase(ParameterPtrArray params);
   ~ParameterDatabase();

   Integer GetNumParameters() const;
   StringArray GetNamesOfParameters() const;
   StringArray GetDescsOfParameters() const;
   ParameterPtrArray GetParameters() const;

   bool HasParameter(const std::string &name) const;
   std::string GetDesc(const std::string &name) const;
   Integer GetParameterCount(const std::string &name) const;

   Parameter* GetParameter(const std::string &name) const;

   void Add(Parameter *param);
   void Remove(const Parameter *param);
   void Remove(const std::string &name);

protected:
private:

   StringParamPtrMap *mStringParamPtrMap;
   Integer mNumParams;

};
#endif // ParameterDatabase_hpp

