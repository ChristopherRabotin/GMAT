//$Header$
//------------------------------------------------------------------------------
//                                Variable
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/09/15
//
/**
 * Declares Variable class which provides methods for general variable evaluation.
 *    Such as: 100 * Sat1.X; where Sat1.X is already defined
 *             Sat1.X + Sat1.X;
 *             A * B; where A and B are already defined
 */
//------------------------------------------------------------------------------
#ifndef Variable_hpp
#define Variable_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "ParameterDatabase.hpp"
#include "ExpressionParser.hpp"

class GMAT_API Variable : public RealVar
{
public:

   Variable(const std::string &name, const std::string &desc = "",
            const std::string &unit = "");
   Variable(const Variable &copy);
   Variable& operator=(const Variable &right);
   virtual ~Variable();

   // methods inherited from RealVar
   virtual Real GetReal();
   virtual Real EvaluateReal();
   
   // methods inherited from Parameter
   virtual const std::string* GetParameterList() const;
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;
   virtual bool SetRefObjectName(const Gmat::ObjectType type,
                                 const std::string &name);
   virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name);
   virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name = "");
   
   virtual const StringArray& GetStringArrayParameter(const Integer id) const; 
   virtual const StringArray& GetStringArrayParameter(const std::string &label) const;

protected:

   ParameterDatabase *mParamDb;
   ExpressionParser *mExpParser;
   
   enum
   {
      REF_PARAMS = RealVarParamCount,
      VariableParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[VariableParamCount - RealVarParamCount];
   static const std::string
      PARAMETER_TEXT[VariableParamCount - RealVarParamCount];
   
};

#endif // Variable_hpp
