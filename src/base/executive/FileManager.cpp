//$Header$
//------------------------------------------------------------------------------
//                            FileManager
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/04/02
/**
 * Implements FileManager class. This is singleton class which manages
 * list of file paths and names.
 */
//------------------------------------------------------------------------------

#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "GmatBaseException.hpp"
#include <fstream>
#include <sstream>

//---------------------------------
// static data
//---------------------------------
const std::string
FileManager::PARAMETER_TEXT[FileManagerParamCount] =
{
    // file path
    "OUTPUT_FILE_PATH",
    "SLP_FILE_PATH",
    "DE_FILE_PATH",
    "TEXTURE_FILE_PATH",
    // file name
    "TIME_COEFF_FILE",
    "SLP_FILE",
    "DE200_FILE",
    "DE202_FILE",
    "DE405_FILE",
    "EARTH_TEXTURE_FILE",
    // full file name
    "FULL_TIME_COEFF_FILE",
    "FULL_SLP_FILE",
    "FULL_DE200_FILE",
    "FULL_DE202_FILE",
    "FULL_DE405_FILE",
    "FULL_EARTH_TEXTURE_FILE",
};

FileManager* FileManager::theInstance = NULL;

//---------------------------------
// public methods
//---------------------------------
   
//------------------------------------------------------------------------------
// FileManager* Instance()
//------------------------------------------------------------------------------
FileManager* FileManager::Instance()
{
    if (theInstance == NULL)
        theInstance = new FileManager;
    return theInstance;
}

//------------------------------------------------------------------------------
// void ReadStartupFile(const std::string &fileName = "")
//------------------------------------------------------------------------------
void FileManager::ReadStartupFile(const std::string &fileName)
{
    char line[200] = "";
    
    if (fileName != "")
        theStartupFileName = fileName;
    
    //MessageInterface::ShowMessage("FileManager::ReadStartupFile() reading:%s\n",
    //                              theStartupFileName.c_str());
    
    std::ifstream instream(theStartupFileName.c_str());

    if (!instream)
        throw GmatBaseException
            ("FileManager::ReadStartupFile() cannot open:" + fileName);

    while (!instream.eof())
    {
        line[0] = '\0';
        instream.getline(line, 200);
        //MessageInterface::ShowMessage("FileManager::ReadStartupFile() line=%s\n",
        //                              line);

        if (line[0] == '\0')
            break;
        if (line[0] == '#')
            continue;
        
        std::string type, equal, name;
        std::stringstream ss("");
        
        ss << line;
        ss >> type >> equal;
        
        if (equal != "=")
        {
            instream.close();
            throw GmatBaseException("FileManager::ReadStartupFile() expecting =");
        }
        
        ss >> name;
            
        //MessageInterface::ShowMessage("FileManager::ReadStartupFile() type=%s, "
        //                              "name=%s\n", type.c_str(), name.c_str());
        
        // find string match
        if (type == "OUTPUT_FILE_PATH")
            theFileList[OUTPUT_FILE_PATH] = name;
        else if (type == "SLP_FILE_PATH")
            theFileList[SLP_FILE_PATH] = name;
        else if (type == "DE_FILE_PATH")
            theFileList[DE_FILE_PATH] = name;
        else if (type == "TEXTURE_FILE_PATH")
            theFileList[TEXTURE_FILE_PATH] = name;
        else if (type == "TIME_COEFF_FILE")
            theFileList[TIME_COEFF_FILE] = name;
        else if (type == "SLP_FILE")
            theFileList[SLP_FILE] = name;
        else if (type == "DE200_FILE")
            theFileList[DE200_FILE] = name;
        else if (type == "DE202_FILE")
            theFileList[DE202_FILE] = name;
        else if (type == "DE405_FILE")
            theFileList[DE405_FILE] = name;
        else if (type == "EARTH_TEXTURE_FILE")
            theFileList[EARTH_TEXTURE_FILE] = name;
        else
        {
            instream.close();
            throw GmatBaseException("FileManager::ReadStartupFile() Invalid file type:" +
                                    type);
        }
    }

    // create full file name
    theFileList[FULL_TIME_COEFF_FILE]
        = theFileList[SLP_FILE_PATH] + theFileList[TIME_COEFF_FILE];
    
    theFileList[FULL_SLP_FILE]
        = theFileList[SLP_FILE_PATH] + theFileList[SLP_FILE];
    
    theFileList[FULL_DE200_FILE]
        = theFileList[DE_FILE_PATH] + theFileList[DE200_FILE];
    
    theFileList[FULL_DE202_FILE]
        = theFileList[DE_FILE_PATH] + theFileList[DE202_FILE];
    
    theFileList[FULL_DE405_FILE]
        = theFileList[DE_FILE_PATH] + theFileList[DE405_FILE];
    
    theFileList[FULL_EARTH_TEXTURE_FILE]
        = theFileList[TEXTURE_FILE_PATH] + theFileList[EARTH_TEXTURE_FILE];
    
    instream.close();
}

//------------------------------------------------------------------------------
// virtual std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string FileManager::GetParameterText(const Integer id) const
{
    if (id >= 0 && id < FileManagerParamCount)
        return PARAMETER_TEXT[id];
    else
        return "UNKNOWN_ID";
    
}

//------------------------------------------------------------------------------
// virtual Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer FileManager::GetParameterID(const std::string &str) const
{
   for (int i=0; i<FileManagerParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i])
          return i;
   }
   
   return -1;
}

//------------------------------------------------------------------------------
// virtual std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string FileManager::GetStringParameter(const Integer id) const
{
    if (id >= 0 && id < FileManagerParamCount)
        return theFileList[id];
    else
        return "UNKNOWN_ID";
}

//------------------------------------------------------------------------------
// virtual std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string FileManager::GetStringParameter(const std::string &label) const
{
    return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool FileManager::SetStringParameter(const Integer id, const std::string &value)
{
    if (id >= 0 && id < FileManagerParamCount)
    {
        theFileList[id] = value;
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value)
//------------------------------------------------------------------------------
bool FileManager::SetStringParameter(const std::string &label,
                                     const std::string &value)
{
    //MessageInterface::ShowMessage("FileManager::SetStringParameter() entered: "
    //                              "label = " + label + ", value = " + value + "\n");

    return SetStringParameter(GetParameterID(label), value);
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// FileManager()
//------------------------------------------------------------------------------
/*
 * Constructor
 *
 * @note It uses DOS file system as a default.  Handling of other file
 *       system (such as MAC) will be implemented later.
 */
//------------------------------------------------------------------------------
FileManager::FileManager()
{    
    // create default paths and files
    theFileList[OUTPUT_FILE_PATH] = ".\\output\\";
    theFileList[SLP_FILE_PATH] = ".\\planetary_ephem\\slp\\";
    theFileList[DE_FILE_PATH] = ".\\planetary_ephem\\de\\";
    theFileList[TEXTURE_FILE_PATH] = ".\\plot\\texture\\";
    theFileList[TIME_COEFF_FILE] = "timecof.pc";
    theFileList[SLP_FILE] = "mn2000.pc";
    theFileList[DE405_FILE] = "winp1941.405";
    theFileList[DE200_FILE] = "winp1941.200";
    theFileList[EARTH_TEXTURE_FILE] = "earth-0512.jpg";

    theStartupFileName = "gmat_startup_file.txt";
}

//------------------------------------------------------------------------------
// ~FileManager(void)
//------------------------------------------------------------------------------
FileManager::~FileManager(void)
{
}

