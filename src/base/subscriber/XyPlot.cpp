//$Header$
//------------------------------------------------------------------------------
//                                  XyPlot
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/22
//
/**
 * Implements XyPlot class.
 */
//------------------------------------------------------------------------------
#include <iomanip>
#include "XyPlot.hpp"
#include "PlotInterface.hpp" // for XY plot
#include "Moderator.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
XyPlot::PARAMETER_TEXT[XyPlotParamCount] =
{
    "XParamName",
    "YParamName",
    "YParamNameList",
    "ClearYParamList",
    "PlotTitle",
    "XAxisTitle",
    "YAxisTitle",
    "DrawGrid",
    "DataCollectFrequency",
    "UpdatePlotFrequency",
}; 

const Gmat::ParameterType
XyPlot::PARAMETER_TYPE[XyPlotParamCount] =
{
    Gmat::STRING_TYPE,
    Gmat::STRING_TYPE,
    Gmat::STRINGARRAY_TYPE,
    Gmat::BOOLEAN_TYPE,
    Gmat::STRING_TYPE,
    Gmat::STRING_TYPE,
    Gmat::STRING_TYPE,
    Gmat::BOOLEAN_TYPE,
    Gmat::INTEGER_TYPE,
    Gmat::INTEGER_TYPE,
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// XyPlot(const std::string &name, Parameter *xParam,
//        Parameter *firstYParam, const std::string &plotTitle,
//        const std::string &xAxisTitle, const std::string &yAxisTitle,
//        bool drawGrid)
//------------------------------------------------------------------------------
XyPlot::XyPlot(const std::string &name, Parameter *xParam,
               Parameter *firstYParam, const std::string &plotTitle,
               const std::string &xAxisTitle, const std::string &yAxisTitle,
               bool drawGrid)
    : Subscriber("XyPlot", name)
{
    // GmatBase data
    parameterCount = XyPlotParamCount;
    
    if (plotTitle == "")
        mPlotTitle = "XyPlot";
    if (xAxisTitle == "")
        mXAxisTitle = "X-Axis Title";
    if (yAxisTitle == "")
        mYAxisTitle = "Y-Axis Title";
    
    mDrawGrid = drawGrid;
    mNumYParams = 0;

    mXParamName = "UndefinedXParam";
    mNumXParams = 0;
    
    mXParam = xParam;
    if (firstYParam != NULL)
        AddYParameter(firstYParam);

    mIsXyPlotWindowSet = false;
    mFirstXVal = 0.0;
    
    mDataCollectFrequency = 1;
    mUpdatePlotFrequency = 10;
}

//------------------------------------------------------------------------------
// ~XyPlot(void)
//------------------------------------------------------------------------------
XyPlot::~XyPlot(void)
{
}

//------------------------------------------------------------------------------
// Integer GetNumYParameters()
//------------------------------------------------------------------------------
Integer XyPlot::GetNumYParameters()
{
    return mNumYParams;
}

//------------------------------------------------------------------------------
// bool SetXParameter(const std::string &paramName)
//------------------------------------------------------------------------------
bool XyPlot::SetXParameter(const std::string &paramName)
{
    bool status = false;
    Moderator *theModerator = Moderator::Instance();

    if (paramName != "")
    {
        // get parameter pointer
        Parameter *param = theModerator->GetParameter(paramName);
        if (param != NULL)
        {
            SetXParameter(param);
            status = true;
            mNumXParams = 1; //loj: only 1 X parameter for now
        }
    }

    return status;
}

//------------------------------------------------------------------------------
// bool SetXParameter(Parameter *param)
//------------------------------------------------------------------------------
bool XyPlot::SetXParameter(Parameter *param)
{
    //loj: Do I really need to validate parameter before set?
    if (param->Validate())
    {
        mXParamName = param->GetName();
        mXParam = param;
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
// bool AddYParameter(const std::string &paramName)
//------------------------------------------------------------------------------
bool XyPlot::AddYParameter(const std::string &paramName)
{
    bool status = false;
    Moderator *theModerator = Moderator::Instance();
    
    if (paramName != "")
    {
        // get parameter pointer
        Parameter *param = theModerator->GetParameter(paramName);
        if (param != NULL)
        {
            AddYParameter(param);
            status = true;
        }
    }

    return status;
}

//------------------------------------------------------------------------------
// bool AddYParameter(Parameter *param)
//------------------------------------------------------------------------------
bool XyPlot::AddYParameter(Parameter *param)
{
    bool added = false;
    
    //loj: Do I really need to validate parameter before add?
    if (param->Validate())
    {
        //MessageInterface::ShowMessage("XyPlot::AddYParameter() param name = %s\n",
        //                              param->GetName().c_str());
        mYParamNames.push_back(param->GetName());
        mYParams.push_back(param);
        mNumYParams = mYParams.size();

        added = true;
    }

    return added;
}

//---------------------------------
// methods inherited from GmatBase
//---------------------------------

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string XyPlot::GetParameterText(const Integer id) const
{
    if (id >= X_PARAM_NAME && id <= DRAW_GRID)
        return PARAMETER_TEXT[id];
    else
        return Subscriber::GetParameterText(id);
    
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer XyPlot::GetParameterID(const std::string &str) const
{
   for (int i=0; i<XyPlotParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i])
          return i;
   }
   
   return Subscriber::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType XyPlot::GetParameterType(const Integer id) const
{
    if (id >= X_PARAM_NAME && id <= DRAW_GRID)
        return PARAMETER_TYPE[id];
    else
        return Subscriber::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string XyPlot::GetParameterTypeString(const Integer id) const
{
    if (id >= X_PARAM_NAME && id <= DRAW_GRID)
        return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
    else
       return Subscriber::GetParameterTypeString(id);
    
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
bool XyPlot::GetBooleanParameter(const Integer id) const
{
    switch (id)
    {
    case DRAW_GRID:
        return mDrawGrid;
    default:
        return Subscriber::GetBooleanParameter(id);
    }
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
bool XyPlot::SetBooleanParameter(const Integer id, const bool value)
{
    switch (id)
    {
    case DRAW_GRID:
        mDrawGrid = value;
        return mDrawGrid;
    case CLEAR_Y_PARAM_LIST:
        mYParams.clear();
        mYParamNames.clear();
        mNumYParams = 0;
        return true;
    default:
        return Subscriber::SetBooleanParameter(id, value);
    }
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
bool XyPlot::GetBooleanParameter(const std::string &label) const
{
    for (int i=0; i<XyPlotParamCount; i++)
        if (label == PARAMETER_TEXT[i])
            return GetBooleanParameter(i);

    return Subscriber::GetBooleanParameter(label);
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label,
//                          const bool value)
//------------------------------------------------------------------------------
bool XyPlot::SetBooleanParameter(const std::string &label,
                                 const bool value)
{
    for (int i=0; i<XyPlotParamCount; i++)
        if (label == PARAMETER_TEXT[i])
            return SetBooleanParameter(i, value);

    return Subscriber::SetBooleanParameter(label, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string XyPlot::GetStringParameter(const Integer id) const
{
    switch (id)
    {
    case X_PARAM_NAME:
        if (mNumXParams == 1)
            return mXParamName;
        else
            return Subscriber::GetStringParameter(id);
    case Y_PARAM_NAME:
        if (mNumYParams > 0)
            return mYParamNames[0]; // first Y param name
        else
            return Subscriber::GetStringParameter(id);
    case PLOT_TITLE:
        return mPlotTitle;
    case X_AXIS_TITLE:
        return mXAxisTitle;
    case Y_AXIS_TITLE:
        return mYAxisTitle;
    default:
        return Subscriber::GetStringParameter(id);
    }
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool XyPlot::SetStringParameter(const Integer id, const std::string &value)
{
    switch (id)
    {
    case X_PARAM_NAME:
        return SetXParameter(value);
    case Y_PARAM_NAME:
        return AddYParameter(value);
    case PLOT_TITLE:
        mPlotTitle = value;
        return true;
    case X_AXIS_TITLE:
        mXAxisTitle = value;
        return true;
    case Y_AXIS_TITLE:
        mYAxisTitle = value;
        return true;
    default:
        return Subscriber::SetStringParameter(id, value);
    }
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string XyPlot::GetStringParameter(const std::string &label) const
{
    for (int i=0; i<XyPlotParamCount; i++)
        if (label == PARAMETER_TEXT[i])
            return GetStringParameter(i);

    return Subscriber::GetStringParameter(label);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool XyPlot::SetStringParameter(const std::string &label,
                                const std::string &value)
{
    for (int i=0; i<XyPlotParamCount; i++)
        if (label == PARAMETER_TEXT[i])
            return SetStringParameter(i, value);

    return Subscriber::SetStringParameter(label, value);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& XyPlot::GetStringArrayParameter(const Integer id) const
{
    switch (id)
    {
    case Y_PARAM_NAME_LIST:
        return mYParamNames;
    default:
        return Subscriber::GetStringArrayParameter(id);
    }
}

//------------------------------------------------------------------------------
// StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& XyPlot::GetStringArrayParameter(const std::string &label) const
{
    for (int i=0; i<XyPlotParamCount; i++)
        if (label == PARAMETER_TEXT[i])
            return GetStringArrayParameter(i);

    return Subscriber::GetStringArrayParameter(label);
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// bool XyPlot::Distribute(int len)
//------------------------------------------------------------------------------
bool XyPlot::Distribute(int len)
{
    //loj: How do I convert data to Real data?
    return false;
}

//------------------------------------------------------------------------------
// bool Distribute(const Real * dat, Integer len)
//------------------------------------------------------------------------------
bool XyPlot::Distribute(const Real * dat, Integer len)
{
//      PlotInterface::CreateXyPlotWindow(instanceName, mPlotTitle, mXAxisTitle,
//                                        mYAxisTitle);
    if (len > 0)
    {
        if (mXParam != NULL && mNumYParams > 0)
        {
            // get x param
            Real xval = mXParam->EvaluateReal();
            //MessageInterface::ShowMessage("XyPlot::Distribute() xval = %f\n", xval);
            //xval = dat[0]; // loj: temp code to test XY plot dat[0] is time
            //MessageInterface::ShowMessage("XyPlot::Distribute() xval = %f\n", xval);
            
            // get y params
            Rvector yvals = Rvector(mNumYParams);

            //loj: 2/27/04 why parameters not getting updated value?
            // because the spacecraft is not updated?
            // put yvals in the order of parameters added
            for (int i=0; i<mNumYParams; i++)
            {
                yvals[i] = mYParams[i]->EvaluateReal();
                //MessageInterface::ShowMessage("XyPlot::Distribute() yvals = %f\n", yvals[i]);
                //yvals[i] = dat[1]; //loj: temp code to test XY plot dat[1] is pos X
                //MessageInterface::ShowMessage("XyPlot::Distribute() yvals = %f\n", yvals[i]);
            }

            if (!mIsXyPlotWindowSet)
            {
                
                //loj: 2/27/04 temp code to set X and Y axis title
                mXAxisTitle = "Time";
                mYAxisTitle = "Position X";
                
                // Create XyPlotWindow
                PlotInterface::CreateXyPlotWindow(false, instanceName, mPlotTitle,
                                                  mXAxisTitle, mYAxisTitle);
                
                // add to Y params to XyPlotWindow
                //loj: temp code
                int yOffset = 0; //loj: I don't how this is used
                Real yMin = -40000.0; //loj: should parameter provide minimum value?
                Real yMax =  40000.0; //loj: should parameter provide maximum value?

                for (int i=0; i<mNumYParams; i++)
                {
                    std::string curveTitle = mYParams[i]->GetName();
                    std::string penColor = "RED"; //loj: should parameter provide pen color?

                    MessageInterface::ShowMessage("XyPlot::Distribute() curveTitle = %s\n",
                                                  curveTitle.c_str());
                    if (PlotInterface::AddXyPlotCurve(instanceName, i, yOffset, yMin, yMax,
                                                      curveTitle, penColor))
                    {
                        mIsXyPlotWindowSet = true;
                        mFirstXVal = xval;
                    }
                }
            }
            
            // update xy plot
            // X value must start from 0
            if (mIsXyPlotWindowSet)
            {
                mNumData++;
                
                if ((mNumData % mDataCollectFrequency) == 0)
                {
                    mNumCollected++;
                    bool update = (mNumCollected % mUpdatePlotFrequency) == 0;

                    return PlotInterface::UpdateXyPlot(instanceName, xval, yvals,
                                                       mPlotTitle, mXAxisTitle, mYAxisTitle,
                                                       update);
                    //loj: 2/27/04 just pass xval instead of xval-mFirstXVal
//                      return PlotInterface::UpdateXyPlot(instanceName, xval-mFirstXVal, yvals,
//                                                         mPlotTitle, mXAxisTitle, mYAxisTitle,
//                                                         update);
                }
            }
        }
    }

    return false;
}

