#include "AtmosphereManager.hpp"
#include <algorithm>                    // for find


// Supported models
#include "ExponentialAtmosphere.hpp"
#include "Msise90Atmosphere.hpp"


AtmosphereManager::AtmosphereManager(std::string body) :
    currentAtmosphere       (NULL),
    atmOwner                (body),
    sunVector               (NULL)
{
    // Add the name of each supported frame to this list
    atmospheres.push_back("Exponential");
    atmospheres.push_back("MSISE90");
    BuildBodyAtmospheres();
}


AtmosphereManager::~AtmosphereManager()
{
    if (currentAtmosphere)
        delete currentAtmosphere;
}


const StringArray& AtmosphereManager::GetSupportedAtmospheres(
                                                        const std::string body)
{
    if (body != "") {
        atmOwner = body;
        BuildBodyAtmospheres();
    }        
    
    return bodyAtmospheres;    
}


AtmosphereModel* AtmosphereManager::GetAtmosphere(const std::string atmType)
{
    if (atmType == "")
        return currentAtmosphere;
        
    if ((find(bodyAtmospheres.begin(), bodyAtmospheres.end(), atmType)) != 
          bodyAtmospheres.end()) {
        if (currentAtmosphere) {
            delete currentAtmosphere;
            currentAtmosphere = NULL;
        }
        
        // Add constructor calls for the supported atmospheres here
        if (atmType == "Exponential")
            currentAtmosphere = new ExponentialAtmosphere;
        if (atmType == "MSISE90")
            currentAtmosphere = new Msise90Atmosphere;
    
        return currentAtmosphere;
    }
        
    return NULL;
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
