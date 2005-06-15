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
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// Save()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
Save::Save() :
   GmatCommand   ("Save"),
   filename      (""),
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
   filename      (sv.filename),
   appendData    (sv.appendData),     // should be false...
   wasWritten    (sv.wasWritten),
   objName       (sv.objName),
   writeVerbose  (sv.writeVerbose)
{
   obj.clear();
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
        
   filename     = sv.filename;
   appendData   = sv.appendData;
   wasWritten   = sv.wasWritten;
   objName      = sv.objName;
   writeVerbose = sv.writeVerbose;
   obj.clear();
    
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
   return objName[0];
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets the reference objects that get saved.
 * 
 * @param type The type of the reference object.
 * @param name THe name of the reference object.
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
   objName.push_back(name);
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
   wasWritten = false;
   bool retval = GmatCommand::Initialize();

   // Save specific initialization goes here:
   StringArray::iterator i = objName.begin();
   if (objName.size() > 1)
   {
      filename = *i;
      ++i;
   }

   for (/* i initialized above */; i != objName.end(); ++i)
   {
      if (objectMap->find(*i) == objectMap->end())
      {
         // Maybe it's a solar system object
         if (solarSys)
         {
            GmatBase *bod = solarSys->GetBody(*i);
            if (bod != NULL)
            {
               obj.push_back(bod);
               return retval;
            }
         }
         
         std::string errorString = "Save command cannot find object \"";
         errorString += *i;
         errorString += "\"";
         throw CommandException(errorString);
      }

      obj.push_back((*objectMap)[*i]);
   }
    
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
   if (!obj[0])
      throw CommandException("Object not set for Save command");
   if (filename == "")
   {
      std::string objectname = obj[0]->GetName();
      filename = objectname;
      filename += ".";
      filename += obj[0]->GetTypeName();
   }

   if (appendData && wasWritten)
      file.open(filename.c_str(), std::ios::app);
   else
      file.open(filename.c_str());
    
   file.precision(18);        /// @todo Make output precision generic

   for (ObjectArray::iterator i = obj.begin(); i != obj.end(); ++i)
      WriteObject(*i);
      
   wasWritten = true;
   file.close();
   
   return true;
}
  

//------------------------------------------------------------------------------
// void Save::WriteObject()
//------------------------------------------------------------------------------
/**
 * Writes out the script snippet that is needed to recreate an object.
 */
//------------------------------------------------------------------------------
void Save::WriteObject(GmatBase *o)
{
   std::string objectname = o->GetName();

   // "Create Propagator" creates a PropSetup.  This code handles
   // that special case.
   std::string tname = o->GetTypeName();
   if (tname == "PropSetup")
      tname = "Propagator";
   file << "Create " << tname << " " << o->GetName() << "\n";

   Integer i;
   for (i = 0; i < o->GetParameterCount(); ++i)
   {
      if (o->GetParameterType(i) != Gmat::UNKNOWN_PARAMETER_TYPE)
      {
         if (o->GetParameterType(i) != Gmat::STRINGARRAY_TYPE)
         {
            // Fill in the l.h.s.
            if (o->GetParameterType(i) == Gmat::STRING_TYPE)
            {
               std::string val = o->GetStringParameter(i);
               // Only save string types if they are not empty or if verbose
               if (writeVerbose || (val != ""))
               {
                  file << "GMAT " << objectname << "."
                       << o->GetParameterText(i) << " = "
                       << val << ";\n";
               }
            }
            else
            {
               file << "GMAT " << objectname << "."
                    << o->GetParameterText(i) << " = ";
               WriteParameterValue(o, file, i);
               file << ";\n";
            }
         }
         else
         {
            // Fill in the l.h.s.
            const StringArray dat = o->GetStringArrayParameter(i);
            // Only save string array types if they are not empty
            if (writeVerbose || (dat.size() > 0))
            {
               file << "GMAT " << objectname << "."
                    << o->GetParameterText(i) << " = ";
               std::string desc = "{";
               for (StringArray::const_iterator i = dat.begin();
                    i != dat.end(); ++i)
               {
                  if (desc != "{")
                     desc += ", ";
                  desc += (*i);
               }
               desc += "}";
               file << desc << "\n";
            }
         }
      }
   }
   file << "\n";
}


//------------------------------------------------------------------------------
// void WriteParameterValue(std::ofstream &file, Integer id)
//------------------------------------------------------------------------------
/**
 * Writes out the values of non-string parameters.
 * 
 * @param id The id for the parameter that is written.
 */
//------------------------------------------------------------------------------
void Save::WriteParameterValue(GmatBase *o, std::ofstream &file, Integer id)
{
   Gmat::ParameterType tid = o->GetParameterType(id);
    
   switch (tid)
   {
      case Gmat::OBJECT_TYPE:
         file << o->GetName();
         break;
         
      case Gmat::INTEGER_TYPE:
         file << o->GetIntegerParameter(id);
         break;
            
      case Gmat::REAL_TYPE:
         file << o->GetRealParameter(id);
         break;

      // DJC, 06/15/05: Temporarily put an accessor in place to help debug the
      // force model.  This piece may be replaced by a different call when epoch
      // issues are updated in the Spacecraft/SpaceObject code.
      case Gmat::TIME_TYPE:
         if (o->IsOfType("CelestialBody"))
            file << o->GetRealParameter(id);
         break;

      case Gmat::RVECTOR_TYPE:
         {
            Rvector st = o->GetRvectorParameter(id);
            file << "[" << st.ToString() << "]";
         }
         break;

      case Gmat::BOOLEAN_TYPE:
         file << ((o->GetBooleanParameter(id)) ? "true" : "false");
         break;
            
      case Gmat::UNSIGNED_INT_TYPE:
         file << o->GetUnsignedIntParameter(id);
         break;
            
      case Gmat::STRING_TYPE:
         file << o->GetStringParameter(id);
         break;
            
     case Gmat::RMATRIX_TYPE:
         if (o->GetTypeName() == "Array")
         {
            file << "[";
            Integer rows = o->GetIntegerParameter("NumRows");
            Integer cols = o->GetIntegerParameter("NumCols");
            for (Integer r = 0; r < rows; ++r)
            {
               for (Integer c = 0; c < cols; ++c)
               {
                  file << o->GetRealParameter("SingleValue", r, c);
                  if (c < cols-1)
                     file << " ";
               }
               if (r < rows - 1)
                  file << "; ";
            }
            file << "]";
         }
         else
            MessageInterface::ShowMessage("Unable to write RMatrix for "
               "parameter %s on object %s\n",
               o->GetParameterText(id).c_str(), o->GetName().c_str());

         break;

      default:
         break;
   }
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
   for (StringArray::iterator i = objName.begin(); i != objName.end(); ++i)
      generatingString += " " + *i;
   generatingString += ";";

   // Then call the base class method
   return GmatCommand::GetGeneratingString();
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
   if (type != Gmat::SPACECRAFT)
      return true;
   
   for (Integer index = 0; index < (Integer)objName.size(); ++index)
   {
      if (objName[index] == oldName)
         objName[index] = newName;
   }

   return true;
}
