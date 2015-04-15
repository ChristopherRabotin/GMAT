#include "stdlib.h"

#include "PythonInterface.hpp"
#include "MessageInterface.hpp"

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

#ifdef IS_PY3K
   PyPathSep();
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
      p = (char *)it->c_str();
      strcat(destPath, p);
      strcat(destPath, plF);     
   }

   //convert char to wchar_t
#ifdef IS_PY3K
   mbtowc(s3K, destPath, sizeof(s3K));
   PySys_SetPath(s3K);
#else
   PySys_SetPath(destPath);
#endif

   delete[] destPath;
   delete[] s3K;
   delete[] p;

   MessageInterface::ShowMessage("Leaving PyAddModulePath( ) \n");

}