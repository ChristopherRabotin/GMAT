//$Header$
//------------------------------------------------------------------------------
//                                   Function
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Defines the Funtion base class used for Matlab and Gmat functions.
 */
//------------------------------------------------------------------------------


#ifndef Function_hpp
#define Function_hpp


#include "GmatBase.hpp"
#include "FunctionException.hpp"

/**
 * All function classes are derived from this base class.
 */
class Function : public GmatBase
{
public:
   Function(const std::string &typeStr, const std::string &nomme);
   virtual ~Function();
   Function(const Function &f);
   Function&                   operator=(const Function &f);
    
    // Inherited (GmatBase) methods
    virtual GmatBase* Clone(void) const;
    virtual std::string     GetParameterText(const Integer id) const;
    virtual Integer         GetParameterID(const std::string &str) const;
    virtual Gmat::ParameterType
                            GetParameterType(const Integer id) const;
    virtual std::string     GetParameterTypeString(const Integer id) const;
    virtual std::string     GetStringParameter(const Integer id) const;
    virtual bool            SetStringParameter(const Integer id, 
                                           const std::string &value);

protected:
    /// Path for function script
    std::string functionPath;

    enum
    {
      FUNCTION_PATH = GmatBaseParamCount,
      FunctionParamCount  /// Count of the parameters for this class
    };

    static const std::string PARAMETER_TEXT[FunctionParamCount - GmatBaseParamCount];
    static const Gmat::ParameterType PARAMETER_TYPE[FunctionParamCount - GmatBaseParamCount];

};


#endif // Function_hpp
