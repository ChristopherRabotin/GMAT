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
 * Implements OpenGlPlot class.
 */
//------------------------------------------------------------------------------
#include <iomanip>
#include "OpenGlPlot.hpp"
#include "PlotInterface.hpp" // for UpdatePlot()

//---------------------------------
// static data
//---------------------------------
const std::string
OpenGlPlot::PARAMETER_TEXT[OpenGlPlotParamCount] =
{
    "DrawAxis",
    "DrawEquatorialPlane",
    "DrawWireFrame"
}; 

const Gmat::ParameterType
OpenGlPlot::PARAMETER_TYPE[OpenGlPlotParamCount] =
{
   Gmat::BOOLEAN_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::BOOLEAN_TYPE
};

//------------------------------------------------------------------------------
// OpenGlPlot(const std::string &name)
//------------------------------------------------------------------------------
OpenGlPlot::OpenGlPlot(const std::string &name)
    : Subscriber      ("OpenGlPlot", name)
{
    // GmatBase data
    parameterCount = OpenGlPlotParamCount;

    drawAxis = false;
    drawEquatorialPlane = true;
    drawWireFrame = false;
}

//------------------------------------------------------------------------------
// ~OpenGlPlot(void)
//------------------------------------------------------------------------------
OpenGlPlot::~OpenGlPlot(void)
{
}

//------------------------------------------------------------------------------
// bool OpenGlPlot::Distribute(int len)
//------------------------------------------------------------------------------
bool OpenGlPlot::Distribute(int len)
{
    //loj: How do I convert data to Real data?
    return false;
}

//------------------------------------------------------------------------------
// bool Distribute(const Real * dat, Integer len)
//------------------------------------------------------------------------------
bool OpenGlPlot::Distribute(const Real * dat, Integer len)
{
    if (len == 0)
        return false;
    else
        //loj: assumes data in time, x, y, z order
        return PlotInterface::UpdateSpacecraft(dat[0], dat[1], dat[2], dat[3]);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string OpenGlPlot::GetParameterText(const Integer id) const
{
    if (id >= DRAW_AXIS && id <= DRAW_WIRE_FRAME)
        return PARAMETER_TEXT[id];
    else
        return Subscriber::GetParameterText(id);
    
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer OpenGlPlot::GetParameterID(const std::string &str) const
{
   for (int i=0; i<OpenGlPlotParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i])
          return i;
   }
   
   return Subscriber::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType OpenGlPlot::GetParameterType(const Integer id) const
{
    if (id >= DRAW_AXIS && id <= DRAW_WIRE_FRAME)
        return PARAMETER_TYPE[id];
    else
        return Subscriber::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string OpenGlPlot::GetParameterTypeString(const Integer id) const
{
    if (id >= DRAW_AXIS && id <= DRAW_WIRE_FRAME)
        return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
    else
       return Subscriber::GetParameterTypeString(id);
    
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
bool OpenGlPlot::GetBooleanParameter(const Integer id) const
{
    switch (id)
    {
    case DRAW_AXIS:
        return drawAxis;
    case DRAW_EQUATORIAL_PLANE:
        return drawEquatorialPlane;
    case DRAW_WIRE_FRAME:
        return drawWireFrame;
    default:
        return Subscriber::GetBooleanParameter(id);
    }
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const bool id, const bool value)
//------------------------------------------------------------------------------
bool OpenGlPlot::SetBooleanParameter(const Integer id, const bool value)
{
    switch (id)
    {
    case DRAW_AXIS:
        drawAxis = value;
        return drawAxis;
    case DRAW_EQUATORIAL_PLANE:
        drawEquatorialPlane = value;
        return drawEquatorialPlane;
    case DRAW_WIRE_FRAME:
        drawWireFrame = value;
        return drawWireFrame;
    default:
        return Subscriber::SetBooleanParameter(id, value);
    }
}
