//$Header$
//------------------------------------------------------------------------------
//                             AtmosphereManager
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/29
//
/**
 * Manager for planetary atmosphere models
 */
//------------------------------------------------------------------------------


#include "AtmosphereManager.hpp"
#include <algorithm>                    // for find


// Supported models
#include "ExponentialAtmosphere.hpp"
#include "Msise90Atmosphere.hpp"
#include "JacchiaRobertsAtmosphere.hpp"


//------------------------------------------------------------------------------
// AtmosphereManager(const std::string& body)
//------------------------------------------------------------------------------
/**
 * Constructor.
 * 
 * @param body Name for the body that owns this atmosphere manager.
 */
//------------------------------------------------------------------------------
AtmosphereManager::AtmosphereManager(const std::string& body) :
    currentAtmosphere       (NULL),
    atmOwner                (body),
    sunVector               (NULL)
{
    // Add the name of each supported frame to this list
    atmospheres.push_back("Exponential");
    atmospheres.push_back("MSISE90");
    BuildBodyAtmospheres();
}


//------------------------------------------------------------------------------
// ~AtmosphereManager(void)
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
AtmosphereManager::~AtmosphereManager(void)
{
    if (currentAtmosphere)
        delete currentAtmosphere;
}


//------------------------------------------------------------------------------
// const StringArray& GetSupportedAtmospheres(const std::string& body)
//------------------------------------------------------------------------------
/**
 * Accesses the atmospheres that can be associated with a specified body.
 * 
 * @param body Name for the body that needs the atmosphere.
 */
//------------------------------------------------------------------------------
const StringArray& AtmosphereManager::GetSupportedAtmospheres(
                                                        const std::string& body)
{
    if (body != "") {
        atmOwner = body;
        BuildBodyAtmospheres();
    }        
    
    return bodyAtmospheres;    
}


//------------------------------------------------------------------------------
// AtmosphereModel* GetAtmosphere(const std::string& atmType, bool isInternal)
//------------------------------------------------------------------------------
/**
 * Accesses the atmospheres that can be associated with a specified body.
 * 
 * @param atmType    The name of the requested atmosphere type.  An empty string 
 *                   ("") indicates that the internal model should be returned.
 * @param isInternal Flag used to determine if the allocated model is managed 
 *                   here, or externally.  The default, true, indicates that the 
 *                   model is allocated and deallocated in this instance of the 
 *                   manager.
 * 
 * @return A pointer to the AtmosphereModel.
 */
//------------------------------------------------------------------------------
AtmosphereModel* AtmosphereManager::GetAtmosphere(const std::string& atmType, 
                                                  bool isInternal)
{
   if (atmType == "")
      return currentAtmosphere;

   AtmosphereModel* model = NULL;
        
   if ((find(bodyAtmospheres.begin(), bodyAtmospheres.end(), atmType)) != 
        bodyAtmospheres.end()) {
      // Add constructor calls for the supported atmospheres here
      if (atmType == "Exponential")
         model = new ExponentialAtmosphere;
      if (atmType == "MSISE90")
         model = new Msise90Atmosphere;
      if (atmType == "Jacchia-Roberts")
         model = new JacchiaRobertsAtmosphere;
          
      if (model == NULL)
         throw AtmosphereException("Cannot create requested atmosphere model");
   }
    
   if (isInternal) {
      if (currentAtmosphere)
         delete currentAtmosphere;
      currentAtmosphere = model;
   }
    
   return model;
}


// Not sure yet how we want to do this -- does the body know where the sun is, 
// or is there some other information I should use instead?
void AtmosphereManager::SetSunVector(Real *sv)
{
    sunVector = sv;
}


void AtmosphereManager::BuildBodyAtmospheres(void)
{
    ///@todo Set this method to parse the atmosphere list for supported bodies
    bodyAtmospheres = atmospheres;
}
