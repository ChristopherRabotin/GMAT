//$Header$
//------------------------------------------------------------------------------
//                                  OrbitData
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/09
//
/**
 * Implements Orbit related data class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "OrbitData.hpp"
#include "ParameterException.hpp"
#include "Rvector3.hpp"
#include "RealUtilities.hpp"
#include "EphemerisUtil.hpp"
#include "Spacecraft.hpp"

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------
const std::string
OrbitData::VALID_OBJECT_LIST[OrbitDataObjectCount] =
{
    "Spacecraft"
}; 

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// OrbitData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
OrbitData::OrbitData()
    : RefData()
{  
}

//------------------------------------------------------------------------------
// OrbitData(const OrbitData &data)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <data> the OrbitData object being copied.
 */
//------------------------------------------------------------------------------
OrbitData::OrbitData(const OrbitData &data)
    : RefData(data)
{
}

//------------------------------------------------------------------------------
// OrbitData& operator= (const OrbitData& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
OrbitData& OrbitData::operator= (const OrbitData& right)
{
    if (this != &right)
        RefData::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// ~OrbitData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
OrbitData::~OrbitData()
{
}

//------------------------------------------------------------------------------
// Real GetCartReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives Cartesian element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetCartReal(const std::string &str)
{
    GmatBase *obj = FindObject(VALID_OBJECT_LIST[SPACECRAFT]);
    
    if (obj == NULL)
    {
        return ORBIT_REAL_UNDEFINED;
    }
    else
    {
        if (str == "Epoch")
            return obj->GetRealParameter("Epoch");
        if (str == "CartX")
            return obj->GetRealParameter("X");
        if (str == "CartY")
            return obj->GetRealParameter("Y");
        if (str == "CartZ")
            return obj->GetRealParameter("Z");
        if (str == "CartVx")
            return obj->GetRealParameter("Vx");
        if (str == "CartVy")
            return obj->GetRealParameter("Vy");
        if (str == "CartVz")
            return obj->GetRealParameter("Vz");
    }
    return ORBIT_REAL_UNDEFINED;
}


//------------------------------------------------------------------------------
// Real GetKepReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives Keplerian element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetKepReal(const std::string &str)
{
    GmatBase *obj = FindObject(VALID_OBJECT_LIST[SPACECRAFT]);
    
    if (obj == NULL)
    {
        return ORBIT_REAL_UNDEFINED;
    }
    else
    {
        //------------------------------------------------------------
        //loj: 3/23/04 temp code until Spacecraft can compute Keplerian
        // elements without converting internal CoordRep to Keplerian
        //------------------------------------------------------------
       
        Integer id = obj->GetParameterID("CoordinateRepresentation");
        std::string elemType = obj->GetStringParameter(id);

        if (elemType == "Keplerian")
        {
            if (str == "KepSMA")
                return obj->GetRealParameter("SMA");
            if (str == "KepEcc")
                return obj->GetRealParameter("ECC");
            if (str == "KepInc")
                return obj->GetRealParameter("INC");
            if (str == "KepRAAN")
                return obj->GetRealParameter("RAAN");
            if (str == "KepAOP")
                return obj->GetRealParameter("AOP");
            if (str == "KepTA")
                return obj->GetRealParameter("TA");
            if (str == "KepMA")
                return obj->GetRealParameter("MA");
        }
        else if (elemType == "Cartesian")
        {
            Real *state = ((Spacecraft*)obj)->GetState();
            Real sma, ecc, inc, raan, aop, ma, ta;
            Real grav = 0.398600448073446198e+06; //loj: temp code for B2
            
            ToKeplerian(Rvector3(state[0], state[1], state[2]),
                        Rvector3(state[3], state[4], state[5]),
                        grav, sma, ecc, inc, raan, aop, ma);
            
            ta = MeanToTrueAnomaly(ma, ecc);
            
            if (str == "KepSMA")
                return sma;
            if (str == "KepEcc")
                return ecc;
            if (str == "KepInc")
                return inc;
            if (str == "KepRAAN")
                return raan;
            if (str == "KepAOP")
                return aop;
            if (str == "KepTA")
                return ta;
            if (str == "KepMA")
                return ma;
        }
    }
    return ORBIT_REAL_UNDEFINED;
}

//------------------------------------------------------------------------------
// Real GetOtherKepReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives other Keplerian element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetOtherKepReal(const std::string &str)
{
    GmatBase *obj = FindObject(VALID_OBJECT_LIST[SPACECRAFT]);
    
    if (obj == NULL)
    {
        return ORBIT_REAL_UNDEFINED;
    }
    else
    {
        Real sma = obj->GetRealParameter("SMA");
        Real ecc = obj->GetRealParameter("ECC");
        Integer id = obj->GetParameterID("ReferenceBody");
        std::string bodyName = obj->GetStringParameter(id);
        
        Real grav = 0.398600448073446198e+06; //loj: temp code for B2
        Real E, R;
        
        if (str == "KepMM")
        {
            return Sqrt((grav / sma ) / (sma * sma));
        }
        if (str == "VelApoapsis")
        {
            E = -grav / (2.0 * sma);
            R = sma * (1.0 - ecc);
            return Sqrt (2.0 * (E + grav/R));
        }
        if (str == "VelPeriapsis")
        {
            E = -grav / (2.0 * sma);
            R = sma * (1.0 + ecc);
            return Sqrt (2.0 * (E + grav/R));
        }
    }
    return ORBIT_REAL_UNDEFINED;
}

//------------------------------------------------------------------------------
// Real GetSphReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives Spherical element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetSphReal(const std::string &str)
{
    GmatBase *obj = FindObject(VALID_OBJECT_LIST[SPACECRAFT]);
    
    if (obj == NULL)
    {
        return ORBIT_REAL_UNDEFINED;
    }
    else
    {
        if (str == "SphRMag")
            return obj->GetRealParameter(1);
        if (str == "SphRA")
            return obj->GetRealParameter(2);
        if (str == "SphDec")
            return obj->GetRealParameter(3);;
        if (str == "SphVMag")
            return obj->GetRealParameter(4);;
        if (str == "SphVRA")
            return obj->GetRealParameter(5);
        if (str == "SphVDec")
            return obj->GetRealParameter(6);
    }
    return ORBIT_REAL_UNDEFINED;
}

//------------------------------------------------------------------------------
// Real GetAngularReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives angular related element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetAngularReal(const std::string &str)
{
    Real result = ORBIT_REAL_UNDEFINED;
    GmatBase *obj = FindObject(VALID_OBJECT_LIST[SPACECRAFT]);
    
    if (obj != NULL)
    {
        Rvector3 pos = Rvector3(obj->GetRealParameter("X"),
                                obj->GetRealParameter("Y"),
                                obj->GetRealParameter("Z"));
        Rvector3 vel = Rvector3(obj->GetRealParameter("Vx"),
                                obj->GetRealParameter("Vy"),
                                obj->GetRealParameter("Vz"));
        
        Rvector3 hVec3 = Cross(pos, vel);
        Real h = Sqrt(hVec3*hVec3);
        
        //grav = get_grav_constant(temp.coord.central_body); // Swingby
        Real grav = 0.398600448073446198e+06; //loj: temp code for B2 - use mu for now

        if (h < ORBIT_TOL)
        {
            result = 0.0;
        }
        else
        {
            if (str == "SemilatusRectum")
                result = (h / grav) * h;      // B M W; eq. 1.6-1
        }
    }

    return result;
}

//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* OrbitData::GetValidObjectList() const
{
    return VALID_OBJECT_LIST;
}

//------------------------------------------------------------------------------
// bool ValidateRefObjects(GmatBase *param)
//------------------------------------------------------------------------------
/**
 * Validates reference objects for given parameter
 */
//------------------------------------------------------------------------------
bool OrbitData::ValidateRefObjects(GmatBase *param)
{
    // assume all parameter derived from OrbitData needs Spacecraft
    // if some parameter needs additional object, add check here
    if (HasObject(VALID_OBJECT_LIST[SPACECRAFT]))
        return true;
    else
        return false;
}

//------------------------------------------------------------------------------
// bool IsValidObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Checks reference object type.
 *
 */
//------------------------------------------------------------------------------
bool OrbitData::IsValidObject(GmatBase *obj)
{
    bool valid = false;
    
    // check for object type if not NULL
    if (obj != NULL)
    {
        for (int i=0; i<OrbitDataObjectCount; i++)
        {
            if (obj->GetTypeName() == VALID_OBJECT_LIST[i])
            {
                valid = true;
                break;
            }
        }
    }
    else
    {
        valid = false;
    }

    return valid;

}
