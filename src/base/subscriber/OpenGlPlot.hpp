//$Header$
//------------------------------------------------------------------------------
//                                  OpenGlPlot
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/12/16
//
/**
 * Declares OpenGlPlot class.
 */
//------------------------------------------------------------------------------
#ifndef OpenGlPlot_hpp
#define OpenGlPlot_hpp

#include "Subscriber.hpp"
#include <sstream>

class OpenGlPlot : public Subscriber
{
public:
    OpenGlPlot(const std::string &name);
    virtual ~OpenGlPlot(void);
    
    virtual std::string GetParameterText(const Integer id) const;
    virtual Integer     GetParameterID(const std::string &str) const;
    virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;

    virtual bool        GetBooleanParameter(const Integer id) const;
    virtual bool        SetBooleanParameter(const Integer id,
                                            const bool value);

protected:

    bool mDrawAxis;
    bool mDrawEquatorialPlane;
    bool mDrawWireFrame;
    
    Integer mDataCollectFrequency;
    Integer mUpdatePlotFrequency;

    Integer mNumData;
    Integer mNumCollected;
    
    enum
    {
        DRAW_AXIS = 0,
        DRAW_EQUATORIAL_PLANE,
        DRAW_WIRE_FRAME,
        DATA_COLLECT_FREQUENCY,
        UPDATE_PLOT_FREQUENCY,
        OpenGlPlotParamCount
    };
    
    static const Gmat::ParameterType PARAMETER_TYPE[OpenGlPlotParamCount];
    static const std::string PARAMETER_TEXT[OpenGlPlotParamCount];
    
    virtual bool Distribute(Integer len);
    virtual bool Distribute(const Real * dat, Integer len);
};

#endif
