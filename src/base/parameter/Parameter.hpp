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
#include "Rvector6.hpp"

class GMAT_API Parameter : public GmatBase
{
public:

    enum ParameterKey
    {
        SYSTEM_PARAM, USER_PARAM, KeyCount
    };

    Parameter(const std::string &name, const std::string &typeStr,
              ParameterKey key, GmatBase *obj,
              const std::string &desc, const std::string &unit,
              bool isTimeParam);
    Parameter(const Parameter &copy);
    Parameter& operator= (const Parameter& right);
    virtual ~Parameter();

    ParameterKey GetKey() const;
    std::string GetDesc() const;
    std::string GetUnit() const;
    bool IsTimeParameter() const;

    void SetKey(const ParameterKey &key);
    void SetDesc(const std::string &desc);
    void SetUnit(const std::string &unit);
    
    bool operator==(const Parameter &right) const;
    bool operator!=(const Parameter &right) const;

    virtual Real EvaluateReal();
    virtual Rvector6 EvaluateRvector6();
    
    virtual const std::string* GetParameterList() const;

    virtual StringArray& GetObjectTypeNames();
    virtual StringArray& GetObjectNames();
    virtual GmatBase* GetObject(const std::string &objTypeName);
    virtual bool SetObject(Gmat::ObjectType objType,
                           const std::string &objName,
                           GmatBase *obj);
    
    virtual bool AddObject(const std::string &name);
    virtual bool AddObject(GmatBase *object) = 0;
    virtual Integer GetNumObjects() const = 0;
    virtual bool Evaluate() = 0;
    virtual bool Validate() = 0;
    
    // methods inherited from GmatBase
    virtual std::string GetParameterText(const Integer id) const;
    virtual Integer GetParameterID(const std::string &str) const;
    virtual Gmat::ParameterType GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;

    virtual std::string GetStringParameter(const Integer id) const;
    virtual bool SetStringParameter(const Integer id, const std::string &value);
    virtual std::string GetStringParameter(const std::string &label) const;
    virtual bool SetStringParameter(const std::string &label,
                                    const std::string &value);
protected:

    void ManageObject(GmatBase *obj);
    
    static const std::string PARAMETER_KEY_STRING[KeyCount];

    ParameterKey  mKey;
    std::string   mDesc;
    std::string   mUnit;
    bool mIsTimeParam;

    StringArray mObjectTypeNames;
    StringArray mObjectNames;
    Integer mNumObjects;
    
    enum
    {
        OBJECT_NAME = 0,
        ParameterParamCount
    };
    
    static const Gmat::ParameterType PARAMETER_TYPE[ParameterParamCount];
    static const std::string PARAMETER_TEXT[ParameterParamCount];
};
#endif // Parameter_hpp

