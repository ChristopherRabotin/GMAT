//$Header$
//------------------------------------------------------------------------------
//                                  Parameter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/09/16
//
/**
 * Declares base class of parameters.
 */
//------------------------------------------------------------------------------
#ifndef Parameter_hpp
#define Parameter_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"

class GMAT_API Parameter : public GmatBase
{
public:

    enum ParameterKey
    {
        SYSTEM_PARAM, USER_PARAM, KeyCount
    };

    Parameter(const std::string &name, const std::string &typeStr,
              ParameterKey key, GmatBase *obj,
              const std::string &desc, const std::string &unit);
    Parameter(const Parameter &param);
    Parameter& operator= (const Parameter& right);
    virtual ~Parameter();

    ParameterKey GetKey() const;
    std::string GetDesc() const;
    std::string GetUnit() const;

    void SetKey(const ParameterKey &key);
    void SetDesc(const std::string &desc);
    void SetUnit(const std::string &desc);
    
    bool operator==(const Parameter &right) const;
    bool operator!=(const Parameter &right) const;

    virtual bool AddObject(GmatBase *object) = 0;
    virtual Integer GetNumObjects() const = 0;

    virtual void Evaluate() = 0;
    virtual bool Validate() = 0;
    virtual const std::string* GetParameterList() const;
    
protected:
    
    static const std::string PARAMETER_KEY_STRING[KeyCount];

    ParameterKey  mKey;
    std::string   mDesc;
    std::string   mUnit;

};
#endif // Parameter_hpp

