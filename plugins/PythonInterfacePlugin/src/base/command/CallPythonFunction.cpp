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
   functionName      (""),
   pythonIf          (NULL)
{
}

CallPythonFunction::CallPythonFunction(const CallPythonFunction& cpf) :
   CallFunction      (cpf),
   moduleName        (cpf.moduleName),
   functionName      (cpf.functionName),
   pythonIf          (cpf.pythonIf)
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
      pythonIf = cpf.pythonIf;
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

   MessageInterface::ShowMessage("  Calling CallPythonFunction::Initialize()\n");

   ret = CallFunction::Initialize();

   MessageInterface::ShowMessage("  Base class init complete\n");

	pythonIf = PythonInterface::PyInstance();
   FileManager *fm = FileManager::Instance();

   MessageInterface::ShowMessage("  pythonIf:  %p\n", pythonIf);
   
	//Initialize Python engine
	pythonIf->PyInitialize();

	// Get all Python module paths from the startup file
	StringArray paths = fm->GetAllPythonModulePaths();

   MessageInterface::ShowMessage("  Adding %d python paths\n", paths.size());

	pythonIf->PyAddModulePath(paths);
   
   //Fill in Inputlist
   Integer sizeIn = FillInputList();
   MessageInterface::ShowMessage("  SizeIn is %d\n", sizeIn);

   //Fill in Outputlist
   Integer sizeOut = FillOutputList();
   MessageInterface::ShowMessage("  SizeOut is %d\n", sizeOut);

	return ret;
}

bool CallPythonFunction::Execute()
{
   // send the in parameters
   std::string formatIn("");
   std::vector<void *> argIn;

   // Prepare the format string specifier (const char *format) to build 
   // Python object.
   SendInParam(mInputList, formatIn, argIn);
  // Next call Python function Wrapper
   PyObject* pyRet = pythonIf->PyFunctionWrapper(moduleName, functionName, formatIn, argIn);
   Real ret = 0;

   if (pyRet && PyFloat_Check(pyRet))
   {
      ret = PyFloat_AsDouble(pyRet);
      MessageInterface::ShowMessage("  ret:  %f\n", ret);
      Py_DECREF(pyRet);
   }

   MessageInterface::ShowMessage("  pyRet:  %p\n", pyRet); 

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
         throw CommandException("   CallPythonFunction command cannot find Parameter " +
                                       *it + " in script line\n   \"" +
                                          GetGeneratingString(Gmat::SCRIPTING) + "\"");
      }
         
      if (mapObj->IsOfType(Gmat::PARAMETER))
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
         throw CommandException("   CallPythonFunction command cannot find Parameter " +
                                       *it + " in script line\n   \"" +
                                          GetGeneratingString(Gmat::SCRIPTING) + "\"");
      }

      if (mapObj->IsOfType(Gmat::PARAMETER))
         mOutputList.push_back((Parameter *)mapObj);
   }

   return mOutputList.size();
}


void CallPythonFunction::SendInParam(const std::vector<Parameter *> InputList, std::string &formatIn, std::vector<void *> &argIn)
{
   for (unsigned int i = 0; i < mInputList.size(); i++)
   {
      Parameter *param = mInputList[i];
      Gmat::ParameterType type = param->GetReturnType();
      
      switch (type)
      {
         case Gmat::REAL_TYPE:
         {
            if (i == 0)
               formatIn.append("(f");
            else
               formatIn.append("f");
        
            Real r = param->EvaluateReal();
            argIn.push_back(&r);

            break;
         }
         case Gmat::STRING:
         {
            if (i == 0)
               formatIn.append("(s");
            else
               formatIn.append("s");

            std::string s = param->EvaluateString();
            argIn.push_back((char*)s.c_str());

            break; 
         }
         case Gmat::ARRAY:
            break;
      }
   }

   formatIn.append(")");

}