//$Header$
//------------------------------------------------------------------------------
//                                TimeReal
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/04/28
//
/**
 * Declares TimeReal class which provides base class for time realated Real
 * Parameters
 */
//------------------------------------------------------------------------------
#ifndef TimeReal_hpp
#define TimeReal_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "TimeData.hpp"


class GMAT_API TimeReal : public RealVar, public TimeData
{
public:

   TimeReal(const std::string &name, const std::string &typeStr, 
             ParameterKey key, GmatBase *obj, const std::string &desc,
             const std::string &unit, bool isTimeParam);
   TimeReal(const TimeReal &copy);
   TimeReal& operator=(const TimeReal &right);
   virtual ~TimeReal();

   // The inherited methods from RealVar
   virtual Real GetReal();
   virtual Real EvaluateReal();

   // The inherited methods from Parameter
   virtual Integer GetNumObjects() const;
   virtual GmatBase* GetObject(const std::string &objTypeName);
    
   virtual bool SetObject(Gmat::ObjectType objType,
                          const std::string &objName,
                          GmatBase *obj);
    
   virtual bool AddObject(GmatBase *obj);
   virtual bool Validate();
   virtual void Initialize();

protected:

};

#endif // TimeReal_hpp
