//$Header$
//------------------------------------------------------------------------------
//                                DragForce
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
 * Drag force modeling
 */
//------------------------------------------------------------------------------


#include "DragForce.hpp"
#include "ForceModelException.hpp"

DragForce::DragForce() :
    PhysicalModel           (Gmat::PHYSICAL_MODEL, "DragForce"),
    sunPosition             (NULL),
    cbPosition              (NULL),
    angVel                  (NULL),
    useExternalAtmosphere   (true),
    atmos                   (NULL),
    ss                      (NULL),
    density                 (NULL),
    prefactor               (NULL)
{
    dimension = 6;
    
    /// @todo Remove hard coded central body parms in DragForce
    sunPosition = new Real[3];
    cbPosition  = new Real[3];
    angVel      = new Real[3];
       
    sunPosition[0] = 138276412034.25;
    sunPosition[1] = -71626341186.98;
    sunPosition[2] =    119832241.16;
    
    cbPosition[0]  = 0.0;
    cbPosition[1]  = 0.0;
    cbPosition[2]  = 0.0;
    
    angVel[0]      = 0.0;
    angVel[1]      = 0.0;
    angVel[2]      = 7.29211585530e-5;
}


DragForce::~DragForce()
{
    if (!useExternalAtmosphere && atmos)
        delete atmos;
        
    if (density)
        delete [] density;
        
    if (prefactor)
        delete [] prefactor;

    /// @todo Remove the following delet calls when SS is integrated
    delete [] sunPosition;
    delete [] cbPosition;
    delete [] angVel;
}


DragForce::DragForce(const DragForce& df) :
    PhysicalModel           (df),
    sunPosition             (NULL),
    cbPosition              (NULL),
    angVel                  (NULL),
    useExternalAtmosphere   (true),
    atmos                   (NULL),
    ss                      (NULL),
    density                 (NULL),
    prefactor               (NULL)
{
    /// @todo Remove hard coded central body parms in DragForce
    sunPosition = new Real[3];
    cbPosition  = new Real[3];
    angVel      = new Real[3];
       
    sunPosition[0] = 138276412034.25;
    sunPosition[1] = -71626341186.98;
    sunPosition[2] =    119832241.16;
    
    cbPosition[0]  = 0.0;
    cbPosition[1]  = 0.0;
    cbPosition[2]  = 0.0;
    
    angVel[0]      = 0.0;
    angVel[1]      = 0.0;
    angVel[2]      = 7.29211585530e-5;
}


DragForce& DragForce::operator=(const DragForce& df)
{
    if (this == &df)
        return *this;
        
    return *this;
}

 
bool DragForce::GetComponentMap(Integer * map, Integer order) const
{
    Integer i6;

    if (order != 1)
        throw ForceModelException("Drag supports 1st order equations of motion only");

    // Calculate how many spacecraft are in the model
    Integer satCount = (Integer)(dimension / 6);
    for (Integer i = 0; i < satCount; i++) 
    {
        i6 = i * 6;

        map[ i6 ] = i6 + 3;
        map[i6+1] = i6 + 4;
        map[i6+2] = i6 + 5;
        map[i6+3] = -1;
        map[i6+4] = -1;
        map[i6+5] = -1;
    }

    return true;
}

//------------------------------------------------------------------------------
// void SetSatelliteParameter(const Integer i, const std::string parmName, 
//                            const Real parm)
//------------------------------------------------------------------------------
/**
 * Passes spacecraft parameters to the force model.
 * 
 * For drag modeling, this method is used to set or update C_d, area, and mass.
 * 
 * @param i ID for the spacecraft
 * @param parmName name of the Spacecraft parameter 
 * @param parm Parameter value
 */
//------------------------------------------------------------------------------
void DragForce::SetSatelliteParameter(const Integer i, 
                                      const std::string parmName, 
                                      const Real parm)
{
    unsigned parmNumber = (unsigned)(i+1);
    
    if (parmName == "Mass")
        if (parmNumber < mass.size())
            mass[i] = parm;
        else
            mass.push_back(parm);
    if (parmName == "CoefficientDrag")
        if (parmNumber < dragCoeff.size())
            dragCoeff[i] = parm;
        else
            dragCoeff.push_back(parm);
    if (parmName == "IncidentArea")
        if (parmNumber < area.size())
            area[i] = parm;
        else
            area.push_back(parm);
}


//------------------------------------------------------------------------------
// void SetSatelliteParameter(const Integer i, const std::string parmName, 
//                            const std::string parm)
//------------------------------------------------------------------------------
/**
 * Passes spacecraft parameters to the force model.
 * 
 * For drag modeling, this method is used to set the body with the atmosphere.
 * 
 * @param i ID for the spacecraft
 * @param parmName name of the Spacecraft parameter 
 * @param parm Parameter value
 */
