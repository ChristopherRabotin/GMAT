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

#include "OpenGlPlot.hpp"
#include "PlotInterface.hpp"     // for UpdateGlSpacecraft()
#include "ColorTypes.hpp"        // for namespace GmatColor::
#include "Publisher.hpp"         // for Instance()
#include "GmatBaseException.hpp" // for GmatBaseException()
#include "MessageInterface.hpp"  // for ShowMessage()
#include <algorithm>             // for find(), distance()

//#define DEBUG_OPENGL_INIT 1
//#define DEBUG_OPENGL_ADD 1
//#define DEBUG_OPENGL_OBJ 2
//#define DEBUG_OPENGL_PARAM 1
//#define DEBUG_OPENGL_UPDATE 1
//#define DEBUG_REMOVE_ACTION 1
//#define DEBUG_RENAME 1

//---------------------------------
// static data
//---------------------------------
const std::string
OpenGlPlot::PARAMETER_TEXT[OpenGlPlotParamCount - SubscriberParamCount] =
{
   "Add",
   "CoordinateSystem",
   "ViewPointRef",
   "ViewPointRefVector",
   "ViewPointVector",
   "ViewPointVectorVector",
   "ViewDirection",
   "ViewDirectionVector",
   "ViewScaleFactor",
   "ViewUpCoordinateSystem",
   "ViewUpAxis",
   "CelestialPlane",
   "EquatorialPlane",
   "WireFrame",
   "TargetStatus",
   "Overlap",
   "UseViewPointInfo",
   "PerspectiveMode",
   //"OrbitColor",
   //"TargetColor",
   "DataCollectFrequency",
   "UpdatePlotFrequency",
}; 


const Gmat::ParameterType
OpenGlPlot::PARAMETER_TYPE[OpenGlPlotParamCount - SubscriberParamCount] =
{
   Gmat::STRINGARRAY_TYPE,       //"Add"
   Gmat::STRING_TYPE,            //"CoordinateSystem"
   Gmat::STRING_TYPE,            //"ViewPointRef",
   Gmat::RVECTOR_TYPE,           //"ViewPointRefVector",
   Gmat::STRING_TYPE,            //"ViewPointVector",
   Gmat::RVECTOR_TYPE,           //"ViewPointVectorVector",
   Gmat::STRING_TYPE,            //"ViewDirection",
   Gmat::RVECTOR_TYPE,           //"ViewDirectionVector",
   Gmat::REAL_TYPE,              //"ViewScaleFactor",
   Gmat::STRING_TYPE,            //"ViewUpCoordinaetSystem"
   Gmat::STRING_TYPE,            //"ViewUpAxis"
   Gmat::STRING_TYPE,            //"CelestialPlane"
   Gmat::STRING_TYPE,            //"EquatorialPlane"
   Gmat::STRING_TYPE,            //"WireFrame"
   Gmat::STRING_TYPE,            //"TargetStatus"
   Gmat::STRING_TYPE,            //"Overlap"
   Gmat::STRING_TYPE,            //"UseViewPointInfo"
   Gmat::STRING_TYPE,            //"PerspectiveMode"
   //Gmat::UNSIGNED_INTARRAY_TYPE, //"OrbitColor"
   //Gmat::UNSIGNED_INTARRAY_TYPE, //"TargetColor"
   Gmat::INTEGER_TYPE,           //"DataCollectFrequency"
   Gmat::INTEGER_TYPE,           //"DataCollectFrequency"
};


const UnsignedInt
OpenGlPlot::DEFAULT_ORBIT_COLOR[MAX_SP_COLOR] =
{
   // first three are defaults: Earth, Luna, Sun
   GmatColor::GREEN32,   GmatColor::L_BROWN32,  GmatColor::YELLOW32,
   GmatColor::RED32,     GmatColor::YELLOW32,   GmatColor::LIME32,
   GmatColor::AQUA32,    GmatColor::BLUE32,     GmatColor::FUCHSIA32,
   GmatColor::PINK32,    GmatColor::ORANGE32,   GmatColor::PURPLE32,
   GmatColor::BEIGE32,   GmatColor::SILVER32,   GmatColor::NAVY32
};


//------------------------------------------------------------------------------
// OpenGlPlot(const std::string &name)
//------------------------------------------------------------------------------
OpenGlPlot::OpenGlPlot(const std::string &name)
   : Subscriber("OpenGLPlot", name)
{
   // GmatBase data
   parameterCount = OpenGlPlotParamCount;

   mEclipticPlane = "Off";
   mEquatorialPlane = "On"; //loj: 5/5/05 Changed from off
   mWireFrame = "Off";
   mTargetStatus = "Off";
   mOverlapPlot = "Off";
   mUseViewPointInfo = "On";
   mPerspectiveMode = "Off";
   
   mOldName = instanceName;
   mViewCoordSysName = "EarthMJ2000Eq";
   mViewUpCoordSysName = "EarthMJ2000Eq";
   mViewUpAxisName = "Z";

   // viewpoint
   mViewPointRefName = "Earth";
   mViewPointVectorName = "Vector";
   mViewDirectionName = "Earth";
   mViewScaleFactor = 1.0;
   mViewPointRefVector.Set(0.0, 0.0, 0.0);
   mViewPointVector.Set(0.0, 0.0, 30000.0);
   mViewDirectionVector.Set(0.0, 0.0, -1.0);
   
   mSolarSystem = NULL;
   mViewCoordSystem = NULL;
   mViewUpCoordSystem = NULL;
   mViewCoordSysOrigin = NULL;
   mViewPointRefObj = NULL;
   mViewPointVectorObj = NULL;
   mViewDirectionObj = NULL;
   
   mDataCollectFrequency = 1;
   mUpdatePlotFrequency = 10;
   mNumData = 0;
   mNumCollected = 0;
   mScNameArray.clear();
   mObjectNameArray.clear();
   mAllSpNameArray.clear();
   mAllRefObjectNames.clear();
   mObjectArray.clear();
   mAllSpArray.clear();
   
   mScXArray.clear();
   mScYArray.clear();
   mScZArray.clear();
   mScOrbitColorArray.clear();
   mScTargetColorArray.clear();
   mOrbitColorArray.clear();
   //mTargetColorArray.clear();
   mOrbitColorMap.clear();
   mTargetColorMap.clear();
   mAllSpCount = 0;
   mScCount = 0;
   mObjectCount = 0;
   
   // add Earth, Luna and Sun as default
   AddSpacePoint("Earth", 0);
   AddSpacePoint("Luna", 1);
   AddSpacePoint("Sun", 2);
   
}


