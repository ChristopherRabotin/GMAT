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
 * Declares Orbit related data class.
 */
//------------------------------------------------------------------------------
#ifndef OrbitData_hpp
#define OrbitData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Rvector6.hpp"

class GMAT_API OrbitData : public RefData
{
public:

    OrbitData();
    OrbitData(const OrbitData &data);
    OrbitData& operator= (const OrbitData& right);
    virtual ~OrbitData();

    Rvector6 GetCartState();
    Rvector6 GetKepState();
    Rvector6 GetSphState();
    
    Real GetCartReal(const std::string &str);
    Real GetKepReal(const std::string &str);
    Real GetOtherKepReal(const std::string &str);
    Real GetSphReal(const std::string &str);
    Real GetAngularReal(const std::string &str);
    
    // The inherited methods from RefData
    virtual bool ValidateRefObjects(GmatBase *param);
    virtual const std::string* GetValidObjectList() const;

    const static Real ORBIT_REAL_UNDEFINED = -9876543210.1234;
    const static Real ORBIT_TOL = 1.0e-10;

protected:
    
    Rvector6 mCartState;
    Rvector6 mKepState;
    Rvector6 mSphState;
    
    Real mMA;
    Real mCartEpoch;
    Real mKepEpoch;
    Real mSphEpoch;
    
    // The inherited methods from RefData
    virtual bool IsValidObject(GmatBase *obj);

    enum {PX, PY, PZ, VX, VY, VZ};
    enum {SMA, ECC, INC, RAAN, AOP, TA};
    
    enum
    {
        SPACECRAFT = 0,
        OrbitDataObjectCount
    };
    
    static const std::string VALID_OBJECT_LIST[OrbitDataObjectCount];
};
#endif // OrbitData_hpp

