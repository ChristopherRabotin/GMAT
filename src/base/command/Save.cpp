//$Header$
//------------------------------------------------------------------------------
//                                  Save
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2004/02/26
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Class implementation for the save command
 */
//------------------------------------------------------------------------------


#include "Save.hpp"
#include "FileManager.hpp"      // for GetPathname()
#include "MessageInterface.hpp"
#include "GmatGlobal.hpp"       // for GetDataPrecision()

// for writing to single file per Save command, if not defined
// it will create files per Save objects
#define __USE_SINGLE_FILE__

//#define DEBUG_SAVE_OUTPUT

//------------------------------------------------------------------------------
// Save()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
Save::Save() :
   GmatCommand   ("Save"),
   appendData    (false),
   wasWritten    (false),
   writeVerbose  (false)
{
}


//------------------------------------------------------------------------------
// ~Save()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Save::~Save()
{
   delete [] fileArray;
}


//------------------------------------------------------------------------------
// Save(const Save& sv)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param sv The instance that is copied.
 */
//------------------------------------------------------------------------------
Save::Save(const Save& sv) :
   GmatCommand   (sv),
   fileNameArray (sv.fileNameArray),
   appendData    (sv.appendData),     // should be false...
   wasWritten    (sv.wasWritten),
   objNameArray  (sv.objNameArray),
   writeVerbose  (sv.writeVerbose)
{
   objArray.clear();
}


//------------------------------------------------------------------------------
// Save& operator=(const Save& sv)
//------------------------------------------------------------------------------
/**
 * Assignmant operator.
 * 
 * @param sv The instance that is copied.
 *
 * @return this instance, with internal data set to match the input command.
 */
//------------------------------------------------------------------------------
Save& Save::operator=(const Save& sv)
{
   if (this == &sv)
      return *this;
        
   fileNameArray = sv.fileNameArray;
   appendData    = sv.appendData;
   wasWritten    = sv.wasWritten;
   objNameArray  = sv.objNameArray;
   writeVerbose  = sv.writeVerbose;
   objArray.clear();
   
   return *this;
}


//------------------------------------------------------------------------------
// bool GetRefObjectName(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves the reference object names.
 * 
 * @param type The type of the reference object.
 * 
 * @return the name of the object.
 */