//------------------------------------------------------------------------------
// OpenGlPlot(const OpenGlPlot &ogl)
//------------------------------------------------------------------------------
OpenGlPlot::OpenGlPlot(const OpenGlPlot &ogl)
   : Subscriber(ogl)
{
   mEclipticPlane = ogl.mEclipticPlane;
   mEquatorialPlane = ogl.mEquatorialPlane;
   mWireFrame = ogl.mWireFrame;
   mTargetStatus = ogl.mTargetStatus;
   mOverlapPlot = ogl.mOverlapPlot;
   mUseViewPointInfo = ogl.mUseViewPointInfo;
   mPerspectiveMode = ogl.mPerspectiveMode;
   
   mOldName = ogl.mOldName;;
   mViewCoordSysName = ogl.mViewCoordSysName;

   // viewpoint
   mViewPointRefName = ogl.mViewPointRefName;
   mViewPointVectorName = ogl.mViewPointVectorName;
   mViewDirectionName = ogl.mViewDirectionName;
   mViewScaleFactor = ogl.mViewScaleFactor;
   mViewPointRefVector = ogl.mViewPointRefVector;
   mViewPointVector = ogl.mViewPointVector;
   mViewDirectionVector = ogl.mViewDirectionVector;
   mViewUpCoordSysName = ogl.mViewUpCoordSysName;
   mViewUpAxisName = ogl.mViewUpAxisName;
   
   mSolarSystem = ogl.mSolarSystem;
   mViewCoordSystem = ogl.mViewCoordSystem;
   mViewUpCoordSystem = ogl.mViewCoordSystem;
   mViewCoordSysOrigin = ogl.mViewCoordSysOrigin;
   mViewPointRefObj = ogl.mViewPointRefObj;
   mViewPointVectorObj = ogl.mViewPointVectorObj;
   mViewDirectionObj = ogl.mViewDirectionObj;
   
   mDataCollectFrequency = ogl.mDataCollectFrequency;
   mUpdatePlotFrequency = ogl.mUpdatePlotFrequency;
   
   mAllSpCount = ogl.mAllSpCount;
   mScCount = ogl.mScCount;
   mObjectCount = ogl.mObjectCount;
   mObjectArray = ogl.mObjectArray;
   mAllSpArray = ogl.mAllSpArray;
   mScNameArray = ogl.mScNameArray;
   mObjectNameArray = ogl.mObjectNameArray;
   mAllSpNameArray = ogl.mAllSpNameArray;
   mAllRefObjectNames = ogl.mAllRefObjectNames;
   mScXArray = ogl.mScXArray;
   mScYArray = ogl.mScYArray;
   mScZArray = ogl.mScZArray;
   mScOrbitColorArray = ogl.mScOrbitColorArray;
   mScTargetColorArray = ogl.mScTargetColorArray;
   mOrbitColorArray = ogl.mOrbitColorArray;
   //mTargetColorArray = ogl.mTargetColorArray;
   mOrbitColorMap = ogl.mOrbitColorMap;
   mTargetColorMap = ogl.mTargetColorMap;
   mNumData = ogl.mNumData;
   mNumCollected = ogl.mNumCollected;
}


//------------------------------------------------------------------------------
// ~OpenGlPlot(void)
//------------------------------------------------------------------------------
OpenGlPlot::~OpenGlPlot(void)
{
}

//------------------------------------------------------------------------------
// const StringArray& GetSpacePointList()
//------------------------------------------------------------------------------
const StringArray& OpenGlPlot::GetSpacePointList()
{
   return mAllSpNameArray;
}


//------------------------------------------------------------------------------
// const StringArray& GetSpacecraftList()
//------------------------------------------------------------------------------
const StringArray& OpenGlPlot::GetSpacecraftList()
{
   return mScNameArray;
}


//------------------------------------------------------------------------------
// const StringArray& GetNonSpacecraftList()
//------------------------------------------------------------------------------
const StringArray& OpenGlPlot::GetNonSpacecraftList()
{
   return mObjectNameArray;
}


//----------------------------------
// inherited methods from Subscriber
//----------------------------------

