//$Header$
//------------------------------------------------------------------------------
//                                  StringVar
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/1/7
//
/**
 * Declares StringVar class which handles std::string value. The string value
 * is stored in Parameter::mExpr.
 */
//------------------------------------------------------------------------------
#ifndef StringVar_hpp
#define StringVar_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"

class GMAT_API StringVar : public Parameter
{
public:

   StringVar(const std::string &name = "", const std::string &desc = "");
   StringVar(const StringVar &copy);
   StringVar& operator= (const StringVar& right);
   virtual ~StringVar();

   bool operator==(const StringVar &right) const;
   bool operator!=(const StringVar &right) const;

   // methods inherited from Parameter
   virtual std::string ToString();
   virtual const std::string& GetString() const;
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   
protected:
    
private:

};
#endif // StringVar_hpp
