//$Header$
//------------------------------------------------------------------------------
//                         ExternalOptimizer
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy C. Shoan/GSFC
// Created: 2006.07.149
//
/**
 * Implementation for the external optimizer base class. 
 *
 */
//------------------------------------------------------------------------------


#include "ExternalOptimizer.hpp"
#include "MessageInterface.hpp"
//#include "MatlabInterface.hpp"  // currently all static



//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string
ExternalOptimizer::PARAMETER_TEXT[ExternalOptimizerParamCount -OptimizerParamCount] =
{
   "ParameterList",
   "SourceType",
};

const Gmat::ParameterType
ExternalOptimizer::PARAMETER_TYPE[ExternalOptimizerParamCount - OptimizerParamCount] =
{
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRING_TYPE,
};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

ExternalOptimizer::ExternalOptimizer(std::string type, std::string name) :
   Optimizer               (type, name), 
   sourceType              ("MATLAB"),
   sourceReady             (false),
   gmatServer              (NULL)
 {
   objectTypeNames.push_back("ExternalOptimizer");
   parameterCount = ExternalOptimizerParamCount;
}


ExternalOptimizer::~ExternalOptimizer()
{
   //FreeArrays();
}

ExternalOptimizer::ExternalOptimizer(const ExternalOptimizer &opt) :
   Optimizer               (opt),
   sourceType              (opt.sourceType),
   sourceReady             (false),
   gmatServer              (opt.gmatServer)
{
   parameterCount   = opt.parameterCount;
}


ExternalOptimizer& 
    ExternalOptimizer::operator=(const ExternalOptimizer& opt)
{
    if (&opt == this)
        return *this;

   Optimizer::operator=(opt);
   
   sourceType          = opt.sourceType;
   sourceReady         = opt.sourceReady;
   gmatServer          = opt.gmatServer;
   parmList            = opt.parmList;
   costConstraintArray = opt.costConstraintArray;
   parameterCount      = opt.parameterCount;
  
   return *this;
}

bool ExternalOptimizer::Initialize()
{
   Optimizer::Initialize();
   
   // what else goes in here?
   
   return true;
}

//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string ExternalOptimizer::GetParameterText(const Integer id) const
{
   if ((id >= OptimizerParamCount) && (id < ExternalOptimizerParamCount))
      return PARAMETER_TEXT[id - OptimizerParamCount];
   return Optimizer::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer ExternalOptimizer::GetParameterID(const std::string &str) const
{
   for (Integer i = OptimizerParamCount; i < ExternalOptimizerParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - OptimizerParamCount])
         return i;
   }

   return Optimizer::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ExternalOptimizer::GetParameterType(
                                              const Integer id) const
{
   if ((id >= OptimizerParamCount) && (id < ExternalOptimizerParamCount))
      return PARAMETER_TYPE[id - OptimizerParamCount];

   return Optimizer::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string ExternalOptimizer::GetParameterTypeString(
                                      const Integer id) const
{
   return Optimizer::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string ExternalOptimizer::GetStringParameter(const Integer id) const
{
    if (id == SOURCE_TYPE)
        return sourceType;
        
    return Optimizer::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  Integer SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method sets a string or string array parameter value, given the input
 * parameter ID.
 *
 * @param <id>    ID for the requested parameter.
 * @param <value> string value for the parameter.
 *
 * @return  The value of the parameter at the completion of the call.
 */
//------------------------------------------------------------------------------
bool ExternalOptimizer::SetStringParameter(const Integer id,
                                           const std::string &value)
{
    if (id == SOURCE_TYPE) 
    {
        sourceType = value;  // currently, only MATLAB allowed
        return true;
    }
        
    
    return Optimizer::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
//  std::string  GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 */
//------------------------------------------------------------------------------
const StringArray& ExternalOptimizer::GetStringArrayParameter(
                                                        const Integer id) const
{
        
    if (id == PARAMETER_LIST)
        return parmList;
        
    return Optimizer::GetStringArrayParameter(id);
}


