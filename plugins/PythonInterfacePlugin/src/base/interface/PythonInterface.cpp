#include "stdlib.h"

#include "PythonInterface.hpp"
#include "MessageInterface.hpp"
#include <iostream>
PythonInterface* PythonInterface::instance = NULL;

PythonInterface* PythonInterface::PyInstance()
{
   if (instance == NULL)
	   instance = new PythonInterface("PythonInterface");

   return instance;
}

PythonInterface::PythonInterface(const std::string &name) : Interface("PythonInterface", name)
{
   isPythonInitialized = false;
   numPyCommands = 0;
   plF = new char[2];
   memset(plF, 0, 2);
}

PythonInterface::~PythonInterface()
{
   delete[] plF;
}

PythonInterface::PythonInterface(const PythonInterface& pi) : Interface(pi)
{
   plF = new char[2];
   memcpy(plF, pi.plF, 2);
}


//------------------------------------------------------------------------------
// PythonInterface& operator=(const PythonInterface& pi)
//------------------------------------------------------------------------------
PythonInterface& PythonInterface::operator=(const PythonInterface& pi)
{
   if (&pi != this)
   {
	   Interface::operator=(pi);
      numPyCommands = pi.numPyCommands;
      isPythonInitialized = pi.isPythonInitialized;
      plF = pi.plF;
   }

   return *this;
}

void PythonInterface::Copy(const GmatBase* orig)
{
   operator=(*((PythonInterface *)(orig)));
}

GmatBase* PythonInterface::Clone() const
{
   return (new PythonInterface(*this));
}

bool PythonInterface::PyInitialize()
{
   // Initialize Python only once.
   if (!isPythonInitialized)
	   Py_Initialize();

   if (Py_IsInitialized())
   {
	   isPythonInitialized = true;
	   numPyCommands++;
   }
   else
   {
	   throw CommandException ("Python is not installed or loaded properly.");
   }
		
   return isPythonInitialized;
}

bool PythonInterface::PyFinalize()
{
   // when all the Python commands in Gmat script is run and completed
   // close and finalize the Python.
   if (--numPyCommands == 0)
   {
	   Py_Finalize();
	   isPythonInitialized = false;
   }
	
   return isPythonInitialized;
}


void PythonInterface::PyPathSep()
{
   if (isPythonInitialized)
   {
	   const char* plForm = Py_GetPlatform();
      std::string str(plForm);
   
      if (str.find("win") != std::string::npos)
         plF = ";";
	   else
		   plF = ":";
   }

}


void PythonInterface::PyAddModulePath(const StringArray& path)
{
   wchar_t *s3K = new wchar_t[8192];
   char *destPath = new char[8192];
   char *p = new char[128];

   // Platform path seperator delimiter
   PyPathSep();

#ifdef IS_PY3K
   //convert wchar_t to char
   wcstombs(destPath, Py_GetPath(), 8192);
   //concatenate the path delimiter (unix , windows and mac)
   strcat(destPath, plF);
#else
   strcpy(destPath, Py_GetPath());
   strcat(destPath, plF);
#endif

   StringArray::const_iterator it;
   for (it = path.begin(); it != path.end(); ++it)
   {
      strcpy(p, it->c_str());
      strcat(destPath, p);
      strcat(destPath, plF);     
   }
 
#ifdef IS_PY3K
   //convert char to wchar_t
   mbstowcs(s3K, destPath, 8192);
   PySys_SetPath(s3K);
#else
   PySys_SetPath(destPath);
#endif

   delete[] destPath;
   delete[] s3K;
   delete[] p;

   MessageInterface::ShowMessage("Leaving PyAddModulePath( ) \n");

}

PyObject* PythonInterface::PyFunctionWrapper(const std::string &modName, const std::string &funcName,
                                                const std::string &formatIn, const std::vector<void *> &argIn)
{
   PyObject* pyRet = NULL;
   PyObject* pyModule;
   PyObject* pyPluginModule;
   
#ifdef IS_PY3K
   // create a python Unicode object from an UTF-8 encoded null terminated char buffer
   pyModule = PyUnicode_FromString(modName.c_str());
#else
   pyModule = PyBytes_FromString(modName.c_str());
#endif
  
   // import the python module
   pyPluginModule = PyImport_Import(pyModule);
   Py_DECREF(pyModule);
   if (!pyPluginModule)
   {
      PyErr_Print();

   }


   return pyRet;
}