//------------------------------------------------------------------------------
void DragForce::SetSatelliteParameter(const Integer i, 
                                      const std::string parmName, 
                                      const std::string parm)
{
    unsigned parmNumber = (unsigned)(i+1);
    
    if (parmName == "ReferenceBody")
        if (parmNumber < mass.size())
            dragBody[i] = parm;
        else
            dragBody.push_back(parm);
}


bool DragForce::Initialize(void)
{
    PhysicalModel::Initialize();

    satCount = dimension / 6;
    if (satCount <= 0)
        throw ForceModelException("Drag called with dimension zero");
        
    density   = new Real[satCount];
    prefactor = new Real[satCount];

    for (Integer i = 0; i < satCount; ++i) {
        prefactor[i] = -0.5 * 2.2 * 15.0 / 875.0;   // Dummy up the product
    }
    
    return true;
}


void DragForce::BuildPrefactors(void) 
{
    for (Integer i = 0; i < satCount; ++i) {
        if (mass.size() < (unsigned)(i+1))
            throw ForceModelException("Spacecraft not set correctly");
        if (mass[i] <= 0.0) {
            std::string errorMsg = "Spacecraft ";
            errorMsg += i;
            errorMsg += " has non-physical mass; Drag modeling cannot be used.";
            throw ForceModelException(errorMsg); 
        }
        prefactor[i] = -0.5 * dragCoeff[i] * area[i] / mass[i];
    }
}


bool DragForce::GetDerivatives(Real *state, Real dt, Integer order)
{
    Integer i, i6;
    Real vRelative[3], vRelMag, factor;
    
    GetDensity(state);
    
    for (i = 0; i < satCount; ++i) {
        i6 = i * 6;

        // v_rel = v - w x R
        vRelative[0] = state[i6+3] - 
                       (angVel[1]*state[i6+2] - angVel[2]*state[i6+1]);
        vRelative[1] = state[i6+4] - 
                       (angVel[2]*state[ i6 ] - angVel[0]*state[i6+2]);
        vRelative[2] = state[i6+5] - 
                       (angVel[0]*state[i6+1] - angVel[1]*state[ i6 ]);
        vRelMag = sqrt(vRelative[0]*vRelative[0] + vRelative[1]*vRelative[1] + 
                       vRelative[2]*vRelative[2]);
        
        factor = prefactor[i] * density[i];
        if (order == 1) 
        {
            // Do dv/dt first, in case deriv = state
            deriv[3+i6] = factor * vRelMag * vRelative[0];
            deriv[4+i6] = factor * vRelMag * vRelative[1];
            deriv[5+i6] = factor * vRelMag * vRelative[2];
            // dr/dt = v
            deriv[i6]   = state[3+i6];
            deriv[1+i6] = state[4+i6];
            deriv[2+i6] = state[5+i6];
        }
        else 
        {
            // Feed accelerations to corresponding components directly for RKN
            deriv[ i6 ] = factor * vRelMag * vRelative[0];
            deriv[1+i6] = factor * vRelMag * vRelative[1];
            deriv[2+i6] = factor * vRelMag * vRelative[2];
            deriv[3+i6] = 0.0; 
            deriv[4+i6] = 0.0; 
            deriv[5+i6] = 0.0; 
        }
    }
    
    return true;
}


std::string DragForce::GetParameterText(const Integer id) const
{
    return PhysicalModel::GetParameterText(id);
}


Integer DragForce::GetParameterID(const std::string &str) const
{
    return PhysicalModel::GetParameterID(str);
}


Gmat::ParameterType DragForce::GetParameterType(const Integer id) const
{
    return PhysicalModel::GetParameterType(id);
}


std::string DragForce::GetParameterTypeString(const Integer id) const
{
    return PhysicalModel::GetParameterTypeString(id);
}


Integer DragForce::GetIntegerParameter(const Integer id) const
{
    return PhysicalModel::GetIntegerParameter(id);
}


Integer DragForce::SetIntegerParameter(const Integer id, const Integer value)
{
    return PhysicalModel::SetIntegerParameter(id, value);
}


Real DragForce::GetRealParameter(const Integer id) const
{
    return PhysicalModel::GetRealParameter(id);
}


Real DragForce::SetRealParameter(const Integer id, const Real value)
{
    return PhysicalModel::SetRealParameter(id, value);
}


void DragForce::GetDensity(Real *state)
{
    // Give it a default value if the atmosphere model is not set
    if (!atmos) {
        for (Integer i = 0; i < satCount; ++i)
            density[i] = 4.0e-13;
    }
    else
        atmos->Density(state, density, satCount);
}
