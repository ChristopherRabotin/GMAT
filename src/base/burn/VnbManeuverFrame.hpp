#ifndef VnbManeuverFrame_hpp
#define VnbManeuverFrame_hpp

#include "ManeuverFrame.hpp"

class VnbManeuverFrame : public ManeuverFrame
{
public:
	VnbManeuverFrame();
	virtual ~VnbManeuverFrame();

    virtual std::string GetFrameLabel(Integer id);

protected:
    void                CalculateBasis(void);
};

#endif // VnbManeuverFrame_hpp
