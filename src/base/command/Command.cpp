//$Header$
//------------------------------------------------------------------------------
//                                  Command
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/23
//
/**
 * Implementation code for the Command base class
 */
//------------------------------------------------------------------------------


#include "Command.hpp"          // class's header file
#include "Publisher.hpp"        // For the publisher


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  Command(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Constructs Command core structures (default constructor).
 */
//------------------------------------------------------------------------------
Command::Command(const std::string &typeStr) :
    GmatBase        (Gmat::COMMAND, typeStr),
    generatingString(""),
    initialized     (false),
    next            (NULL),
    level           (-1),   // Not set
    objectMap       (NULL),
    solarSys        (NULL),
    publisher       (NULL)
{
}


//------------------------------------------------------------------------------
//  ~Command()
//------------------------------------------------------------------------------
/**
 * Destructs Command core structures.
 *
 * The Command destructor calls the destructor for the next command in the 
 * sequence.  Therefore, when a command sequence is cleared, all the Sandbox
 * needs to do is call the destructor on the first command in the sequence, and
 * the remaining commands are all destructed.  
 *
 * @note The branching commands will also destroy all of the commands in the 
 *       subbranch(es)
 */
//------------------------------------------------------------------------------
Command::~Command()
{
    // Delete the subsequent commands
    if (next)
        delete next;   
}


//------------------------------------------------------------------------------
//  Command(const Command &c)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * The copy constructor for commands copies the object lists and associations
 * from the "original" command, c, but not its navigation parameters (next, 
 * level), nor does it initialize the command.
 *
 * @param <c> Command object whose values to use to make "this" copy.
 */
//------------------------------------------------------------------------------
Command::Command(const Command &c) :
    GmatBase        (c),
    generatingString(c.generatingString),
    association     (c.association),
    objects         (c.objects),
    initialized     (false),
    next            (NULL),
    level           (-1),   // Not set
    objectMap       (c.objectMap),
    solarSys        (c.solarSys),
    publisher       (c.publisher)
{
}


//------------------------------------------------------------------------------
//  Command operator=(const Command &c)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * The assignment operator for commands copies the object lists and associations
 * from the "other" command, c, but not its navigation parameters (next, level).
 * This operator clears all object associations, and leaves "this" object in an 
 * uninitialized state.
 *
 * @param <c> Command object whose values to use to set "this" Command object.
 *
 * @return Command object.
 */
//------------------------------------------------------------------------------
Command& Command::operator=(const Command &c)
{
    if (&c == this)
        return *this;

    initialized = false;
    objects.clear();
    association.clear();
    ClearObjects();             // Drop any previously set object pointers

    objects = c.objects;
    association = c.association;

    objectMap = c.objectMap;
    solarSys = c.solarSys;
    publisher = c.publisher;
    generatingString = c.generatingString;

    return *this;
}


//------------------------------------------------------------------------------
//  void SetGeneratingString(const std::string &gs)
//------------------------------------------------------------------------------
/**
 * Method used to store the string that was parsed to build this command.
 *
 * This method is used to store the command string so that the script can be
 * written to a file without inverting the steps used to set up the internal
 * object data.
 *
 * @param <gs> The script line that was interpreted to define this command.
 */
//------------------------------------------------------------------------------
void Command::SetGeneratingString(const std::string &gs)
{
    generatingString = gs;
}

//------------------------------------------------------------------------------
//  const std::string GetGeneratingString(void)
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this command.
 *
 * This method is used to retrieve the command string from the script that was
 * parsed to build the command.  It is used to save the script line, so that
 * the script can be written to a file without inverting the steps taken to set
 * up the internal object data.  As a side benefit, the script line is
 * available in the command structure for debugging purposes.
 *
 * @return The script line that was interpreted to define this command.
 */
//------------------------------------------------------------------------------
const std::string& Command::GetGeneratingString(void)
{
    return generatingString;
}


//------------------------------------------------------------------------------
//  bool SetObject(const std::string &name, const Gmat::ObjectType type,
//                 const std::string &associate, 
//                 const Gmat::ObjectType associateType)
//------------------------------------------------------------------------------
/**
 * Stores the object names and types required for the command.
 *
 * Objects associated with a command are not necessarily available at the time
 * the command is parsed from the script.  This command builds the data store of
 * object names that will be used to execute the command when the script is run.
 * 
 * @param <name> The name of the object.
 * @param <type> The (enumerated) base type assigned to the object.
 * @param <associate> Optional second object associated with the named object.
 *                    Defaults to the empty string ("").
 * @param <associateType> Type for the second object; defaults to
 *                        Gmat::UNKNOWN_OBJECT.
 *
 * @return true object is stored successfully, or it it is already registered
 *         with the same type, false otherwise.
 *
 * @note associate and associateType are provided to allow derived classes to
 *       override this method for commands that link two types of objects
 *       together -- for example, the Propagate command links a propagator
 *       setup to a spacecraft.  These parameters are not used in this default
 *       implementation.
 */
//------------------------------------------------------------------------------
bool Command::SetObject(const std::string &name, const Gmat::ObjectType type,
                        const std::string &associate,
                        const Gmat::ObjectType associateType)
{
    // Check to see if it is already in the object list
    StringArray::iterator item = std::find(objects.begin(), objects.end(), name);
    if (item != objects.end()) 
    {
        if (type != association[*item])  // make sure the types match
            return false;
    }
    else
    {
        // Add it to the lists
        objects.push_back(name);
        association[name] = type; 
    }

    return true;
}


//------------------------------------------------------------------------------
//  GmatBase* GetObject(const Gmat::ObjectType type, const std::string objName)
//------------------------------------------------------------------------------
/**
 * Retrieves the objects used by the command.
 *
 * Objects associated with a command are not necessarily available at the time
 * the command is parsed from the script.  This command builds the data store of
 * object names that will be used to execute the command when the script is run.
 * 
 * @param <type>    The (enumerated) base type assigned to the object.
 * @param <objName> The name of the object.
 *
 * @return A pointer to the requested object, obtained from the Configuration 
 *         Manager if it is a configured object, or from the local pointer if it
 *         is an internal object.
 */
//------------------------------------------------------------------------------
GmatBase* Command::GetObject(const Gmat::ObjectType type, 
                             const std::string objName)
{
    return NULL;
}


//------------------------------------------------------------------------------
//  bool SetObject(GmatBase *obj, const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Stores pointers to objects required exclusively for the current command.
 *
 * Some commands require helper objects to execute successfully -- for example,
 * the propagate command uses a StoppingCondition object to evaluate when the
 * command finishes execution.  Use this method to assign these command
 * specific objects.
 *
 * @param <obj> Pointer to the object.
 * @param <type> The (enumerated) base type assigned to the object.
 *
 * @return true when object is stored successfully, false otherwise.
 */
//------------------------------------------------------------------------------
bool Command::SetObject(GmatBase *obj, const Gmat::ObjectType type)
{
    return false;
}


//------------------------------------------------------------------------------
//  void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox to set the local solar system for the command
 * 
 * @param <ss> Pointer to the local solar system
 */
//------------------------------------------------------------------------------
void Command::SetSolarSystem(SolarSystem *ss)
{
    solarSys = ss;
}


//------------------------------------------------------------------------------
//  void SetAssetMap(std::map<std::string, Asset *> *map)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox to set the local asset store used by the Command
 * 
 * @param <map> Pointer to the local asset map
 */
//------------------------------------------------------------------------------
void Command::SetObjectMap(std::map<std::string, GmatBase *> *map)
{
    objectMap = map;
}


//------------------------------------------------------------------------------
//  void SetPublisher(Publisher *p)
//------------------------------------------------------------------------------
/**
 * Sets the Publisher used for data generated by the Command
 * 
 * @param <p> Pointer to the Publisher
 */
//------------------------------------------------------------------------------
void Command::SetPublisher(Publisher *p)
{
    publisher = p;
}


//------------------------------------------------------------------------------
//  bool Initialize(void)
//------------------------------------------------------------------------------
/**
 * Initializes the Command structures at the start of a run.
 *
 * This method is called by the Sandbox when it receives a run command from the 
 * Moderator.  This method calls AssignObjects to set the local object pointers
 * prior to execution.
 *
 * Derived Command objects can use this method to set up any internal data 
 * structures needed to run the command.  If this method is overridden, the base
 * class method provided here should be called to ensure that AssignObjects is
 * called at the start of the run.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Command::Initialize(void)
{
    // Check to be sure the basic infrastructure is in place
    if (objectMap == NULL) {
        std::string errorstr("Object map has not been initialized for ");
        errorstr += GetTypeName();
        throw CommandException(errorstr);
    }
  
// Uncomment the following when we have a solar system to initialize
//    if (solarSys == NULL) {
//        std::string errorstr("Solar system has not been initialized for ");
//        errorstr += GetTypeName();
//        throw CommandException(errorstr);
//    }


    initialized = AssignObjects();
    if (publisher == NULL)
        publisher = Publisher::Instance();
    return initialized;
}


//------------------------------------------------------------------------------
//  Command* GetNext(void)
//------------------------------------------------------------------------------
/**
 * Accesses the next command to be executed in the command sequence
 * 
 * @return Pointer to the next command
 */
//------------------------------------------------------------------------------
Command* Command::GetNext(void)
{
    return next;
}


//------------------------------------------------------------------------------
//  bool Append(Command *cmd)
//------------------------------------------------------------------------------
/**
 * Adds command at the end of the command sequence
 * 
 * @param <cmd> Pointer to command that is added at the end
 */
//------------------------------------------------------------------------------
bool Command::Append(Command *cmd)
{
    if (cmd == this)
        throw CommandException("Attempting to add command already in list");
    if (next)
        next->Append(cmd);
    else
        next = cmd;
        
    return true;
}


//------------------------------------------------------------------------------
//  bool Insert(Command *cmd, Command *prev)
//------------------------------------------------------------------------------
/**
 * Inserts command into the command sequence
 * 
 * @param <cmd> Pointer to command that is inserted
 * @param <prev> Pointer to command preceding this command
 */
//------------------------------------------------------------------------------
bool Command::Insert(Command *cmd, Command *prev)
{
    if (this == prev) {
        Command *temp = next;
        next = cmd;
        return next->Append(temp);
    }
    
    if (next == NULL)
        return false;
    
    return next->Insert(cmd, prev);
}


//------------------------------------------------------------------------------
//  bool AssignObjects(void)
//------------------------------------------------------------------------------
/**
 * Sets the internal object pointers prior to a run.
 *
 * This default implementation doesn't do anything.  
 *
 * Derived classes should override this method to set the internal data pointers 
 * needed to execute the command by accessing the various maps of objects and 
 * setting corresponding internal pointers.
 * 
 * @return true if the pointers are assigned "correctly", false otherwise.
 */
//------------------------------------------------------------------------------
bool Command::AssignObjects(void)
{
    return true;
}


//------------------------------------------------------------------------------
//  bool ClearObjects(void)
//------------------------------------------------------------------------------
/**
 * Clears the internal object pointers.
 *
 * This default implementation doesn't do anything.  
 *
 * Derived classes should override this method to "uninitialize" internal 
 * pointers when the assignment operator is called on this command.
 * 
 * @return true if the pointers are assigned "correctly", false otherwise.
 */
//------------------------------------------------------------------------------
bool Command::ClearObjects(void)
{
    return true;
}



