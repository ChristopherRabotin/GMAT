#ifndef AtmosphereModel_hpp
#define AtmosphereModel_hpp


#include "GmatBase.hpp"


class AtmosphereModel : public GmatBase
{
public:

    AtmosphereModel(const std::string &typeStr);
    virtual ~AtmosphereModel();
    
    bool                    Density(Real *position, Real *density, 
                                    Integer count = 1);
    
protected:
    AtmosphereModel(const AtmosphereModel& am);
    AtmosphereModel&        operator=(const AtmosphereModel& am);
   
};

#endif // AtmosphereModel_hpp
