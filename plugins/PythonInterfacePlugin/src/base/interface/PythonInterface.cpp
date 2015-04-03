#include "stdlib.h"

#include "PythonInterface.hpp"


PythonInterface* PythonInterface::instance = NULL;

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
      if (plForm == "win")
         plF = ";";
	   else
		   plF = ":";
   }

}


void PythonInterface::PyAddModulePath(const StringArray& path)
{
   wchar_t *s3K = new wchar_t[512];
   char *s2K = new char[512];

   char *destPath = new char[512];
   char *p = new char[128];

#ifdef IS_PY3K
   s3K = Py_GetPath();
   //convert wchar_t to char
   wcstombs(destPath, s3K, sizeof(destPath));
   //concatenate the path delimiter (unix , windows and mac)
   strcat(destPath, plF);
#else
   s2K = Py_GetPath();
   strcpy(destPath, s2K);
   strcat(destPath, plF);
#endif

   StringArray::const_iterator it;
   for (it = path.begin(); it != path.end(); ++it)
   {
      p = (char *)it->c_str();
      strcat(destPath, p);
      strcat(destPath, plF); 
   }

   //convert wchar_t to char
#ifdef IS_PY3K
   mbtowc(s3K, destPath, sizeof(s3K));
   PySys_SetPath(s3K);
#else
   PySys_SetPath(destPath);
#endif

   delete[] destPath;
   delete[] s3K;
   delete[] s2K;
   delete[] p;

}