//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool OpenGlPlot::Initialize()
{
   Subscriber::Initialize();
   
   #if DEBUG_OPENGL_INIT
      MessageInterface::ShowMessage
         ("OpenGlPlot::Initialize() isEndOfReceive = %d, mAllSpCount = %d\n",
          isEndOfReceive, mAllSpCount);
   #endif
   
   if (mAllSpCount > 0)
   {
      if (active)
      {
         #if DEBUG_OPENGL_INIT
         MessageInterface::ShowMessage
            ("OpenGlPlot::Initialize() CreateGlPlotWindow()\n");
         #endif
         
         if (PlotInterface::CreateGlPlotWindow
             (instanceName, mOldName, mViewCoordSysName, mSolarSystem,
              (mEclipticPlane == "On"), (mEquatorialPlane == "On"),
              (mWireFrame == "On"), (mOverlapPlot == "On"),
              (mUseViewPointInfo == "On"), (mPerspectiveMode == "On")))
         {
            #if DEBUG_OPENGL_INIT
            MessageInterface::ShowMessage
               ("OpenGlPlot::Initialize()) mViewPointRefObj=%d, mViewScaleFactor=%f\n",
                mViewPointRefObj, mViewScaleFactor);
            #endif
            
            //--------------------------------------------------------
            // Set Spacecraft and non-Spacecraft objects.
            // If non-Spacecraft, position has to be computed in the
            // TrajPlotCanvas, so need to pass those object pointers.
            //--------------------------------------------------------
            
            ClearDynamicArrays();

            // add non-spacecraft plot objects to the list
            for (int i=0; i<mAllSpCount; i++)
            {
               #if DEBUG_OPENGL_INIT
               MessageInterface::ShowMessage
                  ("OpenGlPlot::Initialize() mAllSpNameArray[%d]=%s, addr=%d\n",
                   i, mAllSpNameArray[i].c_str(), mAllSpArray[i]);
               #endif
               
               if (mAllSpArray[i])
               {
                  //loj: 5/16/05 add all objects to object list
                  mObjectNameArray.push_back(mAllSpNameArray[i]);
                  mOrbitColorArray.push_back(mOrbitColorMap[mAllSpNameArray[i]]);
                  //mTargetColorArray.push_back(mTargetColorMap[mAllSpNameArray[i]]);
                  mObjectArray.push_back(mAllSpArray[i]);
                  
                  if (mAllSpArray[i]->IsOfType(Gmat::SPACECRAFT))
                  {
                     mScNameArray.push_back(mAllSpNameArray[i]);
                     mScOrbitColorArray.push_back(mOrbitColorMap[mAllSpNameArray[i]]);
                     mScTargetColorArray.push_back(mTargetColorMap[mAllSpNameArray[i]]);
                     mScXArray.push_back(0.0);
                     mScYArray.push_back(0.0);
                     mScZArray.push_back(0.0);
                  }
               }
               else
               {
                  MessageInterface::PopupMessage
                     (Gmat::WARNING_, "The SpacePoint name: %s has NULL pointer.\nIt will be "
                      "removed from the OpenGL plot.\n", mAllSpNameArray[i].c_str());
               }
            }
            
            mScCount = mScNameArray.size();
            mObjectCount = mObjectNameArray.size();
            
            // check ViewPoint info to see if any objects need to be
            // included in the non-spacecraft list
            if (mViewCoordSystem == NULL)
               throw GmatBaseException
                  ("OpenGlPlot::Initialize() CoordinateSystem: " + mViewCoordSysName +
                   "not set\n");
            
            // get CoordinateSystem Origin pointer
            mViewCoordSysOrigin = mViewCoordSystem->GetOrigin();
            
            if (mViewCoordSysOrigin != NULL)
               UpdateObjectList(mViewCoordSysOrigin);
            
            if (mViewPointRefObj != NULL)
                  UpdateObjectList(mViewPointRefObj);
            
            if (mViewPointVectorObj != NULL)
                  UpdateObjectList(mViewPointVectorObj);
            
            if (mViewDirectionObj != NULL)
                  UpdateObjectList(mViewDirectionObj);
            
            #if DEBUG_OPENGL_INIT
            MessageInterface::ShowMessage
               ("OpenGlPlot::Initialize() mScNameArray.size=%d, "
                "mScOrbitColorArray.size=%d\n", mScNameArray.size(),
                mScOrbitColorArray.size());
            MessageInterface::ShowMessage
               ("OpenGlPlot::Initialize() mObjectNameArray.size=%d, "
                "mOrbitColorArray.size=%d\n", mObjectNameArray.size(),
                mOrbitColorArray.size());
            for (int i=0; i<mObjectCount; i++)
            {
               MessageInterface::ShowMessage
                  ("OpenGlPlot::Initialize() mObjectNameArray[%d]=%s,"
                   "color=%d\n", i, mObjectNameArray[i].c_str(),
                   mOrbitColorArray[i]);
            }
            #endif

            // set all object array and pointers
            
            PlotInterface::SetGlObject(instanceName, mObjectNameArray,
                                       mOrbitColorArray, mObjectArray);
            
            //--------------------------------------------------------
            // set CoordinateSystem
            //--------------------------------------------------------
            PlotInterface::SetGlCoordSystem(instanceName, mViewCoordSystem,
                                            mViewUpCoordSystem);
            
            //--------------------------------------------------------
            // set viewpoint info
            //--------------------------------------------------------
            PlotInterface::SetGlViewOption
               (instanceName, mViewPointRefObj, mViewPointVectorObj,
                mViewDirectionObj, mViewScaleFactor, mViewPointRefVector,
                mViewPointVector, mViewDirectionVector, mViewUpAxisName,
                (mViewPointRefName == "Vector"), (mViewPointVectorName == "Vector"),
                (mViewDirectionName == "Vector"));
            
            return true;
         }
         else
         {
            return false;
         }
      }
      else
      {
         #if DEBUG_OPENGL_INIT
         MessageInterface::ShowMessage("OpenGlPlot::Initialize() DeleteGlPlot()\n");
         #endif
         
         return PlotInterface::DeleteGlPlot();
      }
   }
   else
   {
      active = false;
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "OpenGlPlot is turned off. No spacecraft "
          "has been added to OpenGlPlot\n");
      return false;
   }
}


//------------------------------------------------------------------------------
// UnsignedInt GetColor(const std::string &item, const std::string &scName)
//------------------------------------------------------------------------------
UnsignedInt OpenGlPlot::GetColor(const std::string &item,
                                 const std::string &scName)
{
   if (item == "Orbit")
   {
      if (mOrbitColorMap.find(scName) != mOrbitColorMap.end())
         return mOrbitColorMap[scName];
   }
   else if (item == "Target")
   {
      if (mTargetColorMap.find(scName) != mTargetColorMap.end())
         return mTargetColorMap[scName];
   }
   
   return GmatBase::UNSIGNED_INT_PARAMETER_UNDEFINED;
}


//------------------------------------------------------------------------------
// bool SetColor(const std::string &item, const std::string &name,
//               const UnsignedInt value)
//------------------------------------------------------------------------------
bool OpenGlPlot::SetColor(const std::string &item, const std::string &name,
                          const UnsignedInt value)
{
   #if DEBUG_OPENGL_PARAM
   MessageInterface::ShowMessage
      ("OpenGlPlot::SetColor() item=%s, name=%s, value=%d\n",
       item.c_str(), item.c_str(), value);
   #endif

   if (item == "Orbit")
   {
      if (mOrbitColorMap.find(name) != mOrbitColorMap.end())
      {
         mOrbitColorMap[name] = value;
         return true;
      }
   }
   else if (item == "Target")
   {
      if (mTargetColorMap.find(name) != mTargetColorMap.end())
      {
         mTargetColorMap[name] = value;
         return true;
      }
   }
   
   return false;
}

//---------------------------------
// inherited methods from GmatBase
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the OpenGlPlot.
 *
 * @return clone of the OpenGlPlot.
 *
 */
