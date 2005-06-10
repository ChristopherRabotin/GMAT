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

#include "TsPlot.hpp"
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
TsPlot::PARAMETER_TEXT[TsPlotParamCount - SubscriberParamCount] =
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
TsPlot::PARAMETER_TYPE[TsPlotParamCount - SubscriberParamCount] =
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
// TsPlot(const std::string &name, Parameter *xParam,
//        Parameter *firstYParam, const std::string &plotTitle,
//        const std::string &xAxisTitle, const std::string &yAxisTitle,
//        bool drawGrid)
//------------------------------------------------------------------------------
TsPlot::TsPlot(const std::string &name, Parameter *xParam,
               Parameter *firstYParam, const std::string &plotTitle,
               const std::string &xAxisTitle, const std::string &yAxisTitle,
               bool drawGrid) :
   Subscriber("TsPlot", name)
{
   // GmatBase data
   parameterCount = TsPlotParamCount;
    
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
   
   mIsTsPlotWindowSet = false;
   mDataCollectFrequency = 1;
   mUpdatePlotFrequency = 10;
}


//------------------------------------------------------------------------------
// TsPlot(const TsPlot &copy)
//------------------------------------------------------------------------------
TsPlot::TsPlot(const TsPlot &copy) :
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
   mIsTsPlotWindowSet = copy.mIsTsPlotWindowSet;
   
   mDataCollectFrequency = copy.mDataCollectFrequency;
   mUpdatePlotFrequency = copy.mUpdatePlotFrequency;
   
   mNumDataPoints = copy.mNumDataPoints;
   mNumCollected = copy.mNumCollected;
}


//------------------------------------------------------------------------------
// ~TsPlot(void)
//------------------------------------------------------------------------------
TsPlot::~TsPlot(void)
{
}

//------------------------------------------------------------------------------
// bool SetXParameter(const std::string &paramName)
//------------------------------------------------------------------------------
bool TsPlot::SetXParameter(const std::string &paramName)
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
bool TsPlot::AddYParameter(const std::string &paramName, Integer index)
{
   #if DEBUG_TsPlot_PARAM
   MessageInterface::ShowMessage("TsPlot::AddYParameter() name = %s\n",
                                 paramName.c_str());
   #endif
   
   if (paramName != "" && index == mNumYParams)
   {
      // if paramName not found, add (based on loj fix to XYPlot)
      if (find(mYParamNames.begin(), mYParamNames.end(), paramName) ==
          mYParamNames.end())
      {
         mYParamNames.push_back(paramName);
         mNumYParams = mYParamNames.size();
         mYParams.push_back(NULL);
         return true;
      }
   }

   return false;
}

//----------------------------------
// methods inherited from Subscriber
//----------------------------------

