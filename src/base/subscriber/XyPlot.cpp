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

#include "XyPlot.hpp"
#include "PlotInterface.hpp"     // for XY plot
#include "ColorTypes.hpp"        // for namespace GmatColor::
#include "Moderator.hpp"         // for GetParameter()
#include "MessageInterface.hpp"  // for ShowMessage()

#define DEBUG_XYPLOT 0

//---------------------------------
// static data
//---------------------------------

const std::string
XyPlot::PARAMETER_TEXT[XyPlotParamCount - SubscriberParamCount] =
{
   "IndVar",
   "Add",               //loj: 6/4/04 added to match script
   "DepVar",
   "DepVarList",
   "ClearDepVarList",
   "Color",
   "PlotTitle",
   "XAxisTitle",
   "YAxisTitle",
   "DrawGrid",
   "DataCollectFrequency",
   "UpdatePlotFrequency",
}; 

const Gmat::ParameterType
XyPlot::PARAMETER_TYPE[XyPlotParamCount - SubscriberParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::UNSIGNED_INT_TYPE,
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
               bool drawGrid) :
   Subscriber("XyPlot", name)
{
   // GmatBase data
   parameterCount = XyPlotParamCount;
    
   mDrawGrid = drawGrid;
   mNumYParams = 0;

   mXParamName = "";
   mNumXParams = 0;
    
   mXParam = xParam;
   if (firstYParam != NULL)
      AddYParameter(firstYParam->GetName());

   mPlotTitle = plotTitle;
   mXAxisTitle = xAxisTitle;
   mYAxisTitle = yAxisTitle;
   
   mIsXyPlotWindowSet = false;
   mDataCollectFrequency = 1;
   mUpdatePlotFrequency = 10;
}

//------------------------------------------------------------------------------
// XyPlot(const XyPlot &copy)
//------------------------------------------------------------------------------
XyPlot::XyPlot(const XyPlot &copy) :
   Subscriber(copy)
{
   mXParam = copy.mXParam;
   mYParams = copy.mYParams; //loj: 6/4/04 remove this later
   mYParamMap = copy.mYParamMap;
   
   mNumXParams = copy.mNumXParams;
   mNumYParams = copy.mNumYParams;

   mXParamName = copy.mXParamName;
   mYParamNames = copy.mYParamNames;
    
   mPlotTitle = copy.mPlotTitle;
   mXAxisTitle = copy.mXAxisTitle;
   mYAxisTitle = copy.mYAxisTitle;
   mDrawGrid = copy.mDrawGrid;
   mIsXyPlotWindowSet = copy.mIsXyPlotWindowSet;
    
   mDataCollectFrequency = copy.mDataCollectFrequency;
   mUpdatePlotFrequency = copy.mUpdatePlotFrequency;
    
   mNumDataPoints = copy.mNumDataPoints;
   mNumCollected = copy.mNumCollected;
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
   //@todo Do not use Moderator
   
   bool status = false;
   Moderator *theModerator = Moderator::Instance();

   if (paramName != "")
   {
      mXParamName = paramName;
      mXParam = NULL;
      
      // get parameter pointer
      Parameter *param = theModerator->GetParameter(paramName);
      if (param != NULL)
      {
         mXParam = param;
         mNumXParams = 1; //loj: only 1 X parameter for now
      }
      status = true;
   }

   return status;
}

//  //------------------------------------------------------------------------------
//  // bool SetXParameter(Parameter *param)
//  //------------------------------------------------------------------------------
//  bool XyPlot::SetXParameter(Parameter *param)
//  {
//     //loj: Do I really need to validate parameter before set?
//     //     Validate when the parameter is actually evaluated?
//     //if (param->Validate()) //loj: 4/27/04 commented out
//     //{
//        mXParamName = param->GetName();
//        mXParam = param;
//        return true;
//     //}

//     //return false;
//  }

