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

#if !defined __UNIT_TEST__
#include "SolarSystem.hpp"
#endif

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
    mCartState = Rvector6::RVECTOR6_UNDEFINED;
    mKepState = Rvector6::RVECTOR6_UNDEFINED;
    mSphState = Rvector6::RVECTOR6_UNDEFINED;
    mCartEpoch = 0.0;
    mKepEpoch = 0.0;
    mSphEpoch = 0.0;
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
// Rvector6 GetCartState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetCartState()
{
    Spacecraft *sc = (Spacecraft*)FindObject(VALID_OBJECT_LIST[SPACECRAFT]);
    
    if (sc != NULL)
    {
        Integer id = sc->GetParameterID("CoordinateRepresentation");
        std::string elemType = sc->GetStringParameter(id);

        if (elemType == "Cartesian")
        {
            if (sc->GetRealParameter("Epoch") > mCartEpoch)
            {
                Real *cartState = sc->GetState(); // should be cartesian state
                mCartEpoch = sc->GetRealParameter("Epoch");

                for (int i=0; i<6; i++)
                    mCartState[i] = cartState[i];
                
            }
        }
        else if (elemType == "Keplerian")
        {
            if (sc->GetRealParameter("Epoch") > mCartEpoch)
            {
                Rvector3 pos, vel;
                Real grav = 0.398600448073446198e+06; //loj: temp code for B2
                
                Real *kepState = sc->GetState(); // should be keplerian state
                mCartEpoch = sc->GetRealParameter("Epoch");
                
                Real ma = TrueToMeanAnomaly(kepState[TA], kepState[ECC]);
                ToCartesian(kepState[SMA], kepState[ECC], kepState[INC],
                            kepState[RAAN], kepState[AOP], ma, grav,
                            pos, vel);

                mCartState[PX] = pos[0];
                mCartState[PY] = pos[1];
                mCartState[PZ] = pos[2];
                mCartState[VX] = vel[0];
                mCartState[VY] = vel[1];
                mCartState[VZ] = vel[2];
            }
        }
    }
    
    return mCartState;
}

//------------------------------------------------------------------------------
// Rvector6 GetKepState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetKepState()
{
    Spacecraft *sc = (Spacecraft*)FindObject(VALID_OBJECT_LIST[SPACECRAFT]);
    
    if (sc != NULL)
    {
        Integer id = sc->GetParameterID("CoordinateRepresentation");
        std::string elemType = sc->GetStringParameter(id);

        if (elemType == "Keplerian")
        {
            if (sc->GetRealParameter("Epoch") > mKepEpoch)
            {
                Real *kepState = sc->GetState(); // should be keplerian state
                mKepEpoch = sc->GetRealParameter("Epoch");

                for (int i=0; i<6; i++)
                    mKepState[i] = kepState[i];
                
            }
        }
        else if (elemType == "Cartesian")
        {
            if (sc->GetRealParameter("Epoch") > mCartEpoch)
            {
                Real *cartState = sc->GetState();     // should be cartesian state
                mCartEpoch = sc->GetRealParameter("Epoch");
                
                Real sma, ecc, inc, raan, aop, ma, ta;
                Real grav = 0.398600448073446198e+06; //loj: temp code for B2
                
                ToKeplerian(Rvector3(cartState[PX], cartState[PY], cartState[PZ]),
                            Rvector3(cartState[VX], cartState[VY], cartState[VZ]),
                            grav, sma, ecc, inc, raan, aop, ma);
                
                ta = MeanToTrueAnomaly(ma, ecc);

                mKepState[SMA] = sma;
                mKepState[ECC] = ecc;
                mKepState[INC] = inc;
                mKepState[RAAN] = raan;
                mKepState[AOP] = aop;
                mKepState[TA] = ta;
                mMA = ma;
            }
        }
    }
    
    return mKepState;
}

//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetSphState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetSphState()
{
    return mSphState;
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
    Rvector6 state = GetCartState();

    if (state.IsValid(ORBIT_REAL_UNDEFINED))
    {
        if (str == "Epoch")
            return mCartEpoch;
        if (str == "CartX")
            return mCartState[PX];
        if (str == "CartY")
            return mCartState[PY];
        if (str == "CartZ")
            return mCartState[PZ];
        if (str == "CartVx")
            return mCartState[VX];
        if (str == "CartVy")
            return mCartState[VY];
        if (str == "CartVz")
            return mCartState[VZ];
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
    Rvector6 state = GetKepState();

    if (state.IsValid(ORBIT_REAL_UNDEFINED))
    {        
        if (str == "KepSMA")
            return mKepState[SMA];
        if (str == "KepEcc")
            return mKepState[ECC];
        if (str == "KepInc")
            return mKepState[INC];
        if (str == "KepRAAN")
            return mKepState[RAAN];
        if (str == "KepAOP")
            return mKepState[AOP];
        if (str == "KepTA")
            return mKepState[TA];
        if (str == "KepMA")
            return mMA;
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
    Spacecraft *sc = (Spacecraft*)FindObject(VALID_OBJECT_LIST[SPACECRAFT]);
    
    if (sc != NULL)
    {
        Real sma = GetKepReal("KepSMA");
        Real ecc = GetKepReal("KepEcc");
        Integer id = sc->GetParameterID("ReferenceBody");
        std::string bodyName = sc->GetStringParameter(id);
        
        Real grav = 0.398600448073446198e+06; //loj: temp code for B2
        Real E, R;
        
        if (str == "KepMM")
        {
            return Sqrt((grav / sma ) / (sma * sma));
        }
        if (str == "VelApoapsis")
        {
            E = -grav / (2.0 * sma);
            R = sma * (1.0 + ecc);
            return Sqrt (2.0 * (E + grav/R));
        }
        if (str == "VelPeriapsis")
        {
            E = -grav / (2.0 * sma);
            R = sma * (1.0 - ecc);
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
    Spacecraft *sc = (Spacecraft*)FindObject(VALID_OBJECT_LIST[SPACECRAFT]);
    
    if (sc != NULL)
    {
        if (str == "SphRMag")
            return sc->GetRealParameter(1);
        if (str == "SphRA")
            return sc->GetRealParameter(2);
        if (str == "SphDec")
            return sc->GetRealParameter(3);
        if (str == "SphVMag")
            return sc->GetRealParameter(4);
        if (str == "SphVRA")
            return sc->GetRealParameter(5);
        if (str == "SphVDec")
            return sc->GetRealParameter(6);
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
    Spacecraft *sc = (Spacecraft*)FindObject(VALID_OBJECT_LIST[SPACECRAFT]);
    
    if (sc != NULL)
    {
        Rvector3 pos = Rvector3(sc->GetRealParameter("X"),
                                sc->GetRealParameter("Y"),
                                sc->GetRealParameter("Z"));
        Rvector3 vel = Rvector3(sc->GetRealParameter("Vx"),
                                sc->GetRealParameter("Vy"),
                                sc->GetRealParameter("Vz"));
        
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