//------------------------------------------------------------------------------
std::string Save::GetRefObjectName(const Gmat::ObjectType type) const
{
   if (objNameArray.size() == 0)
      return "";
   else
      return objNameArray[0];
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Accesses arrays of names for referenced objects.
 * 
 * @param type Type of object requested.
 * 
 * @return the StringArray containing the referenced object names.
 */
//------------------------------------------------------------------------------
const StringArray& Save::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   // it can be any object, so ignore object type
   return objNameArray;
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets the reference objects that get saved.
 * 
 * @param type The type of the reference object.
 * @param name The name of the reference object.
 * 
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool Save::SetRefObjectName(const Gmat::ObjectType type,
                            const std::string &name)
{
   if (name == "")
      return false;
      
   // Save works for all types, so we don't check the type parameter
   objNameArray.push_back(name);
   return true;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Connects up the object associations prior to saving them.
 * 
 * @return true on success
 */
//------------------------------------------------------------------------------
bool Save::Initialize()
{
   bool retval = GmatCommand::Initialize();
   
   wasWritten = false;
   appendData = false;

   FileManager *fm = FileManager::Instance();
   std::string outPath = fm->GetAbsPathname(FileManager::OUTPUT_PATH);
   
   #ifdef DEBUG_SAVE_OUTPUT
   MessageInterface::ShowMessage("Save::Initialize() outPath=%s\n", outPath.c_str());
   #endif
   
   Integer objCount = objNameArray.size();
   
   #ifndef __USE_SINGLE_FILE__
      for (Integer i=0; i<objCount; i++)
         fileNameArray.push_back(outPath + objNameArray[i]);
      fileArray = new std::ofstream[objCount];
   #else
      fileNameArray.push_back(outPath + objNameArray[0]);
      fileArray = new std::ofstream[1];
   #endif
      
   Integer index = 0;
   
   for (StringArray::iterator i = objNameArray.begin(); i != objNameArray.end(); ++i)
   {         
      if (objectMap->find(*i) == objectMap->end())
      {
         // Maybe it's a solar system object
         if (solarSys)
         {
            GmatBase *bod = solarSys->GetBody(*i);
            if (bod != NULL)
            {
               objArray.push_back(bod);
               return retval;
            }
         }
         
         std::string errorString = "Save command cannot find object \"";
         errorString += *i;
         errorString += "\"";
         throw CommandException(errorString);
      }
      
      objArray.push_back((*objectMap)[*i]);
      
      // If saving multiple objects for single file,
      // we want to append object names.
      #ifdef __USE_SINGLE_FILE__
         if (index > 0)
            fileNameArray[0] = fileNameArray[0] + "_" + *i;
         
      #else
         fileNameArray[index] += ("." + objArray[index]->GetTypeName() + ".data");
         
         #ifdef DEBUG_SAVE_OUTPUT
         MessageInterface::ShowMessage
            ("Save::Initialize() fileNameArray[%d]=%s\n", index,
             fileNameArray[index].c_str());
         #endif
      #endif
         
      index++;
   }
   
   #ifdef __USE_SINGLE_FILE__
      if (objCount > 1)
         fileNameArray[0] += ".data";
      else if (index == objCount)
         fileNameArray[0] += ("." + objArray[0]->GetTypeName() + ".data");
      
      #ifdef DEBUG_SAVE_OUTPUT
      MessageInterface::ShowMessage
         ("Save::Initialize() fileNameArray[%d]=%s\n", 0,
          fileNameArray[0].c_str());
      #endif
   #endif
   
   #ifdef DEBUG_SAVE_OUTPUT
   MessageInterface::ShowMessage
      ("Save::Initialize() Saving %d object(s)\n", objArray.size());
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Write objects to a text file.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Save::Execute()
{
   #ifdef DEBUG_SAVE_OUTPUT
   MessageInterface::ShowMessage("Save::Execute() entered\n");
   #endif
   
   Integer prec = GmatGlobal::Instance()->GetDataPrecision();
   
   if (!objArray[0])
      throw CommandException("Object not set for Save command");
   
   #ifndef __USE_SINGLE_FILE__
   
      for (UnsignedInt i=0; i<objArray.size(); i++)
      {
         if (appendData && wasWritten)
            fileArray[i].open(fileNameArray[i].c_str(), std::ios::app);
         else
            fileArray[i].open(fileNameArray[i].c_str());
         
         fileArray[i].precision(prec);
      }
      
   #else

      // Changed to append the data once data was written so that saving data
      // within a loop won't overwrite old data (loj: 4/24/07)
      //if (appendData && wasWritten)
      if (appendData || wasWritten)
      {
         #ifdef DEBUG_SAVE_OUTPUT
         MessageInterface::ShowMessage("   open %s as append\n", fileNameArray[0].c_str());
         #endif
         
         fileArray[0].open(fileNameArray[0].c_str(), std::ios::app);
      }
      else
      {
         #ifdef DEBUG_SAVE_OUTPUT
         MessageInterface::ShowMessage("   open %s as new\n", fileNameArray[0].c_str());
         #endif
         
         fileArray[0].open(fileNameArray[0].c_str());
      }
      
      fileArray[0].precision(prec);
      
   #endif
      
      
   for (UnsignedInt i=0; i<objArray.size(); i++)
      WriteObject(i, objArray[i]);
   
   wasWritten = true;
   
   #ifndef __USE_SINGLE_FILE__   
      for (UnsignedInt i=0; i<objArray.size(); i++)
         fileArray[i].close();
   #else
      fileArray[0].close();
   #endif
   
   BuildCommandSummary(true);
   
   return true;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void Save::RunComplete()
{
   fileNameArray.clear();
   objArray.clear();
   GmatCommand::RunComplete();
}


//------------------------------------------------------------------------------
// void Save::WriteObject()
//------------------------------------------------------------------------------
/**
 * Writes out the script snippet that is needed to recreate an object.
 */
//------------------------------------------------------------------------------
void Save::WriteObject(UnsignedInt i, GmatBase *o)
{
   #ifdef DEBUG_SAVE_OUTPUT
      MessageInterface::ShowMessage("Save: %s has %d parameters\n", 
         o->GetName().c_str(), o->GetParameterCount());
   #endif
   
   std::string objectname = o->GetName();
   std::string prefix = "";

   /**
    * @note:
    * Since we are using GetGeneratingString() we don't need to write "Create"
    * except Variables and Strings.  Variables and Strings are special case that
    * they are written in group in the ScriptInterpreter.  For example,
    * "Create Variable var1 var2 var3;" are written when save script.
    * For Arrays, it needs to know the dimension of array so it is handled in the
    * Array::GetGeneratingString().
    */
   
   // "Create Propagator" creates a PropSetup.  This code handles
   // that special case.
   //std::string tname = o->GetTypeName();
   //if (tname == "PropSetup")
   //   tname = "Propagator";   
   //file << "Create " << tname << " " << o->GetName() << "\n";
   
   if (o->GetTypeName() == "Variable" || o->GetTypeName() == "String")
      prefix = "Create " + o->GetTypeName() + " " + o->GetName() + "\n";
   
   #ifdef __USE_SINGLE_FILE__
      fileArray[0] << prefix << o->GetGeneratingString();
      fileArray[0] << std::endl;
   #else
      fileArray[i] << prefix << o->GetGeneratingString();
      fileArray[i] << std::endl;
   #endif
      
   #ifdef DEBUG_SAVE_OUTPUT
      MessageInterface::ShowMessage("Save:WriteObject() leaving\n");
   #endif
      
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Save.
 *
 * @return clone of the Save.
 */
//------------------------------------------------------------------------------
GmatBase* Save::Clone() const
{
   return (new Save(*this));
}


//------------------------------------------------------------------------------
//  const std::string& GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param mode    Specifies the type of serialization requested.  (Not yet used
 *                in commands)
 * @param prefix  Optional prefix appended to the object's name.  (Not yet used
 *                in commands)
 * @param useName Name that replaces the object's name.  (Not yet used in
 *                commands)
 *
 * @return The script line that, when interpreted, defines this Save command.
 */
//------------------------------------------------------------------------------
const std::string& Save::GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName)
{
   // Build the local string
   generatingString = prefix + "Save";
   for (StringArray::iterator i = objNameArray.begin(); i != objNameArray.end(); ++i)
      generatingString += " " + *i;
   generatingString += ";";

   // Then call the base class method
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,  
//                         const std::string &actionData = "");
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action
 * @return true if action successfully performed
 *
 */
//------------------------------------------------------------------------------
bool Save::TakeAction(const std::string &action, const std::string &actionData)
{
   #if DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("Save::TakeAction() action=%s, actionData=%s\n",
       action.c_str(), actionData.c_str());
   #endif
   
   if (action == "Clear")
   {
      objNameArray.clear();
      return true;
   }

   return false;
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * This method updates object names when the user changes them.
 *
 * @param type Type of object that is renamed.
 * @param oldName Old name for the object.
 * @param newName New name for the object.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool Save::RenameRefObject(const Gmat::ObjectType type,
                           const std::string &oldName,
                           const std::string &newName)
{
   for (Integer index = 0; index < (Integer)objNameArray.size(); ++index)
   {
      if (objNameArray[index] == oldName)
         objNameArray[index] = newName;
   }
   
   return true;
}
