#include "VnbManeuverFrame.hpp"


VnbManeuverFrame::VnbManeuverFrame()
{
}


VnbManeuverFrame::~VnbManeuverFrame()
{
}


void VnbManeuverFrame::CalculateBasis(void)
{
    if (!position || !velocity)
        throw BurnException("VNB frame undefined -- no position/velocity data.");
        
    // "X" direction is towards velocity
    Real temp[3], mag;
    memcpy(temp, velocity, 3*sizeof(Real));
    mag = sqrt(temp[0]*temp[0] + temp[1]*temp[1] + temp[2]*temp[2]);
    if (mag == 0.0)
        throw BurnException("VNB frame undefined for velocity = 0.0.");
    basisMatrix[0][0] = temp[0] / mag;
    basisMatrix[1][0] = temp[1] / mag;
    basisMatrix[2][0] = temp[2] / mag;
    
    // Normal direction defined by r cross v
    temp[0] = position[1]*velocity[2] - position[2]*velocity[1];
    temp[1] = position[2]*velocity[0] - position[0]*velocity[2];
    temp[2] = position[0]*velocity[1] - position[1]*velocity[0];
    mag = sqrt(temp[0]*temp[0] + temp[1]*temp[1] + temp[2]*temp[2]);
    if (mag == 0.0)
        throw BurnException("VNB frame undefined for |r x v| = 0.0.");
    basisMatrix[0][1] = temp[0] / mag;
    basisMatrix[1][1] = temp[1] / mag;
    basisMatrix[2][1] = temp[2] / mag;

    // Binormal direction defined by v cross n
    temp[0] = basisMatrix[1][0]*basisMatrix[2][1] - 
              basisMatrix[2][0]*basisMatrix[1][1];
    temp[1] = basisMatrix[2][0]*basisMatrix[0][1] - 
              basisMatrix[0][0]*basisMatrix[2][1];
    temp[2] = basisMatrix[0][0]*basisMatrix[1][1] - 
              basisMatrix[1][0]*basisMatrix[0][1];
    mag = sqrt(temp[0]*temp[0] + temp[1]*temp[1] + temp[2]*temp[2]);
    if (mag == 0.0)
        throw BurnException("VNB frame undefined for |v x n| = 0.0.");
    basisMatrix[0][2] = temp[0] / mag;
    basisMatrix[1][2] = temp[1] / mag;
    basisMatrix[2][2] = temp[2] / mag;
}


std::string VnbManeuverFrame::GetFrameLabel(Integer id)
{
    switch (id) {
        case 1:
            return "V";
        case 2:
            return "N";
        case 3:
            return "B";
        default:
            ;
    }
    return "Undefined";
}
