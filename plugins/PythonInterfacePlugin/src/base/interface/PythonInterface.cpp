#include "stdlib.h"

#include "PythonInterface.hpp"


PythonInterface* PythonInterface::instance = NULL;

PythonInterface::platform PythonInterface::plF = PythonInterface::platform::win;

PythonInterface* PythonInterface::PyInstance()
{
   if (instance == NULL)
	   instance = new PythonInterface();

   return instance;
}

PythonInterface::PythonInterface(): Interface("PythonInterface", NULL)
{
   isPythonInitialized = false;
   numPyCommands = 0;

}

PythonInterface::~PythonInterface()
{
}

PythonInterface::PythonInterface(const PythonInterface& pi) : Interface(pi)
{
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

Integer PythonInterface::PyPutRealArray()
{
   return 0;
}

Integer PythonInterface::PyGetRealArray()
{
   return 0;
}

void PythonInterface::PyPathSep()
{
   if (isPythonInitialized)
   {
	   const char* plForm = Py_GetPlatform();
	   if (plForm == "win")
		   plF = win;
	   else
		   plF = non_win;
   }

}


void PythonInterface::PyAddModulePath(const StringArray& path)
{
   wchar_t *s3K = NULL;
   char *s2K = NULL;

#ifdef IS_PY3K
	   s3K = Py_GetPath();
#else
   s2K = Py_GetPath();
#endif
   std::wstring s(s3K);

   StringArray::const_iterator it;
   for (it = path.begin(); it != path.end(); ++it)
   {

   }

}