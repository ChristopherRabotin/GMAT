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
#include "MessageInterface.hpp"  // for ShowMessage()

#define DEBUG_OPENGL 0

//---------------------------------
// static data
//---------------------------------
const std::string
OpenGlPlot::PARAMETER_TEXT[OpenGlPlotParamCount - SubscriberParamCount] =
{
   "Add",
   "SpacecraftList",
   "ClearSpacecraftList",
   "OrbitColor",
   "TargetColor",
   "Axis",
   "DrawEquatorialPlane",
   "WireFrame",
   "DataCollectFrequency",
   "UpdatePlotFrequency"
}; 

const Gmat::ParameterType
OpenGlPlot::PARAMETER_TYPE[OpenGlPlotParamCount - SubscriberParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::UNSIGNED_INT_TYPE,
   Gmat::UNSIGNED_INT_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::STRING_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::INTEGER_TYPE
};

//------------------------------------------------------------------------------
// OpenGlPlot(const std::string &name)
//------------------------------------------------------------------------------
OpenGlPlot::OpenGlPlot(const std::string &name)
: Subscriber      ("OpenGlPlot", name)
{
   // GmatBase data
   parameterCount = OpenGlPlotParamCount;

   mDrawAxis = false;
   mDrawEquatorialPlane = true;
   mDrawWireFrame = false;
   mDataCollectFrequency = 1;
   mUpdatePlotFrequency = 10;
   mNumData = 0;
   mNumCollected = 0;
   mScList.clear();
   mScCount = 0;
}

//------------------------------------------------------------------------------
// OpenGlPlot(const OpenGlPlot &ogl)
//------------------------------------------------------------------------------
OpenGlPlot::OpenGlPlot(const OpenGlPlot &ogl) :
   Subscriber(ogl)
{
   // GmatBase data
   //parameterCount = OpenGlPlotParamCount;

   mDrawAxis = ogl.mDrawAxis;
   mDrawEquatorialPlane = ogl.mDrawEquatorialPlane;
   mDrawWireFrame = ogl.mDrawWireFrame;
   mDataCollectFrequency = ogl.mDataCollectFrequency;
   mUpdatePlotFrequency = ogl.mUpdatePlotFrequency;
   mScCount = ogl.mScCount;
   mScList = ogl.mScList;
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
#if DEBUG_OPENGL
   MessageInterface::ShowMessage("OpenGlPlot::Initialize() entered mScCount = %d\n",
                                 mScCount);
#endif
   if (mScCount > 0)
   {
      if (active)
      {
         //MessageInterface::ShowMessage("OpenGlPlot::Initialize() CreateGlPlotWindow()\n");
         return PlotInterface::CreateGlPlotWindow();
      }
      else
      {
         //MessageInterface::ShowMessage("OpenGlPlot::Initialize() DeleteGlPlot()\n");
         return PlotInterface::DeleteGlPlot();
      }
   }
   else
   {
      active = false;
      MessageInterface::PopupMessage(Gmat::WARNING_,
                                     "OpenGlPlot is turned off. No spacecraft "
                                     "has been added to OpenGlPlot");
      return false;
   }
}

//------------------------------------------------------------------------------
// bool Distribute(int len)
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

            //loj: assumes data in time, x, y, z order
            //loj: 6/8/04 try color

#if DEBUG_OPENGL
            MessageInterface::
               ShowMessage("OpenGlPlot::Distribute() time=%f pos = %f %f %f\n",
                           dat[0], dat[1], dat[2], dat[3]);
