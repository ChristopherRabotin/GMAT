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
#include "Publisher.hpp"         // for GetRunState()
#include "PlotInterface.hpp"     // for XY plot
#include "MessageInterface.hpp"  // for ShowMessage()

//#define DEBUG_XYPLOT_INIT 1
//#define DEBUG_XYPLOT_PARAM 1
//#define DEBUG_XYPLOT_OBJECT 1
//#define DEBUG_XYPLOT_UPDATE 1
//#define DEBUG_ACTION_REMOVE 1
//#define DEBUG_RENAME 1

//---------------------------------
// static data
//---------------------------------

const std::string
XyPlot::PARAMETER_TEXT[XyPlotParamCount - SubscriberParamCount] =
{
   "IndVar",
   "Add",
   "PlotTitle",
   "XAxisTitle",
   "YAxisTitle",
   "Grid",
   "TargetStatus",
   "DataCollectFrequency",
   "UpdatePlotFrequency",
}; 

const Gmat::ParameterType
XyPlot::PARAMETER_TYPE[XyPlotParamCount - SubscriberParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
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
   Subscriber("XYPlot", name) //loj: 10/28/04 Changed from XyPlot
{
   // GmatBase data
   parameterCount = XyPlotParamCount;
    
   mDrawGrid = drawGrid;
   mDrawTarget = false;
   mNumYParams = 0;

   mXParamName = "";
   mNumXParams = 0;
    
   mXParam = xParam;
   if (firstYParam != NULL)
      AddYParameter(firstYParam->GetName(), mNumYParams);

   mOldName = instanceName;
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
   mYParams = copy.mYParams;
   
   mNumXParams = copy.mNumXParams;
   mNumYParams = copy.mNumYParams;
   
   mXParamName = copy.mXParamName;
   mYParamNames = copy.mYParamNames;
   
   mOldName = copy.mOldName;
   mPlotTitle = copy.mPlotTitle;
   mXAxisTitle = copy.mXAxisTitle;
   mYAxisTitle = copy.mYAxisTitle;
   mDrawGrid = copy.mDrawGrid;
   mDrawTarget = copy.mDrawTarget;
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
// bool SetXParameter(const std::string &paramName)
//------------------------------------------------------------------------------
bool XyPlot::SetXParameter(const std::string &paramName)
{
   if (paramName != "")
   {
      mXParamName = paramName;
      mNumXParams = 1; //loj: only 1 X parameter for now
      return true;
   }
   
   return false;
}

//------------------------------------------------------------------------------
// bool AddYParameter(const std::string &paramName, Integer index)
//------------------------------------------------------------------------------
bool XyPlot::AddYParameter(const std::string &paramName, Integer index)
{
#if DEBUG_XYPLOT_PARAM
      MessageInterface::ShowMessage("XyPlot::AddYParameter() name = %s\n",
                                    paramName.c_str());
#endif
   if (paramName != "" && index == mNumYParams)
   {
      mYParamNames.push_back(paramName);
      mNumYParams = mYParamNames.size();
      mYParams.push_back(NULL);
      return true;
   }

   return false;
}

//----------------------------------
// methods inherited from Subscriber
//----------------------------------

//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool XyPlot::Initialize()
{
   //loj: 11/2/04 added
   // Check if there are parameters selected for XYPlot
   if (active)
   {
      if (mNumXParams == 0 || mNumYParams == 0)
      {
         active = false;
         MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "XyPlot::Initialize() XYPlot will not be shown.\n"
             "No parameters selected for X Axis or Y Axis\n");
         return false;
      }
      
      if (mXParam == NULL || mYParams[0] == NULL)
      {
         active = false;
         MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "XyPlot::Initialize() XYPlot will not be shown.\n"
             "The first parameter selected for X Axis or Y Axis is NULL\n");
         return false;
      }
   }
   
   Subscriber::Initialize();
   
#if DEBUG_XYPLOT_INIT
   MessageInterface::ShowMessage("XyPlot::Initialize() active=%d\n", active);
#endif
   
   bool status = false;
   DeletePlotCurves();
   
   if (active)
   {
      // build plot title
      BuildPlotTitle();
      
      // Create XyPlotWindow, if not exist
#if DEBUG_XYPLOT_INIT
      MessageInterface::ShowMessage("XyPlot::Initialize() calling CreateXyPlotWindow()\n");
#endif
      PlotInterface::CreateXyPlotWindow(instanceName, mOldName, mPlotTitle,
                                        mXAxisTitle, mYAxisTitle, mDrawGrid);
      
      PlotInterface::SetXyPlotTitle(instanceName, mPlotTitle);
      mIsXyPlotWindowSet = true; //loj: 5/12/04 Do I need this flag?
      
      // add to Y params to XyPlotWindow
      //loj: temp code
      int yOffset = 0; //loj: I don't know how this is used
      Real yMin = -40000.0; //loj: should parameter provide minimum value?
      Real yMax =  40000.0; //loj: should parameter provide maximum value?
      
      for (int i=0; i<mNumYParams; i++)
      {
         std::string curveTitle = mYParams[i]->GetName();
         UnsignedInt penColor = mYParams[i]->GetUnsignedIntParameter("Color");
            
#if DEBUG_XYPLOT_INIT
         MessageInterface::ShowMessage("XyPlot::Initialize() curveTitle = %s\n",
                                       curveTitle.c_str());
#endif
         PlotInterface::AddXyPlotCurve(instanceName, i, yOffset, yMin, yMax,
                                       curveTitle, penColor);
      }
      
      PlotInterface::ShowXyPlotLegend(instanceName);
      status = true;
        
#if DEBUG_XYPLOT_INIT
      MessageInterface::ShowMessage("XyPlot::Initialize() calling ClearXyPlotData()\n");
#endif
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

//loj: 11/19/04 - added
//------------------------------------------------------------------------------
// bool SetName(const std::string &who)
//------------------------------------------------------------------------------
/**
 * Set the name for this instance.
 *
 * @see GmatBase
 *
 */
//------------------------------------------------------------------------------
bool XyPlot::SetName(const std::string &who)
{
#if DEBUG_RENAME
   MessageInterface::ShowMessage("XyPlot::SetName() newName=%s\n", who.c_str());
#endif
   
   mOldName = instanceName;
   return GmatBase::SetName(who);
}

//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,  
//                         const std::string &actionData = "");
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action
 * @return true if action successfully performed
 *
 */
//------------------------------------------------------------------------------
bool XyPlot::TakeAction(const std::string &action,
                        const std::string &actionData)
{
#if DEBUG_ACTION_REMOVE
   MessageInterface::ShowMessage("XyPlot::TakeAction() action=%s, actionData=%s\n",
                                 action.c_str(), actionData.c_str());
#endif
   if (action == "Clear")
   {
      return ClearYParameters();
   }
   else if (action == "Remove")
   {
      return RemoveYParameter(actionData);
   }
   
   return false;
}

//loj: 11/16/04 added
//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool XyPlot::RenameRefObject(const Gmat::ObjectType type,
                             const std::string &oldName,
                             const std::string &newName)
{
#if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("XyPlot::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
#endif
   
   if (type == Gmat::PARAMETER)
   {
      // X parameter
      if (mXParamName == oldName)
         mXParamName = newName;
      
      // Y parameters
      for (unsigned int i=0; i<mYParamNames.size(); i++)
      {
         if (mYParamNames[i] == oldName)
            mYParamNames[i] = newName;
      }
      
      return true;
   }
   
   return false;
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

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool XyPlot::IsParameterReadOnly(const Integer id) const
{
   if ((id == PLOT_TITLE) || (id == X_AXIS_TITLE) || (id == Y_AXIS_TITLE) ||
       (id == DATA_COLLECT_FREQUENCY) || (id == UPDATE_PLOT_FREQUENCY))
      return true;

   return Subscriber::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer XyPlot::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
   case DATA_COLLECT_FREQUENCY:
      return mDataCollectFrequency;
   case UPDATE_PLOT_FREQUENCY:
      return mUpdatePlotFrequency;
   default:
      return Subscriber::GetIntegerParameter(id);
   }
}

//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
Integer XyPlot::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer XyPlot::SetIntegerParameter(const Integer id, const Integer value)
{
   switch (id)
   {
   case DATA_COLLECT_FREQUENCY:
      mDataCollectFrequency = value;
      return value;
   case UPDATE_PLOT_FREQUENCY:
      mUpdatePlotFrequency = value;
      return value;
   default:
      return Subscriber::SetIntegerParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const std::string &label,
//                                     const Integer value)
//------------------------------------------------------------------------------
Integer XyPlot::SetIntegerParameter(const std::string &label,
                                    const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
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
   case PLOT_TITLE:
      return mPlotTitle;
   case X_AXIS_TITLE:
      return mXAxisTitle;
   case Y_AXIS_TITLE:
      return mYAxisTitle;
   case DRAW_GRID:
      if (mDrawGrid)
         return "On";
      else
         return "Off";
   case TARGET_STATUS:
      if (mDrawTarget)
         return "On";
      else
         return "Off";
   default:
      return Subscriber::GetStringParameter(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string XyPlot::GetStringParameter(const std::string &label) const
{
#if DEBUG_XY_PARAM
   MessageInterface::ShowMessage("XyPlot::GetStringParameter() label = %s\n",
                                 label.c_str());
#endif
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool XyPlot::SetStringParameter(const Integer id, const std::string &value)
{
#if DEBUG_XYPLOT_PARAM
   MessageInterface::ShowMessage("XyPlot::SetStringParameter() id = %d, "
                                 "value = %s \n", id, value.c_str());
#endif
   switch (id)
   {
   case IND_VAR:
      return SetXParameter(value);
   case ADD:
      return AddYParameter(value, mNumYParams);
   case PLOT_TITLE:
      mPlotTitle = value;
      return true;
   case X_AXIS_TITLE:
      mXAxisTitle = value;
      return true;
   case Y_AXIS_TITLE:
      mYAxisTitle = value;
      return true;
   case DRAW_GRID:
      if (value == "On" || value == "Off")
      {
         mDrawGrid = (value == "On");
         return true;
      }
      else
      {
         return false;
      }
   case TARGET_STATUS:
      if (value == "On" || value == "Off")
      {
         mDrawTarget = (value == "On");
         return true;
      }
      else
      {
         return false;
      }
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
#if DEBUG_XYPLOT_PARAM
   MessageInterface::ShowMessage("XyPlot::SetStringParameter() label = %s, "
                                 "value = %s \n", label.c_str(), value.c_str());
#endif
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool XyPlot::SetStringParameter(const Integer id, const std::string &value,
                                const Integer index)
{
   switch (id)
   {
   case ADD:
      return AddYParameter(value, index);
   default:
      return Subscriber::SetStringParameter(id, value, index);
   }
}

//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool XyPlot::SetStringParameter(const std::string &label,
                                const std::string &value,
                                const Integer index)
{
#if DEBUG_XYPLOT_PARAM
   MessageInterface::ShowMessage
      ("XyPlot::SetStringParameter() label=%s, value=%s, index=%d \n",
       label.c_str(), value.c_str(), index);
#endif
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& XyPlot::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
   case ADD:
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

//loj: 11/9/04 added
//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
//                                const std::string &name)
//------------------------------------------------------------------------------
GmatBase* XyPlot::GetRefObject(const Gmat::ObjectType type,
                               const std::string &name)
{
   // if name is X parameter
   if (name == mXParamName)
   {
      return mXParam;
   }
   else
   {
      // name is Y parameter
      for (int i=0; i<mNumYParams; i++)
      {
         if (mYParamNames[i] == name)
            return mYParams[i];
      }
   }
   
   throw GmatBaseException("XyPlot::GetRefObject() the object name: " + name +
                           "not found\n");
}

//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
bool XyPlot::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                          const std::string &name)
{
   if (type == Gmat::PARAMETER) //loj: 4/22/04 Added
   {
      if (name == mXParamName)
      {
         mXParam = (Parameter*)obj;
      
         #if DEBUG_XYPLOT_OBJECT
         MessageInterface::ShowMessage
            ("XyPlot::SetRefObject() mXParam:%s successfully set\n",
             obj->GetName().c_str());
         #endif
      
         return true;
      }
      else
      {
         for (int i=0; i<mNumYParams; i++)
         {
            if (mYParamNames[i] == name)
            {
               mYParams[i] = (Parameter*)obj;
               
               #if DEBUG_XYPLOT_OBJECT
               MessageInterface::ShowMessage
                  ("XyPlot::SetRefObject() mYParams[%s] successfully set\n",
                   obj->GetName().c_str());
               #endif
               
               return true;
            }
         }
      }
   }
   
   return false;
}

//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& XyPlot::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   mAllParamNames.clear();

   //loj: 4/29/05 Added UNKNOWN_OBJECT
   switch (type)
   {
   case Gmat::UNKNOWN_OBJECT:
   case Gmat::PARAMETER:
      // add x parameter
      mAllParamNames.push_back(mXParamName);
   
      // add y parameters
      for (int i=0; i<mNumYParams; i++)
         mAllParamNames.push_back(mYParamNames[i]);
      break;
   default:
      break;
   }
   
   return mAllParamNames;
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
   //if (mXAxisTitle == "")
   //{
      if (mXParam)
         mXAxisTitle = mXParam->GetName();
      else {
         mXAxisTitle = "No X parameters";
         mYAxisTitle = "empty";
         mPlotTitle  = "Plot not fully initialized";
         return;
      }
      //}
      
#if DEBUG_XYPLOT_INIT
   MessageInterface::ShowMessage("XyPlot::BuildPlotTitle() mXAxisTitle = %s\n",
                                 mXAxisTitle.c_str());
#endif
   
   //if (mYAxisTitle == "")
   //{
      mYAxisTitle = "";
      for (int i= 0; i<mNumYParams-1; i++)
      {
         mYAxisTitle += (mYParams[i]->GetName() + ", ");
      }
      mYAxisTitle += mYParams[mNumYParams-1]->GetName();
      //}
   
#if DEBUG_XYPLOT_INIT
   MessageInterface::ShowMessage("XyPlot::BuildPlotTitle() mYAxisTitle = %s\n",
                                 mYAxisTitle.c_str());
#endif
   
   //if (mPlotTitle == "")
   //{
      mPlotTitle = "(" + mXAxisTitle + ")" + " vs " + "(" + mYAxisTitle + ")";
      //}
   
#if DEBUG_XYPLOT_INIT
   MessageInterface::ShowMessage("XyPlot::BuildPlotTitle() mPlotTitle = %s\n",
                                 mPlotTitle.c_str());
#endif
}

//------------------------------------------------------------------------------
// bool ClearYParameters()
//------------------------------------------------------------------------------
bool XyPlot::ClearYParameters()
{
   DeletePlotCurves();
   mYParams.clear();
   mYParamNames.clear();
   mNumYParams = 0;
   mPlotTitle = "";
   mXAxisTitle = "";
   mYAxisTitle = "";
   mIsXyPlotWindowSet = false;
   return true;
}

//------------------------------------------------------------------------------
// bool RemoveYParameter(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Removes parameter from the Y parameter list
 *
 * @param <name> parameter name to be removed from the Y parameter list
 *
 * @return true if parameter was removed from the Y parameter list, false otherwise
 *
 */
//------------------------------------------------------------------------------
bool XyPlot::RemoveYParameter(const std::string &name)
{
#if DEBUG_ACTION_REMOVE
   MessageInterface::ShowMessage
      ("XyPlot::RemoveYParameter() name=%s\n--- Before remove:\n", name.c_str());
   for (int i=0; i<mNumYParams; i++)
   {
      MessageInterface::ShowMessage("mYParamNames[%d]=%s\n", i,
                                    mYParamNames[i].c_str());
   }
#endif
   
   StringArray::iterator pos1;
   std::vector<Parameter*>::iterator pos2 = mYParams.begin();

   for (pos1 = mYParamNames.begin(); pos1 != mYParamNames.end(); pos1++)
   {
      if (*pos1 == name)
      {
         mYParamNames.erase(pos1);
         mYParams.erase(pos2);
         mNumYParams = mYParamNames.size();
         
#if DEBUG_ACTION_REMOVE
         MessageInterface::ShowMessage("---After remove\n");
         for (int i=0; i<mNumYParams; i++)
         {
            MessageInterface::ShowMessage("mYParamNames[%d]=%s\n", i,
                                          mYParamNames[i].c_str());
         }
#endif
         return true;
      }
      else
      {
         advance(pos2, 1);
      }
   }
   
   //------------------------------------------
   // loj: 9/29/04
   // Need to remove from PlotCurves also
   //------------------------------------------
   
#if DEBUG_ACTION_REMOVE
   MessageInterface::ShowMessage("XyPlot::RemoveYParameter() name=%s not found\n");
#endif
   return false;
}

//------------------------------------------------------------------------------
// void DeletePlotCurves()
//------------------------------------------------------------------------------
void XyPlot::DeletePlotCurves()
{
   // delete exiting curves
   PlotInterface::DeleteAllXyPlotCurves(instanceName, mOldName);
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
   if (isEndOfReceive)
   {
      return PlotInterface::RefreshXyPlot(instanceName);
   }

   // if targetting and draw target is off, just return
   if (!mDrawTarget && (Publisher::Instance()->GetRunState() == Gmat::TARGETING))
      return true;
   
   if (len > 0)
   {
      if (mXParam != NULL && mNumYParams > 0)
      {
         // get x param
         Real xval = mXParam->EvaluateReal();

#if DEBUG_XYPLOT_UPDATE
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
            
#if DEBUG_XYPLOT_UPDATE
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
               
               return PlotInterface::UpdateXyPlot(instanceName, mOldName, xval, yvals,
                                                  mPlotTitle, mXAxisTitle, mYAxisTitle,
                                                  update, mDrawGrid);
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

