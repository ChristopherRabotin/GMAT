#include "Save.hpp"


Save::Save() :
    GmatCommand("Save"),
    filename    (""),
    append      (true),     // should be false...
    written     (false),
    objName     (""),
    obj         (NULL)
{
}


Save::~Save()
{
}


Save::Save(const Save& sv) :
    GmatCommand (sv),
    filename    (sv.filename),
    append      (sv.append),     // should be false...
    written     (sv.written),
    objName     (sv.objName),
    obj         (NULL)
{
}


Save& Save::operator=(const Save& sv)
{
    filename = sv.filename;
    append   = sv.append;
    written  = sv.written;
    objName  = sv.objName;
}
     

void Save::InterpretAction(void)
{
    /// @todo: Clean up this hack for the Target::InterpretAction method
    // Sample string:  "Save Sat1;" or
    
    // Set starting location to the space following the command string
    Integer loc = generatingString.find("Save", 0) + 4, end;
    const char *str = generatingString.c_str();
    
    // Skip white space
    while (str[loc] == ' ')
        ++loc;
    
    // Stop at the semicolon if it is there
    end = generatingString.find(";", loc);
    
    
    std::string component = generatingString.substr(loc, end-loc);
    if (component == "")
        throw CommandException("Save string does not identify the object");
    objName = component;
}


bool Save::Initialize(void)
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


bool Save::Execute(void)
{
    WriteObject();
}
  

void Save::WriteObject(void)
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
        // Skip StringArray parameters, at least for now
        if (obj->GetParameterType(i) != Gmat::STRINGARRAY_TYPE) {
            // Fill in the l.h.s.
            file << "GMAT " << objectname << "." 
                 << obj->GetParameterText(i) << " = ";
            WriteParameterValue(file, i);
            file << ";\n";
        }
    }
    file << "\n";
        
    written = true;
    file.close();
}


void Save::WriteParameterValue(std::ofstream &file, Integer id)
{
    Gmat::ParameterType tid = obj->GetParameterType(id);
    
    switch (tid) {
        case Gmat::OBJECT_TYPE:
        case Gmat::STRING_TYPE:     // Strings and objects write out a string
            file << obj->GetStringParameter(id);
            break;
            
        case Gmat::INTEGER_TYPE:
            file << obj->GetIntegerParameter(id);
            break;
            
        case Gmat::REAL_TYPE:
            file << obj->GetRealParameter(id);
            break;
            
        case Gmat::BOOLEAN_TYPE:
            file << ((obj->GetBooleanParameter(id)) ? "true" : "false");
            break;
            
        default:
            break;
    }
}
