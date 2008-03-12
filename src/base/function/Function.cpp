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


#include "Function.hpp"
#include "FileManager.hpp"       // for GetPathname()
#include "StringUtil.hpp"        // for Trim()

//#define DEBUG_FUNCTION
#ifdef DEBUG_FUNCTION
#include "MessageInterface.hpp"
#endif

//---------------------------------
// static data
//---------------------------------
const std::string
Function::PARAMETER_TEXT[FunctionParamCount - GmatBaseParamCount] =
{
   "FunctionPath",
};

const Gmat::ParameterType
Function::PARAMETER_TYPE[FunctionParamCount - GmatBaseParamCount] =
{
   Gmat::STRING_TYPE,
};


//------------------------------------------------------------------------------
//  Function(std::string typeStr, std::string nomme)
//------------------------------------------------------------------------------
/**
 * Constructs the Function object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <nomme>   Name for the object
 */
//------------------------------------------------------------------------------
Function::Function(const std::string &typeStr, const std::string &nomme) :
    GmatBase        (Gmat::FUNCTION, typeStr, nomme),
    functionPath      ("")
{
   objectTypes.push_back(Gmat::FUNCTION);
   objectTypeNames.push_back("Function");
   parameterCount = FunctionParamCount;
   // Functions are Global by default
   isGlobal = true;

   // function path
   FileManager *fm = FileManager::Instance();
   std::string pathname;
   
   try
   {
      if (functionPath == "")
      {         
         if (typeStr == "MatlabFunction")
            // matlab uses directory path
            pathname = fm->GetFullPathname("MATLAB_FUNCTION_PATH");
         else if (typeStr == "GmatFunction")
            // gmat function uses whole path name
            pathname = fm->GetFullPathname("GMAT_FUNCTION_PATH") +nomme +".gmf";
         
         functionPath = pathname;
      }
   }
   catch (GmatBaseException &e)
   {
      #ifdef DEBUG_FUNCTION
      MessageInterface::ShowMessage(e.GetFullMessage());
      #endif
      
      try
      {
         // see if there is FUNCTION_PATH
         pathname = fm->GetFullPathname("FUNCTION_PATH");
         functionPath = pathname;
      }
      catch (GmatBaseException &e)
      {
         #ifdef DEBUG_FUNCTION
         MessageInterface::ShowMessage(e.GetFullMessage());
         #endif
      }
   }
}


//------------------------------------------------------------------------------
//  ~Function(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Function object (destructor).
 */
//------------------------------------------------------------------------------
Function::~Function()
{
}


//------------------------------------------------------------------------------
//  Function(const Function &f)
//------------------------------------------------------------------------------
/**
 * Constructs the Function object (copy constructor).
 * 
 * @param <f> Object that is copied
 */
//------------------------------------------------------------------------------
Function::Function(const Function &f) :
    GmatBase        (f),
    functionPath    (f.functionPath)
{
    parameterCount = FunctionParamCount;
}


//------------------------------------------------------------------------------
//  Function& operator=(const Function &f)
//------------------------------------------------------------------------------
/**
 * Sets one Function object to match another (assignment operator).
 * 
 * @param <f> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
Function& Function::operator=(const Function &f)
{
    if (this == &f)
        return *this;
        
    GmatBase::operator=(f);

    functionPath  = f.functionPath;

    return *this;
}


//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the name of the parameter with the input id.
 * 
 * @param <id> Integer id for the parameter.
 * 
 * @return The string name of the parameter.
 */
//------------------------------------------------------------------------------
std::string Function::GetParameterText(const Integer id) const
{
    if (id >= FUNCTION_PATH && id < FunctionParamCount)
        return PARAMETER_TEXT[id - GmatBaseParamCount];
    else
        return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Gets the id corresponding to a named parameter.
 * 
 * @param <str> Name of the parameter.
 * 
 * @return The ID.
 */
//------------------------------------------------------------------------------
Integer Function::GetParameterID(const std::string &str) const
{
    for (Integer i = FUNCTION_PATH; i < FunctionParamCount; i++)
    {
        if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
            return i;
    }

    return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the type of a parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The type of the parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Function::GetParameterType(const Integer id) const
{
   if (id >= FUNCTION_PATH&& id < FunctionParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the text description for the type of a parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The text description of the type of the parameter.
 */
//------------------------------------------------------------------------------
std::string Function::GetParameterTypeString(const Integer id) const
{
   if (id >= FUNCTION_PATH&& id < FunctionParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return GmatBase::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a std::string parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
std::string Function::GetStringParameter(const Integer id) const
{
   if (id == FUNCTION_PATH)
      return functionPath;
   return GmatBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * 
 * @return If value of the parameter was set.
 */
//------------------------------------------------------------------------------
bool Function::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == FUNCTION_PATH)
   {
      // Compose full path if it has relative path.
      // Assuming if first char has '.', it has relative path.
      std::string temp = GmatStringUtil::Trim(value);
      if (temp[0] == '.')
      {
         FileManager *fm = FileManager::Instance();
         std::string currPath = fm->GetCurrentPath();
         
         #ifdef DEBUG_FUNCTION
         MessageInterface::ShowMessage
            ("Function::SetStringParameter() currPath=%s\n",
             currPath.c_str());
         #endif
         
         functionPath = currPath + temp.substr(1);
      }
      else
      {
         functionPath = value;
      }

      #ifdef DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("   functionPath = %s\n", functionPath.c_str());
      #endif
      
      return true;
   }

   return GmatBase::SetStringParameter(id, value);
}

