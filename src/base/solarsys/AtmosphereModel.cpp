//$Header$
//------------------------------------------------------------------------------
//                              AtmosphereModel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/21
//
/**
 * Base class for the atmosphere models
 */
//------------------------------------------------------------------------------

#include "AtmosphereModel.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
//  AtmosphereModel()
//------------------------------------------------------------------------------
/**
 *  Constructor.
 */
//------------------------------------------------------------------------------
AtmosphereModel::AtmosphereModel(const std::string &typeStr) :
    GmatBase            (Gmat::ATMOSPHERE, typeStr),
    fileReader          (NULL),
    sunVector           (NULL),
    centralBody         ("Earth"),
    centralBodyLocation (NULL),
    cbRadius            (6378.14),
    newFile             (false),
    fileRead            (false)
{
    fileName = "";
}

//------------------------------------------------------------------------------
//  ~AtmosphereModel()
//------------------------------------------------------------------------------
/**
 *  Destructor.
 */
//------------------------------------------------------------------------------
AtmosphereModel::~AtmosphereModel()
{
}

//------------------------------------------------------------------------------
//  AtmosphereModel(const AtmosphereModel& am)
//------------------------------------------------------------------------------
/**
 *  Copy constructor.
 */
//------------------------------------------------------------------------------
AtmosphereModel::AtmosphereModel(const AtmosphereModel& am) :
GmatBase            (am),
fileReader          (NULL),
sunVector           (NULL),
centralBody         (am.centralBody),
centralBodyLocation (NULL),
cbRadius            (am.cbRadius),
newFile             (am.newFile),
fileRead            (am.fileRead)
{
}

//------------------------------------------------------------------------------
// AtmosphereModel& operator=(const AtmosphereModel& am)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param am AtmosphereModel instance used as a template for this copy.
 * 
 * @return A reference to this class, with members set to match the template.
 */
//------------------------------------------------------------------------------
AtmosphereModel& AtmosphereModel::operator=(const AtmosphereModel& am)
{
    if (this == &am)
        return *this;
        
    fileReader = NULL;
    sunVector = NULL;
    centralBodyLocation = NULL;
    
    centralBody = am.centralBody;
    cbRadius = am.cbRadius;
    
    return *this;
}

//------------------------------------------------------------------------------
//  void SetSunVector(Real *sv)
//------------------------------------------------------------------------------
/**
 *  Sets the position vector for the Sun.
 * 
 * @param sv   The Sun vector.
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetSunVector(Real *sv)
{
    sunVector = sv;
}

//------------------------------------------------------------------------------
//  void SetCentralBodyVector(Real *cv)
//------------------------------------------------------------------------------
/**
 *  Sets the position vector for the body with the atmosphere.
 * 
 * @param cv   The body's position vector.
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetCentralBodyVector(Real *cv)
{
    centralBodyLocation = cv;
}

//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer
 * 
 * @param ss Pointer to the solar system used in the modeling.
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetSolarSystem(SolarSystem *ss)
{
   solarSystem = ss;
}

//------------------------------------------------------------------------------
//  void SetSolarFluxFile(std::string file)
//------------------------------------------------------------------------------
/**
 * @param file The solar flux file
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetSolarFluxFile(std::string file)
{
   if (strcmp(fileName.c_str(), file.c_str()) == 0)
      SetOpenFileFlag(true);
   else
   {
      fileName = file;
      SetOpenFileFlag(false);
   }   
}

//------------------------------------------------------------------------------
// void SetNewFileFlag(bool flag)
//------------------------------------------------------------------------------
/**
 * Sets the new file flag
 * 
 * @param flag
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetNewFileFlag(bool flag)
{
   newFile = flag;
}

//------------------------------------------------------------------------------
// void SetOpenFileFlag(bool flag)
//------------------------------------------------------------------------------
/**
 * Sets the file opened flag
 * 
 * @param flag
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetOpenFileFlag(bool flag)
{
   fileRead = flag;
}

//------------------------------------------------------------------------------
// void CloseFile()
//------------------------------------------------------------------------------
/**
 * Sets the new file flag
 * 
 * @param flag
 */
//------------------------------------------------------------------------------
void AtmosphereModel::CloseFile()
{
   if (fileReader->CloseSolarFluxFile(solarFluxFile))
       fileRead = false;
    else
       throw AtmosphereException("Error closing Atmosphere Model data file.\n");   
}