//------------------------------------------------------------------------------
// bool AddYParameter(const std::string &paramName)
//------------------------------------------------------------------------------
bool XyPlot::AddYParameter(const std::string &paramName)
{
   bool status = false;
   Moderator *theModerator = Moderator::Instance();
    
   if (paramName != "")
   {
      mYParamNames.push_back(paramName);
      mYParamMap[paramName] = NULL;
      mNumYParams = mYParamNames.size();

      mColorMap[paramName] = GmatColor::BLACK32;

      // get parameter pointer
      Parameter *param = theModerator->GetParameter(paramName);
      if (param != NULL)
      {
#if DEBUG_XYPLOT
         MessageInterface::ShowMessage("XyPlot::AddYParameter() name = %s\n",
                                       param->GetName().c_str());
#endif
         mYParamMap[paramName] = param;
         mYParams.push_back(param);
      }
      status = true;
   }

   return status;
}

//  //------------------------------------------------------------------------------
//  // bool AddYParameter(Parameter *param)
//  //------------------------------------------------------------------------------
//  bool XyPlot::AddYParameter(Parameter *param)
//  {
//     bool added = false;
    
//     //loj: Do I really need to validate parameter before add?
//     //if (param->Validate()) //loj: 4/27/04 commented out
//     //{
//        //MessageInterface::ShowMessage("XyPlot::AddYParameter() param name = %s\n",
//        //                              param->GetName().c_str());
//        mYParams.push_back(param);

//        added = true;
//     //}

//     return added;
//  }

//----------------------------------
// methods inherited from Subscriber
//----------------------------------

//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool XyPlot::Initialize()
{
   Subscriber::Initialize();

   //-----------------------------------
   //@todo
   // need to set Parameter pointers
   //-----------------------------------
   // implement this later

   
   //MessageInterface::ShowMessage("XyPlot::Initialize() entered\n");
   bool status = false;
   DeletePlotCurves(); //loj: 5/3/04 added to fix index out of range
   
   //MessageInterface::ShowMessage("XyPlot::Initialize() active=%d\n", active);
   
   if (active)
   {
      // build plot title
      BuildPlotTitle();
      
      // Create XyPlotWindow, if not exist
      //MessageInterface::ShowMessage("XyPlot::Initialize() calling CreateXyPlotWindow()\n");
      PlotInterface::CreateXyPlotWindow(instanceName, mPlotTitle,
                                        mXAxisTitle, mYAxisTitle);

      mIsXyPlotWindowSet = true; //loj: 5/12/04 Do I need this flag?
            
      // add to Y params to XyPlotWindow
      //loj: temp code
      int yOffset = 0; //loj: I don't know how this is used
      Real yMin = -40000.0; //loj: should parameter provide minimum value?
      Real yMax =  40000.0; //loj: should parameter provide maximum value?

      for (int i=0; i<mNumYParams; i++)
      {
         std::string curveTitle = mYParams[i]->GetName();
         std::string penColor = "RED"; //loj: should parameter provide pen color?

         //MessageInterface::ShowMessage("XyPlot::Initialize() curveTitle = %s\n",
         //                              curveTitle.c_str());
                
         PlotInterface::AddXyPlotCurve(instanceName, i, yOffset, yMin, yMax,
                                       curveTitle, penColor);
      }

      status = true;
        
      //MessageInterface::ShowMessage("XyPlot::Initialize() calling ClearXyPlotData()\n");
      PlotInterface::ClearXyPlotData(instanceName);
   }
   else
   {
      if (mIsXyPlotWindowSet)
      {        
         mIsXyPlotWindowSet = false;
         status = PlotInterface::DeleteXyPlot(true);
      }
      else
      {
         status = true;
      }
   }

   return status;
}

//---------------------------------
// methods inherited from GmatBase
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the XyPlot.
 *
 * @return clone of the XyPlot.
 *
 */
