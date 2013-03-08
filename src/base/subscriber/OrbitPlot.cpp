//$Id$
//------------------------------------------------------------------------------
//                                  OrbitPlot
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number X-XXXXX-X
//
// Author: Linda Jun
// Created: 2011/05/23
//
/**
 * Implements OrbitPlot class.
 */
//------------------------------------------------------------------------------

#include "OrbitPlot.hpp"
#include "PlotInterface.hpp"       // for UpdateGlPlot()
#include "ColorTypes.hpp"          // for namespace GmatColor::
#include "SubscriberException.hpp" // for SubscriberException()
#include "MessageInterface.hpp"    // for ShowMessage()
#include "TextParser.hpp"          // for SeparateBrackets()
#include "StringUtil.hpp"          // for ToReal()
#include "CoordinateConverter.hpp" // for Convert()
#include "SpaceObject.hpp"         // for GetEpoch()
#include <algorithm>               // for find(), distance()

#define __REMOVE_OBJ_BY_SETTING_FLAG__

//#define DBGLVL_INIT 2
//#define DBGLVL_DATA 2
//#define DBGLVL_DATA_LABELS 1
//#define DBGLVL_UPDATE 2
//#define DBGLVL_ADD 1
//#define DBGLVL_OBJ 2
//#define DBGLVL_PARAM 2
//#define DBGLVL_PARAM_STRING 2
//#define DBGLVL_TAKE_ACTION 1
//#define DBGLVL_REMOVE_SP 1
//#define DBGLVL_RENAME 1
//#define DBGLVL_SOLVER_CURRENT_ITER 2

//---------------------------------
// static data
//---------------------------------
const std::string
OrbitPlot::PARAMETER_TEXT[OrbitPlotParamCount - SubscriberParamCount] =
{
   "Add",
   "CoordinateSystem",
   "DrawObject",
   "OrbitColor",
   "TargetColor",
   "DataCollectFrequency",
   "UpdatePlotFrequency",
   "NumPointsToRedraw",
   "ShowPlot",
}; 


const Gmat::ParameterType
OrbitPlot::PARAMETER_TYPE[OrbitPlotParamCount - SubscriberParamCount] =
{
   Gmat::OBJECTARRAY_TYPE,       //"Add"
   Gmat::OBJECT_TYPE,            //"CoordinateSystem"
   Gmat::BOOLEANARRAY_TYPE,      //"DrawObject"
   Gmat::UNSIGNED_INTARRAY_TYPE, //"OrbitColor",
   Gmat::UNSIGNED_INTARRAY_TYPE, //"TargetColor",
   Gmat::INTEGER_TYPE,           //"DataCollectFrequency"
   Gmat::INTEGER_TYPE,           //"UpdatePlotFrequency"
   Gmat::INTEGER_TYPE,           //"NumPointsToRedraw"
   Gmat::BOOLEAN_TYPE,           //"ShowPlot"
};


const UnsignedInt
OrbitPlot::DEFAULT_ORBIT_COLOR[MAX_SP_COLOR] =
{
   GmatColor::RED32,       GmatColor::LIME32,    GmatColor::YELLOW32,
   GmatColor::AQUA32,      GmatColor::PINK32,    GmatColor::L_BLUE32,
   GmatColor::L_GRAY32,    GmatColor::BLUE32,    GmatColor::FUCHSIA32,
   GmatColor::BEIGE32,     GmatColor::RED32,     GmatColor::LIME32,
   GmatColor::YELLOW32,    GmatColor::AQUA32,    GmatColor::PINK32
};


//------------------------------------------------------------------------------
// OrbitPlot(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * The default constructor
 */
//------------------------------------------------------------------------------
OrbitPlot::OrbitPlot(const std::string &type, const std::string &name)
   : Subscriber(type, name)
{
   // GmatBase data
   parameterCount = OrbitPlotParamCount;
   objectTypeNames.push_back("OrbitPlot");
   
   mViewCoordSystem = NULL;
   
   mOldName = instanceName;
   mViewCoordSysName = "EarthMJ2000Eq";
   
   mDataCollectFrequency = 1;
   mUpdatePlotFrequency = 50;
   mNumPointsToRedraw = 0;
   mNumData = 0;
   mNumCollected = 0;
   
   mScNameArray.clear();
   mObjectNameArray.clear();
   mAllSpNameArray.clear();
   mAllRefObjectNames.clear();
   mObjectArray.clear();
   mDrawOrbitArray.clear();
   mDrawObjectArray.clear();
   mAllSpArray.clear();
   
   mScXArray.clear();
   mScYArray.clear();
   mScZArray.clear();
   mScVxArray.clear();
   mScVyArray.clear();
   mScVzArray.clear();
   mScOrbitColorArray.clear();
   mScTargetColorArray.clear();
   mOrbitColorArray.clear();
   mTargetColorArray.clear();
   
   mOrbitColorMap.clear();
   mTargetColorMap.clear();
   mDrawOrbitMap.clear();
   mShowObjectMap.clear();
   
   mAllSpCount = 0;
   mScCount = 0;
   mObjectCount = 0;
   mNonStdBodyCount = 0;
   
   // default planet color
   mOrbitColorMap["Earth"] = GmatColor::GREEN32;
   mOrbitColorMap["Luna"] = GmatColor::SILVER32;
   mOrbitColorMap["Sun"] = GmatColor::ORANGE32;
   mOrbitColorMap["Mercury"] = GmatColor::GRAY32;
   mOrbitColorMap["Venus"] = GmatColor::BEIGE32;
   mOrbitColorMap["Mars"] = GmatColor::L_GRAY32;
   mOrbitColorMap["Jupiter"] = GmatColor::L_BROWN32;
   mOrbitColorMap["Saturn"] = GmatColor::D_BROWN32;
   mOrbitColorMap["Uranus"] = GmatColor::BLUE32;
   mOrbitColorMap["Neptune"] = GmatColor::NAVY32;
   mOrbitColorMap["Pluto"] = GmatColor::PURPLE32;
   
}


//------------------------------------------------------------------------------
// OrbitPlot(const OrbitPlot &plot)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 */
//------------------------------------------------------------------------------
OrbitPlot::OrbitPlot(const OrbitPlot &plot)
   : Subscriber(plot)
{
   // Just copy configured object pointer
   mViewCoordSystem = plot.mViewCoordSystem;
   
   mOldName = plot.mOldName;
   mViewCoordSysName = plot.mViewCoordSysName;
   
   mDataCollectFrequency = plot.mDataCollectFrequency;
   mUpdatePlotFrequency = plot.mUpdatePlotFrequency;
   mNumPointsToRedraw = plot.mNumPointsToRedraw;
   
   mAllSpCount = plot.mAllSpCount;
   mScCount = plot.mScCount;
   mObjectCount = plot.mObjectCount;
   mNonStdBodyCount = plot.mNonStdBodyCount;
   
   mObjectArray = plot.mObjectArray;
   mDrawOrbitArray = plot.mDrawOrbitArray;
   mDrawObjectArray = plot.mDrawObjectArray;
   mAllSpArray = plot.mAllSpArray;
   mScNameArray = plot.mScNameArray;
   mObjectNameArray = plot.mObjectNameArray;
   mAllSpNameArray = plot.mAllSpNameArray;
   mAllRefObjectNames = plot.mAllRefObjectNames;
   
   mScXArray = plot.mScXArray;
   mScYArray = plot.mScYArray;
   mScZArray = plot.mScZArray;
   mScVxArray = plot.mScVxArray;
   mScVyArray = plot.mScVyArray;
   mScVzArray = plot.mScVzArray;
   
   mScOrbitColorArray = plot.mScOrbitColorArray;
   mScTargetColorArray = plot.mScTargetColorArray;
   mOrbitColorArray = plot.mOrbitColorArray;
   mTargetColorArray = plot.mTargetColorArray;
   
   mOrbitColorMap = plot.mOrbitColorMap;
   mTargetColorMap = plot.mTargetColorMap;
   mDrawOrbitMap = plot.mDrawOrbitMap;
   mShowObjectMap = plot.mShowObjectMap;
   
   mNumData = plot.mNumData;
   mNumCollected = plot.mNumCollected;
}


//------------------------------------------------------------------------------
// OrbitPlot& operator=(const OrbitPlot &plot)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
OrbitPlot& OrbitPlot::operator=(const OrbitPlot& plot)
{
   if (this == &plot)
      return *this;
   
   Subscriber::operator=(plot);
   
   // Just copy configured object pointer
   mViewCoordSystem = plot.mViewCoordSystem;
   
   mOldName = plot.mOldName;
   mViewCoordSysName = plot.mViewCoordSysName;
   
   mDataCollectFrequency = plot.mDataCollectFrequency;
   mUpdatePlotFrequency = plot.mUpdatePlotFrequency;
   mNumPointsToRedraw = plot.mNumPointsToRedraw;
   
   mAllSpCount = plot.mAllSpCount;
   mScCount = plot.mScCount;
   mObjectCount = plot.mObjectCount;
   mNonStdBodyCount = plot.mNonStdBodyCount;
   
   mObjectArray = plot.mObjectArray;
   mDrawOrbitArray = plot.mDrawOrbitArray;
   mDrawObjectArray = plot.mDrawObjectArray;
   mAllSpArray = plot.mAllSpArray;
   mScNameArray = plot.mScNameArray;
   mObjectNameArray = plot.mObjectNameArray;
   mAllSpNameArray = plot.mAllSpNameArray;
   mAllRefObjectNames = plot.mAllRefObjectNames;
   mScXArray = plot.mScXArray;
   mScYArray = plot.mScYArray;
   mScZArray = plot.mScZArray;
   mScVxArray = plot.mScVxArray;
   mScVyArray = plot.mScVyArray;
   mScVzArray = plot.mScVzArray;
   mScOrbitColorArray = plot.mScOrbitColorArray;
   mScTargetColorArray = plot.mScTargetColorArray;
   mOrbitColorArray = plot.mOrbitColorArray;
   mTargetColorArray = plot.mTargetColorArray;
   
   mOrbitColorMap = plot.mOrbitColorMap;
   mTargetColorMap = plot.mTargetColorMap;
   mDrawOrbitMap = plot.mDrawOrbitMap;
   mShowObjectMap = plot.mShowObjectMap;
   
   mNumData = plot.mNumData;
   mNumCollected = plot.mNumCollected;
   
   return *this;
}


