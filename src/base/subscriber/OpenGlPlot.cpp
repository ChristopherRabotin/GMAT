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
   "OrbitColor",
   "TargetColor",
   "EquatorialPlane",
   "WireFrame",
   "TargetStatus",
   "Overlap",
   "DataCollectFrequency",
   "UpdatePlotFrequency",
}; 

const Gmat::ParameterType
OpenGlPlot::PARAMETER_TYPE[OpenGlPlotParamCount - SubscriberParamCount] =
{
   Gmat::STRINGARRAY_TYPE,       //"Add"
   Gmat::UNSIGNED_INTARRAY_TYPE, //"OrbitColor"
   Gmat::UNSIGNED_INTARRAY_TYPE, //"TargetColor"
   Gmat::STRING_TYPE,            //"EquatorialPlane"
   Gmat::STRING_TYPE,            //"WireFrame"
   Gmat::STRING_TYPE,            //"TargetStatus"
   Gmat::STRING_TYPE,            //"Overlap"
   Gmat::INTEGER_TYPE,           //"DataCollectFrequency"
   Gmat::INTEGER_TYPE            //"DataCollectFrequency"
};

const UnsignedInt
OpenGlPlot::DEFAULT_ORBIT_COLOR[MAX_SC_COLOR] =
{
   GmatColor::RED32,     GmatColor::YELLOW32,  GmatColor::LIME32,
   GmatColor::AQUA32,    GmatColor::BLUE32,    GmatColor::FUCHSIA32,
   GmatColor::PINK32,    GmatColor::ORANGE32,  GmatColor::L_BLUE32,
   GmatColor::BEIGE32,   GmatColor::SILVER32,  GmatColor::GREEN32,
   GmatColor::L_BROWN32, GmatColor::PURPLE32,  GmatColor::NAVY32
};

//------------------------------------------------------------------------------
// OpenGlPlot(const std::string &name)
//------------------------------------------------------------------------------
OpenGlPlot::OpenGlPlot(const std::string &name) :
   Subscriber("OpenGLPlot", name) //loj: 10/28/04 Channged from OpenGlPlot
{
   // GmatBase data
   parameterCount = OpenGlPlotParamCount;

   mDrawEquatorialPlane = true;
   mDrawWireFrame = false;
   mDrawTarget = false;
   mOverlapPlot = false;
   mDataCollectFrequency = 1;
   mUpdatePlotFrequency = 10;
   mNumData = 0;
   mNumCollected = 0;
   mScNameArray.clear();
   mScXArray.clear();
   mScYArray.clear();
   mScZArray.clear();
   mOrbitColorArray.clear();
   mTargetColorArray.clear();
   mScCount = 0;
}

//------------------------------------------------------------------------------
// OpenGlPlot(const OpenGlPlot &ogl)
//------------------------------------------------------------------------------
OpenGlPlot::OpenGlPlot(const OpenGlPlot &ogl) :
   Subscriber(ogl)
{
   mDrawEquatorialPlane = ogl.mDrawEquatorialPlane;
   mDrawWireFrame = ogl.mDrawWireFrame;
   mDrawTarget = ogl.mDrawTarget;
   mOverlapPlot = ogl.mOverlapPlot;
   mDataCollectFrequency = ogl.mDataCollectFrequency;
   mUpdatePlotFrequency = ogl.mUpdatePlotFrequency;
   mScCount = ogl.mScCount;
   mScNameArray = ogl.mScNameArray;
   mScXArray = ogl.mScXArray;
   mScYArray = ogl.mScYArray;
   mScZArray = ogl.mScZArray;
   mOrbitColorArray = ogl.mOrbitColorArray;
   mTargetColorArray = ogl.mTargetColorArray;
   mOrbitColorMap = ogl.mOrbitColorMap;
   mTargetColorMap = ogl.mTargetColorMap;
   mNumData = 0;
   mNumCollected = 0;
}

