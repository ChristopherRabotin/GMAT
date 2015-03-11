#include "CallPythonFunction.hpp"

CallPythonFunction::CallPythonFunction() :
   CallFunction      ("CallPythonFunction")
{
}

CallPythonFunction::CallPythonFunction(const CallPythonFunction& cpf) :
   CallFunction      (cpf)
{
}

CallPythonFunction& CallPythonFunction::operator =(
      const CallPythonFunction& cpf)
{
   if (this != &cpf)
   {
      CallFunction::operator=(cpf);
   }

   return *this;
}

CallPythonFunction::~CallPythonFunction()
{
}