//------------------------------------------------------------------------------
GmatBase* OpenGlPlot::Clone(void) const
{
   return (new OpenGlPlot(*this));
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
bool OpenGlPlot::SetName(const std::string &who)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage("OpenGlPlot::SetName() newName=%s\n", who.c_str());
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
bool OpenGlPlot::TakeAction(const std::string &action,
                            const std::string &actionData)
{
   #if DEBUG_REMOVE_ACTION
   MessageInterface::ShowMessage("OpenGlPlot::TakeAction() action=%s, actionData=%s\n",
                                 action.c_str(), actionData.c_str());
   #endif
   if (action == "Clear")
   {
      return ClearSpacePointList();
   }
   else if (action == "Remove")
   {
      return RemoveSpacePoint(actionData);
   }
   
   return false;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool OpenGlPlot::RenameRefObject(const Gmat::ObjectType type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("OpenGlPlot::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type != Gmat::SPACECRAFT)
      return true;

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
   orbColorPos = mOrbitColorMap.find(oldName);
   targColorPos = mTargetColorMap.find(oldName);
   
   if (orbColorPos != mOrbitColorMap.end() &&
       targColorPos != mTargetColorMap.end())
   {
      // add new spacecraft name key and delete old
      mOrbitColorMap[newName] = mOrbitColorMap[oldName];
      mTargetColorMap[newName] = mTargetColorMap[oldName];
      mOrbitColorMap.erase(orbColorPos);
      mTargetColorMap.erase(targColorPos);
      
      #if DEBUG_RENAME
      MessageInterface::ShowMessage("---After rename\n");
      for (orbColorPos = mOrbitColorMap.begin();
           orbColorPos != mOrbitColorMap.end(); ++orbColorPos)
      {
         MessageInterface::ShowMessage
            ("sc=%s, color=%d\n", orbColorPos->first.c_str(), orbColorPos->second);
      }
      #endif
      
      return true;
   }
   
   return false;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string OpenGlPlot::GetParameterText(const Integer id) const
{
   if (id >= SubscriberParamCount && id < OpenGlPlotParamCount)
      return PARAMETER_TEXT[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterText(id);
    
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer OpenGlPlot::GetParameterID(const std::string &str) const
{
   for (int i=SubscriberParamCount; i<OpenGlPlotParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SubscriberParamCount])
         return i;
   }
   
   return Subscriber::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType OpenGlPlot::GetParameterType(const Integer id) const
{
   if (id >= SubscriberParamCount && id < OpenGlPlotParamCount)
      return PARAMETER_TYPE[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string OpenGlPlot::GetParameterTypeString(const Integer id) const
{
   if (id >= SubscriberParamCount && id < OpenGlPlotParamCount)
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
bool OpenGlPlot::IsParameterReadOnly(const Integer id) const
{
   if ((id == OVERLAP_PLOT) || (id == DATA_COLLECT_FREQUENCY) ||
       (id == UPDATE_PLOT_FREQUENCY) || (id == USE_VIEWPOINT_INFO) ||
       (id == PERSPECTIVE_MODE))// || (id == ORBIT_COLOR) || (id == TARGET_COLOR))
      return true;

   return Subscriber::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer OpenGlPlot::GetIntegerParameter(const Integer id) const
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
Integer OpenGlPlot::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer OpenGlPlot::SetIntegerParameter(const Integer id, const Integer value)
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
Integer OpenGlPlot::SetIntegerParameter(const std::string &label,
                                        const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


// //------------------------------------------------------------------------------
// // virtual UnsignedInt SetUnsignedIntParameter(const Integer id,
// //                                             const UnsignedInt value,
// //                                             const Integer index)
// //------------------------------------------------------------------------------
// UnsignedInt OpenGlPlot::SetUnsignedIntParameter(const Integer id,
//                                                 const UnsignedInt value,
//                                                 const Integer index)
// {
//    switch (id)
//    {
//    case ORBIT_COLOR:
//       if (index < mAllSpCount)
//       {
//          mOrbitColorArray[index] = value;
//          return value;
//       }
//       break;
//    case TARGET_COLOR:
//       if (index < mAllSpCount)
//       {
//          mTargetColorArray[index] = value;
//          return value;
//       }
//       break;
//    default:
//       return Subscriber::SetUnsignedIntParameter(id, value, index);
//    }

//    throw GmatBaseException("OpenGlPlot::SetUnsignedIntParameter() index out of bounds "
//                            "for " + GetParameterText(id));
// }


// //------------------------------------------------------------------------------
// // virtual UnsignedInt SetUnsignedIntParameter(const std::string &label,
// //                                             const UnsignedInt value,
// //                                             const Integer index)
// //------------------------------------------------------------------------------
// UnsignedInt OpenGlPlot::SetUnsignedIntParameter(const std::string &label,
//                                                 const UnsignedInt value,
//                                                 const Integer index)
// {
//    return SetUnsignedIntParameter(GetParameterID(label), value, index);
// }


// //------------------------------------------------------------------------------
// // virtual const UnsignedIntArray&
// // GetUnsignedIntArrayParameter(const Integer id) const
// //------------------------------------------------------------------------------
// const UnsignedIntArray&
// OpenGlPlot::GetUnsignedIntArrayParameter(const Integer id) const
// {
//    switch (id)
//    {
//    case ORBIT_COLOR:
//       return mOrbitColorArray;
//    case TARGET_COLOR:
//       return mTargetColorArray;
//    default:
//       return Subscriber::GetUnsignedIntArrayParameter(id);
//    }
// }


// //------------------------------------------------------------------------------
// // virtual const UnsignedIntArray& 
// // GetUnsignedIntArrayParameter(const std::string &label) const
// //------------------------------------------------------------------------------
// const UnsignedIntArray& 
// OpenGlPlot::GetUnsignedIntArrayParameter(const std::string &label) const
// {
//    return GetUnsignedIntArrayParameter(GetParameterID(label));
// }


//------------------------------------------------------------------------------
// virtual Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
Real OpenGlPlot::GetRealParameter(const Integer id) const
{
   switch (id)
   {
   case VIEW_SCALE_FACTOR:
      return mViewScaleFactor;
   default:
      return Subscriber::GetRealParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
Real OpenGlPlot::GetRealParameter(const std::string &label) const
{
   #if DEBUG_OPENGL_PARAM
     MessageInterface::ShowMessage
        ("OpenGlPlot::GetRealParameter() label = %s\n", label.c_str());
   #endif
   
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real OpenGlPlot::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case VIEW_SCALE_FACTOR:
      mViewScaleFactor = value;
      return value;
   default:
      return Subscriber::SetRealParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
Real OpenGlPlot::SetRealParameter(const std::string &label, const Real value)
{
   #if DEBUG_OPENGL_PARAM
      MessageInterface::ShowMessage
         ("OpenGlPlot::SetRealParameter() label = %s, value = %f \n",
          label.c_str(), value);
   #endif
   
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual const Rvector& GetRvectorParameter(const Integer id) const
//------------------------------------------------------------------------------
const Rvector& OpenGlPlot::GetRvectorParameter(const Integer id) const
{
   switch (id)
   {
   case VIEWPOINT_REF_VECTOR:
      return mViewPointRefVector;
   case VIEWPOINT_VECTOR_VECTOR:
      {
      #if DEBUG_OPENGL_PARAM
        Rvector vec = mViewPointVector;
        MessageInterface::ShowMessage
           ("OpenGlPlot::GetRvectorParameter() returning = %s\n",
            vec.ToString().c_str());
      #endif
      return mViewPointVector;
      }
   case VIEW_DIRECTION_VECTOR:
      return mViewDirectionVector;
   default:
      return Subscriber::GetRvectorParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual const Rvector& GetRvectorParameter(const std::string &label) const
//------------------------------------------------------------------------------
const Rvector& OpenGlPlot::GetRvectorParameter(const std::string &label) const
{
   #if DEBUG_OPENGL_PARAM
     MessageInterface::ShowMessage
        ("OpenGlPlot::GetRvectorParameter() label = %s\n", label.c_str());
   #endif
   
   return GetRvectorParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual const Rvector& SetRvectorParameter(const Integer id,
//                                            const Rvector &value)
//------------------------------------------------------------------------------
const Rvector& OpenGlPlot::SetRvectorParameter(const Integer id,
                                               const Rvector &value)
{
   switch (id)
   {
   case VIEWPOINT_REF_VECTOR:
      mViewPointRefVector[0] = value[0];
      mViewPointRefVector[1] = value[1];
      mViewPointRefVector[2] = value[2];
      return value;
      
   case VIEWPOINT_VECTOR_VECTOR:
      mViewPointVector[0] = value[0];
      mViewPointVector[1] = value[1];
      mViewPointVector[2] = value[2];
      return value;
      
   case VIEW_DIRECTION_VECTOR:
      mViewDirectionVector[0] = value[0];
      mViewDirectionVector[1] = value[1];
      mViewDirectionVector[2] = value[2];
      return value;
      
   default:
      return Subscriber::SetRvectorParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual const Rvector& SetRvectorParameter(const std::string &label,
//                                            const Rvector &value)
//------------------------------------------------------------------------------
const Rvector& OpenGlPlot::SetRvectorParameter(const std::string &label,
                                               const Rvector &value)
{
   #if DEBUG_OPENGL_PARAM
      Rvector val = value;
      MessageInterface::ShowMessage
         ("OpenGlPlot::SetRvectorParameter() label = %s, "
          "value = %s \n", label.c_str(), val.ToString().c_str());
   #endif
   
   return SetRvectorParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string OpenGlPlot::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case COORD_SYSTEM:
      return mViewCoordSysName;
   case VIEWPOINT_REF:
      return mViewPointRefName;
   case VIEWPOINT_VECTOR:
      return mViewPointVectorName;
   case VIEW_DIRECTION:
      return mViewDirectionName;
   case VIEW_UP_COORD_SYSTEM:
      return mViewUpCoordSysName;
   case VIEW_UP_AXIS:
      return mViewUpAxisName;
   case CELESTIAL_PLANE:
      return mEclipticPlane;
   case EQUATORIAL_PLANE:
      return mEquatorialPlane;
   case WIRE_FRAME:
      return mWireFrame;
   case TARGET_STATUS:
      return mTargetStatus;
   case OVERLAP_PLOT:
      return mOverlapPlot;
   case USE_VIEWPOINT_INFO:
      return mUseViewPointInfo;
   case PERSPECTIVE_MODE:
      return mPerspectiveMode;
   default:
      return Subscriber::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string OpenGlPlot::GetStringParameter(const std::string &label) const
{
   #if DEBUG_OPENGL_PARAM
      MessageInterface::ShowMessage
         ("OpenGlPlot::GetStringParameter() label = %s\n", label.c_str());
   #endif
   
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool OpenGlPlot::SetStringParameter(const Integer id, const std::string &value)
{
   #if DEBUG_OPENGL_PARAM
      MessageInterface::ShowMessage
         ("OpenGlPlot::SetStringParameter() id = %d, value = %s \n", id, value.c_str());
   #endif
   
   switch (id)
   {
   case ADD:
      return AddSpacePoint(value, mAllSpCount);
   case COORD_SYSTEM:
      mViewCoordSysName = value;
      return true;
   case VIEWPOINT_REF:
      mViewPointRefName = value;
      return true;
   case VIEWPOINT_VECTOR:
      mViewPointVectorName = value;
      return true;
   case VIEW_DIRECTION:
      mViewDirectionName = value;
      return true;
   case VIEW_UP_COORD_SYSTEM:
      mViewUpCoordSysName = value;
      return true;
   case VIEW_UP_AXIS:
      mViewUpAxisName = value;
      return true;
   case CELESTIAL_PLANE:
      mEclipticPlane = value;
      return true;
   case EQUATORIAL_PLANE:
      mEquatorialPlane = value;
      return true;
   case WIRE_FRAME:
      mWireFrame = value;
      return true;
   case TARGET_STATUS:
      mTargetStatus = value;
      return true;
   case OVERLAP_PLOT:
      mOverlapPlot = value;
      return true;
   case USE_VIEWPOINT_INFO:
      mUseViewPointInfo = value;
      return true;
   case PERSPECTIVE_MODE:
      mPerspectiveMode = value;
      return true;
   default:
      return Subscriber::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool OpenGlPlot::SetStringParameter(const std::string &label,
                                    const std::string &value)
{
   #if DEBUG_OPENGL_PARAM
      MessageInterface::ShowMessage("OpenGlPlot::SetStringParameter() label = %s, "
                                    "value = %s \n", label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool OpenGlPlot::SetStringParameter(const Integer id, const std::string &value,
                                    const Integer index)
{
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
bool OpenGlPlot::SetStringParameter(const std::string &label,
                                    const std::string &value,
                                    const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& OpenGlPlot::GetStringArrayParameter(const Integer id) const
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
// StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& OpenGlPlot::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
std::string OpenGlPlot::GetRefObjectName(const Gmat::ObjectType type) const
{
   #if DEBUG_OPENGL_OBJ
   MessageInterface::ShowMessage
      ("OpenGlPlot::GetRefObjectName() type: %s\n",
       GmatBase::GetObjectTypeString(type).c_str());
   #endif
   
   if (type == Gmat::SOLAR_SYSTEM)
   {
      return mSolarSystem->GetName();
   }
   else if (type == Gmat::COORDINATE_SYSTEM)
   {
      return mViewCoordSysName; //just return this
   }
   
   #if DEBUG_OPENGL_OBJ
   std::string msg = "type: " + GmatBase::GetObjectTypeString(type) + " not found";
   MessageInterface::ShowMessage
      ("OpenGlPlot::GetRefObjectName() %s\n", msg.c_str());
   #endif
   
   return Subscriber::GetRefObjectName(type);
   //return "OpenGlPlot::GetRefObjectName() " + msg;
}


//loj: 5/16/05 only add unique object name to array
//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& OpenGlPlot::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   mAllRefObjectNames.clear();

   if (type == Gmat::SOLAR_SYSTEM)
   {
      mAllRefObjectNames.push_back(mSolarSystem->GetName());
   }
   else if (type == Gmat::COORDINATE_SYSTEM)
   {
      mAllRefObjectNames.push_back(mViewCoordSysName);
      mAllRefObjectNames.push_back(mViewUpCoordSysName);
   }
   else if (type == Gmat::SPACE_POINT)
   {
      mAllRefObjectNames = mAllSpNameArray;
      
      if (find(mAllRefObjectNames.begin(), mAllRefObjectNames.end(),
               mViewPointRefName) == mAllRefObjectNames.end())
         mAllRefObjectNames.push_back(mViewPointRefName);
      
      if (mViewPointVectorName != "Vector")
      {
         if (find(mAllRefObjectNames.begin(), mAllRefObjectNames.end(),
                  mViewPointVectorName) == mAllRefObjectNames.end())
            mAllRefObjectNames.push_back(mViewPointVectorName);
      }
      
      if (mViewDirectionName != "Vector")
      {
         if (find(mAllRefObjectNames.begin(), mAllRefObjectNames.end(),
                  mViewDirectionName) == mAllRefObjectNames.end())
            mAllRefObjectNames.push_back(mViewDirectionName);
      }
   }
   else if (type == Gmat::UNKNOWN_OBJECT)
   {
      mAllRefObjectNames = mAllSpNameArray;
      
      mAllRefObjectNames.push_back(mViewCoordSysName);

      if (mViewCoordSysName != mViewUpCoordSysName)
         mAllRefObjectNames.push_back(mViewUpCoordSysName);

      if (find(mAllRefObjectNames.begin(), mAllRefObjectNames.end(),
               mViewPointRefName) == mAllRefObjectNames.end())
         mAllRefObjectNames.push_back(mViewPointRefName);
      
      if (mViewPointVectorName != "Vector")
      {
         if (find(mAllRefObjectNames.begin(), mAllRefObjectNames.end(),
                  mViewPointVectorName) == mAllRefObjectNames.end())
            mAllRefObjectNames.push_back(mViewPointVectorName);
      }

      
      if (mViewDirectionName != "Vector")
      {
         if (find(mAllRefObjectNames.begin(), mAllRefObjectNames.end(),
                  mViewDirectionName) == mAllRefObjectNames.end())
            mAllRefObjectNames.push_back(mViewDirectionName);
      }
   }

   #if DEBUG_OPENGL_OBJ
   MessageInterface::ShowMessage
      ("OpenGlPlot::GetRefObjectNameArray() returning for type:%d\n", type);
   for (unsigned int i=0; i<mAllRefObjectNames.size(); i++)
      MessageInterface::ShowMessage("%s\n", mAllRefObjectNames[i].c_str());
   #endif
   
   return mAllRefObjectNames;
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
//                                const std::string &name)
//------------------------------------------------------------------------------
GmatBase* OpenGlPlot::GetRefObject(const Gmat::ObjectType type,
                                   const std::string &name)
{
   if (type == Gmat::SOLAR_SYSTEM)
      return mSolarSystem;
   else if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (name == mViewCoordSysName)
         return mViewCoordSystem;
      if (name == mViewUpCoordSysName)
         return mViewUpCoordSystem;
   }
   else if (type == Gmat::SPACE_POINT)
   {
      if (name == mViewPointRefName)
         return mViewPointRefObj;
      else if (name == mViewPointVectorName)
         return mViewPointVectorObj;
      else if (name == mViewDirectionName)
         return mViewDirectionObj;
   }

   return Subscriber::GetRefObject(type, name);
   
//    throw GmatBaseException("OpenGlPlot::GetRefObject() the object name: " + name +
//                            "not found\n");
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
bool OpenGlPlot::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                              const std::string &name)
{
   #if DEBUG_OPENGL_OBJ
   MessageInterface::ShowMessage
      ("OpenGlPlot::SetRefObject() type=%d, typename:%s, name=%s addr=%d\n",
       type, obj->GetTypeName().c_str(), obj->GetName().c_str(), obj);
   #endif
   
   // just check for the type
   if (type == Gmat::SOLAR_SYSTEM)
   {
      mSolarSystem = (SolarSystem*)obj;
      return true;
   }
   else if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (name == mViewCoordSysName)
         mViewCoordSystem = (CoordinateSystem*)obj;
      if (name == mViewUpCoordSysName)
         mViewUpCoordSystem = (CoordinateSystem*)obj;
      return true;
   }
//    else if (type == Gmat::SPACE_POINT || type == Gmat::CELESTIAL_BODY ||
//             type == Gmat::SPACECRAFT || type == Gmat::CALCULATED_POINT)
   else if (obj->IsOfType(Gmat::SPACE_POINT)) //loj: 5/13/05 changed to use IsOfType()
   {
      for (Integer i=0; i<mAllSpCount; i++)
      {
         if (mAllSpNameArray[i] == name)
         {
            #if DEBUG_OPENGL_OBJ > 1
            MessageInterface::ShowMessage
               ("Setting name=%s\n", mAllSpNameArray[i].c_str());
            #endif
            
            mAllSpArray[i] = (SpacePoint*)(obj);
         }
      }
      
      // ViewPoint info
      if (name == mViewPointRefName)
         mViewPointRefObj = (SpacePoint*)obj;
      
      if (name == mViewPointVectorName)
         mViewPointVectorObj = (SpacePoint*)obj;
      
      else if (name == mViewDirectionName)
         mViewDirectionObj = (SpacePoint*)obj;
      
      return true;
   }

   return Subscriber::SetRefObject(obj, type, name);
   //return false;
}


//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// bool AddSpacePoint(const std::string &name, Integer index)
//------------------------------------------------------------------------------
bool OpenGlPlot::AddSpacePoint(const std::string &name, Integer index)
{
   #if DEBUG_OPENGL_ADD
   MessageInterface::ShowMessage
      ("OpenGlPlot::AddSpacePoint() name=%s\n", name.c_str());
   #endif
   
   bool status = false;

   // if name not in the list, add
   if (find(mAllSpNameArray.begin(), mAllSpNameArray.end(), name) == mAllSpNameArray.end())
   {

      if (name != "" && index == mAllSpCount)
      {
         mAllSpNameArray.push_back(name);
         mAllSpArray.push_back(NULL);
         mAllSpCount = mAllSpNameArray.size();
      
         if (mAllSpCount < MAX_SP_COLOR)
         {
            mOrbitColorMap[name] = DEFAULT_ORBIT_COLOR[mAllSpCount-1];
            mTargetColorMap[name] = GmatColor::TEAL32;
         }
         else
         {
            mOrbitColorMap[name] = GmatColor::RED32;
            mTargetColorMap[name] = GmatColor::TEAL32;
         }
      
         status = true;
      }
   }
   
   #if DEBUG_OPENGL_ADD   
   std::string objName;
   for (int i=0; i<mAllSpCount; i++)
   {
      objName = mAllSpNameArray[i];
      MessageInterface::ShowMessage
         ("OpenGlPlot::AddSpacePoint() mAllSpNameArray[%d]=%s, orbColor=%d, "
          "targColor=%d\n", i, objName.c_str(), mOrbitColorMap[objName],
          mTargetColorMap[objName]);
   }
   #endif
   
   return status;
}


//------------------------------------------------------------------------------
// bool ClearSpacePointList()
//------------------------------------------------------------------------------
bool OpenGlPlot::ClearSpacePointList()
{
   MessageInterface::ShowMessage("OpenGlPlot::ClearSpacePointList()\n");
   
   mAllSpNameArray.clear();
   mAllSpArray.clear();
   mObjectArray.clear();
   mScNameArray.clear();
   mObjectNameArray.clear();
   mOrbitColorArray.clear();
   //mTargetColorArray.clear();
   
   mScXArray.clear();
   mScYArray.clear();
   mScZArray.clear();
   mOrbitColorMap.clear();
   mTargetColorMap.clear();
   mAllSpCount = 0;
   mScCount = 0;
   mObjectCount = 0;
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
bool OpenGlPlot::RemoveSpacePoint(const std::string &name)
{
   #if DEBUG_REMOVE_ACTION
   MessageInterface::ShowMessage
      ("OpenGlPlot::RemoveSpacePoint() name=%s\n--- Before remove:\n", name.c_str());
   for (int i=0; i<mAllSpCount; i++)
   {
      MessageInterface::ShowMessage("mAllSpNameArray[%d]=%s\n", i,
                                    mAllSpNameArray[i].c_str());
   }
   #endif
   
   StringArray::iterator scPos = 
      find(mAllSpNameArray.begin(), mAllSpNameArray.end(), name);
   
   if (scPos != mAllSpNameArray.end())
   {
      std::map<std::string, UnsignedInt>::iterator orbColorPos, targColorPos;
      orbColorPos = mOrbitColorMap.find(name);
      targColorPos = mTargetColorMap.find(name);
      
      if (orbColorPos != mOrbitColorMap.end() &&
          targColorPos != mTargetColorMap.end())
      {
         // erase given spacecraft name
         mAllSpNameArray.erase(scPos);
         mOrbitColorMap.erase(orbColorPos);
         mTargetColorMap.erase(targColorPos);

         // reduce the size of array
//          mOrbitColorArray.erase(--mOrbitColorArray.end());
//          mTargetColorArray.erase(--mTargetColorArray.end());
         mScXArray.erase(--mScXArray.end());
         mScYArray.erase(--mScYArray.end());
         mScZArray.erase(--mScZArray.end());
         
         mAllSpCount = mAllSpNameArray.size();

//          // update color array
//          for (int i=0; i<mAllSpCount; i++)
//          {
//             mOrbitColorArray[i] = mOrbitColorMap[mAllSpNameArray[i]];
//             mTargetColorArray[i] = mTargetColorMap[mAllSpNameArray[i]];
//          }
         
         #if DEBUG_REMOVE_ACTION
         MessageInterface::ShowMessage("---After remove\n");
         for (int i=0; i<mAllSpCount; i++)
         {
            MessageInterface::ShowMessage("mAllSpNameArray[%d]=%s\n", i,
                                          mAllSpNameArray[i].c_str());
         }
         #endif
         
         return true;
      }
   }

   return false;
}


//------------------------------------------------------------------------------
// Integer FindIndexOfElement(StringArray &labelArray, const std::string &label)
//------------------------------------------------------------------------------
Integer OpenGlPlot::FindIndexOfElement(StringArray &labelArray,
                                       const std::string &label)
{
   std::vector<std::string>::iterator pos;
   pos = find(labelArray.begin(), labelArray.end(),  label);
   if (pos == labelArray.end())
      return -1; //loj: 5/3/05 Added
   else
      return distance(labelArray.begin(), pos);
}


// //------------------------------------------------------------------------------
// // bool IsNonSpacecraft(const std::string &name)
// //------------------------------------------------------------------------------
// bool OpenGlPlot::IsNonSpacecraft(const std::string &name)
// {
//    // How do I know from the name? mSolarSystem may be null.
//    // so create SolarSystem here?
   
//    SolarSystem ss("");
//    StringArray bodies = ss.GetBodiesInUse();
   
//    for (unsigned int i=0; i<bodies.size(); i++)
//    {
//       if (name == bodies[i])
//          return true;
//    }

//    MessageInterface::ShowMessage
//       ("OpenGlPlot::IsNonSpacecraft() %s is a Spacecraft\n", name.c_str());
   
//    return false;
// }


//------------------------------------------------------------------------------
// void ClearDynamicArrays()
//------------------------------------------------------------------------------
void OpenGlPlot::ClearDynamicArrays()
{
   mObjectNameArray.clear();
   mOrbitColorArray.clear();
   //mTargetColorArray.clear();
   mObjectArray.clear();
   mScNameArray.clear();
   mScOrbitColorArray.clear();
   mScTargetColorArray.clear();
   mScXArray.clear();
   mScYArray.clear();
   mScZArray.clear();
}


//------------------------------------------------------------------------------
// void UpdateObjectList(SpacePoint *sp)
//------------------------------------------------------------------------------
/**
 * Add non-spacecraft object to the list.
 */
//------------------------------------------------------------------------------
void OpenGlPlot::UpdateObjectList(SpacePoint *sp)
{
   //loj: 5/16/05 Add all spacepoint objects
//    if (!sp->IsOfType(Gmat::SPACECRAFT))
//    {
      std::string name = sp->GetName();
      StringArray::iterator pos = 
         find(mObjectNameArray.begin(), mObjectNameArray.end(), name);

      // if name not found
      if (pos == mObjectNameArray.end())
      {
         mObjectNameArray.push_back(name);
         mOrbitColorArray.push_back(mOrbitColorMap[name]);
         //mTargetColorArray.push_back(mTargetColorMap[name]);
         mObjectArray.push_back(sp);
         mObjectCount = mObjectNameArray.size();
      }
//    }
}


//--------------------------------------
// methods inherited from Subscriber
//--------------------------------------

//------------------------------------------------------------------------------
// bool Distribute(int len)
//------------------------------------------------------------------------------
bool OpenGlPlot::Distribute(int len)
{
   //loj: How do I convert data to Real data?
   return false;
}


//------------------------------------------------------------------------------
// bool Distribute(const Real *dat, Integer len)
//------------------------------------------------------------------------------
bool OpenGlPlot::Distribute(const Real *dat, Integer len)
{
   #if DEBUG_OPENGL_UPDATE > 1
   MessageInterface::ShowMessage
      ("OpenGlPlot::Distribute() isEndOfReceive=%d, mAllSpCount=%d, len=%d\n",
       isEndOfReceive, mAllSpCount, len);
   #endif
   
   if (isEndOfReceive)
   {
      if (active) //loj: 5/2/05 Added
         return PlotInterface::RefreshGlPlot(instanceName);
   }

   Publisher *thePublisher = Publisher::Instance();

   // if targetting and draw target is off, just return
   if ((mTargetStatus == "Off") && (thePublisher->GetRunState() == Gmat::TARGETING))
   {
      #if DEBUG_OPENGL_UPDATE > 1
      MessageInterface::ShowMessage
         ("OpenGlPlot::Distribute() TargetStatus is off and TARGETING\n");
      #endif
      
      return true;
   }
   
   if (mScCount > 0)
   {
      if (len > 0)
      {
         mNumData++;
         
         if ((mNumData % mDataCollectFrequency) == 0)
         {
            mNumData = 0;
            mNumCollected++;
            bool update = (mNumCollected % mUpdatePlotFrequency) == 0;
            
            //get data being published
            StringArray labelArray =
               thePublisher->GetStringArrayParameter("PublishedDataMap");
            
            #if DEBUG_OPENGL_UPDATE > 1
            MessageInterface::ShowMessage("OpenGlPlot::Distribute() labelArray=\n");
            for (int j=0; j<(int)labelArray.size(); j++)
            {
               MessageInterface::ShowMessage("%s ", labelArray[j].c_str());
            }
            MessageInterface::ShowMessage("\n");
            #endif
            
            Integer idX, idY, idZ;
            Integer scIndex = -1;
            for (int i=0; i<mScCount; i++)
            {
               idX = FindIndexOfElement(labelArray, mScNameArray[i]+".X");
               idY = FindIndexOfElement(labelArray, mScNameArray[i]+".Y");
               idZ = FindIndexOfElement(labelArray, mScNameArray[i]+".Z");
               
               #if DEBUG_OPENGL_UPDATE > 1
               MessageInterface::ShowMessage
                  ("OpenGlPlot::Distribute() i=%d, idX=%d, idY=%d, idZ=%d\n",
                   i, idX, idY, idZ);
               #endif
               
               scIndex++;
               mScXArray[scIndex] = dat[idX];
               mScYArray[scIndex] = dat[idY];
               mScZArray[scIndex] = dat[idZ];
                              
               #if DEBUG_OPENGL_UPDATE
               MessageInterface::ShowMessage
                  ("OpenGlPlot::Distribute() scNo=%d x=%f y=%f z=%f\n",
                   i, mScXArray[scIndex], mScYArray[scIndex], mScZArray[scIndex]);
               #endif
               
            }
            
            // If targeting, use targeting color
            if (thePublisher->GetRunState() == Gmat::TARGETING)
            {
               PlotInterface::
                  UpdateGlPlot(instanceName, mOldName, mViewCoordSysName,
                               mScNameArray, dat[0], mScXArray,
                               mScYArray, mScZArray, mScTargetColorArray, update);
            }
            else
            {
               PlotInterface::
                  UpdateGlPlot(instanceName, mOldName, mViewCoordSysName,
                               mScNameArray, dat[0], mScXArray,
                               mScYArray, mScZArray, mScOrbitColorArray, update);
            }
            
            if (update)
               mNumCollected = 0;
         }
      }
   }

   //loj: always return true otherwise next subscriber will not call ReceiveData()
   //     in Publisher::Publish()
   return true;
}