#endif
            
            return PlotInterface::
               UpdateGlSpacecraft(dat[0], dat[1], dat[2], dat[3],
                                  mOrbitColorMap[mScList[0]],
                                  mTargetColorMap[mScList[0]],
                                  update, mDrawWireFrame);
            //return PlotInterface::UpdateGlSpacecraft(dat[0], dat[1], dat[2], dat[3],
            //                                         update, mDrawWireFrame);
            if (update)
               mNumCollected = 0;
         }
      }
   }
   //loj: always return true otherwise next subscriber will not call ReceiveData()
   //     in Publisher::Publish()
   return true;
}

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
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
bool OpenGlPlot::GetBooleanParameter(const Integer id) const
{
   switch (id)
   {
   case DRAW_AXIS:
      return mDrawAxis;
   case DRAW_EQUATORIAL_PLANE:
      return mDrawEquatorialPlane;
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
      mDrawAxis = value;
      return mDrawAxis;
   case DRAW_EQUATORIAL_PLANE:
      mDrawEquatorialPlane = value;
      return mDrawEquatorialPlane;
   case CLEAR_SPACECRAFT_LIST:
      ClearSpacecraftList();
      return true;
   default:
      return Subscriber::SetBooleanParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// UnsignedInt GetUnsignedIntParameter(const Integer id,
//                                     const std::string &item)
//------------------------------------------------------------------------------
UnsignedInt OpenGlPlot::GetUnsignedIntParameter(const Integer id,
                                                const std::string &item)
{   
   switch (id)
   {
   case ORBIT_COLOR:
      if (mOrbitColorMap.find(item) != mOrbitColorMap.end())
         return mOrbitColorMap[item];
      else
         return GmatBase::UNSIGNED_INT_PARAMETER_UNDEFINED;
   case TARGET_COLOR:
      if (mTargetColorMap.find(item) != mTargetColorMap.end())
         return mTargetColorMap[item];
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
UnsignedInt OpenGlPlot::GetUnsignedIntParameter(const std::string &label,
                                                const std::string &item)
{
   return GetUnsignedIntParameter(GetParameterID(label), item);
}

//------------------------------------------------------------------------------
// UnsignedInt SetUnsignedIntParameter(const Integer id,
//                                     const std::string &item,
//                                     const UnsignedInt value)
//------------------------------------------------------------------------------
UnsignedInt OpenGlPlot::SetUnsignedIntParameter(const Integer id,
                                                const std::string &item,
                                                const UnsignedInt value)
{
#if DEBUG_OPENGL
   MessageInterface::ShowMessage
      ("OpenGlPlot::SetUnsignedIntParameter()"
       "id=%d, item=%s, value=%d\n", id, item.c_str(), value);
#endif
   
   switch (id)
   {
   case ORBIT_COLOR:
      if (mOrbitColorMap.find(item) != mOrbitColorMap.end())
      {
         mOrbitColorMap[item] = value;
         return value;
      }
      return GmatBase::UNSIGNED_INT_PARAMETER_UNDEFINED;
   case TARGET_COLOR:
      if (mTargetColorMap.find(item) != mTargetColorMap.end())
      {
         mTargetColorMap[item] = value;
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
UnsignedInt OpenGlPlot::SetUnsignedIntParameter(const std::string &label,
                                                const std::string &item,
                                                const UnsignedInt value)
{
   return SetUnsignedIntParameter(GetParameterID(label), item, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string OpenGlPlot::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case WIRE_FRAME:
      if (mDrawWireFrame)
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
   //MessageInterface::ShowMessage("OpenGlPlot::GetStringParameter() label = %s\n",
   //                              label.c_str());

   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool OpenGlPlot::SetStringParameter(const Integer id, const std::string &value)
{
   //MessageInterface::ShowMessage("OpenGlPlot::SetStringParameter() id = %d, "
   //                              "value = %s \n", id, value.c_str());
   
   switch (id)
   {
   case ADD:
      return AddSpacecraft(value);
   case WIRE_FRAME:
      if (value == "On" || value == "Off")
      {
         mDrawWireFrame = (value == "On");
#if DEBUG_OPENGL
         MessageInterface::ShowMessage
            ("OpenGlPlot::SetStringParameter() mDrawWireFrame=%d\n", mDrawWireFrame);
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
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool OpenGlPlot::SetStringParameter(const std::string &label,
                                    const std::string &value)
{
#if DEBUG_OPENGL
   MessageInterface::ShowMessage("OpenGlPlot::SetStringParameter() label = %s, "
                                 "value = %s \n", label.c_str(), value.c_str());
#endif
   
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& OpenGlPlot::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
   case SPACECRAFT_LIST:
      return mScList;
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
// bool AddSpacecraft(const std::string &name)
//------------------------------------------------------------------------------
bool OpenGlPlot::AddSpacecraft(const std::string &name)
{
   bool status = false;
    
   if (name != "")
   {
      mScList.push_back(name);
      mScCount = mScList.size();

      mOrbitColorMap[name] = GmatColor::RED32;
      mTargetColorMap[name] = GmatColor::ORANGE32;
      status = true;
   }

   return status;
}

//------------------------------------------------------------------------------
// void ClearSpacecraftList()
//------------------------------------------------------------------------------
void OpenGlPlot::ClearSpacecraftList()
{
   mScList.clear();
   mOrbitColorMap.clear();
   mTargetColorMap.clear();
   mScCount = 0;
}
