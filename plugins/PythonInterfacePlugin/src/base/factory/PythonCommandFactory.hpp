/**
 * Factory class that creates the plugin scriptable objects
 */

#include "pythoninterface_defs.hpp"
#include "Factory.hpp"

class PYTHON_API PythonCommandFactory : public Factory
{
public:
   PythonCommandFactory();
   virtual ~PythonCommandFactory();
   PythonCommandFactory(const PythonCommandFactory& elf);
   PythonCommandFactory& operator=(const PythonCommandFactory& elf);

   virtual GmatCommand*     CreateCommand(const std::string &ofType,
                                          const std::string &withName = "");
};
