//$Header$
//------------------------------------------------------------------------------
//                                  RefData
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
 * Declares base class of reference data.
 */
//------------------------------------------------------------------------------
#ifndef RefData_hpp
#define RefData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include <map>

class GMAT_API RefData
{
public:

    RefData();
    RefData(const RefData &data);
    RefData& operator= (const RefData& right);
    virtual ~RefData();

    bool AddRefObject(GmatBase *obj);
    Integer GetNumRefObjects() const;
    
    virtual bool ValidateRefObjects(GmatBase *param) = 0;
    virtual const std::string* GetValidObjectList() const;

protected:
    bool HasObject(const std::string &objTypeName) const;
    GmatBase* FindObject(const std::string &objTypeName) const;
     
    virtual bool CheckRefObjectType(GmatBase *obj) = 0;
    std::map<std::string, GmatBase*> *mStringObjectMap;

    Integer mNumRefObjects;
};
#endif // RefData_hpp

