//$Header$
//------------------------------------------------------------------------------
//                              RefFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/08/25
//
/**
 * Declares base class of Reference Frame.
 */
//------------------------------------------------------------------------------
#ifndef RefFrame_hpp
#define RefFrame_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CelestialBody.hpp"
#include "A1Mjd.hpp"

class GMAT_API RefFrame : public GmatBase
{
public:

    RefFrame(const std::string &name, const std::string &typeStr,
             const A1Mjd &refDate, CelestialBody *centralBody);
    RefFrame(const RefFrame &copy);
    RefFrame& operator= (const RefFrame &right); 
    virtual ~RefFrame();

    //const CelestialBody* GetCentralBody() const;
    CelestialBody* GetCentralBody() const;
    void SetCentralBody(CelestialBody *centralBody);
    bool SetCentralBody(const std::string &name);
    std::string GetCentralBodyName() const;
    A1Mjd GetRefDate() const;

    bool operator== (const RefFrame &right) const;
    bool operator!= (const RefFrame &right) const;
 
    // methods inherited from GmatBase
    virtual std::string GetParameterText(const Integer id) const;
    virtual Integer GetParameterID(const std::string &str) const;
    virtual Gmat::ParameterType GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;

    virtual std::string GetStringParameter(const Integer id) const;
    virtual std::string GetStringParameter(const std::string &label) const;
    virtual bool SetStringParameter(const Integer id, const std::string &value);
    virtual bool SetStringParameter(const std::string &label,
                                    const std::string &value);
protected:
    
    //loj: 3/22/04 commented out, moved to public:
//      RefFrame(const CelestialBody &centralBody, const A1Mjd &refDate,
//               const std::string &frameName);

    enum
    {
        CENTRAL_BODY = GmatBaseParamCount,
        RefFrameParamCount
    };
    
    static const Gmat::ParameterType PARAMETER_TYPE[RefFrameParamCount];
    static const std::string PARAMETER_TEXT[RefFrameParamCount];

private:

    //loj: 3/22/04 commented out
    //const CelestialBody* mCentralBody;
    CelestialBody* mCentralBody;
    A1Mjd mRefDate;
};

#endif // RefFrame_hpp