//------------------------------------------------------------------------------
GmatBase* XyPlot::Clone(void) const
{
   return (new XyPlot(*this));
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string XyPlot::GetParameterText(const Integer id) const
{
   if (id >= SubscriberParamCount && id < XyPlotParamCount)
      return PARAMETER_TEXT[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterText(id);
    
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer XyPlot::GetParameterID(const std::string &str) const
{
   for (int i=SubscriberParamCount; i<XyPlotParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SubscriberParamCount])
         return i;
   }
   
   return Subscriber::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType XyPlot::GetParameterType(const Integer id) const
{
   if (id >= 0 && id < XyPlotParamCount)
      return PARAMETER_TYPE[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string XyPlot::GetParameterTypeString(const Integer id) const
{
   if (id >= SubscriberParamCount && id < XyPlotParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - SubscriberParamCount)];
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
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
bool XyPlot::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
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
   case CLEAR_DEP_VAR_LIST:
      ClearYParameters();
      return true;
   default:
      return Subscriber::SetBooleanParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label,
//                          const bool value)
//------------------------------------------------------------------------------
bool XyPlot::SetBooleanParameter(const std::string &label,
                                 const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// UnsignedInt GetUnsignedIntParameter(const Integer id,
//                                     const std::string &item)
//------------------------------------------------------------------------------
UnsignedInt XyPlot::GetUnsignedIntParameter(const Integer id,
                                            const std::string &item)
{   
   switch (id)
   {
   case COLOR:
      if (mColorMap.find(item) != mColorMap.end())
         return mColorMap[item];
      else
         return GmatBase::UNSIGNED_INT_PARAMETER_UNDEFINED;
   default:
      return Subscriber::GetUnsignedIntParameter(id);
   }
}

//------------------------------------------------------------------------------
// UnsignedInt GetUnsignedIntParameter(const std::string &label,
//                                     const std::string &item)
//------------------------------------------------------------------------------
UnsignedInt XyPlot::GetUnsignedIntParameter(const std::string &label,
                                                const std::string &item)
{
   return GetUnsignedIntParameter(GetParameterID(label), item);
}

//------------------------------------------------------------------------------
// UnsignedInt SetUnsignedIntParameter(const Integer id,
//                                     const std::string &item,
//                                     const UnsignedInt value)
//------------------------------------------------------------------------------
UnsignedInt XyPlot::SetUnsignedIntParameter(const Integer id,
                                            const std::string &item,
                                            const UnsignedInt value)
{
#ifdef DEBUG_OPENGL
   MessageInterface::ShowMessage
      ("XyPlot::SetUnsignedIntParameter()"
       "id=%d, item=%s, value=%d\n", id, item.c_str(), value);
#endif
   
   switch (id)
   {
   case COLOR:
      if (mColorMap.find(item) != mColorMap.end())
      {
         mColorMap[item] = value;
         return value;
      }
      return GmatBase::UNSIGNED_INT_PARAMETER_UNDEFINED;
   default:
      return Subscriber::SetUnsignedIntParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// UnsignedInt SetUnsignedIntParameter(const std::string &label,
//                                     const std::string &item,
//                                     const UnsignedInt value)
//------------------------------------------------------------------------------
UnsignedInt XyPlot::SetUnsignedIntParameter(const std::string &label,
                                            const std::string &item,
                                            const UnsignedInt value)
{
   return SetUnsignedIntParameter(GetParameterID(label), item, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string XyPlot::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case IND_VAR:
      return mXParamName;
   case DEP_VAR:
      if (mNumYParams > 0)
         return mYParamNames[0]; //loj: return first Y Param
      else
         return "";
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
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string XyPlot::GetStringParameter(const std::string &label) const
{
   //MessageInterface::ShowMessage("XyPlot::GetStringParameter() label = %s\n",
   //                              label.c_str());

   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool XyPlot::SetStringParameter(const Integer id, const std::string &value)
{
#if DEBUG_XYPLOT
   MessageInterface::ShowMessage("XyPlot::SetStringParameter() id = %d, "
                                 "value = %s \n", id, value.c_str());
#endif
   switch (id)
   {
   case IND_VAR:
      return SetXParameter(value);
   case ADD:
      return AddYParameter(value);
   case DEP_VAR:
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
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool XyPlot::SetStringParameter(const std::string &label,
                                const std::string &value)
{
   //MessageInterface::ShowMessage("XyPlot::SetStringParameter() label = %s, "
   //                              "value = %s \n", label.c_str(), value.c_str());

   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& XyPlot::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
   case DEP_VAR_LIST:
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
   return GetStringArrayParameter(GetParameterID(label));
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// void BuildPlotTitle()
//------------------------------------------------------------------------------
void XyPlot::BuildPlotTitle()
{
   //set X and Y axis title
   if (mXAxisTitle == "")
      mXAxisTitle = mXParam->GetName();

#if DEBUG_XYPLOT
   MessageInterface::ShowMessage("XyPlot::BuildPlotTitle() mXAxisTitle = %s\n",
                                 mXAxisTitle.c_str());
#endif
   
   if (mYAxisTitle == "")
   {
      mYAxisTitle = "";
      for (int i= 0; i<mNumYParams-1; i++)
      {
         mYAxisTitle += (mYParams[i]->GetName() + ", ");
      }
      mYAxisTitle += mYParams[mNumYParams-1]->GetName();
   }
            
#if DEBUG_XYPLOT
   MessageInterface::ShowMessage("XyPlot::BuildPlotTitle() mYAxisTitle = %s\n",
                                 mYAxisTitle.c_str());
#endif
    
   if (mPlotTitle == "")
   {
      mPlotTitle = "(" + mXAxisTitle + ")" + " vs " + "(" + mYAxisTitle + ")";
   }
    
#if DEBUG_XYPLOT
   MessageInterface::ShowMessage("XyPlot::BuildPlotTitle() mPlotTitle = %s\n",
                                 mPlotTitle.c_str());
#endif
}

//loj: 5/4/04 added
//------------------------------------------------------------------------------
// void ClearYParameters()
//------------------------------------------------------------------------------
void XyPlot::ClearYParameters()
{
   DeletePlotCurves();
   mYParams.clear();
   mYParamNames.clear();
   mColorMap.clear();
   mNumYParams = 0;
   mPlotTitle = "";
   mXAxisTitle = "";
   mYAxisTitle = "";
   mIsXyPlotWindowSet = false;
}

//------------------------------------------------------------------------------
// void DeletePlotCurves()
//------------------------------------------------------------------------------
void XyPlot::DeletePlotCurves()
{
   // delete exiting curves
   PlotInterface::DeleteAllXyPlotCurves(instanceName);
}


// methods inherited from Subscriber
//------------------------------------------------------------------------------
// bool Distribute(int len)
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
   //loj: 6/22/04 added if (isEndOfReceive)
   if (isEndOfReceive)
   {
      return PlotInterface::RefreshXyPlot(instanceName);
   }
   
   if (len > 0)
   {
      if (mXParam != NULL && mNumYParams > 0)
      {
         // get x param
         Real xval = mXParam->EvaluateReal();

#if DEBUG_XYPLOT
         MessageInterface::ShowMessage("XyPlot::Distribute() xval = %f\n", xval);
#endif
         //xval = dat[0]; // loj: temp code to test XY plot dat[0] is time
         //MessageInterface::ShowMessage("XyPlot::Distribute() xval = %f\n", xval);
            
         // get y params
         Rvector yvals = Rvector(mNumYParams);

         // put yvals in the order of parameters added
         for (int i=0; i<mNumYParams; i++)
         {
            yvals[i] = mYParams[i]->EvaluateReal();
            
#if DEBUG_XYPLOT
            MessageInterface::ShowMessage
               ("XyPlot::Distribute() yvals[%d] = %f\n", i, yvals[i]);
#endif
            //yvals[i] = dat[1]; //loj: temp code to test XY plot dat[1] is pos X
            //MessageInterface::ShowMessage("XyPlot::Distribute() yvals = %f\n", yvals[i]);
         }
            
         // update xy plot
         // X value must start from 0
         if (mIsXyPlotWindowSet)
         {
            mNumDataPoints++;
                
            if ((mNumDataPoints % mDataCollectFrequency) == 0)
            {
               mNumDataPoints = 0;
               mNumCollected++;
               bool update = (mNumCollected % mUpdatePlotFrequency) == 0;

               //MessageInterface::ShowMessage
               //   ("XyPlot::Distribute() calling PlotInterface::UpdateXyPlot()\n");
               
               return PlotInterface::UpdateXyPlot(instanceName, xval, yvals,
                                                  mPlotTitle, mXAxisTitle, mYAxisTitle,
                                                  update);
               if (update)
                  mNumCollected = 0;
            }
         }
      }
   }
   
   //loj: always return true otherwise next subscriber will not call ReceiveData()
   //     in Publisher::Publish()
   return true;
}

