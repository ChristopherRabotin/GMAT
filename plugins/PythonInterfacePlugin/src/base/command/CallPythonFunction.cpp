#include "CallPythonFunction.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// Static Data
//------------------------------------------------------------------------------
const std::string
CallPythonFunction::PARAMETER_TEXT[PythonFunctionParamCount - CallFunctionParamCount] =
{
      "PythonModule",
      "PythonFunction"
};


const Gmat::ParameterType
CallPythonFunction::PARAMETER_TYPE[PythonFunctionParamCount - CallFunctionParamCount] =
{
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE
};


CallPythonFunction::CallPythonFunction() :
   CallFunction      ("CallPythonFunction"),
   moduleName        (""),
   functionName      ("")
{
}

CallPythonFunction::CallPythonFunction(const CallPythonFunction& cpf) :
   CallFunction      (cpf),
   moduleName        (cpf.moduleName),
   functionName      (cpf.functionName)
{
}

CallPythonFunction& CallPythonFunction::operator =(
      const CallPythonFunction& cpf)
{
   if (this != &cpf)
   {
      CallFunction::operator=(cpf);
      moduleName   = cpf.moduleName;
      functionName = cpf.functionName;
   }

   return *this;
}

CallPythonFunction::~CallPythonFunction()
{
}

std::string CallPythonFunction::GetParameterText(const Integer id) const
{
   if (id >= CallFunctionParamCount && id < PythonFunctionParamCount)
      return PARAMETER_TEXT[id - CallFunctionParamCount];
   else
      return CallFunction::GetParameterText(id);
}

Integer CallPythonFunction::GetParameterID(const std::string& str) const
{
   for (Integer i = CallFunctionParamCount; i < PythonFunctionParamCount; ++i)
      if (str == PARAMETER_TEXT[i - CallFunctionParamCount])
         return i;

   return CallFunction::GetParameterID(str);
}

Gmat::ParameterType CallPythonFunction::GetParameterType(const Integer id) const
{
   if (id >= CallFunctionParamCount && id < PythonFunctionParamCount)
      return PARAMETER_TYPE[id - CallFunctionParamCount];
   else
      return CallFunction::GetParameterType(id);
}

std::string CallPythonFunction::GetParameterTypeString(const Integer id) const
{
   if (id >= CallFunctionParamCount && id < PythonFunctionParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - CallFunctionParamCount)];
   else
      return CallFunction::GetParameterTypeString(id);
}

bool CallPythonFunction::IsParameterReadOnly(const Integer id) const
{
   return CallFunction::IsParameterReadOnly(id);
}

bool CallPythonFunction::IsParameterReadOnly(const std::string& label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

std::string CallPythonFunction::GetStringParameter(const Integer id) const
{
   if (id == MODULENAME)
      return moduleName;
   if (id == FUNCTIONNAME)
      return functionName;

   return CallFunction::GetStringParameter(id);
}

bool CallPythonFunction::SetStringParameter(const Integer id, const char* value)
{
   if (id == MODULENAME)
   {
      moduleName = value;
      return true;
   }
   if (id == FUNCTIONNAME)
   {
      functionName = value;
      return true;
   }

   return CallFunction::SetStringParameter(id, value);
}

bool CallPythonFunction::SetStringParameter(const Integer id,
      const std::string& value)
{
   if (id == MODULENAME)
   {
      moduleName = value;
      return true;
   }
   if (id == FUNCTIONNAME)
   {
      functionName = value;
      mFunctionName = "Python." + moduleName + "." + functionName;
      return true;
   }

   return CallFunction::SetStringParameter(id, value);
}

//std::string CallPythonFunction::GetStringParameter(const Integer id,
//      const Integer index) const
//{
//   return CallFunction::GetStringParameter(id, index);
//}

//bool CallPythonFunction::SetStringParameter(const Integer id, const char* value,
//      const Integer index)
//{
//   return CallFunction::SetStringParameter(id, value, index);
//}

//bool CallPythonFunction::SetStringParameter(const Integer id,
//      const std::string& value, const Integer index)
//{
//   return CallFunction::GetStringParameter(id, value, index);
//}

std::string CallPythonFunction::GetStringParameter(
      const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}

bool CallPythonFunction::SetStringParameter(const std::string& label,
      const char* value)
{
   return SetStringParameter(GetParameterID(label), value);
}

bool CallPythonFunction::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//std::string CallPythonFunction::GetStringParameter(const std::string& label,
//      const Integer index) const
//{
//   return GetStringParameter(GetParameterID(label), index);
//}

//bool CallPythonFunction::SetStringParameter(const std::string& label,
//      const std::string& value, const Integer index)
//{
//   return SetStringParameter(GetParameterID(label), value, index);
//}

bool CallPythonFunction::Initialize()
{
	bool ret = false;

	ret = CallFunction::Initialize();

	pythonIf = PythonInterface::PyInstance();
	FileManager *fm = FileManager::Instance();
	
	//Initialize Python engine
	pythonIf->PyInitialize();

	// Get all Python module paths from the startup file
	StringArray paths = fm->GetAllPythonModulePaths();
	pythonIf->PyAddModulePath(paths);
   
   //Fill in Inputlist
   Integer sizeIn = FillInputList();
   
   //Fill in Outputlist
   Integer sizeOut = FillOutputList();

	return ret;
}

bool CallPythonFunction::Execute()
{
	return true;
}

void CallPythonFunction::RunComplete()
{
	return;
}


Integer CallPythonFunction::FillInputList()
{
   GmatBase *mapObj;

   mInputList.clear();
  
   StringArray ar = GetStringArrayParameter(ADD_INPUT);
   StringArray::iterator it;
   for (it = ar.begin(); it != ar.end(); ++it)
   {
      if ((mapObj = FindObject(*it)) == NULL)
      {
         throw CommandException("CallPythonFunction command cannot find Parameter " +
                                    *it + " in script line\n   \"" +
                                       GetGeneratingString(Gmat::SCRIPTING) + "\"");
      }
         
      mInputList.push_back((Parameter *)mapObj);
   }
      
   return mInputList.size();
}

Integer CallPythonFunction::FillOutputList()
{
   GmatBase *mapObj;

   mOutputList.clear();

   StringArray ar = GetStringArrayParameter(ADD_OUTPUT);
   StringArray::iterator it;
   for (it = ar.begin(); it != ar.end(); ++it)
   {
      if ((mapObj = FindObject(*it)) == NULL)
      {
         throw CommandException("CallPythonFunction command cannot find Parameter " +
            *it + " in script line\n   \"" +
            GetGeneratingString(Gmat::SCRIPTING) + "\"");
      }

      mOutputList.push_back((Parameter *)mapObj);
   }

   return mOutputList.size();
}