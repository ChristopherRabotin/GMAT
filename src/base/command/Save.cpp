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


//------------------------------------------------------------------------------
// Save()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
Save::Save() :
    GmatCommand("save"),
    filename    (""),
    append      (false),
    written     (false),
    objName     (""),
    obj         (NULL),
    verbose     (false)
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
    GmatCommand (sv),
    filename    (sv.filename),
    append      (sv.append),     // should be false...
    written     (sv.written),
    objName     (sv.objName),
    obj         (NULL),
    verbose     (sv.verbose)
{
}


//------------------------------------------------------------------------------
// Save& operator=(const Save& sv)
//------------------------------------------------------------------------------
/**
 * Assignmant operator.
 * 
 * @param sv The instance that is copied.
 */
//------------------------------------------------------------------------------
Save& Save::operator=(const Save& sv)
{
    if (this == &sv)
        return *this;
        
    filename = sv.filename;
    append   = sv.append;
    written  = sv.written;
    objName  = sv.objName;
    verbose  = sv.verbose;
    
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
   return objName;
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
bool Save::SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
{
   if (name == "")
      return false;
      
   // Save works for all types, so we don't check the type parameter
   objName = name;
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
    written = false;
    bool retval = GmatCommand::Initialize();

    // Save specific initialization goes here:
    if (objectMap->find(objName) == objectMap->end()) {
        std::string errorString = "Save command cannot find object \"";
        errorString += objName;
        errorString += "\"";
        throw CommandException(errorString);
    }

    obj = (*objectMap)[objName];
        
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
    WriteObject();
    return true;
}
  

//------------------------------------------------------------------------------
// void Save::WriteObject()
//------------------------------------------------------------------------------
/**
 * Writes out the script snippet that is needed to recreate an object.
 */
//------------------------------------------------------------------------------
void Save::WriteObject()
{
    if (!obj)
        throw CommandException("Object not set for Save command");
    std::string objectname = obj->GetName();
    filename = objectname;
    filename += ".";
    filename += obj->GetTypeName();

    std::ofstream file;
    
    if (append && written)
        file.open(filename.c_str(), std::ios::app);
    else
        file.open(filename.c_str());
    
    file.precision(18);        /// @todo Make output precision generic
    
    // For now, "Create Propagator" creates a PropSetup.  This kludge handles 
    // that special case.
    std::string tname = obj->GetTypeName();
    if (tname == "PropSetup")
        tname = "Propagator";
    file << "Create " << tname << " " << obj->GetName() << "\n";
               
    Integer i;
    for (i = 0; i < obj->GetParameterCount(); ++i) 
    {
        if (obj->GetParameterType(i) != Gmat::UNKNOWN_PARAMETER_TYPE) {
           if (obj->GetParameterType(i) != Gmat::STRINGARRAY_TYPE) {
               // Fill in the l.h.s.
              if (obj->GetParameterType(i) == Gmat::STRING_TYPE) {
                 std::string val = obj->GetStringParameter(i);
                 // Only save string types if they are not empty or if verbose
                 if (verbose || (val != "")) {
                    file << "GMAT " << objectname << "." 
                         << obj->GetParameterText(i) << " = "
                         << val << ";\n";
                 }
              }
              else {
                  file << "GMAT " << objectname << "." 
                       << obj->GetParameterText(i) << " = ";
                  WriteParameterValue(file, i);
                  file << ";\n";
              }
           }
           else {
               // Fill in the l.h.s.
               const StringArray dat = obj->GetStringArrayParameter(i);
               // Only save string array types if they are not empty
               if (verbose || (dat.size() > 0)) {
                  file << "GMAT " << objectname << "." 
                       << obj->GetParameterText(i) << " = ";
                  std::string desc = "{";
                  for (StringArray::const_iterator i = dat.begin(); 
                       i != dat.end(); ++i) {
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
        
    written = true;
    file.close();
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
void Save::WriteParameterValue(std::ofstream &file, Integer id)
{
    Gmat::ParameterType tid = obj->GetParameterType(id);
    
    switch (tid) {
        case Gmat::OBJECT_TYPE:
            
        case Gmat::INTEGER_TYPE:
            file << obj->GetIntegerParameter(id);
            break;
            
        case Gmat::REAL_TYPE:
            file << obj->GetRealParameter(id);
            break;
            
        case Gmat::BOOLEAN_TYPE:
            file << ((obj->GetBooleanParameter(id)) ? "true" : "false");
            break;
            
        case Gmat::UNSIGNED_INT_TYPE:
            file << obj->GetUnsignedIntParameter(id);
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


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * This method updates object names when the user changes them.
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
   
   if (objName == oldName)
      objName = newName;

   return true;
}
