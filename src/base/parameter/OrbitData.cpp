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

#include <math.h>

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
            return obj->GetRealParameter(0); //("Epoch"); -- just to compile
        if (str == "CartX")
            return obj->GetRealParameter(1); //("CartX");
        if (str == "CartY")
            return obj->GetRealParameter(2); //("CartY");
        if (str == "CartZ")
            return obj->GetRealParameter(3); //("CartZ");
        if (str == "CartVx")
            return obj->GetRealParameter(4); //("CartVx");
        if (str == "CartVy")
            return obj->GetRealParameter(5); //("CartVy");
        if (str == "CartVz")
            return obj->GetRealParameter(6); //("CartVz");
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
        if (str == "KepSma")
            return obj->GetRealParameter(1); //("KepSma");
        if (str == "KepEcc")
            return obj->GetRealParameter(2); //("KepEcc");
        if (str == "KepInc")
            return obj->GetRealParameter(3); //("KepInc");
        if (str == "KepRaan")
            return obj->GetRealParameter(4); //("KepRaan");
        if (str == "KepAop")
            return obj->GetRealParameter(5); //("KepAop");
        if (str == "KepTa")
            return obj->GetRealParameter(6); //("KepTa");
        if (str == "KepMa")
            return obj->GetRealParameter(6); //("KepMa");
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
        Real sma = obj->GetRealParameter(1); //("KepSma"); -- just to compile
        Real ecc = obj->GetRealParameter(2); //("KepEcc"); -- just to compile
        //Real grav = obj->GetRealParameter("RefBody");
        Real grav = 3.986005e14; //loj: temp code
        Real E, R;
        
        if (str == "KepMm")
        {
            return sqrt((grav / sma ) / (sma * sma));
        }
        if (str == "VelApoapsis")
        {
            E = -grav / (2.0 * sma);
            R = sma * (1.0 - ecc);
            return sqrt (2.0 * (E + grav/R));
        }
        if (str == "VelPeriapsis")
        {
            E = -grav / (2.0 * sma);
            R = sma * (1.0 + ecc);
            return sqrt (2.0 * (E + grav/R));
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
        if (str == "SphRmag")
            return obj->GetRealParameter(1); //("SphRmag");
        if (str == "SphRa")
            return obj->GetRealParameter(2); //("SphRa");
        if (str == "SphDec")
            return obj->GetRealParameter(3); //("SphDec");
        if (str == "SphVmag")
            return obj->GetRealParameter(4); //("SphVmag");
        if (str == "SphRaV")
            return obj->GetRealParameter(5); //("SphRaV");
        if (str == "SphDecV")
            return obj->GetRealParameter(6); //("SphDecV");
    }
    return ORBIT_REAL_UNDEFINED;
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
// bool CheckRefObjectType(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Checks reference object type.
 *
 */
//------------------------------------------------------------------------------
bool OrbitData::CheckRefObjectType(GmatBase *obj)
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
