#include "PythonCommandFactory.hpp"
#include "CallPythonFunction.hpp"

//------------------------------------------------------------------------------
// PythonCommandFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * Replace the type Gmat::COMMAND in the base class call with the factory type
 * you need.
 */
//------------------------------------------------------------------------------
PythonCommandFactory::PythonCommandFactory() :
   Factory           (Gmat::COMMAND)
{
   if (creatables.empty())
   {
      creatables.push_back("CallPythonFunction");
   }
}


//------------------------------------------------------------------------------
// ~PythonCommandFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
PythonCommandFactory::~PythonCommandFactory()
{
}


//------------------------------------------------------------------------------
// PythonCommandFactory(const PythonCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
PythonCommandFactory::PythonCommandFactory(const PythonCommandFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("CallPythonFunction");
   }
}


//------------------------------------------------------------------------------
// PythonCommandFactory& operator=(const PythonCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
PythonCommandFactory& PythonCommandFactory::operator=(
      const PythonCommandFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         // Replace the SampleClass string here with your class name.  For multiple
         // classes of the same type, push back multiple names here
         creatables.push_back("CallPythonFunction");
      }
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(const std::string &ofType,
//       const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creation method for GMAT commands
 *
 * @param ofType The subtype of the command
 * @param withName The new command's name
 *
 * @return A newly created GmatCommand (or NULL if this factory doesn't create
 *         the requested type)
 */
//------------------------------------------------------------------------------
GmatCommand* PythonCommandFactory::CreateCommand(const std::string &ofType,
                                     const std::string &withName)
{
   if (ofType == "CallPythonFunction")
      return new CallPythonFunction();
   // add more here .......

   return NULL;   // doesn't match any type of Command known by this factory
}
