#ifndef AtmosphereManager_hpp
#define AtmosphereManager_hpp


#include "AtmosphereModel.hpp"


class AtmosphereManager
{
public:
	AtmosphereManager(const std::string& body = "Earth");
	virtual ~AtmosphereManager();
 
    const StringArray&  GetSupportedAtmospheres(const std::string& body = "");
    AtmosphereModel*    GetAtmosphere(const std::string& atmType = "", 
                                      bool isInternal = true);
    void                SetSunVector(Real *sv);
    
protected:
    /// Table of available atmospheres
    StringArray                 atmospheres;
    /// Table of available atmospheres for a specific body
    StringArray                 bodyAtmospheres;
    /// Current atmosphere used
    AtmosphereModel             *currentAtmosphere;
    /// Body that has the atmosphere
    std::string                 atmOwner;
    /// Vector from the owning body to the Sun
    Real                        *sunVector;
    
    void                        BuildBodyAtmospheres(void);

private:
    AtmosphereManager(const AtmosphereManager& am);
    AtmosphereManager&          operator=(const AtmosphereManager& am);
};

#endif // AtmosphereManager_hpp
