#ifndef InertialManeuverFrame_hpp
#define InertialManeuverFrame_hpp

#include "ManeuverFrame.hpp"

class InertialManeuverFrame : public ManeuverFrame{
public:

	InertialManeuverFrame();
	virtual ~InertialManeuverFrame();

protected:
    void            CalculateBasis(void);
};

#endif // InertialManeuverFrame_hpp