//------------------------------------------------------------------------------
// ~OrbitPlot()
//------------------------------------------------------------------------------
/**
 * Destructor
 *
 * @note This destructor does not delete 3DView window, but clears data.
 *       3DView window is deleted when it is closed by the user or GMAT
 *       shuts down.
 */
//------------------------------------------------------------------------------
OrbitPlot::~OrbitPlot()
{
}


//------------------------------------------------------------------------------
// const StringArray& GetSpacePointList()
//------------------------------------------------------------------------------
const StringArray& OrbitPlot::GetSpacePointList()
{
   return mAllSpNameArray;
}


//------------------------------------------------------------------------------
// const StringArray& GetNonSpacecraftList()
//------------------------------------------------------------------------------
const StringArray& OrbitPlot::GetNonSpacecraftList()
{
   return mObjectNameArray;
}


//------------------------------------------------------------------------------
// UnsignedInt GetColor(const std::string &item, const std::string &name)
//------------------------------------------------------------------------------
UnsignedInt OrbitPlot::GetColor(const std::string &item,
                                const std::string &name)
{
   #if DBGLVL_PARAM
   MessageInterface::ShowMessage
      ("OrbitPlot::GetColor() item=%s, name=%s\n",
       item.c_str(), name.c_str());
   #endif
   
   if (item == "Orbit")
   {
      if (mOrbitColorMap.find(name) != mOrbitColorMap.end())
         return mOrbitColorMap[name];
   }
   else if (item == "Target")
   {
      if (mTargetColorMap.find(name) != mTargetColorMap.end())
         return mTargetColorMap[name];
   }
   
   return GmatBase::UNSIGNED_INT_PARAMETER_UNDEFINED;
}