//------------------------------------------------------------------------------
// ~OpenGlPlot(void)
//------------------------------------------------------------------------------
OpenGlPlot::~OpenGlPlot(void)
{
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
   MessageInterface::ShowMessage("OpenGlPlot::Initialize() entered mScCount = %d\n",
                                 mScCount);
#endif
   if (mScCount > 0)
   {
      if (active)
      {
#if DEBUG_OPENGL_INIT
         MessageInterface::ShowMessage("OpenGlPlot::Initialize() CreateGlPlotWindow()\n");
#endif
         return PlotInterface::CreateGlPlotWindow(instanceName, mDrawWireFrame,
                                                  mOverlapPlot);
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
          "has been added to OpenGlPlot\n"); //loj: 6/25/04 added \n
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
// bool SetColor(const std::string &item, const std::string &scName,
//               const UnsignedInt value)
//------------------------------------------------------------------------------
bool OpenGlPlot::SetColor(const std::string &item, const std::string &scName,
                          const UnsignedInt value)
{
#if DEBUG_OPENGL_PARAM
   MessageInterface::ShowMessage
      ("OpenGlPlot::SetColor() item=%s, scName=%s, value=%d\n",
       item.c_str(), item.c_str(), value);
#endif

   if (item == "Orbit")
   {
      if (mOrbitColorMap.find(scName) != mOrbitColorMap.end())
      {
         mOrbitColorMap[scName] = value;
         return true;
      }
   }
   else if (item == "Target")
   {
      if (mTargetColorMap.find(scName) != mTargetColorMap.end())
      {
         mTargetColorMap[scName] = value;
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
      return ClearSpacecraftList();
   }
   else if (action == "Remove")
   {
      return RemoveSpacecraft(actionData);
   }
   
   return false;
}

//loj: 11/16/04 added
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
   
   if (type == Gmat::SPACECRAFT)
   {
      // for spacecraft name
      for (int i=0; i<mScCount; i++)
      {
         if (mScNameArray[i] == oldName)
         {
            mScNameArray[i] = newName;
         }
      }

      //----------------------------------------------------
      // Since spacecraft name is used as key for spacecraft
      // color map, I can't change the key name, so it is
      // removed and insert back
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

//------------------------------------------------------------------------------
// virtual UnsignedInt SetUnsignedIntParameter(const Integer id,
//                                             const UnsignedInt value,
//                                             const Integer index)
//------------------------------------------------------------------------------
UnsignedInt OpenGlPlot::SetUnsignedIntParameter(const Integer id,
                                                const UnsignedInt value,
                                                const Integer index)
{
   switch (id)
   {
   case ORBIT_COLOR:
      if (index < mScCount)
      {
         mOrbitColorArray[index] = value;
         return value;
      }
      break;
   case TARGET_COLOR:
      if (index < mScCount)
      {
         mTargetColorArray[index] = value;
         return value;
      }
      break;
   default:
      return Subscriber::SetUnsignedIntParameter(id, value, index);
   }

   throw GmatBaseException("OpenGlPlot::SetUnsignedIntParameter() index out of bounds "
                           "for " + GetParameterText(id));
}

//------------------------------------------------------------------------------
// virtual UnsignedInt SetUnsignedIntParameter(const std::string &label,
//                                             const UnsignedInt value,
//                                             const Integer index)
//------------------------------------------------------------------------------
UnsignedInt OpenGlPlot::SetUnsignedIntParameter(const std::string &label,
                                                const UnsignedInt value,
                                                const Integer index)
{
   return SetUnsignedIntParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// virtual const UnsignedIntArray&
// GetUnsignedIntArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const UnsignedIntArray&
OpenGlPlot::GetUnsignedIntArrayParameter(const Integer id) const
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
// virtual const UnsignedIntArray& 
// GetUnsignedIntArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const UnsignedIntArray& 
OpenGlPlot::GetUnsignedIntArrayParameter(const std::string &label) const
{
   return GetUnsignedIntArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string OpenGlPlot::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case EQUATORIAL_PLANE:
      if (mDrawEquatorialPlane)
         return "On";
      else
         return "Off";
   case WIRE_FRAME:
      if (mDrawWireFrame)
         return "On";
      else
         return "Off";
   case TARGET_STATUS:
      if (mDrawTarget)
         return "On";
      else
         return "Off";
   case OVERLAP_PLOT:
      if (mOverlapPlot)
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
      ("OpenGlPlot::SetStringParameter() id = %d, value = %s \n",
       id, value.c_str());
#endif
   
   switch (id)
   {
   case ADD: //loj: 9/28/04 still needs here to work with current interpreter
      return AddSpacecraft(value, mScCount);
   case EQUATORIAL_PLANE:
      if (value == "On" || value == "Off")
      {
         mDrawEquatorialPlane = (value == "On");
         return true;
      }
      else
      {
         return false;
      }
   case WIRE_FRAME:
      if (value == "On" || value == "Off")
      {
         mDrawWireFrame = (value == "On");
#if DEBUG_OPENGL_PARAM
         MessageInterface::ShowMessage
            ("OpenGlPlot::SetStringParameter() mDrawWireFrame=%d\n", mDrawWireFrame);
#endif
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
#if DEBUG_OPENGL_PARAM
         MessageInterface::ShowMessage
            ("OpenGlPlot::SetStringParameter() mDrawTarget=%d\n", mDrawTarget);
#endif
         return true;
      }
      else
      {
         return false;
      }
   case OVERLAP_PLOT:
      if (value == "On" || value == "Off")
      {
         mOverlapPlot = (value == "On");
#if DEBUG_OPENGL_PARAM
         MessageInterface::ShowMessage
            ("OpenGlPlot::SetStringParameter() mOverlapPlot=%d\n", mOverlapPlot);
#endif
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
      return AddSpacecraft(value, index);
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
      return mScNameArray;
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

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// bool AddSpacecraft(const std::string &name, Integer index)
//------------------------------------------------------------------------------
bool OpenGlPlot::AddSpacecraft(const std::string &name, Integer index)
{
   bool status = false;
   
   if (name != "" && index == mScCount)
   {
      mScNameArray.push_back(name);
      mScXArray.push_back(0.0);
      mScYArray.push_back(0.0);
      mScZArray.push_back(0.0);
      mOrbitColorArray.push_back(0);
      mTargetColorArray.push_back(0);
      mScCount = mScNameArray.size();

      if (mScCount < MAX_SC_COLOR)
      {
         mOrbitColorMap[name] = DEFAULT_ORBIT_COLOR[mScCount-1];
         mTargetColorMap[name] = GmatColor::TEAL32;
      }
      else
      {
         mOrbitColorMap[name] = GmatColor::RED32;
         mTargetColorMap[name] = GmatColor::TEAL32;
      }
      
      status = true;
   }

#if DEBUG_OPENGL_PARAM
   MessageInterface::ShowMessage
      ("OpenGlPlot::AddSpacecraft() mScCount=%d name=%s\n",
       mScCount, name.c_str());
#endif
   
   return status;
}

//------------------------------------------------------------------------------
// bool ClearSpacecraftList()
//------------------------------------------------------------------------------
bool OpenGlPlot::ClearSpacecraftList()
{
   mScNameArray.clear();
   mScXArray.clear();
   mScYArray.clear();
   mScZArray.clear();
   mOrbitColorMap.clear();
   mTargetColorMap.clear();
   mScCount = 0;
   return true;
}

//------------------------------------------------------------------------------
// bool RemoveSpacecraft(const std::string &name)
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
bool OpenGlPlot::RemoveSpacecraft(const std::string &name)
{
#if DEBUG_REMOVE_ACTION
   MessageInterface::ShowMessage
      ("OpenGlPlot::RemoveSpacecraft() name=%s\n--- Before remove:\n", name.c_str());
   for (int i=0; i<mScCount; i++)
   {
      MessageInterface::ShowMessage("mScNameArray[%d]=%s\n", i,
                                    mScNameArray[i].c_str());
   }
#endif
   
   StringArray::iterator scPos = 
      find(mScNameArray.begin(), mScNameArray.end(), name);
   
   if (scPos != mScNameArray.end())
   {
      std::map<std::string, UnsignedInt>::iterator orbColorPos, targColorPos;
      orbColorPos = mOrbitColorMap.find(name);
      targColorPos = mTargetColorMap.find(name);
      
      if (orbColorPos != mOrbitColorMap.end() &&
          targColorPos != mTargetColorMap.end())
      {
         // erase given spacecraft name
         mScNameArray.erase(scPos);
         mOrbitColorMap.erase(orbColorPos);
         mTargetColorMap.erase(targColorPos);

         // reduce the size of array
         mOrbitColorArray.erase(--mOrbitColorArray.end());
         mTargetColorArray.erase(--mTargetColorArray.end());
         mScXArray.erase(--mScXArray.end());
         mScYArray.erase(--mScYArray.end());
         mScZArray.erase(--mScZArray.end());
         
         mScCount = mScNameArray.size();

         // update color array
         for (int i=0; i<mScCount; i++)
         {
            mOrbitColorArray[i] = mOrbitColorMap[mScNameArray[i]];
            mTargetColorArray[i] = mTargetColorMap[mScNameArray[i]];
         }
         
#if DEBUG_REMOVE_ACTION
         MessageInterface::ShowMessage("---After remove\n");
         for (int i=0; i<mScCount; i++)
         {
            MessageInterface::ShowMessage("mScNameArray[%d]=%s\n", i,
                                          mScNameArray[i].c_str());
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
   return distance(labelArray.begin(), pos);
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
   //loj: 6/22/04 added if (isEndOfReceive)
   if (isEndOfReceive)
   {
      return PlotInterface::RefreshGlPlot(instanceName);
   }

   Publisher *thePublisher = Publisher::Instance();

   // if targetting and draw target is off, just return
   if (!mDrawTarget && (thePublisher->GetRunState() == Gmat::TARGETING))
      return true;
   
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

            //loj: 7/30/04 -- try new PublishedDataMap
            StringArray labelArray =
               thePublisher->GetStringArrayParameter("PublishedDataMap");

#if DEBUG_OPENGL_UPDATE
            MessageInterface::ShowMessage("OpenGlPlot::Distribute() labelArray=\n");
            for (int j=0; j<(int)labelArray.size(); j++)
            {
               MessageInterface::ShowMessage
                  ("%s ", labelArray[j].c_str());
            }
            MessageInterface::ShowMessage("\n");
#endif
            
            Integer idX, idY, idZ;
            for (int i=0; i<mScCount; i++)
            {
               idX = FindIndexOfElement(labelArray, mScNameArray[i]+".X");
               idY = FindIndexOfElement(labelArray, mScNameArray[i]+".Y");
               idZ = FindIndexOfElement(labelArray, mScNameArray[i]+".Z");
               mScXArray[i] = dat[idX];
               mScYArray[i] = dat[idY];
               mScZArray[i] = dat[idZ];
               mOrbitColorArray[i] = mOrbitColorMap[mScNameArray[i]];
               mTargetColorArray[i] = mTargetColorMap[mScNameArray[i]];
               
#if DEBUG_OPENGL_UPDATE
               MessageInterface::ShowMessage
                  ("OpenGlPlot::Distribute() scNo=%d x=%f y=%f z=%f\n",
                   i, mScXArray[i], mScYArray[i], mScZArray[i]);
#endif
            }

            //loj: 8/5/04 added
            // If targeting, use targeting color
            if (thePublisher->GetRunState() == Gmat::TARGETING)
            {
               //loj: 7/13/04 used new method taking arrays
               PlotInterface::
                  UpdateGlSpacecraft(instanceName,
                                     dat[0], mScXArray, mScYArray, mScZArray,
                                     mTargetColorArray, update, mDrawWireFrame);
            }
            else
            {
               PlotInterface::
                  UpdateGlSpacecraft(instanceName,
                                     dat[0], mScXArray, mScYArray, mScZArray,
                                     mOrbitColorArray, update, mDrawWireFrame);
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