//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool TsPlot::Initialize()
{
   // Check if there are parameters selected for TsPlot
   if (active)
   {
      if (mNumXParams == 0 || mNumYParams == 0)
      {
         active = false;
         MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "TsPlot::Initialize() TsPlot will not be shown.\n"
             "No parameters selected for X Axis or Y Axis\n");
         return false;
      }
      
      if (mXParam == NULL || mYParams[0] == NULL)
      {
         active = false;
         MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "TsPlot::Initialize() TsPlot will not be shown.\n"
             "The first parameter selected for X Axis or Y Axis is NULL\n");
         return false;
      }
   }
   
   Subscriber::Initialize();
   
   #if DEBUG_TsPlot_INIT
   MessageInterface::ShowMessage("TsPlot::Initialize() active=%d\n", active);
   #endif
   
   bool status = false;
   DeletePlotCurves();
   
   if (active)
   {
      // build plot title
      BuildPlotTitle();
      
      // Create TsPlotWindow, if not exist
      #if DEBUG_TsPlot_INIT
      MessageInterface::ShowMessage("TsPlot::Initialize() calling CreateTsPlotWindow()\n");
      #endif
      
      PlotInterface::CreateTsPlotWindow(instanceName, mOldName, mPlotTitle,
                                        mXAxisTitle, mYAxisTitle, mDrawGrid);
      
      PlotInterface::SetTsPlotTitle(instanceName, mPlotTitle);
      mIsTsPlotWindowSet = true;
      
      // add to Y params to TsPlotWindow
      //loj: temp code
      int yOffset = 0; //loj: I don't know how this is used
      Real yMin = -40000.0; //loj: should parameter provide minimum value?
      Real yMax =  40000.0; //loj: should parameter provide maximum value?
      
      for (int i=0; i<mNumYParams; i++)
      {
         std::string curveTitle = mYParams[i]->GetName();
         UnsignedInt penColor = mYParams[i]->GetUnsignedIntParameter("Color");
            
         #if DEBUG_TsPlot_INIT
         MessageInterface::ShowMessage("TsPlot::Initialize() curveTitle = %s\n",
                                       curveTitle.c_str());
         #endif
         
         PlotInterface::AddTsPlotCurve(instanceName, i, yOffset, yMin, yMax,
                                       curveTitle, penColor);
      }
      
      PlotInterface::ShowTsPlotLegend(instanceName);
      status = true;
        
      #if DEBUG_TsPlot_INIT
      MessageInterface::ShowMessage("TsPlot::Initialize() calling ClearTsPlotData()\n");
      #endif
      
      PlotInterface::ClearTsPlotData(instanceName);
   }
   else
   {
      if (mIsTsPlotWindowSet)
      {        
         mIsTsPlotWindowSet = false;
         status = PlotInterface::DeleteTsPlot(true);
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
 * This method returns a clone of the TsPlot.
 *
 * @return clone of the TsPlot.
 *
 */
//------------------------------------------------------------------------------
GmatBase* TsPlot::Clone(void) const
{
   return (new TsPlot(*this));
}


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
bool TsPlot::SetName(const std::string &who)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage("TsPlot::SetName() newName=%s\n", who.c_str());
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
bool TsPlot::TakeAction(const std::string &action,
                        const std::string &actionData)
{
   #if DEBUG_ACTION_REMOVE
   MessageInterface::ShowMessage("TsPlot::TakeAction() action=%s, actionData=%s\n",
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


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool TsPlot::RenameRefObject(const Gmat::ObjectType type,
                             const std::string &oldName,
                             const std::string &newName)
{
   #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("TsPlot::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
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
std::string TsPlot::GetParameterText(const Integer id) const
{
   if (id >= SubscriberParamCount && id < TsPlotParamCount)
      return PARAMETER_TEXT[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterText(id);
    
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer TsPlot::GetParameterID(const std::string &str) const
{
   for (int i=SubscriberParamCount; i<TsPlotParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SubscriberParamCount])
         return i;
   }
   
   return Subscriber::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType TsPlot::GetParameterType(const Integer id) const
{
   if (id >= 0 && id < TsPlotParamCount)
      return PARAMETER_TYPE[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string TsPlot::GetParameterTypeString(const Integer id) const
{
   if (id >= SubscriberParamCount && id < TsPlotParamCount)
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
bool TsPlot::IsParameterReadOnly(const Integer id) const
{
   if ((id == PLOT_TITLE) || (id == X_AXIS_TITLE) || (id == Y_AXIS_TITLE) ||
       (id == DATA_COLLECT_FREQUENCY) || (id == UPDATE_PLOT_FREQUENCY))
      return true;

   return Subscriber::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer TsPlot::GetIntegerParameter(const Integer id) const
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
Integer TsPlot::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer TsPlot::SetIntegerParameter(const Integer id, const Integer value)
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
Integer TsPlot::SetIntegerParameter(const std::string &label,
                                    const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string TsPlot::GetStringParameter(const Integer id) const
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
std::string TsPlot::GetStringParameter(const std::string &label) const
{
   #if DEBUG_XY_PARAM
   MessageInterface::ShowMessage("TsPlot::GetStringParameter() label = %s\n",
                                 label.c_str());
   #endif
   
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool TsPlot::SetStringParameter(const Integer id, const std::string &value)
{
   #if DEBUG_TsPlot_PARAM
   MessageInterface::ShowMessage("TsPlot::SetStringParameter() id = %d, "
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
bool TsPlot::SetStringParameter(const std::string &label,
                                const std::string &value)
{
   #if DEBUG_TsPlot_PARAM
   MessageInterface::ShowMessage("TsPlot::SetStringParameter() label = %s, "
                                 "value = %s \n", label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool TsPlot::SetStringParameter(const Integer id, const std::string &value,
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
bool TsPlot::SetStringParameter(const std::string &label,
                                const std::string &value,
                                const Integer index)
{
   #if DEBUG_TsPlot_PARAM
   MessageInterface::ShowMessage
      ("TsPlot::SetStringParameter() label=%s, value=%s, index=%d \n",
       label.c_str(), value.c_str(), index);
   #endif
   
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& TsPlot::GetStringArrayParameter(const Integer id) const
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
const StringArray& TsPlot::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
//                                const std::string &name)
//------------------------------------------------------------------------------
GmatBase* TsPlot::GetRefObject(const Gmat::ObjectType type,
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
   
   throw GmatBaseException("TsPlot::GetRefObject() the object name: " + name +
                           "not found\n");
}

//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
bool TsPlot::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                          const std::string &name)
{
   if (type == Gmat::PARAMETER)
   {
      if (name == mXParamName)
      {
         mXParam = (Parameter*)obj;
      
         #if DEBUG_TsPlot_OBJECT
         MessageInterface::ShowMessage
            ("TsPlot::SetRefObject() mXParam:%s successfully set\n",
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
               
               #if DEBUG_TsPlot_OBJECT
               MessageInterface::ShowMessage
                  ("TsPlot::SetRefObject() mYParams[%s] successfully set\n",
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
const StringArray& TsPlot::GetRefObjectNameArray(const Gmat::ObjectType type)
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
void TsPlot::BuildPlotTitle()
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
      
   #if DEBUG_TsPlot_INIT
   MessageInterface::ShowMessage("TsPlot::BuildPlotTitle() mXAxisTitle = %s\n",
                                 mXAxisTitle.c_str());
   #endif
   
   mYAxisTitle = "";
   for (int i= 0; i<mNumYParams-1; i++)
   {
      mYAxisTitle += (mYParams[i]->GetName() + ", ");
   }
   mYAxisTitle += mYParams[mNumYParams-1]->GetName();
   
   #if DEBUG_TsPlot_INIT
   MessageInterface::ShowMessage("TsPlot::BuildPlotTitle() mYAxisTitle = %s\n",
                                 mYAxisTitle.c_str());
   #endif
   
   mPlotTitle = "(" + mXAxisTitle + ")" + " vs " + "(" + mYAxisTitle + ")";
   
   #if DEBUG_TsPlot_INIT
   MessageInterface::ShowMessage("TsPlot::BuildPlotTitle() mPlotTitle = %s\n",
                                 mPlotTitle.c_str());
   #endif
}

//------------------------------------------------------------------------------
// bool ClearYParameters()
//------------------------------------------------------------------------------
bool TsPlot::ClearYParameters()
{
   DeletePlotCurves();
   mYParams.clear();
   mYParamNames.clear();
   mNumYParams = 0;
   mPlotTitle = "";
   mXAxisTitle = "";
   mYAxisTitle = "";
   mIsTsPlotWindowSet = false;
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
bool TsPlot::RemoveYParameter(const std::string &name)
{
   #if DEBUG_ACTION_REMOVE
   MessageInterface::ShowMessage
      ("TsPlot::RemoveYParameter() name=%s\n--- Before remove:\n", name.c_str());
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
   MessageInterface::ShowMessage("TsPlot::RemoveYParameter() name=%s not found\n");
   #endif
   
   return false;
}

//------------------------------------------------------------------------------
// void DeletePlotCurves()
//------------------------------------------------------------------------------
void TsPlot::DeletePlotCurves()
{
   // delete exiting curves
   PlotInterface::DeleteAllTsPlotCurves(instanceName, mOldName);
}


// methods inherited from Subscriber
//------------------------------------------------------------------------------
// bool Distribute(int len)
//------------------------------------------------------------------------------
bool TsPlot::Distribute(int len)
{
   //loj: How do I convert data to Real data?
   return false;
}

//------------------------------------------------------------------------------
// bool Distribute(const Real * dat, Integer len)
//------------------------------------------------------------------------------
bool TsPlot::Distribute(const Real * dat, Integer len)
{
   #if DEBUG_TsPlot_UPDATE > 1
   MessageInterface::ShowMessage("TsPlot::Distribute() entered. isEndOfReceive=%d\n",
                                 isEndOfReceive);
   #endif
   
   if (isEndOfReceive)
   {
      if (active) //loj: 5/2/05 Added
         return PlotInterface::RefreshTsPlot(instanceName);
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

         #if DEBUG_TsPlot_UPDATE
         MessageInterface::ShowMessage("TsPlot::Distribute() xval = %f\n", xval);
         #endif
         
         //xval = dat[0]; // loj: temp code to test XY plot dat[0] is time
         //MessageInterface::ShowMessage("TsPlot::Distribute() xval = %f\n", xval);
         
         // get y params
         Rvector yvals = Rvector(mNumYParams);
         
         // put yvals in the order of parameters added
         for (int i=0; i<mNumYParams; i++)
         {
            yvals[i] = mYParams[i]->EvaluateReal();
            
            #if DEBUG_TsPlot_UPDATE
            MessageInterface::ShowMessage
               ("TsPlot::Distribute() yvals[%d] = %f\n", i, yvals[i]);
            #endif
            
            //yvals[i] = dat[1]; //loj: temp code to test XY plot dat[1] is pos X
            //MessageInterface::ShowMessage("TsPlot::Distribute() yvals = %f\n", yvals[i]);
         }
         
         // update xy plot
         // X value must start from 0
         if (mIsTsPlotWindowSet)
         {
            mNumDataPoints++;
                
            if ((mNumDataPoints % mDataCollectFrequency) == 0)
            {
               mNumDataPoints = 0;
               mNumCollected++;
               bool update = (mNumCollected % mUpdatePlotFrequency) == 0;
               
               //MessageInterface::ShowMessage
               //   ("TsPlot::Distribute() calling PlotInterface::UpdateTsPlot()\n");
               
               return PlotInterface::UpdateTsPlot(instanceName, mOldName, xval, yvals,
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