//------------------------------------------------------------------------------
// bool SetColor(const std::string &item, const std::string &name,
//               UnsignedInt value)
//------------------------------------------------------------------------------
bool OrbitPlot::SetColor(const std::string &item, const std::string &name,
                         UnsignedInt value)
{
   #if DBGLVL_PARAM
   MessageInterface::ShowMessage
      ("OrbitPlot::SetColor() item=%s, name=%s, value=%u\n",
       item.c_str(), name.c_str(), value);
   #endif
   
   if (item == "Orbit")
   {
      if (mOrbitColorMap.find(name) != mOrbitColorMap.end())
      {
         mOrbitColorMap[name] = value;
         
         for (int i=0; i<mAllSpCount; i++)
            if (mAllSpNameArray[i] == name)
               mOrbitColorArray[i] = value;
         
         return true;
      }
   }
   else if (item == "Target")
   {
      if (mTargetColorMap.find(name) != mTargetColorMap.end())
      {
         mTargetColorMap[name] = value;
         
         for (int i=0; i<mAllSpCount; i++)
            if (mAllSpNameArray[i] == name)
               mTargetColorArray[i] = value;
         
         return true;
      }
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool GetShowObject(const std::string &name)
//------------------------------------------------------------------------------
bool OrbitPlot::GetShowObject(const std::string &name)
{
   #if DBGLVL_PARAM
   MessageInterface::ShowMessage
      ("OrbitPlot::GetShowObject() name=%s returning %d\n",
       name.c_str(), mDrawOrbitMap[name]);
   #endif
   
   return mShowObjectMap[name];
}


//------------------------------------------------------------------------------
// void SetShowObject(const std::string &name, bool value)
//------------------------------------------------------------------------------
void OrbitPlot::SetShowObject(const std::string &name, bool value)
{
   #if DBGLVL_PARAM
   MessageInterface::ShowMessage
      ("OrbitPlot::SetShowObject() name=%s setting %d\n", name.c_str(), value);
   #endif
   
   mShowObjectMap[name] = value;
   if (value)
      mDrawOrbitMap[name] = value;
   
   if (mShowObjectMap.find(name) != mShowObjectMap.end())
   {
      for (int i=0; i<mAllSpCount; i++)
         if (mAllSpNameArray[i] == name)
            mDrawObjectArray[i] = value;
   }
}



//----------------------------------
// inherited methods from Subscriber
//----------------------------------

//------------------------------------------------------------------------------
// void Activate(bool state)
//------------------------------------------------------------------------------
/**
 * Turns on or off the plot.
 * This methods is called from the Toggle command.
 */
//------------------------------------------------------------------------------
void OrbitPlot::Activate(bool state)
{
   #ifdef DEBUG_ACTIVATE
   MessageInterface::ShowMessage
      ("OrbitPlot::Activate() this=<%p>'%s' entered, state=%d, isInitialized=%d\n",
       this, GetName().c_str(), state, isInitialized);
   #endif
   
   Subscriber::Activate(state);
}


//---------------------------------
// inherited methods from GmatBase
//---------------------------------

//------------------------------------------------------------------------------
//  bool Validate()
//------------------------------------------------------------------------------
/**
 * Performs any pre-run validation that the object needs.
 *
 * @return true unless validation fails.
 */
//------------------------------------------------------------------------------
bool OrbitPlot::Validate()
{
   // We may use this code for more validation in the future. (LOJ: 2011.01.14)
   // Check size of objects and those option arrays
   //Integer objCount = mAllSpNameArray.size();
   //
   //if (mDrawObjectArray.size() != objCount)
   //{
   //   lastErrorMessage = "There are missing or extra values in \"DrawObject\" field. Expecting " +
   //      GmatStringUtil::ToString(objCount, 1) + " values";
   //   return false;
   //}
   
   return true;
}


//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool OrbitPlot::Initialize()
{
   if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING_NO_PLOTS)
      return true;
   
   Subscriber::Initialize();
   
   #if DBGLVL_INIT
   MessageInterface::ShowMessage
      ("OrbitPlot::Initialize() this=<%p>'%s', active=%d, isInitialized=%d, "
       "isEndOfReceive=%d, mAllSpCount=%d\n", this, GetName().c_str(), active,
       isInitialized, isEndOfReceive, mAllSpCount);
   #endif
   
   bool foundSc = false;
   bool retval = false;
   Integer nullCounter = 0;
   
   if (mAllSpCount == 0)
   {
      active = false;
      MessageInterface::ShowMessage
         ("*** WARNING *** The %s named \"%s\" will be turned off. "
          "No SpacePoints were added to plot.\n", GetTypeName().c_str(), GetName().c_str());
      return false;
   }
   
   // check for spacecaft is included in the plot
   for (int i=0; i<mAllSpCount; i++)
   {
      #if DBGLVL_INIT > 1
      MessageInterface::ShowMessage
         ("OrbitPlot::Initialize() mAllSpNameArray[%d]=<%p>'%s'\n",
          i, mAllSpArray[i], mAllSpNameArray[i].c_str());
      #endif
      
      if (mAllSpArray[i])
      {                  
         if (mAllSpArray[i]->IsOfType(Gmat::SPACECRAFT))
         {
            foundSc = true;
            break;
         }
      }
      else
         nullCounter++;
   }
   
   if (nullCounter == mAllSpCount)
   {
      active = false;
      MessageInterface::ShowMessage
         ("*** WARNING *** The %s named \"%s\" will be turned off.  "
          "%d SpaceObjects have NULL pointers.\n", GetTypeName().c_str(),
          GetName().c_str(), nullCounter);
      return false;
   }
   
   if (!foundSc)
   {
      active = false;
      MessageInterface::ShowMessage
         ("*** WARNING *** The %s named \"%s\" will be turned off. "
          "No Spacecraft was added to plot.\n", GetTypeName().c_str(), GetName().c_str());
      return false;
   }
   
   #if DBGLVL_INIT
   MessageInterface::ShowMessage("OrbitPlot::Initialize() exiting\n");
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the OrbitPlot.
 *
 * @return clone of the OrbitPlot.
 *
 */
//------------------------------------------------------------------------------
GmatBase* OrbitPlot::Clone() const
{
   return (new OrbitPlot(*this));
}


//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void OrbitPlot::Copy(const GmatBase* orig)
{
   operator=(*((OrbitPlot *)(orig)));
}


//------------------------------------------------------------------------------
// bool SetName(const std::string &who, const std;:string &oldName = "")
//------------------------------------------------------------------------------
/**
 * Set the name for this instance.
 *
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool OrbitPlot::SetName(const std::string &who, const std::string &oldName)
{
   #if DBGLVL_RENAME
   MessageInterface::ShowMessage
      ("OrbitPlot::SetName() newName=%s, oldName=%s\n", who.c_str(),
       oldName.c_str());
   #endif
   
   if (oldName == "")
      mOldName = instanceName;
   else
      mOldName = oldName;
   
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
bool OrbitPlot::TakeAction(const std::string &action,
                           const std::string &actionData)
{
   #if DBGLVL_TAKE_ACTION
   MessageInterface::ShowMessage
      ("OrbitPlot::TakeAction() '%s' entered, action='%s', actionData='%s'\n",
       GetName().c_str(), action.c_str(), actionData.c_str());
   #endif
   
   if (action == "Clear")
   {
      return ClearSpacePointList();
   }
   else if (action == "Remove")
   {
      return RemoveSpacePoint(actionData);
   }
   else if (action == "Finalize")
   {
      PlotInterface::DeleteGlPlot(instanceName);
   }
   else if (action == "PenUp")
   {
      isDataOn = false;
      return true;
   }
   else if (action == "PenDown")
   {
      isDataOn = true;
      return true;
   }
   
   return false;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool OrbitPlot::RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName)
{
   #if DBGLVL_RENAME
   MessageInterface::ShowMessage
      ("OrbitPlot::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type == Gmat::SPACECRAFT || type == Gmat::GROUND_STATION ||
       type == Gmat::CALCULATED_POINT)
   {
      // for spacecraft name
      for (int i=0; i<mAllSpCount; i++)
         if (mAllSpNameArray[i] == oldName)
            mAllSpNameArray[i] = newName;
      
      //----------------------------------------------------
      // Since spacecraft name is used as key for spacecraft
      // color map, I can't change the key name, so it is
      // removed and inserted with new name
      //----------------------------------------------------
      std::map<std::string, UnsignedInt>::iterator orbColorPos, targColorPos;
      std::map<std::string, bool>::iterator drawOrbitPos, showObjectPos;
      orbColorPos = mOrbitColorMap.find(oldName);
      targColorPos = mTargetColorMap.find(oldName);
      drawOrbitPos = mDrawOrbitMap.find(oldName);
      showObjectPos = mShowObjectMap.find(oldName);
      
      if (orbColorPos != mOrbitColorMap.end() &&
          targColorPos != mTargetColorMap.end())
      {
         // add new spacecraft name key and delete old
         mOrbitColorMap[newName] = mOrbitColorMap[oldName];
         mTargetColorMap[newName] = mTargetColorMap[oldName];
         mDrawOrbitMap[newName] = mDrawOrbitMap[oldName];
         mShowObjectMap[newName] = mShowObjectMap[oldName];
         mOrbitColorMap.erase(orbColorPos);
         mTargetColorMap.erase(targColorPos);
         mDrawOrbitMap.erase(drawOrbitPos);
         mShowObjectMap.erase(showObjectPos);
         
         #if DBGLVL_RENAME
         MessageInterface::ShowMessage("--- After rename\n");
         for (orbColorPos = mOrbitColorMap.begin();
              orbColorPos != mOrbitColorMap.end(); ++orbColorPos)
         {
            MessageInterface::ShowMessage
               ("obj = %20s, color = %d\n", orbColorPos->first.c_str(), orbColorPos->second);
         }
         #endif
      }
   }
   else if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (mViewCoordSysName == oldName)
         mViewCoordSysName = newName;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string OrbitPlot::GetParameterText(const Integer id) const
{
   if (id >= SubscriberParamCount && id < OrbitPlotParamCount)
      return PARAMETER_TEXT[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterText(id);
    
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer OrbitPlot::GetParameterID(const std::string &str) const
{
   for (int i=SubscriberParamCount; i<OrbitPlotParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SubscriberParamCount])
         return i;
   }
   
   return Subscriber::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType OrbitPlot::GetParameterType(const Integer id) const
{
   if (id >= SubscriberParamCount && id < OrbitPlotParamCount)
      return PARAMETER_TYPE[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string OrbitPlot::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer OrbitPlot::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
   case DATA_COLLECT_FREQUENCY:
      return mDataCollectFrequency;
   case UPDATE_PLOT_FREQUENCY:
      return mUpdatePlotFrequency;
   case NUM_POINTS_TO_REDRAW:
      return mNumPointsToRedraw;
   default:
      return Subscriber::GetIntegerParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
Integer OrbitPlot::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer OrbitPlot::SetIntegerParameter(const Integer id, const Integer value)
{
   switch (id)
   {
   case DATA_COLLECT_FREQUENCY:
      if (value > 0)
      {
         mDataCollectFrequency = value;
         return value;
      }
      else
      {
         SubscriberException se;
         se.SetDetails(errorMessageFormat.c_str(),
                       GmatStringUtil::ToString(value, 1).c_str(),
                       "DataCollectFrequency", "Integer Number > 0");
         throw se;
      }
   case UPDATE_PLOT_FREQUENCY:
      if (value > 0)
      {
         mUpdatePlotFrequency = value;
         return value;
      }
      else
      {
         SubscriberException se;
         se.SetDetails(errorMessageFormat.c_str(),
                       GmatStringUtil::ToString(value, 1).c_str(),
                       "UpdatePlotFrequency", "Integer Number > 0");
         throw se;
      }
   case NUM_POINTS_TO_REDRAW:
      if (value >= 0)
      {
         mNumPointsToRedraw = value;
         return value;
      }
      else
      {
         SubscriberException se;
         se.SetDetails(errorMessageFormat.c_str(),
                       GmatStringUtil::ToString(value, 1).c_str(),
                       "NumPointsToRedraw", "Integer Number >= 0");
         throw se;
      }
   default:
      return Subscriber::SetIntegerParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const std::string &label,
//                                     const Integer value)
//------------------------------------------------------------------------------
Integer OrbitPlot::SetIntegerParameter(const std::string &label,
                                       const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string OrbitPlot::GetStringParameter(const Integer id) const
{
   #if DBGLVL_PARAM_STRING
   MessageInterface::ShowMessage("OrbitPlot::GetStringParameter() id = %d\n", id);
   #endif
   
   switch (id)
   {
   case ADD:
      {
         Integer objCount = mAllSpNameArray.size();
         std::string objList = "{ ";
         for (Integer i = 0; i < objCount; i++)
         {
            if (i == objCount - 1)
               objList += mAllSpNameArray[i];
            else
               objList += mAllSpNameArray[i] + ", ";
         }
         objList += " }";
         return objList;
      }
   case COORD_SYSTEM:
      return mViewCoordSysName;
   default:
      return Subscriber::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string OrbitPlot::GetStringParameter(const std::string &label) const
{
   #if DBGLVL_PARAM_STRING
   MessageInterface::ShowMessage
      ("OrbitPlot::GetStringParameter() label = %s\n", label.c_str());
   #endif
   
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool OrbitPlot::SetStringParameter(const Integer id, const std::string &value)
{
   #if DBGLVL_PARAM_STRING
   MessageInterface::ShowMessage
      ("OrbitPlot::SetStringParameter() this=<%p>'%s', id=%d<%s>, value='%s'\n",
       this, instanceName.c_str(), id, GetParameterText(id).c_str(), value.c_str());
   #endif
   
   switch (id)
   {
   case COORD_SYSTEM:
      mViewCoordSysName = value;
      return true;
   case ADD:
      {
         if (value[0] == '{')
         {
            try
            {
               TextParser tp;
               ClearSpacePointList();
               StringArray spList = tp.SeparateBrackets(value, "{}", ",");
               for (UnsignedInt i = 0; i < spList.size(); i++)
                  AddSpacePoint(spList[i], mAllSpCount);
               return true;
            }
            catch (BaseException &)
            {
               SubscriberException se;
               se.SetDetails(errorMessageFormat.c_str(), value.c_str(),
                             "Add", "Valid CelestialBody list");
               throw se;
            }
         }
         else
         {
            return AddSpacePoint(value, mAllSpCount);
         }
      }
   case ORBIT_COLOR:
   case TARGET_COLOR:
      #if DBGLVL_PARAM_STRING
      MessageInterface::ShowMessage
         ("   Setting ORBIT_COLOR or TARGET_COLOR, value='%s'\n", value.c_str());
      #endif
      
      if (value[0] == '[')
         PutUnsignedIntValue(id, value);
      return true;
   default:
      return Subscriber::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool OrbitPlot::SetStringParameter(const std::string &label,
                                   const std::string &value)
{
   #if DBGLVL_PARAM_STRING
   MessageInterface::ShowMessage
      ("OrbitPlot::SetStringParameter()<%s> label=%s, value=%s \n",
       instanceName.c_str(), label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool OrbitPlot::SetStringParameter(const Integer id, const std::string &value,
                                   const Integer index)
{
   #if DBGLVL_PARAM_STRING
   MessageInterface::ShowMessage
      ("OrbitPlot::SetStringParameter()<%s> id=%d<%s>, value=%s, index= %d\n",
       instanceName.c_str(), id, GetParameterText(id).c_str(), value.c_str(), index);
   #endif
   
   switch (id)
   {
   case ADD:
      return AddSpacePoint(value, index);
   default:
      return Subscriber::SetStringParameter(id, value, index);
   }
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool OrbitPlot::SetStringParameter(const std::string &label,
                                   const std::string &value,
                                   const Integer index)
{
   #if DBGLVL_PARAM_STRING
   MessageInterface::ShowMessage
      ("OrbitPlot::SetStringParameter() label = %s, value = %s, index = %d\n",
       label.c_str(), value.c_str(), index);
   #endif
   
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// virtual const UnsignedIntArray&
// GetUnsignedIntArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const UnsignedIntArray&
OrbitPlot::GetUnsignedIntArrayParameter(const Integer id) const
{   
   switch (id)
   {
   case ORBIT_COLOR:
      return mOrbitColorArray;
   case TARGET_COLOR:
      return mTargetColorArray;
   default:
      return Subscriber::GetUnsignedIntArrayParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual UnsignedInt SetUnsignedIntParameter(const Integer id,
//                                             const UnsignedInt value,
//                                             const Integer index)
//------------------------------------------------------------------------------
UnsignedInt OrbitPlot::SetUnsignedIntParameter(const Integer id,
                                               const UnsignedInt value,
                                               const Integer index)
{
   #if DBGLVL_PARAM
   MessageInterface::ShowMessage
      ("OrbitPlot::SetUnsignedIntParameter() entered, this=<%p>'%s'\n   id=%d, value=%u<%08x>, index=%d, "
       "mAllSpCount=%d, mOrbitColorArray.size()=%d, mTargetColorArray.size()=%d\n", this,
       instanceName.c_str(), id, value, value, index, mAllSpCount, mOrbitColorArray.size(),
       mTargetColorArray.size());
   #endif
   
   switch (id)
   {
   case ORBIT_COLOR:
      {
         Integer size = mAllSpNameArray.size();
         #if DBGLVL_PARAM
         MessageInterface::ShowMessage("   mAllSpNameArray.size()=%d\n", size);
         #endif
         
         if (index >= size)
         {
            SubscriberException se;
            se.SetDetails("Index out of bounds for %s, %d object(s) added to plot",
                          GetParameterText(id).c_str(), size);
            throw se;
         }
         
         for (int i=0; i<size; i++)
         {
            if (index == i)
               mOrbitColorMap[mAllSpNameArray[i]] = value;
            
            if (index >= 0 && index < size)
               mOrbitColorArray[index] = value;
            else
               mOrbitColorArray.push_back(value);
         }
         #if DBGLVL_PARAM
         MessageInterface::ShowMessage
            ("OrbitPlot::SetUnsignedIntParameter() returning %u\n", value);
         #endif
         return value;
      }
   case TARGET_COLOR:
      {
         Integer size = mAllSpNameArray.size();
         #if DBGLVL_PARAM
         MessageInterface::ShowMessage("   mAllSpNameArray.size()=%d\n", size);
         #endif
         
         if (index >= size)
         {
            SubscriberException se;
            se.SetDetails("Index out of bounds for %s, %d object(s) added to plot",
                          GetParameterText(id).c_str(), size);
            throw se;
         }
         
         for (int i=0; i<size; i++)
         {
            if (index == i)
               mTargetColorMap[mAllSpNameArray[i]] = value;
            
            if (index >= 0 && index < size)
               mTargetColorArray[index] = value;
            else
               mTargetColorArray.push_back(value);
         }
         #if DBGLVL_PARAM
         MessageInterface::ShowMessage
            ("OrbitPlot::SetUnsignedIntParameter() returning %u\n", value);
         #endif
         return value;
      }
   default:
      #if DBGLVL_PARAM
      MessageInterface::ShowMessage
         ("OrbitPlot::SetUnsignedIntParameter() returning output from "
          "Subscriber::SetUnsignedIntParameter()\n");
      #endif
      return Subscriber::SetUnsignedIntParameter(id, value, index);
   }
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& OrbitPlot::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
   case ADD:
      return mAllSpNameArray;
   default:
      return Subscriber::GetStringArrayParameter(id);
   }
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
bool OrbitPlot::GetBooleanParameter(const Integer id) const
{
   if (id == SHOW_PLOT)
      return active;
   return Subscriber::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
bool OrbitPlot::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
bool OrbitPlot::SetBooleanParameter(const Integer id, const bool value)
{
   #if DBGLVL_PARAM
   MessageInterface::ShowMessage
      ("OrbitPlot::SetBooleanParameter()<%s> id=%d, value=%d\n",
       instanceName.c_str(), id, value);
   #endif
   
   if (id == SHOW_PLOT)
   {
      active = value;
      return active;
   }
   
   return Subscriber::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
bool OrbitPlot::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


//---------------------------------------------------------------------------
//const BooleanArray& GetBooleanArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const BooleanArray& OrbitPlot::GetBooleanArrayParameter(const Integer id) const
{
   if (id == DRAW_OBJECT)
   {
      return mDrawObjectArray;
   }
   return Subscriber::GetBooleanArrayParameter(id);
}


//---------------------------------------------------------------------------
//const BooleanArray& GetBooleanArrayParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const BooleanArray& OrbitPlot::GetBooleanArrayParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetBooleanArrayParameter(id);
}


//---------------------------------------------------------------------------
//  bool SetBooleanArrayParameter(const Integer id, const BooleanArray &valueArray)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool OrbitPlot::SetBooleanArrayParameter(const Integer id,
                                         const BooleanArray &valueArray)
{
   #if DBGLVL_PARAM
   MessageInterface::ShowMessage
      ("OrbitPlot::SetBooleanArrayParameter() '%s' entered, id=%d, "
       "valueArray.size()=%d\n", GetName().c_str(), id, valueArray.size());
   #endif
   
   if (id == DRAW_OBJECT)
   {
      #if DBGLVL_PARAM
      MessageInterface::ShowMessage
         ("   mAllSpNameArray.size()=%d\n", mAllSpNameArray.size());
      #endif
      
      // Check size of arrays in Initialize() or Interpreter::FinalPass()?
      //if (mAllSpNameArray.size() != valueArray.size())
      //   throw SubscriberException
      //      ("The count doesn't match with added objects" + GetParameterText(id));
      
      mDrawObjectArray = valueArray;
      Integer minCount = mAllSpNameArray.size() < mDrawObjectArray.size() ?
         mAllSpNameArray.size() : mDrawObjectArray.size();
      
      // GUI uses mShowObjectMap so update it
      for (Integer i = 0; i < minCount; i++)
      {
         bool tf = mDrawObjectArray[i];
         #if DBGLVL_PARAM > 1
         MessageInterface::ShowMessage
            ("      mDrawObjectArray[%d]=%s\n", i, tf ? "true" : "false");
         #endif
         mShowObjectMap[mAllSpNameArray[i]] = tf;
      }
      return true;
   }
   return Subscriber::SetBooleanArrayParameter(id, valueArray);
}


//---------------------------------------------------------------------------
//  bool SetBooleanArrayParameter(const std::string &label,
//                                const BooleanArray &valueArray)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool OrbitPlot::SetBooleanArrayParameter(const std::string &label,
                                         const BooleanArray &valueArray)
{
   Integer id = GetParameterID(label);
   return SetBooleanArrayParameter(id, valueArray);
}


//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
std::string OrbitPlot::GetRefObjectName(const Gmat::ObjectType type) const
{
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      return mViewCoordSysName; //just return this
   }
   
   return Subscriber::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool OrbitPlot::HasRefObjectTypeArray()
{
   return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& OrbitPlot::GetRefObjectTypeArray()
{
   refObjectTypes.push_back(Gmat::SPACE_POINT);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& OrbitPlot::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   #ifdef DBGLVL_OBJ
   MessageInterface::ShowMessage
      ("OrbitPlot::GetRefObjectNameArray() '%s' entered, refObjectNames.size()=%d, "
       "mAllSpNameArray.size()=%d\n", GetName().c_str(), refObjectNames.size(),
       mAllSpNameArray.size());
   #endif
   
   if (type == Gmat::COORDINATE_SYSTEM || type == Gmat::UNKNOWN_OBJECT)
   {
      refObjectNames.push_back(mViewCoordSysName);
   }
   
   if (type == Gmat::SPACE_POINT || type == Gmat::UNKNOWN_OBJECT)
   {
      refObjectNames.insert(refObjectNames.end(), mAllSpNameArray.begin(),
                            mAllSpNameArray.end());
   }
   
   #if DBGLVL_OBJ
   MessageInterface::ShowMessage
      ("OrbitPlot::GetRefObjectNameArray() returning %d names for type:%d\n",
       refObjectNames.size(), type);
   for (unsigned int i=0; i<refObjectNames.size(); i++)
      MessageInterface::ShowMessage("   %s\n", refObjectNames[i].c_str());
   #endif
   
   return refObjectNames;
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
//                                const std::string &name)
//------------------------------------------------------------------------------
GmatBase* OrbitPlot::GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name)
{
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (name == mViewCoordSysName)
         return mViewCoordSystem;
   }
   
   return Subscriber::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Set reference object pointer.
 *
 * @param <obj>  Reference object pointer to set to given object type and name
 * @param <type> Reference object type
 * @param <name> Reference object name
 */
//------------------------------------------------------------------------------
bool OrbitPlot::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name)
{
   #if DBGLVL_OBJ
   MessageInterface::ShowMessage
      ("OrbitPlot::SetRefObject() this=<%p>'%s' entered, obj=<%p><%s>'%s', type=%d, "
       "name='%s'\n", this, GetName().c_str(), obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL", type, name.c_str());
   #endif
   
   if (obj == NULL)
   {
      #if DBGLVL_OBJ
      MessageInterface::ShowMessage
         ("OrbitPlot::SetRefObject() this=<%p>'%s' returning false, object is NULL\n",
          this, GetName().c_str() );
      #endif
      return false;
   }
   
   std::string realName = name;
   if (name == "")
      realName = obj->GetName();
   
   if (obj->IsOfType(Gmat::SPACE_POINT))
   {
      #if DBGLVL_OBJ
      MessageInterface::ShowMessage("   mAllSpCount=%d\n", mAllSpCount);
      #endif
      
      for (Integer i=0; i<mAllSpCount; i++)
      {
         #if DBGLVL_OBJ
         MessageInterface::ShowMessage
            ("   mAllSpNameArray[%d]='%s'\n", i, mAllSpNameArray[i].c_str());
         #endif
         
         if (mAllSpNameArray[i] == realName)
         {
            #if DBGLVL_OBJ > 1
            MessageInterface::ShowMessage
               ("   Setting object to '%s'\n", mAllSpNameArray[i].c_str());
            #endif
            
            mAllSpArray[i] = (SpacePoint*)(obj);
         }
      }
      
      // If spacecraft, save initial epoch so that data will not be buffered before
      // the initial epoch
      if (obj->IsOfType(Gmat::SPACECRAFT))
      {
         SpaceObject *so = (SpaceObject*)(obj);
         #if DBGLVL_OBJ
         MessageInterface::ShowMessage
            ("   '%s' is a spacecraft so saving epoch: %f\n", so->GetName().c_str(),
             so->GetEpoch());
         #endif
         mScInitialEpochMap[so->GetName()] = so->GetEpoch();
      }
      
      #if DBGLVL_OBJ > 1
      MessageInterface::ShowMessage
         ("OrbitPlot::SetRefObject() this=<%p>'%s' returning true\n",
          this, GetName().c_str());
      #endif
      return true;
   }
   
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (realName == mViewCoordSysName)
         mViewCoordSystem = (CoordinateSystem*)obj;
      
      #if DBGLVL_OBJ > 1
      WriteCoordinateSystem(mViewCoordSystem, "   mViewCoordSystem");
      MessageInterface::ShowMessage
         ("OrbitPlot::SetRefObject() this=<%p>'%s' returning true\n",
          this, GetName().c_str());
      #endif
      return true;
   }
   
   #if DBGLVL_OBJ > 1
   MessageInterface::ShowMessage
      ("OrbitPlot::SetRefObject() this=<%p>'%s' returning Subscriber::SetRefObject()\n",
       this, GetName().c_str());
   #endif
   return Subscriber::SetRefObject(obj, type, realName);
}


//---------------------------------------------------------------------------
// Gmat::ObjectType GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
Gmat::ObjectType OrbitPlot::GetPropertyObjectType(const Integer id) const
{
   if (id == ADD)
      return Gmat::SPACE_POINT;
   
   return Subscriber::GetPropertyObjectType(id);
}


//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// bool AddSpacePoint(const std::string &name, Integer index, bool show = true)
//------------------------------------------------------------------------------
bool OrbitPlot::AddSpacePoint(const std::string &name, Integer index, bool show)
{
   #if DBGLVL_ADD
   MessageInterface::ShowMessage
      ("OrbitPlot::AddSpacePoint()<%s> name=%s, index=%d, show=%d, mAllSpCount=%d\n",
       instanceName.c_str(), name.c_str(), index, show, mAllSpCount);
   #endif
   
   // if name not in the list, add
   if (find(mAllSpNameArray.begin(), mAllSpNameArray.end(), name) ==
       mAllSpNameArray.end())
   {
      // Do we want to add any new object here? Like Sun in the following.
      // OrbitPlot.Add = {DefaultSC, Earth};
      // OrbitPlot.Add = {Sun};
      // If yes, we should not check for index. Just commenting out for now (LOJ: 2011.01.14)
      //if (name != "" && index == mAllSpCount)
      if (name != "")
      {
         mAllSpNameArray.push_back(name);
         mAllSpArray.push_back(NULL);
         mAllSpCount = mAllSpNameArray.size();
         
         mDrawOrbitMap[name] = show;
         mShowObjectMap[name] = show;
         
         // Ignore array value more than actual map size
         if (mDrawObjectArray.size() < mShowObjectMap.size())
            mDrawObjectArray.push_back(true); //added (LOJ: 2011.01.13 for bug 2215 fix)
         
         UnsignedInt targetColor = GmatColor::TEAL32;
         // Make lighter target color for ground track plot since it draws on the texture map
         if (GetTypeName() == "GroundTrackPlot")
            targetColor = GmatColor::WHITE32;
         
         if (mAllSpCount < MAX_SP_COLOR)
         {
            // If object is non-standard-body, use mNonStdBodyCount.
            // So that spacecraft color starts from DEFAULT_ORBIT_COLOR
            if (mOrbitColorMap.find(name) == mOrbitColorMap.end())
            {
               mOrbitColorMap[name] = DEFAULT_ORBIT_COLOR[mNonStdBodyCount];
               mTargetColorMap[name] = targetColor;
               mOrbitColorArray.push_back(DEFAULT_ORBIT_COLOR[mNonStdBodyCount]);
               mTargetColorArray.push_back(targetColor);
               mNonStdBodyCount++;
            }
            else
            {
               mOrbitColorArray.push_back(mOrbitColorMap[name]);
               mTargetColorArray.push_back(mTargetColorMap[name]);
            }
         }
         else
         {
            mOrbitColorMap[name] = GmatColor::RED32;
            mTargetColorMap[name] = targetColor;
            mOrbitColorArray.push_back(GmatColor::RED32);
            mTargetColorArray.push_back(targetColor);
         }
      }
   }
   
   #if DBGLVL_ADD   
   std::string objName;
   MessageInterface::ShowMessage
      ("mAllSpNameArray.size()=%d, mAllSpCount=%d\n", mAllSpNameArray.size(), mAllSpCount);
   for (int i=0; i<mAllSpCount; i++)
   {
      objName = mAllSpNameArray[i];
      MessageInterface::ShowMessage
         ("   mAllSpNameArray[%d]=%s, draw=%d, show=%d "
          "orbColor=%u, targColor=%u\n", i, objName.c_str(), mDrawOrbitMap[objName],
          mShowObjectMap[objName], mOrbitColorMap[objName], mTargetColorMap[objName]);
      MessageInterface::ShowMessage
         ("   mOrbitColorArray[%d]=%u, mTargetColorArray[%d]=%u\n", i, mOrbitColorArray[i],
          i, mTargetColorArray[i]);
   }
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool ClearSpacePointList()
//------------------------------------------------------------------------------
bool OrbitPlot::ClearSpacePointList()
{
   //MessageInterface::ShowMessage("OrbitPlot::ClearSpacePointList()\n");
   
   mAllSpNameArray.clear();
   mAllSpArray.clear();
   mObjectArray.clear();
   mDrawOrbitArray.clear();
   mDrawObjectArray.clear();
   mScNameArray.clear();
   mObjectNameArray.clear();
   mOrbitColorArray.clear();
   mTargetColorArray.clear();
   
   mScXArray.clear();
   mScYArray.clear();
   mScZArray.clear();
   mScVxArray.clear();
   mScVyArray.clear();
   mScVzArray.clear();
   mOrbitColorMap.clear();
   mTargetColorMap.clear();
   mAllSpCount = 0;
   mScCount = 0;
   mObjectCount = 0;
   mNonStdBodyCount = 0;
   
   return true;
}


//------------------------------------------------------------------------------
// bool RemoveSpacePoint(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Removes spacecraft from the spacecraft list
 *
 * @param <name> spacecraft name to be removed from the list
 *
 * @return true if spacecraft was removed from the list, false otherwise
 *
 */
//------------------------------------------------------------------------------
bool OrbitPlot::RemoveSpacePoint(const std::string &name)
{
   //-----------------------------------------------------------------
   #ifdef __REMOVE_OBJ_BY_SETTING_FLAG__
   //-----------------------------------------------------------------

   for (UnsignedInt i=0; i<mObjectNameArray.size(); i++)
   {
      if (mObjectNameArray[i] == name)
      {
         mDrawOrbitArray[i] = false;
         PlotInterface::SetGlDrawOrbitFlag(instanceName, mDrawOrbitArray);
         return true;
      }
   }
   
   return false;
   
   //-----------------------------------------------------------------
   #else
   //-----------------------------------------------------------------
   
   bool removedFromScArray = false;
   bool removedFromAllSpArray = false;
   
   //-------------------------------------------------------
   // remove from mScNameArray
   //-------------------------------------------------------
   #if DBGLVL_REMOVE_SP
   MessageInterface::ShowMessage
      ("OrbitPlot::RemoveSpacePoint() name=%s\n--- Before remove from "
       "mScNameArray:\n", name.c_str());
   MessageInterface::ShowMessage("mScCount=%d\n", mScCount);
   for (int i=0; i<mScCount; i++)
   {
      MessageInterface::ShowMessage
         ("mScNameArray[%d]=%s\n", i, mScNameArray[i].c_str());
   }
   #endif
   
   StringArray::iterator scPos = 
      find(mScNameArray.begin(), mScNameArray.end(), name);
   
   if (scPos != mScNameArray.end())
   {
      MessageInterface::ShowMessage("sc to be erased=%s\n", (*scPos).c_str());
      
      // erase given spacecraft from the arrays
      mScNameArray.erase(scPos);
      
      // just reduce the size of array
      mScOrbitColorArray.erase(mScOrbitColorArray.begin());
      mScTargetColorArray.erase(mScTargetColorArray.begin());
      mScXArray.erase(mScXArray.begin());
      mScYArray.erase(mScYArray.begin());
      mScZArray.erase(mScZArray.begin());
      mScVxArray.erase(mScVxArray.begin());
      mScVyArray.erase(mScVyArray.begin());
      mScVzArray.erase(mScVzArray.begin());
      
      mScCount = mScNameArray.size();
      
      // update color array
      for (int i=0; i<mScCount; i++)
      {
         mScOrbitColorArray[i] = mOrbitColorMap[mScNameArray[i]];
         mScTargetColorArray[i] = mTargetColorMap[mScNameArray[i]];
      }
      
      #if DBGLVL_REMOVE_SP
      MessageInterface::ShowMessage("---After remove from mScNameArray:\n");
      MessageInterface::ShowMessage("mScCount=%d\n", mScCount);
      for (int i=0; i<mScCount; i++)
      {
         MessageInterface::ShowMessage
            ("mScNameArray[%d]=%s\n", i, mScNameArray[i].c_str());
      }
      #endif
      
      removedFromScArray = true;
      //return true;
   }
   
   
   //-------------------------------------------------------
   // remove from mAllSpNameArray and mObjectNameArray
   //-------------------------------------------------------
   #if DBGLVL_REMOVE_SP
   MessageInterface::ShowMessage
      ("OrbitPlot::RemoveSpacePoint() name=%s\n--- Before remove from "
       "mAllSpNameArray:\n", name.c_str());
   MessageInterface::ShowMessage("mAllSpCount=%d\n", mAllSpCount);
   for (int i=0; i<mAllSpCount; i++)
   {
      MessageInterface::ShowMessage
         ("mAllSpNameArray[%d]=%s\n", i, mAllSpNameArray[i].c_str());
   }
   #endif
   
   StringArray::iterator spPos = 
      find(mAllSpNameArray.begin(), mAllSpNameArray.end(), name);
   StringArray::iterator objPos = 
      find(mObjectNameArray.begin(), mObjectNameArray.end(), name);
   
   if (spPos != mAllSpNameArray.end() && objPos != mObjectNameArray.end())
   {
      std::map<std::string, UnsignedInt>::iterator orbColorPos, targColorPos;
      orbColorPos = mOrbitColorMap.find(name);
      targColorPos = mTargetColorMap.find(name);
      
      if (orbColorPos != mOrbitColorMap.end() &&
          targColorPos != mTargetColorMap.end())
      {
         // erase given spacecraft name
         mAllSpNameArray.erase(spPos);
         mObjectNameArray.erase(objPos);
         mOrbitColorMap.erase(orbColorPos);
         mTargetColorMap.erase(targColorPos);
         
         // reduce the size of array
         mOrbitColorArray.erase(mOrbitColorArray.begin());
         mTargetColorArray.erase(mTargetColorArray.begin());
         
         mAllSpCount = mAllSpNameArray.size();
         
         // update color array
         for (int i=0; i<mAllSpCount; i++)
         {
            mOrbitColorArray[i] = mOrbitColorMap[mAllSpNameArray[i]];
            mTargetColorArray[i] = mTargetColorMap[mAllSpNameArray[i]];
         }
         
         #if DBGLVL_REMOVE_SP
         MessageInterface::ShowMessage("---After remove from mAllSpNameArray\n");
         MessageInterface::ShowMessage("mAllSpCount=%d\n", mAllSpCount);
         for (int i=0; i<mAllSpCount; i++)
         {
            MessageInterface::ShowMessage
               ("mAllSpNameArray[%d]=%s\n", i, mAllSpNameArray[i].c_str());
         }
         #endif
         
         removedFromAllSpArray = true;
      }
   }

   //-------------------------------------------------------
   // remove from mObjectArray
   //-------------------------------------------------------
   #if DBGLVL_REMOVE_SP
   MessageInterface::ShowMessage
      ("OrbitPlot::RemoveSpacePoint() name=%s\n--- Before remove from "
       "mObjectArray:\n", name.c_str());
   MessageInterface::ShowMessage("size=%d\n", mObjectArray.size());
   #endif
   
   for (std::vector<SpacePoint*>::iterator objptPos = mObjectArray.begin();
        objptPos != mObjectArray.end(); ++objptPos)
   {
      MessageInterface::ShowMessage
         ("mObjectArray=%s\n", (*objptPos)->GetName().c_str());
      if ((*objptPos)->GetName() == name)
      {
         mObjectArray.erase(objptPos);
         break;
      }
   }
   
   #if DBGLVL_REMOVE_SP
   MessageInterface::ShowMessage
      ("OrbitPlot::RemoveSpacePoint() name=%s\n--- After remove from "
       "mObjectArray:\n", name.c_str());
   MessageInterface::ShowMessage("size=%d\n", mObjectArray.size());
   #endif
   
   if (removedFromScArray && removedFromAllSpArray)
      // set all object array and pointers
      PlotInterface::SetGlObject(instanceName, mObjectNameArray,
                                 mOrbitColorArray, mObjectArray);
   
   return (removedFromScArray && removedFromAllSpArray);
   
   #endif
}


//------------------------------------------------------------------------------
// Integer FindIndexOfElement(StringArray &labelArray, const std::string &label)
//------------------------------------------------------------------------------
/*
 * Finds the index of the element label from the element label array.
 *
 * Typical element label array contains:
 *    All.epoch, scName.X, scName.Y, scName.Z, scName.Vx, scName.Vy, scName.Vz.
 */
//------------------------------------------------------------------------------
Integer OrbitPlot::FindIndexOfElement(StringArray &labelArray,
                                       const std::string &label)
{
   std::vector<std::string>::iterator pos;
   pos = find(labelArray.begin(), labelArray.end(),  label);
   if (pos == labelArray.end())
      return -1;
   else
      return distance(labelArray.begin(), pos);
}


//------------------------------------------------------------------------------
// void BuildDynamicArrays()
//------------------------------------------------------------------------------
void OrbitPlot::BuildDynamicArrays()
{
   #if DBGLVL_INIT
   MessageInterface::ShowMessage
      ("OrbitPlot::BuildDynamicArrays() entered, mAllSpNameArray.size()=%d, "
       "mAllSpArray.size()=%d\n", mAllSpNameArray.size(), mAllSpArray.size());
   #endif
   
   // Add spacecraft objects to the list first
   for (int i=0; i<mAllSpCount; i++)
   {
      #if DBGLVL_INIT > 1
      MessageInterface::ShowMessage
         ("OrbitPlot::BuildDynamicArrays() mAllSpNameArray[%d]=%s, addr=%d\n",
          i, mAllSpNameArray[i].c_str(), mAllSpArray[i]);
      #endif
      
      if (mAllSpArray[i])
      {
         if (mAllSpArray[i]->IsOfType(Gmat::SPACECRAFT))
         {
            // Add to spacecraft list
            mScNameArray.push_back(mAllSpNameArray[i]);
            mScOrbitColorArray.push_back(mOrbitColorMap[mAllSpNameArray[i]]);
            mScTargetColorArray.push_back(mTargetColorMap[mAllSpNameArray[i]]);
            mScXArray.push_back(0.0);
            mScYArray.push_back(0.0);
            mScZArray.push_back(0.0);
            mScVxArray.push_back(0.0);
            mScVyArray.push_back(0.0);
            mScVzArray.push_back(0.0);
            
            // Add to all object list
            mObjectNameArray.push_back(mAllSpNameArray[i]);                  
            mDrawOrbitArray.push_back(mDrawOrbitMap[mAllSpNameArray[i]]);
            mDrawObjectArray.push_back(mShowObjectMap[mAllSpNameArray[i]]);
            mOrbitColorArray.push_back(mOrbitColorMap[mAllSpNameArray[i]]);
            mTargetColorArray.push_back(mTargetColorMap[mAllSpNameArray[i]]);
            mObjectArray.push_back(mAllSpArray[i]);
         }
      }
      else
      {
         MessageInterface::ShowMessage
            ("The SpacePoint name: %s has NULL pointer.\n It will be removed "
             "from the %s.\n", mAllSpNameArray[i].c_str(), GetTypeName().c_str());
      }
   }
   
   // Add non-spacecraft objects to the list
   bool groundStationFound = false;
   for (int i=0; i<mAllSpCount; i++)
   {      
      if (mAllSpArray[i])
      {
         if (!mAllSpArray[i]->IsOfType(Gmat::SPACECRAFT))
         {
            if (mAllSpArray[i]->IsOfType(Gmat::GROUND_STATION))
               groundStationFound = true;
            
            // Add to all object list
            mObjectNameArray.push_back(mAllSpNameArray[i]);                  
            mDrawOrbitArray.push_back(mDrawOrbitMap[mAllSpNameArray[i]]);
            mDrawObjectArray.push_back(mShowObjectMap[mAllSpNameArray[i]]);
            mOrbitColorArray.push_back(mOrbitColorMap[mAllSpNameArray[i]]);
            mTargetColorArray.push_back(mTargetColorMap[mAllSpNameArray[i]]);
            mObjectArray.push_back(mAllSpArray[i]);
         }
      }
   }
   
   mScCount = mScNameArray.size();
   mObjectCount = mObjectNameArray.size();
   
   // Since we don't know the type of objects until object is initialized,
   // change to use the same default spacecraft orbit color for OrbitView and
   // GroundTrackPlot.  If GroundStation is added for GroundTrackPlot, its color
   // is set to the first default color, which makes color different from OrbitView.
   // (LOJ: 2012.09.13)
   if (groundStationFound)
   {
      #if DBGLVL_INIT
      MessageInterface::ShowMessage
         ("   Now changing colors for spacecraft to use default colors\n");
      #endif
      
      UnsignedIntArray colorArray;
      
      for (Integer i = 0; i < mObjectCount; i++)
      {
         if (mObjectArray[i]->IsOfType(Gmat::SPACECRAFT) ||
             mObjectArray[i]->IsOfType(Gmat::GROUND_STATION))
            colorArray.push_back(mOrbitColorMap[mAllSpNameArray[i]]);
      }
      
      #if DBGLVL_INIT
      MessageInterface::ShowMessage("   colorArray.size() = %d\n", colorArray.size());
      for (UnsignedInt i = 0; i < colorArray.size(); i++)
         MessageInterface::ShowMessage("      color[%d] = %u\n", i, colorArray[i]);
      #endif
      
      Integer colorIndex = 0;
      for (Integer i = 0; i < mObjectCount; i++)
      {
         if (mObjectArray[i]->IsOfType(Gmat::SPACECRAFT))
         {
            mOrbitColorArray[i] = colorArray[colorIndex];
            colorIndex++;
         }
      }
      
      for (Integer i = 0; i < mObjectCount; i++)
      {
         if (mObjectArray[i]->IsOfType(Gmat::GROUND_STATION))
         {
            mOrbitColorArray[i] = colorArray[colorIndex];
            colorIndex++;
         }
      }
      
      // Now change mScOrbitColorArray
      colorIndex = 0;
      for (Integer i = 0; i < mScCount; i++)
      {
         mScOrbitColorArray[i] = colorArray[colorIndex];
         colorIndex++;
      }
   }
   
   #if DBGLVL_INIT
   MessageInterface::ShowMessage("OrbitPlot::BuildDynamicArrays() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void ClearDynamicArrays()
//------------------------------------------------------------------------------
void OrbitPlot::ClearDynamicArrays()
{
   mObjectNameArray.clear();
   mOrbitColorArray.clear();
   mTargetColorArray.clear();
   mObjectArray.clear();
   mDrawOrbitArray.clear();
   mDrawObjectArray.clear();
   mScNameArray.clear();
   mScOrbitColorArray.clear();
   mScTargetColorArray.clear();
   mScXArray.clear();
   mScYArray.clear();
   mScZArray.clear();
   mScVxArray.clear();
   mScVyArray.clear();
   mScVzArray.clear();
}


//------------------------------------------------------------------------------
// void UpdateObjectList(SpacePoint *sp, bool show = false)
//------------------------------------------------------------------------------
/**
 * Add non-spacecraft object to the list.
 */
//------------------------------------------------------------------------------
void OrbitPlot::UpdateObjectList(SpacePoint *sp, bool show)
{
   #if DBGLVL_INIT > 1
   MessageInterface::ShowMessage
      ("OrbitPlot::UpdateObjectList() <%p>'%s' entered, sp=<%p>'%s', show=%d\n",
       this, GetName().c_str(), sp, sp ? sp->GetName().c_str() : "NULL", show);
   #endif
   
   // Add all spacepoint objects
   std::string name = sp->GetName();
   StringArray::iterator pos = 
      find(mObjectNameArray.begin(), mObjectNameArray.end(), name);
   
   // if name not found, add to arrays
   if (pos == mObjectNameArray.end())
   {
      #if DBGLVL_INIT > 1
		MessageInterface::ShowMessage
			("   '%s' NOT found in the object list, so adding\n", name.c_str());
		#endif
      mObjectNameArray.push_back(name);
      mOrbitColorArray.push_back(mOrbitColorMap[name]);
      mTargetColorArray.push_back(mTargetColorMap[name]);
      mObjectArray.push_back(sp);
      mDrawOrbitMap[name] = show;
      mShowObjectMap[name] = show;
      mDrawOrbitArray.push_back(show);
      mDrawObjectArray.push_back(show);
      mObjectCount = mObjectNameArray.size();
   }
   else
	{
      #if DBGLVL_INIT > 1
		MessageInterface::ShowMessage
			("   '%s' found in the object list, so skipping\n", name.c_str());
		#endif
	}
	
   #if DBGLVL_INIT > 1
   Integer draw, showObj;
   for (int i=0; i<mObjectCount; i++)
   {
      draw = mDrawOrbitArray[i] ? 1 : 0;
      showObj = mDrawObjectArray[i] ? 1 : 0;
      MessageInterface::ShowMessage
         ("   mObjectNameArray[%d]=%s, draw=%d, show=%d, color=%d\n", i,
          mObjectNameArray[i].c_str(), draw, showObj, mOrbitColorArray[i]);
   }
   MessageInterface::ShowMessage
      ("OrbitPlot::UpdateObjectList() <%p>'%s' leaving\n", this, instanceName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// bool UpdateData(const Real *dat, Integer len)
//------------------------------------------------------------------------------
bool OrbitPlot::UpdateData(const Real *dat, Integer len)
{
   if (len == 0)
      return true;
   
   mNumData++;
   
   #if DBGLVL_UPDATE > 1
   MessageInterface::ShowMessage
      ("   mNumData=%d, mDataCollectFrequency=%d, currentProvider=<%p>\n",
       mNumData, mDataCollectFrequency, currentProvider);
   #endif
   
   // Buffer data if data collect frequency is met or fist data
   if ((mNumData % mDataCollectFrequency) == 0 || (mNumData == 1))
   {
      Integer status = BufferOrbitData(dat, len);
      
      // if solving and plotting current iteration just return
      if (status == 2)
         return true;
      
      #if DBGLVL_UPDATE > 0
      MessageInterface::ShowMessage("==========> Updating plot data\n");
      #endif
      
      bool solving = false;
      UnsignedIntArray colorArray = mScOrbitColorArray;
      if (runstate == Gmat::SOLVING)
      {
         solving = true;
         colorArray = mScTargetColorArray;
      }
      
      bool inFunction = false;
      if (currentProvider && currentProvider->TakeAction("IsInFunction"))
         inFunction = true;
      
      bool update = (mNumCollected % mUpdatePlotFrequency) == 0;
      
      PlotInterface::
         UpdateGlPlot(instanceName, mOldName, mScNameArray, dat[0], mScXArray,
                      mScYArray, mScZArray, mScVxArray, mScVyArray, mScVzArray,
                      colorArray, solving, mSolverIterOption, update,
                      isDataOn, inFunction);
      
      if (update)
         mNumCollected = 0;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool UpdateSolverData()
//------------------------------------------------------------------------------
bool OrbitPlot::UpdateSolverData()
{
   int size = mCurrEpochArray.size();
   int last = size - 1;
   
   #if DBGLVL_SOLVER_CURRENT_ITER
   MessageInterface::ShowMessage("===> num buffered data = %d\n", size);
   MessageInterface::ShowMessage("==========> now update solver plot\n");
   #endif
   
   if (size == 0)
      return true;
   
   UnsignedIntArray colorArray = mScOrbitColorArray;
   if (runstate == Gmat::SOLVING)
      colorArray = mScTargetColorArray;
   else
      colorArray = mScOrbitColorArray;
   
   // Update plot with last iteration data
   for (int i=0; i<size-1; i++)
   {
      #if DBGLVL_SOLVER_CURRENT_ITER > 1
      for (int sc=0; sc<mScCount; sc++)
         MessageInterface::ShowMessage
            ("   i=%d, sc=%d, solver epoch = %f, X,Y,Z = %f, %f, %f\n", i, sc,
             mCurrEpochArray[i], mCurrXArray[i][sc], mCurrYArray[i][sc],
             mCurrZArray[i][sc]);
      #endif
      
      // Just buffer data up to last point - 1
      PlotInterface::
         UpdateGlPlot(instanceName, mOldName, mCurrScArray[i],
                      mCurrEpochArray[i], mCurrXArray[i], mCurrYArray[i],
                      mCurrZArray[i], mCurrVxArray[i], mCurrVyArray[i],
                      mCurrVzArray[i], colorArray, true, mSolverIterOption,
                      false, isDataOn);
   }
   
   // Buffer last point and Update the plot
   PlotInterface::
      UpdateGlPlot(instanceName, mOldName, mCurrScArray[last],
                   mCurrEpochArray[last], mCurrXArray[last], mCurrYArray[last],
                   mCurrZArray[last], mCurrVxArray[last], mCurrVyArray[last],
                   mCurrVzArray[last], colorArray, true, mSolverIterOption,
                   true, isDataOn);
   
   // clear arrays
   mCurrScArray.clear();
   mCurrEpochArray.clear();
   mCurrXArray.clear();
   mCurrYArray.clear();
   mCurrZArray.clear();
   mCurrVxArray.clear();
   mCurrVyArray.clear();
   mCurrVzArray.clear();
   
   if (runstate == Gmat::SOLVING)
      PlotInterface::TakeGlAction(instanceName, "ClearSolverData");
   
   return true;
}


//------------------------------------------------------------------------------
// void BufferZeroData(Integer scIndex)
//------------------------------------------------------------------------------
/**
 * Fills spacecraft state with zero
 */
//------------------------------------------------------------------------------
void OrbitPlot::BufferZeroData(Integer scIndex)
{
   mScXArray[scIndex] = 0.0;
   mScYArray[scIndex] = 0.0;
   mScZArray[scIndex] = 0.0;
   mScVxArray[scIndex] = 0.0;
   mScVyArray[scIndex] = 0.0;
   mScVzArray[scIndex] = 0.0;
}


//------------------------------------------------------------------------------
// Integer BufferOrbitData(const Real *dat, Integer len)
//------------------------------------------------------------------------------
/**
 * @return 1 if continue to updating plot
 *         2 if solving and plotting current iteration
 */
//------------------------------------------------------------------------------
Integer OrbitPlot::BufferOrbitData(const Real *dat, Integer len)
{
   #if DBGLVL_DATA
   MessageInterface::ShowMessage
      ("OrbitPlot::BufferOrbitData() '%s' entered, len=%d\n", GetName().c_str(),
       len);
   #endif
   
   //------------------------------------------------------------
   // buffer orbit data
   //------------------------------------------------------------
   // zero data length is already checked in UpdateData()
   
   #if DBGLVL_DATA > 1
   MessageInterface::ShowMessage
      ("   mNumData=%d, mDataCollectFrequency=%d, currentProvider=<%p>\n",
       mNumData, mDataCollectFrequency, currentProvider);
   WriteCoordinateSystem(theDataCoordSystem, "   theDataCoordSystem");
   WriteCoordinateSystem(mViewCoordSystem, "   mViewCoordSystem");
   #endif
   
   mNumCollected++;
   
   #if DBGLVL_DATA > 1
   MessageInterface::ShowMessage
      ("   currentProvider=%d, theDataLabels.size()=%d\n",
       currentProvider, theDataLabels.size());
   #endif
   
   // @note
   // New Publisher code doesn't assign currentProvider anymore,
   // it just copies current labels. There was an issue with
   // provider id keep incrementing if data is regisgered and
   // published inside a GmatFunction
   StringArray dataLabels = theDataLabels[0];
   
   #if DBGLVL_DATA_LABELS
   MessageInterface::ShowMessage("   Data labels for %s =\n   ", GetName().c_str());
   for (int j=0; j<(int)dataLabels.size(); j++)
      MessageInterface::ShowMessage("%s ", dataLabels[j].c_str());
   MessageInterface::ShowMessage("\n");
   #endif
   
   Integer idX, idY, idZ;
   Integer idVx, idVy, idVz;
   Integer scIndex = -1;
   
   for (Integer i = 0; i < mScCount; i++)
   {
      idX = FindIndexOfElement(dataLabels, mScNameArray[i]+".X");
      idY = FindIndexOfElement(dataLabels, mScNameArray[i]+".Y");
      idZ = FindIndexOfElement(dataLabels, mScNameArray[i]+".Z");
      
      idVx = FindIndexOfElement(dataLabels, mScNameArray[i]+".Vx");
      idVy = FindIndexOfElement(dataLabels, mScNameArray[i]+".Vy");
      idVz = FindIndexOfElement(dataLabels, mScNameArray[i]+".Vz");
      
      #if DBGLVL_DATA_LABELS
      MessageInterface::ShowMessage
         ("   mScNameArray[%d]=%s, idX=%d, idY=%d, idZ=%d, idVx=%d, idVy=%d, idVz=%d\n",
          i, mScNameArray[i].c_str(), idX, idY, idZ, idVx, idVy, idVz);
      #endif
      
      scIndex++;
      
      // If any of index not found, fill with zeros and continue with the next spacecraft
      if (idX  == -1 || idY  == -1 || idZ  == -1 ||
          idVx == -1 || idVy == -1 || idVz == -1)
      {
         BufferZeroData(scIndex);
         continue;
      }

      // DJC: A fix in the Propagate command corrects the error that made this filter
      // necessary, so commenting it out:
      
      // If data epoch is before the spacecraft initial epoch,
      // fill with zeros and continue with the next spacecraft
//      if (dat[0] < mScInitialEpochMap[mScNameArray[i]])
//      {
//         BufferZeroData(scIndex);
//         continue;
//      }
      
      // Buffer actual data
      
      #if DBGLVL_DATA
      MessageInterface::ShowMessage
         ("   %s, epoch = %.11f, X,Y,Z = %f, %f, %f\n", GetName().c_str(), dat[0],
          dat[idX], dat[idY], dat[idZ]);
      #endif
      
      // If distributed data coordinate system is different from view
      // coordinate system, convert data here.
      // If we convert after current epoch, it will not give correct
      // results, if origin is spacecraft,
      // ie, sat->GetMJ2000State(epoch) will not give correct results.
      if ((theDataCoordSystem != NULL && mViewCoordSystem != NULL) &&
          (mViewCoordSystem != theDataCoordSystem))
      {
         #if DBGLVL_DATA
         MessageInterface::ShowMessage
            ("   Converting data from '%s' to '%s'\n", theDataCoordSystem->GetName().c_str(),
             mViewCoordSystem->GetName().c_str());
         #endif
         
         CoordinateConverter coordConverter;
         Rvector6 inState, outState;
         
         // convert position and velocity
         inState.Set(dat[idX], dat[idY], dat[idZ],
                     dat[idVx], dat[idVy], dat[idVz]);
         
         coordConverter.Convert(dat[0], inState, theDataCoordSystem,
                                outState, mViewCoordSystem);
         
         mScXArray[scIndex] = outState[0];
         mScYArray[scIndex] = outState[1];
         mScZArray[scIndex] = outState[2];
         mScVxArray[scIndex] = outState[3];
         mScVyArray[scIndex] = outState[4];
         mScVzArray[scIndex] = outState[5];
      }
      else
      {
         mScXArray[scIndex] = dat[idX];
         mScYArray[scIndex] = dat[idY];
         mScZArray[scIndex] = dat[idZ];
         mScVxArray[scIndex] = dat[idVx];
         mScVyArray[scIndex] = dat[idVy];
         mScVzArray[scIndex] = dat[idVz];
      }
      
      #if DBGLVL_DATA
      MessageInterface::ShowMessage
         ("   after buffering, scNo=%d, scIndex=%d, X,Y,Z = %f, %f, %f\n",
          i, scIndex, mScXArray[scIndex], mScYArray[scIndex], mScZArray[scIndex]);
      #endif
      
      #if DBGLVL_DATA > 1
      MessageInterface::ShowMessage
         ("   Vx,Vy,Vz = %f, %f, %f\n",
          mScVxArray[scIndex], mScVyArray[scIndex], mScVzArray[scIndex]);
      #endif
   }
   
   // if only showing current iteration, buffer data and return
   if (mSolverIterOption == SI_CURRENT)
   {
      // save data when targeting or last iteration
      if (runstate == Gmat::SOLVING || runstate == Gmat::SOLVEDPASS)
      {
         mCurrScArray.push_back(mScNameArray);
         mCurrEpochArray.push_back(dat[0]);
         mCurrXArray.push_back(mScXArray);
         mCurrYArray.push_back(mScYArray);
         mCurrZArray.push_back(mScZArray);
         mCurrVxArray.push_back(mScVxArray);
         mCurrVyArray.push_back(mScVyArray);
         mCurrVzArray.push_back(mScVzArray);
      }
      
      if (runstate == Gmat::SOLVING)
      {
         #if DBGLVL_DATA
         MessageInterface::ShowMessage
            ("=====> num buffered = %d\n", mCurrEpochArray.size());
         #endif
         return 2;
      }
   }
   
   return 1;
}


//------------------------------------------------------------------------------
// void WriteCoordinateSystem(CoordinateSystem *cs, const std::string &label = "")
//------------------------------------------------------------------------------
void OrbitPlot::WriteCoordinateSystem(CoordinateSystem *cs, const std::string &label)
{
   if (cs == NULL)
   {
      MessageInterface::ShowMessage("%s CoordinateSystem is NULL\n");
      return;
   }
   
   std::string originType = "UNKNOWN";
   std::string originName = "UNKNOWN";
   if (cs->GetOrigin())
   {
      originType = (cs->GetOrigin())->GetTypeName();
      originName = (cs->GetOrigin())->GetName();
   }
   
   MessageInterface::ShowMessage
      ("%s = <%p>'%s', isInitialized = %d, origin = <%p><%s>'%s'\n", label.c_str(), cs,
       cs->GetName().c_str(), cs->IsInitialized(), cs->GetOrigin(), originType.c_str(),
       originName.c_str());
}


