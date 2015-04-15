#ifndef PYTHONINTERFACE_HPP
#define PYTHONINTERFACE_HPP

#include <Python.h>
#include "pythoninterface_defs.hpp"
#include "Interface.hpp"
#include "CommandException.hpp"

#if PY_MAJOR_VERSION >= 3
#define IS_PY3K
#endif


class PYTHON_API PythonInterface : public Interface
{

public:
	
   /// Singleton class
   static PythonInterface* PyInstance();

   /// Load Python engine with all built-in modules
   bool PyInitialize();
   /// Unload Python modules
   bool PyFinalize();
   /// Put Real Array
   Integer PyPutRealArray();
   /// Get Real Array
   Integer PyGetRealArray();
   /// Add Module path to sys.path
   void PyAddModulePath(const StringArray& path);
	
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

private:
   static PythonInterface *instance;
	
   PythonInterface(const std::string &name);
   ~PythonInterface();
   PythonInterface(const PythonInterface &pi);
   PythonInterface& operator=(const PythonInterface &pi);

   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);

   /// Is Python engine loaded
   bool isPythonInitialized;
   /// Number of Python command referenced in Gmat script
   Integer numPyCommands;
   /// Identify the platform we are running
   char *plF;
   void PyPathSep();
	
};

#endif