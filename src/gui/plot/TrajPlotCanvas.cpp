//$Header$
//------------------------------------------------------------------------------
//                              TrajPlotCanvas
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2003/11/25
/**
 * Implements TrajPlotCanvas for opengl plot.
 */
//------------------------------------------------------------------------------
#include "TrajPlotCanvas.hpp"
#include "GmatAppData.hpp"        // for GetGuiInterpreter()
#include "FileManager.hpp"        // for texture files
#include "ColorTypes.hpp"         // for namespace GmatColor::
#include "Rvector3.hpp"           // for Rvector3::GetMagnitude()
#include "AngleUtil.hpp"          // for ComputeAngleInDeg()
#include "MdiGlPlotData.hpp"
#include "MessageInterface.hpp"

#include <string.h>               // for strlen()

#ifdef __WXMAC__
#  ifdef __DARWIN__
#    include <OpenGL/gl.h>
#    include <OpenGL/glu.h>
#  else
#    include <gl.h>
#    include <glu.h>
#  endif
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
//#  include <GL/glut.h>
#endif

//#define SKIP_DEVIL
#ifndef SKIP_DEVIL
#  include <IL/il.h>
#  include <IL/ilu.h>
#  include <IL/ilut.h>
#endif


//#define USE_TRACKBALL
#ifdef USE_TRACKBALL
#include "AttitudeUtil.hpp"
using namespace FloatAttUtil;
#endif

//#define ENABLE_LIGHT_SOURCE
#define USE_MHA_TO_ROTATE_EARTH

// If Sleep in not defined (on unix boxes)
#ifndef Sleep 
#ifndef __WXMSW__
#include <unistd.h>
#define Sleep(t) usleep((t))
#endif
#endif

// if test Euler angle on mouse event
#define COMPUTE_EULER_ANGLE
#ifdef COMPUTE_EULER_ANGLE
#include "AttitudeUtil.hpp"
//#define USE_MODELVIEW_MAT
//#define DEBUG_TRAJCANVAS_EULER 1
#endif

// skip over limit data (loj: 1/9/06 added)
//#define SKIP_OVER_LIMIT_DATA

//loj: 1/4/06 debug
//#define DEBUG_TRAJCANVAS_INIT 1
//#define DEBUG_TRAJCANVAS_UPDATE 1
//#define DEBUG_TRAJCANVAS_UPDATE_OBJECT 2
//#define DEBUG_TRAJCANVAS_ACTION 1
//#define DEBUG_TRAJCANVAS_CONVERT 1
//#define DEBUG_TRAJCANVAS_DRAW 1
//#define DEBUG_TRAJCANVAS_ZOOM 1
//#define DEBUG_TRAJCANVAS_OBJECT 1
//#define DEBUG_TRAJCANVAS_TEXTURE 2
//#define DEBUG_TRAJCANVAS_PERSPECTIVE 1
//#define DEBUG_TRAJCANVAS_PROJ 2
//#define DEBUG_TRAJCANVAS_ANIMATION 1
//#define DEBUG_TRAJCANVAS_LONGITUDE 1
//#define DEBUG_SHOW_SKIP 1

BEGIN_EVENT_TABLE(TrajPlotCanvas, wxGLCanvas)
   EVT_SIZE(TrajPlotCanvas::OnTrajSize)
   EVT_PAINT(TrajPlotCanvas::OnPaint)
   EVT_MOUSE_EVENTS(TrajPlotCanvas::OnMouse)
   EVT_KEY_DOWN(TrajPlotCanvas::OnKeyDown)
END_EVENT_TABLE()

using namespace GmatPlot;
using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

//const int TrajPlotCanvas::MAX_DATA = 20000;
//const int TrajPlotCanvas::MAX_OBJECT = 50;
const int TrajPlotCanvas::LAST_STD_BODY_ID = 10;
const int TrajPlotCanvas::MAX_COORD_SYS = 10;
const float TrajPlotCanvas::MAX_ZOOM_IN = 3700.0;
const float TrajPlotCanvas::RADIUS_ZOOM_RATIO = 2.2;
const float TrajPlotCanvas::DEFAULT_DIST = 30000.0;
const int TrajPlotCanvas::UNKNOWN_OBJ_ID = -999;

struct GlColorType
{
   Byte red;
   Byte green;
   Byte blue;
   Byte not_used;
};


// color
static int *sIntColor = new int;
static GlColorType *sGlColor = (GlColorType*)sIntColor;

//------------------------------------------------------------------------------
// TrajPlotCanvas(wxWindow *parent, wxWindowID id,
//                const wxPoint& pos, const wxSize& size, const wxString &csName,
//                SolarSystem *solarSys, const wxString& name, long style)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <parent>   parent window pointer
 * @param <id>       window id
 * @param <pos>      position (top, left) where the window to be placed within the
 *                   parent window
 * @param <size>     size of the window
 * @param <csName>   coordinate system name for data to be plotted in
 * @param <solarSys> solar system pointer to retrieve body information
 * @param <name>     title of window
 * @param <style>    style of window
 *
 */
//------------------------------------------------------------------------------
TrajPlotCanvas::TrajPlotCanvas(wxWindow *parent, wxWindowID id,
                               const wxPoint& pos, const wxSize& size,
                               const wxString &csName, SolarSystem *solarSys,
                               const wxString& name, long style)
   : wxGLCanvas(parent, id, pos, size, style, name)
{    
   // initalize data members
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theStatusBar = GmatAppData::GetMainFrame()->GetStatusBar();
   mTextTrajFile = NULL;
   mGlList = 0;
   mNumData = 0;
   mIsEndOfData = false;
   mIsEndOfRun = false;
   
   // projection
   mUsePerspectiveMode = false;
   
   // viewpoint
   mViewPointRefObjName = "UNKNOWN";
   mViewPointRefObj = NULL;
   mViewPointVectorObj = NULL;
   mViewDirectionObj = NULL;
   mViewPointRefVector.Set(0.0, 0.0, 0.0);
   mViewPointVector.Set(0.0, 0.0, 30000.0);
   mViewDirectionVector.Set(0.0, 0.0, -1.0);
   mVpLocVec.Set(0.0, 0.0, 30000.0);
   mViewScaleFactor = 1.0;
   mUseInitialViewPoint = false;
   mUseViewPointRefVector = true;
   mUseViewPointVector = true;
   mUseViewDirectionVector = true;
   mVpRefObjId = UNKNOWN_OBJ_ID;
   mVpVecObjId = UNKNOWN_OBJ_ID;
   mVdirObjId = UNKNOWN_OBJ_ID;
   
   // view
   mCanvasSize = size;
   mDefaultRotXAngle = 90.0;
   mDefaultRotYAngle = 0.0;
   mDefaultRotZAngle = 0.0;
   mDefaultViewDist = DEFAULT_DIST;
   
   mCurrRotXAngle = mDefaultRotXAngle;
   mCurrRotYAngle = mDefaultRotYAngle;
   mCurrRotZAngle = mDefaultRotZAngle;
   mCurrViewDist = mDefaultViewDist;
   
   mfCamTransX = 0;
   mfCamTransY = 0;
   mfCamTransZ = 0;
   
   // view model
   mUseGluLookAt = false;
   mUseSingleRotAngle = true;
   
   // performance
   // if mNumPointsToRedraw =  0 It redraws whole plot
   // if mNumPointsToRedraw = -1 It does not clear GL_COLOR_BUFFER
   mNumPointsToRedraw = 0;
   mRedrawLastPointsOnly = false;
   mUpdateFrequency = 50;
   
   mAxisLength = mCurrViewDist;
   
   mOriginName = "";
   mOriginId = 0;
   
   //original value
   mRotateAboutXaxis = true;   // 2-3-1
   mRotateAboutYaxis = false;  // 3-1-2
   mRotateAboutZaxis = false;  // 1-2-3
   
//    mRotateAboutXaxis = false;   // 2-3-1
//    mRotateAboutYaxis = false ;  // 3-1-2
//    mRotateAboutZaxis = true;    // 1-2-3
   
   mRotateXy = true;
   
   mZoomAmount = 300.0;
   
   // projection
   ChangeProjection(size.x, size.y, mAxisLength);
   
   mEarthRadius = 6378.14; //km
   mScRadius = 200;        //km: make big enough to see
   
   // light source
   mSunPresent = false;
   mEnableLightSource = true;
   
   // drawing options
   mDrawWireFrame = false;
   mDrawXyPlane = false;
   mDrawEcPlane = false;
   mDrawAxes = false;
   mDrawGrid = false;
   mDrawOrbitNormal = true;
   mXyPlaneColor = GmatColor::SKYBLUE;
   mEcPlaneColor = GmatColor::CHESTNUT;
   mESLinecolor = GmatColor::GOLDTAN;
   
   // animation
   mViewAnimation = false;
   mHasUserInterrupted = false;
   mUpdateInterval = 50;
   
   // message
   mShowMaxWarning = true;
   mOverCounter = 0;
   
   // solar system
   mSolarSystem = solarSys;
   //MessageInterface::ShowMessage
   //   ("TrajPlotCanvas::TrajPlotCanvas() mSolarSystem=%d\n", mSolarSystem);
   
   for (int i=0; i<MAX_BODIES; i++)
   {
      mObjMaxZoomIn[i] = MAX_ZOOM_IN;
   }
   
   // objects
   mObjectDefaultRadius = 200; //km: make big enough to see
   
   for (int obj=0; obj<MAX_OBJECT; obj++)
      mObjectRadius[obj] = 0.0;
   
   // Zoom
   mMaxZoomIn = MAX_ZOOM_IN;
   
   // Spacecraft
   mScCount = 0;
   
   // Coordinate System
   mInternalCoordSystem = theGuiInterpreter->GetInternalCoordinateSystem();
   mInternalCoordSysName = wxString(mInternalCoordSystem->GetName().c_str());

   mViewCoordSysName = csName;
   mViewCoordSystem = theGuiInterpreter->
      GetCoordinateSystem(std::string(csName.c_str()));

   // CoordinateSystem conversion
   mIsInternalCoordSystem = true;
   mNeedSpacecraftConversion = false;
   mNeedOriginConversion = false;
   mNeedObjectConversion = false;
   
   if (!mViewCoordSysName.IsSameAs(mInternalCoordSysName))
       mNeedInitialConversion = true;
   else
       mNeedInitialConversion = false;

   #if DEBUG_TRAJCANVAS_INIT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas() internalCS=%s, viewCS=%s\n",
       mInternalCoordSystem->GetName().c_str(), mViewCoordSystem->GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// ~TrajPlotCanvas()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
TrajPlotCanvas::~TrajPlotCanvas()
{
   if (mTextTrajFile)
      delete mTextTrajFile;

}


//------------------------------------------------------------------------------
// bool TrajPlotCanvas::InitGL()
//------------------------------------------------------------------------------
/**
 * Initializes GL and IL.
 */
//------------------------------------------------------------------------------
bool TrajPlotCanvas::InitGL()
{
   // remove back faces
   glDisable(GL_CULL_FACE);
   
   // enable depth testing, so that objects further away from the
   // viewer aren't drawn over closer objects
   glEnable(GL_DEPTH_TEST);
   
   glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
   glDepthFunc(GL_LESS);
   //glDepthRange(0.0, 100.0); //loj: just tried - made no difference
   
   // speedups
   glEnable(GL_DITHER);
   
   // set polygons to be smoothly shaded (i.e. interpolate lighting equations
   // between points on polygons).
   glShadeModel(GL_SMOOTH);
   
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
   glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
   
#ifndef SKIP_DEVIL
   
   // initalize devIL library
   ilInit();
   ilutInit();
   ilutRenderer(ILUT_OPENGL);
   
#endif
   
   if (!LoadGLTextures())
      return false;

   // pixel format
   if (!SetPixelFormatDescriptor())
      return false;
   
   // font
   SetDefaultGLFont();
   
   mShowMaxWarning = true;
   mNumData = 0;
   mOverCounter = 0;
   mIsEndOfData = false;
   mIsEndOfRun = false;
   mViewAnimation = false;
   
   if (mUseInitialViewPoint)
   {
      mfCamRotXAngle = 0;
      mfCamRotYAngle = 0;
      mfCamRotZAngle = 0;
   }

   #ifdef USE_TRACKBALL
      ToQuat(mQuat, 0.0f, 0.0f, 0.0f, 0.0);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// wxString GetGotoObjectName()
//------------------------------------------------------------------------------
wxString TrajPlotCanvas::GetGotoObjectName()
{
   return mObjectNames[mViewObjId];
}


//------------------------------------------------------------------------------
// void SetEndOfRun(bool flag = true)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetEndOfRun(bool flag)
{
   mIsEndOfRun = flag;
   mIsEndOfData = flag;

   if (mIsEndOfRun)
   {
      #if DEBUG_TRAJCANVAS_LONGITUDE
      MessageInterface::ShowMessage
         ("===> TrajPlotCanvas::SetEndOfRun() mIsEndOfRun=%d, mNumData=%d\n",
          mIsEndOfRun, mNumData);
      #endif
      
      //-------------------------------------------------------
      // get first spacecraft id
      //-------------------------------------------------------
      int objId = UNKNOWN_OBJ_ID;
      for (int sc=0; sc<mScCount; sc++)
      {
         objId = GetObjectId(mScNameArray[sc].c_str());
         
         if (objId != UNKNOWN_OBJ_ID)
            break;
      }
      
      Real time = mTime[mNumData-1];
      Real x = mObjectGciPos[objId][mNumData-1][0];
      Real y = mObjectGciPos[objId][mNumData-1][1];
      Real lst, longitude, mha;
      
      ComputeLongitudeLst(time, x, y, &mha, &longitude, &lst);
      mFinalMha = mha;
      mFinalLongitude = longitude;
      mFinalLst = lst;
      
      #if DEBUG_TRAJCANVAS_LONGITUDE
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::SetEndOfRun() mInitialLongitude=%f, time=%f, x=%f,\n   "
          "y=%f, mFinalMha=%f, mFinalLongitude=%f, mFinalLst=%f\n",
          mInitialLongitude, time, x, y, mha, longitude, lst);
      #endif

      //-------------------------------------------------------------------
      // Process mouse left click event to avoid flipping back to default
      // view when OnPaint() is called via the GmatMainFrame. This only
      // happens when opening a new script file for the first time after
      // the run. After the second time, it doesn't flip the view.
      // I don't know why this is happening.
      // This causes to keep adding rotation angle, so comment out.
      //-------------------------------------------------------------------
      //wxMouseEvent event(wxEVT_LEFT_DOWN);
      //event.SetEventObject(this);
      //GetEventHandler()->ProcessEvent(event);
   }
}


//------------------------------------------------------------------------------
// void SetViewCoordSystem(const wxString &csName)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetViewCoordSystem(const wxString &csName)
{
   #if DEBUG_TRAJCANVAS_INIT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas()::SetViewCoordSysName() internalCS=%s, viewCS=%s\n",
       mInternalCoordSystem->GetName().c_str(), csName.c_str());
   #endif
   
   mViewCoordSysName = csName;
   
   mViewCoordSystem =
      theGuiInterpreter->GetCoordinateSystem(std::string(csName.c_str()));
   
   if (!mViewCoordSysName.IsSameAs(mInternalCoordSysName))
   {
      mIsInternalCoordSystem = false;
      mNeedInitialConversion = true;
      mNeedOriginConversion = true;
      mNeedObjectConversion = true;
   }
   else
   {
      mIsInternalCoordSystem = true;
      mNeedInitialConversion = false;
      mNeedOriginConversion = false;
      mNeedObjectConversion = false;
   }
   
}


//------------------------------------------------------------------------------
// void SetUsePerspectiveMode(bool perspMode)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetUsePerspectiveMode(bool perspMode)
{
   #if DEBUG_TRAJCANVAS_INIT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas()::SetUsePerspectiveMode() perspMode=%d\n", perspMode);
   #endif
   
   mUsePerspectiveMode = perspMode;
   
   if (mUsePerspectiveMode)
   {
      mfCamTransX = -mVpLocVec[0];
      mfCamTransY = -mVpLocVec[1];
      mfCamTransZ = -mVpLocVec[2];
      mUseGluLookAt = true; //loj: 12/7/05 try gluLookAt()
   }
   else
   {
      mfCamTransX = 0;
      mfCamTransY = 0;
      mfCamTransZ = 0;
      
      //loj: 12/27/07 set to use gluLookAt, it works for up direction
      //mUseGluLookAt = false;
      mUseGluLookAt = true;
   }
}


//------------------------------------------------------------------------------
// void SetObjectColors(const wxStringColorMap &objectColorMap)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetObjectColors(const wxStringColorMap &objectColorMap)
{
   mObjectColorMap = objectColorMap;
}


//------------------------------------------------------------------------------
// void SetShowObjects(const wxStringColorMap &showObjMap)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetShowObjects(const wxStringBoolMap &showObjMap)
{
   mShowObjectMap = showObjMap;
}


//------------------------------------------------------------------------------
// void SetShowOrbitNormals(const wxStringColorMap &showOrbNormMap)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetShowOrbitNormals(const wxStringBoolMap &showOrbNormMap)
{
   mShowOrbitNormalMap = showOrbNormMap;
}


//------------------------------------------------------------------------------
// void ClearPlot()
//------------------------------------------------------------------------------
/**
 * Clears plot.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::ClearPlot()
{
   //loj: black for now.. eventually it will use background color
   glClearColor(0.0, 0.0, 0.0, 1);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glFlush();
   SwapBuffers();
   mNumData = 0;
}


//------------------------------------------------------------------------------
// void RedrawPlot(bool viewAnimation)
//------------------------------------------------------------------------------
/**
 * Redraws plot.
 *
 * @param <viewAnimation> true if animation is viewed
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::RedrawPlot(bool viewAnimation)
{
   if (mAxisLength < mMaxZoomIn)
   {
      mAxisLength = mMaxZoomIn;
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::RedrawPlot() distance < max zoom in. distance set to %f\n",
          mAxisLength);
   }

   //loj: 1/11/06 commented out. It seems to work without resetting.
   //loj: 5/23/05 Why mVpVecObjId changed?, so need to reset.
   //    if (!mUseViewPointVector && mViewPointVectorObj != NULL)
   //       mVpVecObjId = GetObjectId(mViewPointVectorObj->GetName().c_str());
   
   //    ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
   
   if (viewAnimation)
      ViewAnimation(mUpdateInterval);
   else
      Refresh(false);
   
}


//------------------------------------------------------------------------------
// void ShowDefaultView()
//------------------------------------------------------------------------------
/**
 * Shows default view.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::ShowDefaultView()
{
   int clientWidth, clientHeight;
   GetClientSize(&clientWidth, &clientHeight);

   SetDefaultView();
   ChangeView(mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);
   ChangeProjection(clientWidth, clientHeight, mAxisLength);
   Refresh(false);
}


//------------------------------------------------------------------------------
// void ZoomIn()
//------------------------------------------------------------------------------
/**
 * Zoom in the picture.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::ZoomIn()
{
   #if DEBUG_TRAJCANVAS_ZOOM
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::ZoomIn() mAxisLength=%f, mMaxZoomIn=%f\n",
       mAxisLength, mMaxZoomIn);
   #endif

   Real realDist = (mAxisLength - mZoomAmount) / log(mAxisLength);

   if (mUsePerspectiveMode)
   {
      if (mAxisLength > mMaxZoomIn/mFovDeg*4)
      {
         mAxisLength = mAxisLength - realDist;
         
         if (mAxisLength < mObjectRadius[mOriginId]/2.0)
            mAxisLength = mObjectRadius[mOriginId]/2.0;
         
         ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
      }
   }
   else
   {
      if (mAxisLength > mMaxZoomIn)
      {
         mAxisLength = mAxisLength - realDist;

         if (mAxisLength < mMaxZoomIn)
            mAxisLength = mMaxZoomIn;
   
         ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
      }
   }
   
   Refresh(false);
}


//------------------------------------------------------------------------------
// void ZoomOut()
//------------------------------------------------------------------------------
/**
 * Zoom out the picture.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::ZoomOut()
{
   // the further object is the faster zoom out
   Real realDist = (mAxisLength + mZoomAmount) / log(mAxisLength);
   mAxisLength = mAxisLength + realDist;

   ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
    
   Refresh(false);
}


//------------------------------------------------------------------------------
// void DrawWireFrame(bool flag)
//------------------------------------------------------------------------------
/**
 * Shows objects in wire frame.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawWireFrame(bool flag)
{
   mDrawWireFrame = flag;
   Refresh(false);
}


//------------------------------------------------------------------------------
// void DrawXyPlane(bool flag)
//------------------------------------------------------------------------------
/**
 * Draws equatorial plane
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawXyPlane(bool flag)
{
   mDrawXyPlane = flag;
   Refresh(false);
}


//------------------------------------------------------------------------------
// void DrawEcPlane(bool flag)
//------------------------------------------------------------------------------
/**
 * Draws ecliptic plane
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawEcPlane(bool flag)
{
   mDrawEcPlane = flag;
   Refresh(false);
}


//------------------------------------------------------------------------------
// void OnDrawAxes(bool flag)
//------------------------------------------------------------------------------
/**
 * Draws axes.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::OnDrawAxes(bool flag)
{
   mDrawAxes = flag;
   Refresh(false);
}


//------------------------------------------------------------------------------
// void OnDrawGrid(bool flag)
//------------------------------------------------------------------------------
/**
 * Draws axes.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::OnDrawGrid(bool flag)
{
   mDrawGrid = flag;
   Refresh(false);
}


//------------------------------------------------------------------------------
// void DrawInOtherCoordSystem(const wxString &csName)
//------------------------------------------------------------------------------
/**
 * Draws objects in other coordinate system.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawInOtherCoordSystem(const wxString &csName)
{
   #if DEBUG_TRAJCANVAS_ACTION
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::DrawInOtherCoordSysName() viewCS=%s, newCS=%s\n",
       mViewCoordSysName.c_str(), csName.c_str());
   #endif
   
   // if current view CS name is different from the new CS name
   if (!mViewCoordSysName.IsSameAs(csName))
   {
      mViewCoordSysName = csName;
      
      mViewCoordSystem =
         theGuiInterpreter->GetCoordinateSystem(std::string(csName.c_str()));
      
      if (mViewCoordSystem->GetName() == mInternalCoordSystem->GetName())
         mIsInternalCoordSystem = true;
      else
         mIsInternalCoordSystem = false;
      
      wxString oldOriginName = mOriginName;
      mOriginName = mViewCoordSystem->GetOriginName().c_str();
      mOriginId = GetObjectId(mOriginName);
      
      mNeedSpacecraftConversion = true;
      mNeedOriginConversion = true;
      mNeedObjectConversion = true;

      UpdateRotateFlags();
      
      if (!mOriginName.IsSameAs(oldOriginName))
         GotoObject(mOriginName);
      
      ConvertObjectData();
      Refresh(false);
   }
   else
   {
      mNeedSpacecraftConversion = false;
      mNeedOriginConversion = false;
      mNeedObjectConversion = false;
   }
}


//---------------------------------------------------------------------------
// void GotoObject(const wxString &objName)
//---------------------------------------------------------------------------
void TrajPlotCanvas::GotoObject(const wxString &objName)
{
   int objId = GetObjectId(objName);
   
   mViewObjId = objId;
   mMaxZoomIn = mObjMaxZoomIn[objId];
   
   // if goto Object is center(0,0,0), zoom out to see the object,
   // otherwise, set to final position of the object
   if (objName == mViewObjName)
   {
      mAxisLength = mMaxZoomIn;
   }
   else
   {
      // compute mAxisLength
      Rvector3 pos(mObjectTempPos[objId][mNumData-1][0],
                   mObjectTempPos[objId][mNumData-1][1],
                   mObjectTempPos[objId][mNumData-1][2]);
      
      mAxisLength = pos.GetMagnitude();
      
      if (mAxisLength == 0.0)
         mAxisLength = mMaxZoomIn;
   }
   
   #ifdef DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::GotoObject() objName=%s, mViewObjId=%d, mMaxZoomIn=%f\n"
       "   mAxisLength=%f\n", objName.c_str(), mViewObjId, mMaxZoomIn, mAxisLength);
   #endif

   //loj: 1/11/06 adde to make GotoObject to work
   mIsEndOfData = true;
   mIsEndOfRun = true;
   //Refresh(false); //loj: 1/11/06 commented out
}


//---------------------------------------------------------------------------
// void GotoOtherBody(const wxString &body)
//---------------------------------------------------------------------------
void TrajPlotCanvas::GotoOtherBody(const wxString &body)
{
   #ifdef DEBUG_TRAJCANVAS_OBJECT
      MessageInterface::ShowMessage("TrajPlotCanvas::GotoOtherBody() body=%s\n",
                                    body.c_str());
   #endif
}


//---------------------------------------------------------------------------
// void ViewAnimation(int interval)
//---------------------------------------------------------------------------
void TrajPlotCanvas::ViewAnimation(int interval)
{
   #ifdef DEBUG_TRAJCANVAS_ANIMATION
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::ViewAnimation() interval=%d\n", interval);
   #endif
   
   this->SetFocus(); // so that it can get key interrupt
   mViewAnimation = true;
   mUpdateInterval = interval;
   mHasUserInterrupted = false;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   DrawFrame();
}


//------------------------------------------------------------------------------
// void SetGlObject(const StringArray &objNames,
//                  const UnsignedIntArray &objOrbitColors,
//                  const std::vector<SpacePoint*> &objArray)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetGlObject(const StringArray &objNames,
                                 const UnsignedIntArray &objOrbitColors,
                                 const std::vector<SpacePoint*> &objArray)
{
   #if DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::SetGlObject() objCount=%d, colorCount=%d.\n",
       objNames.size(), objOrbitColors.size());
   #endif
   
   mObjectArray = objArray;
   wxArrayString tempList;
   
   if (objNames.size() == objOrbitColors.size() &&
       objNames.size() == objArray.size())
   {
      for (UnsignedInt i=0; i<objNames.size(); i++)
      {
         tempList.Add(objNames[i].c_str());
      
         #if DEBUG_TRAJCANVAS_OBJECT > 1
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::SetGlObject()  objNames[%d]=%s, objName=%s, "
             "addr=%d\n", i, objNames[i].c_str(),
             mObjectArray[i]->GetName().c_str(), mObjectArray[i]);
         #endif
      }
      
      AddObjectList(tempList, objOrbitColors);
   }
   else
   {
      MessageInterface::ShowMessage("TrajPlotCanvas::SetGlObject() object sizes "
                                    "are not the same. No ojbects added.\n");
   }
}


//------------------------------------------------------------------------------
// void SetGlCoordSystem(CoordinateSystem *viewCs, CoordinateSystem *viewUpCs)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetGlCoordSystem(CoordinateSystem *viewCs,
                                      CoordinateSystem *viewUpCs)
{
   mInitialCoordSystem = viewCs;
   mInitialCoordSysName = wxString(viewCs->GetName().c_str());
   
   mViewCoordSystem = mInitialCoordSystem;
   mViewCoordSysName = mInitialCoordSysName;
   
   mViewUpCoordSystem = viewUpCs;
   mViewUpCoordSysName = wxString(viewUpCs->GetName().c_str());
   
   // set view center object
   mOriginName = wxString(viewCs->GetOriginName().c_str());   
   mOriginId = GetObjectId(mOriginName);
   
   mViewObjName = mOriginName;
   mViewObjId = mOriginId;
   
   //loj: 12/2/05
   // if view coordinate system origin is spacecraft, make spacecraft radius smaller.
   // So that spapcecraft won't overlap each other.
   //@todo: need better way to scale spacecraft size. (Issue L54)
   if (viewCs->GetOrigin()->IsOfType(Gmat::SPACECRAFT))
      mScRadius = 50;
   
   // set center view object as origin of the CoordinateSystem if view direction
   // is not an object
   if (!mUseViewDirectionVector && mViewDirectionObj != NULL)
   {
      mViewObjName = wxString(mViewDirectionObj->GetName().c_str());
      //mViewObjId = GetObjectId(mViewObjName);
      //loj: 11/2/05 commented out because when solar system is deleted in the
      //Sandbox it crashes when rerunning the default mission
   }
   
   mMaxZoomIn = mObjMaxZoomIn[mOriginId];
   
   if (mUseInitialViewPoint)
   {
      mAxisLength = mMaxZoomIn;
   }
   
   UpdateRotateFlags();
   MakeValidCoordSysList();

   // add initial view coord. system if not added already
   if (mValidCSNames.Index(mInitialCoordSysName) == wxNOT_FOUND)
      mValidCSNames.Add(mInitialCoordSysName);

   #if DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage
      ("   viewCSName=%s, mViewCoordSystem=%d, originName=%s, "
       "mOriginId=%d\n", mViewCoordSysName.c_str(), mViewCoordSystem,
       mOriginName.c_str(),  mOriginId);
   MessageInterface::ShowMessage
      ("   mViewObjName=%s, mViewObjId=%d\n", mViewObjName.c_str(), mViewObjId);
   MessageInterface::ShowMessage
      ("   mViewUpCoordSysName=%s\n", mViewUpCoordSysName.c_str());
   #endif

} // end SetGlCoordSystem()


//------------------------------------------------------------------------------
// void SetGlViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
//                      SpacePoint *vdObj, Real vsFactor,
//                      const Rvector3 &vpRefVec, const Rvector3 &vpVec,
//                      const Rvector3 &vdVec, bool usevpRefVec,
//                      bool usevpVec, bool usevdVec,
//                      bool useFixedFov, Real fov)
//------------------------------------------------------------------------------
/*
 * Sets OpenGL view options
 *
 * @param <vpRefObj> Viewpoint reference object pointer
 * @param <vpVecObj> Viewpoint vector object pointer
 * @param <vdObj>  View direction object pointer
 * @param <vsFactor> Viewpoint scale factor
 * @param <vpRefVec> 3 element vector for viewpoint ref. vector (use if usevpVec is true)
 * @param <vpVec> 3 element vector for viewpoint vector (use if usevpVec is true)
 * @param <vdVec> 3 element vector for view direction (use if usevdVec is true)
 * @param <usevpRefVec> true if use vector for viewpoint reference vector
 * @param <usevpVec> true if use vector for viewpoint vector
 * @param <usevdVec> true if use vector for view direction
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetGlViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                                     SpacePoint *vdObj, Real vsFactor,
                                     const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                                     const Rvector3 &vdVec, const std::string &upAxis,
                                     bool usevpRefVec, bool usevpVec, bool usevdVec,
                                     bool useFixedFov, Real fov)
{
   mViewPointRefObj = vpRefObj;
   mViewPointVectorObj = vpVecObj;
   mViewDirectionObj = vdObj;
   mViewScaleFactor = vsFactor;
   mViewPointRefVector = vpRefVec;
   mViewPointVector = vpVec;
   mViewDirectionVector = vdVec;
   mViewUpAxisName = upAxis;
   mUseViewPointRefVector = usevpRefVec;
   mUseViewPointVector = usevpVec;
   mUseViewDirectionVector = usevdVec;
   mUseFixedFov = useFixedFov;
   mFixedFovAngle = fov;
   
   Rvector3 lvpRefVec(vpRefVec);
   Rvector3 lvpVec(vpVec);
   Rvector3 lvdVec(vdVec);
   
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::SetGlViewOption() mViewPointRefObj=%d, "
       "mViewPointVectorObj=%d\n   mViewDirectionObj=%d, mViewScaleFactor=%f   "
       "mViewPointRefVector=%s\n   mViewPointVector=%s, mViewDirectionVector=%s, "
       "mViewUpAxisName=%s\n   mUseViewPointRefVector=%d, mUseViewDirectionVector=%d, "
       "mUseFixedFov=%d, mFixedFovAngle=%f\n",  mViewPointRefObj, mViewPointVectorObj,
       mViewDirectionObj, mViewScaleFactor, lvpRefVec.ToString().c_str(),
       lvpVec.ToString().c_str(), lvdVec.ToString().c_str(), mViewUpAxisName.c_str(),
       mUseViewPointRefVector, mUseViewDirectionVector, mUseFixedFov, mFixedFovAngle);
   #endif
   
   // Set viewpoint ref. object id
   if (!mUseViewPointRefVector && mViewPointRefObj)
   {
      mViewPointRefObjName = mViewPointRefObj->GetName();
      
      mVpRefObjId = GetObjectId(mViewPointRefObj->GetName().c_str());
      
      if (mVpRefObjId == GmatPlot::UNKNOWN_BODY)
      {
         mUseViewPointRefVector = true;
         MessageInterface::ShowMessage
            ("*** Warning *** TrajPlotCanvas::SetGlViewOption() Cannot find "
             "mViewPointRefObj name=%s, so using vector=%s\n",
             mViewPointRefObj->GetName().c_str(),
             mViewPointRefVector.ToString().c_str());
      }
   }
   else
   {
      mViewPointRefObjName = "Earth";
      
      if (!mUseViewPointRefVector)
         MessageInterface::ShowMessage
            ("*** Warning *** TrajPlotCanvas::SetGlViewOption() "
             "ViewPointRefObject is NULL,"
             "so will use default Vector instead.\n");
   }
   
   // Set viewpoint vector object id
   if (!mUseViewPointVector && mViewPointVectorObj)
   {
      mVpVecObjId = GetObjectId(mViewPointVectorObj->GetName().c_str());
      
      if (mVpVecObjId == GmatPlot::UNKNOWN_BODY)
      {
         mUseViewPointVector = true;
         MessageInterface::ShowMessage
            ("*** Warning *** TrajPlotCanvas::SetGlViewOption() Cannot find "
             "mViewPointVectorObj name=%s, so using vector=%s\n",
             mViewPointVectorObj->GetName().c_str(),
             mViewPointVector.ToString().c_str());
      }
   }
   else
   {
      if (!mUseViewPointVector)
         MessageInterface::ShowMessage
            ("*** Warning *** TrajPlotCanvas::SetGlViewOption() "
             "ViewPointVectorObject is NULL, "
             "so will use default Vector instead.\n");
   }
   
   // Set view direction object id
   if (!mUseViewDirectionVector && mViewDirectionObj)
   {
      mVdirObjId = GetObjectId(mViewDirectionObj->GetName().c_str());
      
      if (mVdirObjId == GmatPlot::UNKNOWN_BODY)
      {
         mUseViewDirectionVector = true;
         MessageInterface::ShowMessage
            ("*** Warning *** TrajPlotCanvas::SetGlViewOption() Cannot find "
             "mViewDirectionObj name=%s, so using vector=%s\n",
             mViewDirectionObj->GetName().c_str(),
             mViewDirectionVector.ToString().c_str());
      }
   }
   else
   {
      if (!mUseViewDirectionVector)
         MessageInterface::ShowMessage
            ("*** Warning *** TrajPlotCanvas::SetGlViewOption() "
             "ViewDirectionObject is NULL,"
             "so will use default Vector instead.\n");
   }
   
   // Set view up direction
   if (mViewUpAxisName == "X")
      mUpState.Set(-1.0, 0.0, 0.0, 0.0, 0.0, 0.0);
   else if (mViewUpAxisName == "-X")
      mUpState.Set(1.0, 0.0, 0.0, 0.0, 0.0, 0.0);
   else if (mViewUpAxisName == "Y")
      mUpState.Set(0.0, -1.0, 0.0, 0.0, 0.0, 0.0);
   else if (mViewUpAxisName == "-Y")
      mUpState.Set(0.0, 1.0, 0.0, 0.0, 0.0, 0.0);
   else if (mViewUpAxisName == "Z")
      mUpState.Set(0.0, 0.0, 1.0, 0.0, 0.0, 0.0);
   else if (mViewUpAxisName == "-Z")
      mUpState.Set(0.0, 0.0, -1.0, 0.0, 0.0, 0.0);
   
} //end SetGlViewOption()


//------------------------------------------------------------------------------
// void SetGlDrawOrbitFlag(const std::vector<bool> &drawArray)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetGlDrawOrbitFlag(const std::vector<bool> &drawArray)
{
   mDrawOrbitArray = drawArray;
   
   #if DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::SetGlDrawObjectFlag() mDrawOrbitArray.size()=%d, "
       "mObjectCount=%d\n", mDrawOrbitArray.size(), mObjectCount);
   #endif
   
   bool draw;
   for (int i=0; i<mObjectCount; i++)
   {
      draw = mDrawOrbitArray[i] ? true : false;
      
      #if DEBUG_TRAJCANVAS_OBJECT
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::SetGlDrawObjectFlag() mDrawOrbitArray[%d]=%d\n",
          i, draw);
      #endif
   }
}


//------------------------------------------------------------------------------
// void SetGlShowObjectFlag(const std::vector<bool> &showArray)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetGlShowObjectFlag(const std::vector<bool> &showArray)
{
   mShowObjectArray = showArray;

   #if DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::SetGlDrawObjectFlag() mDrawOrbitArray.size()=%d, "
       "mObjectCount=%d\n", mShowObjectArray.size(), mObjectCount);
   #endif
   
   bool show;
   mSunPresent = false;
   
   for (int i=0; i<mObjectCount; i++)
   {
      show = mShowObjectArray[i] ? true : false;
      mShowObjectMap[mObjectNames[i]] = show;
      
      if (mObjectNames[i] == "Sun" && mShowObjectMap["Sun"])
         mSunPresent = true;
      
      #if DEBUG_TRAJCANVAS_OBJECT
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::SetGlShowObjectFlag() mShowObjectMap[%s]=%d\n",
          mObjectNames[i].c_str(), show);
      #endif
   }
   
   #if DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::SetGlDrawObjectFlag() mEnableLightSource=%d, mSunPresent=%d\n",
       mEnableLightSource, mSunPresent);
   #endif
   
   //loj: 11/4/05 Added light source
   #ifdef ENABLE_LIGHT_SOURCE
   if (mEnableLightSource && mSunPresent)
   {
      //----------------------------------------------------------------------
      // set OpenGL to recognize the counter clockwise defined side of a polygon
      // as its 'front' for lighting and culling purposes
      glFrontFace(GL_CCW);
      
      // enable face culling, so that polygons facing away (defines by front face)
      // from the viewer aren't drawn (for efficieny).
      glEnable(GL_CULL_FACE);

      // create a light:
      float lightColor[4]={1.0f, 1.0f, 1.0f, 1.0f};
      
      glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, lightColor);
      glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
      
      // enable the light
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      
      // tell OpenGL to use glColor() to get material properties for..
      glEnable(GL_COLOR_MATERIAL);
      
      // ..the front face's ambient and diffuse components
      glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
      //----------------------------------------------------------------------
   }
   #endif
   
}


//------------------------------------------------------------------------------
// void SetNumPointsToRedraw(Integer numPoints)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetNumPointsToRedraw(Integer numPoints)
{
   mNumPointsToRedraw = numPoints;
   mRedrawLastPointsOnly = false;

   // if mNumPointsToRedraw =  0 It redraws whole plot
   // if mNumPointsToRedraw = -1 It does not clear GL_COLOR_BUFFER
   if (mNumPointsToRedraw > 0)
      mRedrawLastPointsOnly = true;
}


//------------------------------------------------------------------------------
// void SetUpdateFrequency(Integer updFreq)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetUpdateFrequency(Integer updFreq)
{
   mUpdateFrequency = updFreq;
}


//------------------------------------------------------------------------------
// void UpdatePlot(const StringArray &scNames, const Real &time,
//                 const RealArray &posX, const RealArray &posY,
//                 const RealArray &posZ, const RealArray &velX,
//                 const RealArray &velY, const RealArray &velZ,
//                 const UnsignedIntArray &scColors)
//------------------------------------------------------------------------------
/**
 * Updates spacecraft trajectory.
 *
 * @param <scNames> spacecraft name array
 * @param <time> time
 * @param <posX> position x array
 * @param <posY> position y array
 * @param <posZ> position z array
 * @param <velX> velocity x array
 * @param <velY> velocity y array
 * @param <velZ> velocity z array
 * @param <scColors> orbit color array
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::UpdatePlot(const StringArray &scNames, const Real &time,
                                const RealArray &posX, const RealArray &posY,
                                const RealArray &posZ, const RealArray &velX,
                                const RealArray &velY, const RealArray &velZ,
                                const UnsignedIntArray &scColors)
{
   mScCount = scNames.size();
   if (mScCount > MAX_SCS)
      mScCount = MAX_SCS;
   
   #if DEBUG_TRAJCANVAS_UPDATE
   static int sNumDebugOutput = 100;
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::UpdatePlot() plot=%s, time=%f, mNumData=%d, mScCount=%d\n",
       GetName().c_str(), time, mNumData, mScCount);
   #endif
   
   mScNameArray = scNames;
   
   if (mNumData < MAX_DATA)
   {      
      mTime[mNumData] = time;
      Real mha, longitude, lst;
      ComputeLongitudeLst(mTime[mNumData], posX[0], posY[0], &mha, &longitude, &lst);
      
      #if DEBUG_TRAJCANVAS_LONGITUDE
      MessageInterface::ShowMessage
         ("===> time=%f, mNumData=%d, mha=%f, longitude=%f, lst = %f\n",
          mTime[mNumData], mNumData, mha, longitude, lst);
      #endif
      
      if (mNumData == 0)
      {
         mInitialLongitude = longitude;
         mInitialMha = mha;
         #if DEBUG_TRAJCANVAS_LONGITUDE
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::UpdatePlot() mInitialLongitude = %f, mInitialMha = %f\n",
             mInitialLongitude, mInitialMha);
         #endif
      }
      
      //-------------------------------------------------------
      // update spacecraft position
      //-------------------------------------------------------
      for (int sc=0; sc<mScCount; sc++)
      {
         int objId = GetObjectId(mScNameArray[sc].c_str());
         
         if (objId != UNKNOWN_OBJ_ID)
         {
            if (!mDrawOrbitArray[objId])
            {
               mDrawOrbitFlag[objId][mNumData] = false;
               continue;
            }
            
            mDrawOrbitFlag[objId][mNumData] = true;
            
            mObjectOrbitColor[objId][mNumData]  = scColors[sc];
            mObjectGciPos[objId][mNumData][0] = posX[sc];
            mObjectGciPos[objId][mNumData][1] = posY[sc];
            mObjectGciPos[objId][mNumData][2] = posZ[sc];
            
            mObjectGciVel[objId][mNumData][0] = velX[sc];
            mObjectGciVel[objId][mNumData][1] = velY[sc];
            mObjectGciVel[objId][mNumData][2] = velZ[sc];
            
            #if DEBUG_TRAJCANVAS_UPDATE
            if (mNumData < sNumDebugOutput)
            {
               MessageInterface::ShowMessage
                  ("   object=%s, objId=%d, color=%d\n",
                   mObjectNames[sc].c_str(), objId, mObjectOrbitColor[objId][mNumData]);
               MessageInterface::ShowMessage
                  ("   objId:%d gcipos = %f, %f, %f, color=%d\n", objId,
                   mObjectGciPos[objId][mNumData][0],
                   mObjectGciPos[objId][mNumData][1],
                   mObjectGciPos[objId][mNumData][2]);
            }
            #endif
            
            if (mNeedInitialConversion)
            {
               Rvector6 inState, outState;
               
               // convert position and velocity
               inState.Set(posX[sc], posY[sc], posZ[sc],
                           velX[sc], velY[sc], velZ[sc]);
               
               mCoordConverter.Convert(time, inState, mInternalCoordSystem,
                                       outState, mViewCoordSystem);
               
               mObjectTempPos[objId][mNumData][0] = outState[0];
               mObjectTempPos[objId][mNumData][1] = outState[1];
               mObjectTempPos[objId][mNumData][2] = outState[2];
               
               mObjectTempVel[objId][mNumData][0] = outState[3];
               mObjectTempVel[objId][mNumData][1] = outState[4];
               mObjectTempVel[objId][mNumData][2] = outState[5];

               // copy to initial view coordinate system array
               CopyVector3(mObjectInitialPos[objId][mNumData],
                           mObjectTempPos[objId][mNumData]);
               CopyVector3(mObjectInitialVel[objId][mNumData],
                           mObjectTempVel[objId][mNumData]);               
            }
            else
            {
               CopyVector3(mObjectTempPos[objId][mNumData],
                           mObjectGciPos[objId][mNumData]);
               CopyVector3(mObjectTempVel[objId][mNumData],
                           mObjectGciVel[objId][mNumData]);            
            }
            
            #if DEBUG_TRAJCANVAS_UPDATE
            if (mNumData < sNumDebugOutput)
            {
               MessageInterface::ShowMessage
                  ("   objId:%d tmppos = %f, %f, %f\n", objId,
                   mObjectTempPos[objId][mNumData][0],
                   mObjectTempPos[objId][mNumData][1],
                   mObjectTempPos[objId][mNumData][2]);
            }
            #endif
            
            #if DEBUG_TRAJCANVAS_UPDATE > 1
            if (mNumData < sNumDebugOutput)
            {
               MessageInterface::ShowMessage
                  ("   objId:%d tmpvel = %f, %f, %f\n", objId,
                   mObjectTempVel[objId][mNumData][0],
                   mObjectTempVel[objId][mNumData][1],
                   mObjectTempVel[objId][mNumData][2]);
            }
            #endif
         }
      }
      
      //----------------------------------------------------
      // update object position
      //----------------------------------------------------
      for (int obj=0; obj<mObjectCount; obj++)
      {
         // if object pointer is not NULL
         if (mObjectArray[obj])
         {
            int objId = GetObjectId(mObjectNames[obj]);
            
            #if DEBUG_TRAJCANVAS_UPDATE_OBJECT
            MessageInterface::ShowMessage
               ("TrajPlotCanvas::UpdatePlot() object=%s, objId=%d\n",
                mObjectNames[obj].c_str(), objId);
            #endif
            
            // if object id found
            if (objId != UNKNOWN_OBJ_ID)
            {
               if (!mDrawOrbitArray[objId])
               {
                  mDrawOrbitFlag[objId][mNumData] = false;
                  continue;
               }
               
               mDrawOrbitFlag[objId][mNumData] = true;
               
               Rvector6 objState = mObjectArray[obj]->GetMJ2000State(time);
               mObjectGciPos[objId][mNumData][0] = objState[0];
               mObjectGciPos[objId][mNumData][1] = objState[1];
               mObjectGciPos[objId][mNumData][2] = objState[2];
               
               mObjectGciVel[objId][mNumData][0] = objState[3];
               mObjectGciVel[objId][mNumData][1] = objState[4];
               mObjectGciVel[objId][mNumData][2] = objState[5];
               
               #if DEBUG_TRAJCANVAS_UPDATE_OBJECT > 1
               MessageInterface::ShowMessage
                  ("TrajPlotCanvas::UpdatePlot() objState=%s\n",
                   objState.ToString().c_str());
               MessageInterface::ShowMessage
                  ("    %s gcipos = %f, %f, %f\n", mObjectNames[obj].c_str(),
                   mObjectGciPos[objId][mNumData][0],
                   mObjectGciPos[objId][mNumData][1],
                   mObjectGciPos[objId][mNumData][2]);
               #endif
               
               // convert objects to view CoordinateSystem
               if (mNeedInitialConversion)
               {
                  Rvector6 outState;
                  
                  mCoordConverter.Convert(time, objState, mInternalCoordSystem,
                                          outState, mViewCoordSystem);
                  
                  mObjectTempPos[objId][mNumData][0] = outState[0];
                  mObjectTempPos[objId][mNumData][1] = outState[1];
                  mObjectTempPos[objId][mNumData][2] = outState[2];
                  
                  mObjectTempVel[objId][mNumData][0] = outState[3];
                  mObjectTempVel[objId][mNumData][1] = outState[4];
                  mObjectTempVel[objId][mNumData][2] = outState[5];
                  
                  // copy to initial view coordinate system array
                  CopyVector3(mObjectInitialPos[objId][mNumData],
                              mObjectTempPos[objId][mNumData]);
                  CopyVector3(mObjectInitialVel[objId][mNumData],
                              mObjectTempVel[objId][mNumData]);          
               }
               else
               {
                  CopyVector3(mObjectTempPos[objId][mNumData],
                              mObjectGciPos[objId][mNumData]);
                  CopyVector3(mObjectTempVel[objId][mNumData],
                              mObjectGciVel[objId][mNumData]);
               }
               
               #if DEBUG_TRAJCANVAS_UPDATE_OBJECT > 1
               MessageInterface::ShowMessage
                  ("    %s tmppos = %f, %f, %f\n", mObjectNames[obj].c_str(),
                   mObjectTempPos[objId][mNumData][0],
                   mObjectTempPos[objId][mNumData][1],
                   mObjectTempPos[objId][mNumData][2]);
               #endif
               
            }
            else
            {
               #if DEBUG_TRAJCANVAS_UPDATE_OBJECT > 1
               MessageInterface::ShowMessage
                  ("TrajPlotCanvas::UpdatePlot() Cannot Add data. Invalid objId=%d\n",
                   objId);
               #endif
            }
         }
         else
         {
            #if DEBUG_TRAJCANVAS_UPDATE_OBJECT > 1
            MessageInterface::ShowMessage
               ("TrajPlotCanvas::UpdatePlot() Cannot add data. %s is NULL\n",
                mObjectNames[obj].c_str());
            #endif
         }
      }
      
      mNumData++;
      
      //} // if (mNumData < MAX_DATA)
      
   } // if (mNumData < MAX_DATA)
   else
   {
      mOverCounter++;
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      DrawStatus("Over Counter#: ", mOverCounter, time);
      glFlush();
      SwapBuffers();
      
      if (mShowMaxWarning)
      {
         mShowMaxWarning = false;
         wxString msg;
         msg.Printf("The number of data points exceeded the maximum of %d.\n"
                    "Please adjust data collect frequency to see the whole plot.\n",
                    MAX_DATA);
         MessageInterface::PopupMessage(Gmat::INFO_, msg.c_str());
         MessageInterface::ShowMessage(msg.c_str());
      }
   }
}


//---------------------------------------------------------------------------
// void AddObjectList(wxArrayString &objNames, UnsignedIntArray &objColors,
//                    bool clearList=true)
//---------------------------------------------------------------------------
void TrajPlotCanvas::AddObjectList(const wxArrayString &objNames,
                                   const UnsignedIntArray &objColors,
                                   bool clearList)
{
   #if DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::AddObjectList() object count=%d, color count=%d\n",
       objNames.GetCount(), objColors.size());
   #endif
   
   // clear bodies
   if (clearList)
   {
      mObjectNames.Empty();
   }
   
   RgbColor rgb;

   mObjectCount = objNames.GetCount();
   
   for (int i=0; i<mObjectCount; i++)
   {
      // add object names
      mObjectNames.Add(objNames[i]);
      
      if (mObjectTextureIdMap.find(objNames[i]) == mObjectTextureIdMap.end())
      {
         #if DEBUG_TRAJCANVAS_OBJECT
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::AddObjectList()  Bind new texture object=%s\n",
             objNames[i].c_str());
         #endif
         
         mObjectTextureIdMap[objNames[i]] = GmatPlot::UNINIT_TEXTURE;
      }
      
      // initialize show object
      mShowObjectMap[objNames[i]] = true;
      mShowOrbitNormalMap[objNames[i]] = false;
      
      // initialize object color
      rgb.Set(objColors[i]);
      mObjectColorMap[objNames[i]] = rgb;
      
      // set real object radius, if it is CelestialBody
      if (mObjectArray[i]->IsOfType(Gmat::CELESTIAL_BODY))
      {
         mObjectRadius[i] = ((CelestialBody*)(mObjectArray[i]))->GetEquatorialRadius();
         mObjMaxZoomIn[i] = mObjectRadius[i] * RADIUS_ZOOM_RATIO;
      }
      else
      {
         mObjectRadius[i] = mObjectDefaultRadius;
         mObjMaxZoomIn[i] = mObjectDefaultRadius * RADIUS_ZOOM_RATIO;
      }
      
      #if DEBUG_TRAJCANVAS_OBJECT > 1
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::AddObjectList() objNames[%d]=%s\n",
          i, objNames[i].c_str());
      #endif
   }

   InitGL();
   ClearPlot();

} //AddObjectList()


//------------------------------------------------------------------------------
// int ReadTextTrajectory(const wxString &filename)
//------------------------------------------------------------------------------
/**
 * Reads text trajectory file and initializes OpenGL.
 *
 * @param <filename> file name
 * @return number of data points.
 *
 * @note Assumes the trajectory file has time, x, y, z, vx, vy, vz.
 */
//------------------------------------------------------------------------------
int TrajPlotCanvas::ReadTextTrajectory(const wxString &filename)
{
   int numDataPoints = 0;
   mTextTrajFile =  new TextTrajectoryFile(std::string(filename.c_str()));
   
   if (mTextTrajFile->Open())
   {
      mTrajectoryData = mTextTrajFile->GetData();
        
      numDataPoints = mTrajectoryData.size();

      mObjectArray.push_back(NULL);
      wxArrayString tempList;
      tempList.Add("SC1");
      UnsignedIntArray objOrbitColors;
      objOrbitColors.push_back(GmatColor::RED32);
      AddObjectList(tempList, objOrbitColors);
      
      int sc = 0;
      for(int i=0; i<numDataPoints && i < MAX_DATA; i++)
      {
         mTime[mNumData] = mTrajectoryData[i].time;
         mObjectOrbitColor[sc][mNumData] = GmatColor::RED32;
         mObjectTempPos[sc][mNumData][0] = mTrajectoryData[i].x;
         mObjectTempPos[sc][mNumData][1] = mTrajectoryData[i].y;
         mObjectTempPos[sc][mNumData][2] = mTrajectoryData[i].z;
         mNumData++;
      }
      
      mTextTrajFile->Close();
      wxString text;
      text.Printf("Number of data points: %d", numDataPoints);
      theStatusBar->SetStatusText(text, 2);
      //wxLogStatus(GmatAppData::GetMainFrame(),
      //            wxT("Number of data points: %d"), numDataPoints);
   }
   else
   {
      wxString info;
      info.Printf(_T("Cannot open trajectory file name: %s\n"),
                  filename.c_str());
        
      wxMessageDialog msgDialog(this, info, _T("ReadTextTrajectory File"));
      msgDialog.ShowModal();
      return numDataPoints;
   }
   
   // initialize GL
   if (!InitGL())
   {
      wxMessageDialog msgDialog(this, _T("InitGL() failed"),
                                _T("ReadTextTrajectory File"));
      msgDialog.ShowModal();
      return false;
   }
    
   return numDataPoints;
    
} //end ReadTextTrajectory()


//------------------------------------------------------------------------------
// void OnPaint(wxPaintEvent& event)
//------------------------------------------------------------------------------
/**
 * Processes wxPaintEvent.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::OnPaint(wxPaintEvent& event)
{
   //MessageInterface::ShowMessage("===> TrajPlotCanvas::OnPaint() called\n");
   
   // must always be here
   wxPaintDC dc(this);
   
#ifndef __WXMOTIF__
   if (!GetContext()) return;
#endif
   
   SetCurrent();    
   
   if (mDrawWireFrame)
   {
      glPolygonMode(GL_FRONT, GL_LINE); // for wire frame
      glPolygonMode(GL_BACK, GL_LINE);  // for wire frame
   }
   else
   {
      glPolygonMode(GL_FRONT, GL_FILL);
      glPolygonMode(GL_BACK, GL_FILL);
   }
   
   DrawPlot();
}


//------------------------------------------------------------------------------
// void OnTrajSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
/**
 * Processes wxSizeEvent.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::OnTrajSize(wxSizeEvent& event)
{
   // this is also necessary to update the context on some platforms
   wxGLCanvas::OnSize(event);
    
   // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
   int nWidth, nHeight;
   GetClientSize(&nWidth, &nHeight);
   mCanvasSize.x = nWidth;
   mCanvasSize.y = nHeight;
#ifndef __WXMOTIF__
   if (GetContext())
#endif
   {
      //loj: need this to make picture not to stretch to canvas
      ChangeProjection(nWidth, nHeight, mAxisLength);
      SetCurrent();
      glViewport(0, 0, (GLint) nWidth, (GLint) nHeight);
   }
}


//------------------------------------------------------------------------------
// void OnMouse(wxMouseEvent& event)
//------------------------------------------------------------------------------
/**
 * Processes wxMouseEvent.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::OnMouse(wxMouseEvent& event)
{
   
   // Allow user to change the view during the run
   // if not using initial viewpoint or
   // when run ended if using initial viewpoint

   //MessageInterface::ShowMessage
   //   ("===> OnMouse() mUseInitialViewPoint=%d, mIsEndOfData=%d\n",
   //    mUseInitialViewPoint, mIsEndOfData);

   //loj: 9/29/05 commented out to allow mouse movement even in locked-view mode
   //if (mUseInitialViewPoint && !mIsEndOfData)
   //   return;
   
   int flippedY;
   int width, height;
   int mouseX, mouseY;
   
   mViewAnimation = false;
   
   GetClientSize(&width, &height);
   ChangeProjection(width, height, mAxisLength);
   
   mouseX = event.GetX();
   mouseY = event.GetY();
   
   // First, flip the mouseY value so it is oriented right (bottom left is 0,0)
   flippedY = height - mouseY;
   
   GLfloat fEndX = mfLeftPos + ((GLfloat)mouseX /(GLfloat)width) *
      (mfRightPos - mfLeftPos);
   GLfloat fEndY = mfBottomPos + ((GLfloat)flippedY /(GLfloat)height)*
      (mfTopPos - mfBottomPos);
   
   if (mUseSingleRotAngle)
   {
      if (mIsEndOfRun)
         mUseSingleRotAngle = false;
      
      //loj: 9/29/05
      // This code is trying to compute Euler angle of last plot transformation
      // so that the plot can stay in the same orientation when user clicks it.
      // But it is not working yet.
      #ifdef COMPUTE_EULER_ANGLE
      if (mfCamSingleRotAngle != 0.0 || mfUpAngle != 0.0)
      {
         //compute Euler angles of viewpoint and up axis
         Rvector3 rotAngle = ComputeEulerAngles();
         
         mCurrRotXAngle = rotAngle[0];
         mCurrRotYAngle = rotAngle[1];
         mCurrRotZAngle = rotAngle[2];

         #if DEBUG_TRAJCANVAS_EULER
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::OnMouse() mCurrRotXAngle=%f, %f, %f\n",
             mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::OnMouse() mfCamRotXYZAngle=%f, %f, %f\n",
             mfCamRotXAngle, mfCamRotYAngle, mfCamRotZAngle);
         #endif

         //loj: 12/7/05 added
         if (mUsePerspectiveMode)
         {
            // always look at from Z for rotation and zooming
            mfCamTransX = 0.0;
            mfCamTransY = 0.0;
            mfCamTransZ = -mVpLocVec.GetMagnitude();
         }
      }
      #endif
   }
   
   //-------------------------------------------------------------------
   // see comment in SetEndOfRun()
   //-------------------------------------------------------------------
   ////if left mouse clicked
   //if (event.LeftDown())
   //{
   //   MessageInterface::ShowMessage("event.LeftDown())\n");
   //   if (mIsEndOfRun)
   //      ChangeView(mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);
   //   
   //   ComputeView(fEndX, fEndY);
   //   ChangeView(mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);
   //}
   //-------------------------------------------------------------------
   
   //if mouse dragging
   if (event.Dragging())
   {
      //------------------------------
      // translating
      //------------------------------
      if (event.ShiftDown() && event.LeftIsDown())
      {
         // Do a X/Y Translate of the camera
         mfCamTransX += (fEndX - mfStartX);
         mfCamTransY += (fEndY - mfStartY);
         
         // repaint
         Refresh(false);
      }
      //------------------------------
      // rotating
      //------------------------------
      else if (event.LeftIsDown())
      {
         #ifdef USE_TRACKBALL
         
            // drag in progress, simulate trackball
            float spin_quat[4];
            //float speed = 2.0;
            float speed = 4.0;
            ToQuat(spin_quat,
                      (speed*mLastMouseX - width) / width,
                      (     height - speed*mLastMouseY) / height,
                      (     speed*mouseX - width) / width,
                      (     height - speed*mouseY) / height);
         
            AddQuats(spin_quat, mQuat, mQuat);
            //MessageInterface::ShowMessage
            //   ("===> mQuat=%f, %f, %f, %f\n", mQuat[0], mQuat[1], mQuat[2], mQuat[2]);
         
         #else
         
            //MessageInterface::ShowMessage
            //   ("===> event.LeftIsDown() mCurrRotXYZAngle=%f, %f, %f\n",
            //    mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);

            // if end-of-run compute new mfCamRotXYZAngle by calling ChangeView()
            if (mIsEndOfRun)
               ChangeView(mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);

            ComputeView(fEndX, fEndY);
            ChangeView(mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);

            //MessageInterface::ShowMessage
            //   ("===> after ChangeView() mfCamRotXYZAngle=%f, %f, %f\n",
            //    mfCamRotXAngle, mfCamRotYAngle, mfCamRotZAngle);
         
         #endif
         
         // repaint
         Refresh(false);
      }
      //------------------------------
      // zooming
      //------------------------------
      else if (event.RightIsDown())
      {            
         // if end-of-run compute new mfCamRotXYZAngle by calling ChangeView()
         if (mIsEndOfRun)
            ChangeView(mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);
         
         // find the length
         Real x2 = pow(mouseX - mLastMouseX, 2);
         Real y2 = pow(mouseY - mLastMouseY, 2);
         Real length = sqrt(x2 + y2);
         mZoomAmount = length * 100;

         if (mouseX < mLastMouseX && mouseY > mLastMouseY)
         {
            // dragging from upper right corner to lower left corner
            ZoomIn();
         }
         else if (mouseX > mLastMouseX && mouseY < mLastMouseY)
         {
            // dragging from lower left corner to upper right corner
            ZoomOut();
         }
         else
         {
            // if mouse moves toward left then zoom in
            if (mouseX < mLastMouseX || mouseY < mLastMouseY)
               ZoomIn();
            else
               ZoomOut();
         }         
      }
   } // end if (event.Dragging())
   
   // save last position
   mLastMouseX = mouseX;
   mLastMouseY = mouseY;
   
   mfStartX = fEndX;
   mfStartY = fEndY;
   
   wxString mousePosStr;
   mousePosStr.Printf("X = %g Y = %g", fEndX, fEndY);
   //wxStatusBar *statusBar = GmatAppData::GetMainFrame()->GetStatusBar();
   theStatusBar->SetStatusText(mousePosStr, 2);
   
   //wxLogStatus(MdiGlPlot::mdiParentGlFrame,
   //            wxT("X = %d Y = %d lastX = %f lastY = %f Zoom amount = %f Distance = %f"),
   //            event.GetX(), event.GetY(), mfStartX, mfStartY, mZoomAmount, mAxisLength);
   event.Skip();
}


//------------------------------------------------------------------------------
// void OnKeyDown(wxKeyEvent &event)
//------------------------------------------------------------------------------
/**
 * Processes wxKeyEvent.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::OnKeyDown(wxKeyEvent &event)
{
   int keyDown = event.GetKeyCode();
   if (keyDown == WXK_ESCAPE)
      mHasUserInterrupted = true;
}


//------------------------------------------------------------------------------
// bool SetPixelFormatDescriptor()
//------------------------------------------------------------------------------
/**
 * Sets pixel format on Windows.
 */
//------------------------------------------------------------------------------
bool TrajPlotCanvas::SetPixelFormatDescriptor()
{
#ifdef __WXMSW__
   // On Windows, for OpenGL, you have to set the pixel format
   // once before doing your drawing stuff. This function
   // properly sets it up.
   
   HDC hdc = wglGetCurrentDC();
   
   PIXELFORMATDESCRIPTOR pfd =
   {
      sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd
      1,                     // version number
      PFD_DRAW_TO_WINDOW |   // support window
      PFD_SUPPORT_OPENGL |   // support OpenGL
      PFD_DOUBLEBUFFER,      // double buffered
      PFD_TYPE_RGBA,         // RGBA type
      24,                    // 24-bit color depth
      0, 0, 0, 0, 0, 0,      // color bits ignored
      0,                     // no alpha buffer
      0,                     // shift bit ignored
      0,                     // no accumulation buffer
      0, 0, 0, 0,            // accum bits ignored
      32,                    // 32-bit z-buffer
      0,                     // no stencil buffer
      0,                     // no auxiliary buffer
      PFD_MAIN_PLANE,        // main layer
      0,                     // reserved
      0, 0, 0                // layer masks ignored
   };
   
   // get the device context's best-available-match pixel format
   int pixelFormatId = ChoosePixelFormat(hdc, &pfd);
   if(pixelFormatId == 0)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "TrajPlotCanvas::SetPixelFormatDescriptor():"
          "ChoosePixelFormat failed. pixelFormatId = %d \n");
      return false;
   }

   //loj: 3/10/05 Why this failing?
//     // make that the device context's current pixel format
//     if (!SetPixelFormat(hdc, pixelFormatId, &pfd))
//     {
//        MessageInterface::PopupMessage
//           (Gmat::ERROR_, "TrajPlotCanvas::SetPixelFormatDescriptor():"
//            "SetPixelFormat failed\n");
//        return false;
//     }
   
   return true;
#endif
   return false;
}


//------------------------------------------------------------------------------
//  void SetDefaultGLFont()
//------------------------------------------------------------------------------
/**
 * Sets default GL font.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetDefaultGLFont()
{
#ifdef __WXMSW__
   // Set up font stuff for windows -
   // Make the Current font the device context's selected font
   //SelectObject(dc, Font->Handle);
   HDC hdc = wglGetCurrentDC();
   
   wglUseFontBitmaps(hdc, 0, 255, 1000);
   glListBase(1000); // base for displaying
#endif
}


//------------------------------------------------------------------------------
//  bool LoadGLTextures()
//------------------------------------------------------------------------------
/**
 * Loads textures.
 */
//------------------------------------------------------------------------------
bool TrajPlotCanvas::LoadGLTextures()
{
   
// #ifdef __WXMSW__

   #if DEBUG_TRAJCANVAS_TEXTURE
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::LoadGLTextures() mObjectCount=%d\n", mObjectCount);
   #endif
   
   //--------------------------------------------------
   // load object texture if used
   //--------------------------------------------------
   for (int i=0; i<mObjectCount; i++)
   {
      if (mObjectArray[i]->IsOfType(Gmat::SPACECRAFT))
         continue;
      
      if (mObjectTextureIdMap[mObjectNames[i]] == GmatPlot::UNINIT_TEXTURE)
      {
         #if DEBUG_TRAJCANVAS_TEXTURE > 1
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::LoadGLTextures() object=%s\n", mObjectNames[i].c_str());
         #endif
         
         mObjectTextureIdMap[mObjectNames[i]] = BindTexture(mObjectNames[i]);
      }
   }
   
   return true;
// #else
//    return false;
// #endif
} //end LoadGLTextures()


//------------------------------------------------------------------------------
// GLuint BindTexture(const wxString &objName)
//------------------------------------------------------------------------------
/**
 * Loads textures and returns binding index.
 */
//------------------------------------------------------------------------------
GLuint TrajPlotCanvas::BindTexture(const wxString &objName)
{
   GLuint ret = GmatPlot::UNINIT_TEXTURE;

   //MessageInterface::ShowMessage("===> TrajPlotCanvas::BindTexture() ret = %d\n", ret);
   
   FileManager *fm = FileManager::Instance();
   std::string textureFile;  
   std::string name = std::string(objName.Upper().c_str());
   std::string filename = name + "_TEXTURE_FILE";
   
   try
   {
      textureFile = fm->GetFullPathname(filename);
   
      #ifndef SKIP_DEVIL
         ILboolean status = ilLoadImage((char*)textureFile.c_str());
         if (!status)
         {
            MessageInterface::ShowMessage
               ("*** Warning *** TrajPlotCanvas::BindTexture() Unable to load "
                "texture file for %s\nfile name:%s\n", objName.c_str(),
                textureFile.c_str());
         }
         else
         {
            ret = ilutGLBindTexImage();
         }
      #else
         
         glGenTextures(1, &ret);
         glBindTexture(GL_TEXTURE_2D, ret);

         // load image file
         if (!LoadImage((char*)textureFile.c_str()))
            ret = GmatPlot::UNINIT_TEXTURE;
         
      #endif
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("*** Warning *** TrajPlotCanvas::BindTexture() Cannot bind texture "
          "image for %s.\n%s\n", objName.c_str(), e.GetMessage().c_str());
   }
   
   #if DEBUG_TRAJCANVAS_TEXTURE
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::BindTexture() objName=%s ret=%d\n", objName.c_str(),
       ret);
   #endif
   
   return ret;
} //end BindTexture()


//------------------------------------------------------------------------------
// void SetDefaultView()
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetDefaultView()
{
   mCurrRotXAngle = mDefaultRotXAngle;
   mCurrRotYAngle = mDefaultRotYAngle;
   mCurrRotZAngle = mDefaultRotZAngle;
   mCurrViewDist = mDefaultViewDist;
   mAxisLength = mCurrViewDist;
   mfCamTransX = 0;
   mfCamTransY = 0;
   mfCamTransZ = 0;
   mfCamRotXAngle = 0;
   mfCamRotYAngle = 0;
   mfCamRotZAngle = 0;

   mOriginId = GetObjectId("Earth");
}


//------------------------------------------------------------------------------
//  void SetProjection()
//------------------------------------------------------------------------------
/**
 * Sets view projection.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetProjection()
{
   // Setup the world view
   glMatrixMode(GL_PROJECTION); // first go to projection mode
   glLoadIdentity();
   SetupWorld();                // set it up
   glMatrixMode(GL_MODELVIEW);
}


//------------------------------------------------------------------------------
//  void SetupWorld()
//------------------------------------------------------------------------------
/**
 * Sets world view as orthographic projection. With an orthographic projection,
 * the viewing volumn is a rectangular parallelepiped. Unlike perspective
 * projection, the size of the viewing volumn doesn't change from one end to the
 * other, so distance from the camera doesn't affect how large a object appears.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetupWorld()
{

   #if DEBUG_TRAJCANVAS_PROJ > 2
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::SetupWorld() mUsePerspectiveMode=%d, mUseSingleRotAngle=%d\n",
       mUsePerspectiveMode, mUseSingleRotAngle);
   #endif
   
   #if DEBUG_TRAJCANVAS_PROJ > 2
   if (mUseSingleRotAngle)
   {
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::SetupWorld() mfLeftPos=%f, mfRightPos=%f\n   mfBottomPos=%f, "
          "mfTopPos=%f\n   mfViewNear=%f, mfViewFar=%f\n", mfLeftPos, mfRightPos,
          mfBottomPos, mfTopPos, mfViewNear, mfViewFar);
   }
   #endif
   
   if (mUsePerspectiveMode)
   {
      
      // Setup how we view the world
      GLfloat aspect = (GLfloat)mCanvasSize.x / (GLfloat)mCanvasSize.y;

      //loj: 12/7/05 commented out, need only x and y size.
      //Real size = GmatMathUtil::Sqrt(mfRightPos * mfRightPos +
      //                               mfTopPos   * mfTopPos +
      //                               mfViewFar  * mfViewFar);
      
      Real size = GmatMathUtil::Sqrt(mfRightPos * mfRightPos +
                                     mfTopPos   * mfTopPos);
      
      Real dist = mVpLocVec.GetMagnitude();
      
      //loj: 12/7/05 mViewObjRadius = mObjectDefaultRadius*50;
      mViewObjRadius = mObjectDefaultRadius;
      
      if (mUseFixedFov && mUseSingleRotAngle)
      {
         mFovDeg = mFixedFovAngle;
      }
      else
      {
         if (!mUseViewDirectionVector && mViewDirectionObj != NULL)
         {
            // if showing view object (loj: 12/7/05 added)
            if (mShowObjectMap[mViewObjName])
            {
               //loj: 12/7/05 use mViewObjId
               //int objId = GetObjectId(mViewDirectionObj->GetName().c_str());
               //mViewObjRadius = mObjectRadius[objId];
               mViewObjRadius = mObjectRadius[mViewObjId];
            }
         }
                  
         // compute fov angle
         //loj: 12/7/05 add mViewObjRadius to dist
         //mFovDeg = 2.0 * ATan(size/2.0, dist - mViewObjRadius) * DEG_PER_RAD;
         mFovDeg = 2.0 * ATan(size/2.0, dist + mViewObjRadius) * DEG_PER_RAD;
      }

      Real ratio = dist / mAxisLength;
      
      #if DEBUG_TRAJCANVAS_PERSPECTIVE
      if (mUseSingleRotAngle)
      {
         MessageInterface::ShowMessage
            ("   mAxisLength=%f, size=%f, dist=%f, mViewObjRadius=%f\n   "
             "mFovDeg=%f, ratio=%f, \n", mAxisLength, size, dist, mViewObjRadius,
             mFovDeg, ratio);
      }
      #endif

      //loj: 12/7/05 Added ratio to prevent near side clipping
      gluPerspective(mFovDeg, aspect, mAxisLength/(mFovDeg*15), mAxisLength * mFovDeg * ratio);

      //glFrustum(mfLeftPos, mfRightPos, mfViewBottom, mfTopPos, mAxisLength, mfViewFar*mAxisLength);
   }
   else
   {
      // Setup how we view the world
      glOrtho(mfLeftPos, mfRightPos, mfBottomPos, mfTopPos, mfViewNear, mfViewFar);
   }
   
   // if using mouse to rotate the object
   if (!mUseSingleRotAngle)
   {
      
      #ifdef USE_TRACKBALL         
         //MessageInterface::ShowMessage
         //   ("===> mQuat=%f, %f, %f, %f\n", mQuat[0], mQuat[1], mQuat[2], mQuat[2]);
         GLfloat m[4][4];
         BuildRotMatrix( m, mQuat );
         glMultMatrixf( &m[0][0] );      
      #else      
         //Translate Camera
         glTranslatef(mfCamTransX, mfCamTransY, mfCamTransZ);
         ApplyEulerAngles();      
      #endif
   }
   
   //camera moves opposite direction to center on object
   //this is the point of rotation
   
   glTranslatef(mObjectTempPos[mViewObjId][mNumData-1][0],
                mObjectTempPos[mViewObjId][mNumData-1][1],
                -mObjectTempPos[mViewObjId][mNumData-1][2]);
   
} // end SetupWorld()


//------------------------------------------------------------------------------
//  void ComputeView(GLfloat fEndX, GLfloat fEndY)
//------------------------------------------------------------------------------
/**
 * Calculates a percentage of how much the mouse has moved. When moving the mouse
 * left-right, we want to rotate about the Y axis, and vice versa
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::ComputeView(GLfloat fEndX, GLfloat fEndY)
{
   float fYAmnt = 360*(fEndX - mfStartX)/(mfRightPos - mfLeftPos);
   float fXAmnt = 360*(fEndY - mfStartY)/(mfBottomPos - mfTopPos);
   
   
   // always rotate the y axis
   mCurrRotYAngle = mfCamRotYAngle + fYAmnt;

   // Are we rotating the x or the z in this case?
   if (mRotateXy)
   {
      // x axis
      mCurrRotXAngle = mfCamRotXAngle + fXAmnt - 270;
      
      // z axis (loj: 9/28/05 Added)
      mCurrRotZAngle = mfCamRotZAngle + fXAmnt;
   }
   else
   {
      // z axis
      mCurrRotZAngle = mfCamRotZAngle + fXAmnt;
   }
}


//------------------------------------------------------------------------------
//  void ChangeView(float viewX, float viewY, float viewZ)
//------------------------------------------------------------------------------
/**
 * Changes view by rotating the camera.
 *
 * @param <viewX> rotation angle of X component.
 * @param <viewY> rotation angle of Y component.
 * @param <viewZ> rotation angle of Z component.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::ChangeView(float viewX, float viewY, float viewZ)
{

   mfCamRotXAngle = (int)(viewX) % 360 + 270;
   mfCamRotYAngle = (int)(viewY) % 360;
   mfCamRotZAngle = (int)(viewZ) % 360;
   
   //MessageInterface::ShowMessage
   //   ("===> TrajPlotCanvas::ChangeView() mfCamRotXYZAngle = %f %f %f\n",
   //    mfCamRotXAngle, mfCamRotYAngle, mfCamRotZAngle);
   
   // don't let the rotation angles build up to some insane size
   if (mfCamRotYAngle > 360)
      mfCamRotYAngle -= 360;
   else if (mfCamRotYAngle < 0)
      mfCamRotYAngle += 360;

   // don't let the rotation angles build up to some insane size
   if (mfCamRotXAngle > 450)
      mfCamRotXAngle -= 360;
   else if (mfCamRotXAngle < 90)
      mfCamRotXAngle += 360;
   
   // don't let the rotation angles build up to some insane size
   if (mfCamRotZAngle > 360)
      mfCamRotZAngle -= 360;
   else if (mfCamRotZAngle < 0)
      mfCamRotZAngle += 360;
  
} // end ChangeView()


//------------------------------------------------------------------------------
//  void ChangeProjection(int width, int height, float axisLength)
//------------------------------------------------------------------------------
/**
 * Changes view projection by viewing area in pixel and axis length in
 * orghographic projection.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::ChangeProjection(int width, int height, float axisLength)
{    
   GLfloat fAspect = (GLfloat) height / (GLfloat) width;
   
   mfViewLeft   = -axisLength/2;
   mfViewRight  =  axisLength/2;
   
   mfViewTop    =  axisLength/2;
   mfViewBottom = -axisLength/2;

   //texture maps upside down
   //mfViewTop    = -axisLength/2;
   //mfViewBottom =  axisLength/2;

   if (mUseGluLookAt)
   {
      //loj: 1/10/06 changed *2 to * 10000 to fix near/far clipping
      //mfViewNear = -axisLength * 10000.0;
      //mfViewFar  =  axisLength * 10000.0;
      mfViewNear = -axisLength * 100000.0;
      mfViewFar  =  axisLength * 100000.0;
   }
   else
   {
      mfViewNear = -axisLength/2;
      mfViewFar  =  axisLength/2;
   }
   
   // save the size we are setting the projection for later use
   if (width <= height)
   {
      mfLeftPos = mfViewLeft;
      mfRightPos = mfViewRight;
      mfBottomPos = mfViewBottom*fAspect;
      mfTopPos = mfViewTop*fAspect;
   }
   else
   {
      mfLeftPos = mfViewLeft / fAspect;
      mfRightPos = mfViewRight / fAspect;
      mfBottomPos = mfViewBottom;
      mfTopPos = mfViewTop;
   }
}


//------------------------------------------------------------------------------
//  void ComputeViewVectors(int frame)
//------------------------------------------------------------------------------
/**
 * Computes viewing vectors using viewing options.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::ComputeViewVectors(int frame)
{
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage("ComputeViewVectors() frame=%d, time=%f\n",
                                 frame, mTime[frame]);
   #endif
   
   //-----------------------------------------------------------------
   // get viewpoint reference vector
   //-----------------------------------------------------------------
   mVpRefVec.Set(0.0, 0.0, 0.0);
   
   if (!mUseViewPointRefVector && mViewPointRefObj != NULL)
   {
      #if DEBUG_TRAJCANVAS_PROJ
      MessageInterface::ShowMessage
         ("ComputeViewVectors() mViewPointRefObj=%d, name=%s\n",
          mViewPointRefObj, mViewPointRefObj->GetName().c_str());
      #endif

      // if valid body id
      if (mVpRefObjId != UNKNOWN_OBJ_ID)
      {
         // for efficiency, body data are computed in UpdatePlot() once.
         mVpRefVec.Set(mObjectTempPos[mVpRefObjId][frame][0],
                       mObjectTempPos[mVpRefObjId][frame][1],
                       mObjectTempPos[mVpRefObjId][frame][2]);
      }
      else
      {
         MessageInterface::ShowMessage
            ("*** Warning *** TrajPlotCanvas::ComputeViewVectors() Invalid "
             "mVpRefObjId=%d\n", mVpRefObjId);
      }
   }
   
   //-----------------------------------------------------------------
   // get viewpoint vector
   //-----------------------------------------------------------------
   //Rvector3 vpVec(mViewPointVector);
   mVpVec = mViewPointVector;
   
   if (!mUseViewPointVector && mViewPointVectorObj != NULL)
   {
      #if DEBUG_TRAJCANVAS_PROJ
      MessageInterface::ShowMessage
         ("ComputeViewVectors() mViewPointVectorObj=%d, name=%s, mVpVecObjId=%d\n",
          mViewPointVectorObj, mViewPointVectorObj->GetName().c_str(),
          mVpVecObjId);
      #endif
      
      // if valid body id
      if (mVpVecObjId != UNKNOWN_OBJ_ID)
      {
         if (mUseGluLookAt)
         {
            // if look at from object, negate it so that we can see the object
            mVpVec.Set(-mObjectTempPos[mVpVecObjId][frame][0],
                       -mObjectTempPos[mVpVecObjId][frame][1],
                       -mObjectTempPos[mVpVecObjId][frame][2]);

            // set z component to zero so that plane doesn't move up and down
            mVpVec[2] = 0.0;
            
         }
         else
         {
            mVpVec.Set(mObjectTempPos[mVpVecObjId][frame][0],
                       mObjectTempPos[mVpVecObjId][frame][1],
                       mObjectTempPos[mVpVecObjId][frame][2]);
         }
      }
      else
      {
         MessageInterface::ShowMessage
            ("*** Warning *** TrajPlotCanvas::ComputeViewVectors() Invalid "
             "mVpVecObjId=%d\n", mVpVecObjId);
      }
   }
   
   //-----------------------------------------------------------------
   // get viewpoint location
   //-----------------------------------------------------------------
   mVpLocVec = mVpRefVec + (mViewScaleFactor * mVpVec);
   
   //-----------------------------------------------------------------
   // get view direction and view center vector
   //-----------------------------------------------------------------
   mVdVec = mViewDirectionVector;
   
   if (!mUseViewDirectionVector && mViewDirectionObj != NULL)
   {
      #if DEBUG_TRAJCANVAS_PROJ
      MessageInterface::ShowMessage
         ("ComputeViewVectors() mViewDirectionObj=%d, name=%s\n",
          mViewDirectionObj, mViewDirectionObj->GetName().c_str());
      #endif
      
      // if viewpoint ref object is same as view direction object
      // just look opposite side
      if (mViewDirectionObj->GetName() == mViewPointRefObjName)
      {
         mVdVec = -mVpLocVec;
      }
      else if (mVdirObjId != UNKNOWN_OBJ_ID)
      {
         // for efficiency, body data are computed in UpdatePlot() once.
         mVdVec.Set(mObjectTempPos[mVdirObjId][frame][0],
                    mObjectTempPos[mVdirObjId][frame][1],
                    mObjectTempPos[mVdirObjId][frame][2]);

         // check for 0.0 direction 
         if (mVdVec.GetMagnitude() == 0.0)
            mVdVec = mViewDirectionVector;
      }
      else
      {
         MessageInterface::ShowMessage
            ("*** Warning *** TrajPlotCanvas::ComputeViewVectors() Invalid "
             "mVdirObjId=%d\n", mVdirObjId);
      }
   }

   // set view center vector for gluLookAt()
   mVcVec = mVdVec;
   
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("ComputeViewVectors() mVpRefVec=%s, mVpVec=%s\nmVpLocVec=%s, "
       " mVdVec=%s, mVcVec=%s\n", mVpRefVec.ToString().c_str(),
       mVpVec.ToString().c_str(), mVpLocVec.ToString().c_str(),
       mVdVec.ToString().c_str(), mVcVec.ToString().c_str());
   #endif
   
   
   //-----------------------------------------------------------------
   // set view center object
   //-----------------------------------------------------------------
      
   // compute axis length (this tells how far zoom out is)

   // Initially use mVpLocVec and later use changed value by mouse zoom-in/out.
   // This will use scale factor correctly for data points are less than
   // update frequency.
   if (mNumData <= mUpdateFrequency)
      mAxisLength = mVpLocVec.GetMagnitude();
   
   // if mAxisLength is too small, set to max zoom in value
   if (mAxisLength < mMaxZoomIn)
      mAxisLength = mMaxZoomIn;
   
   // compute camera rotation angle
   Real vdMag = mVdVec.GetMagnitude();
   
   mfCamSingleRotAngle = ACos(-(mVdVec[2]/vdMag)) * DEG_PER_RAD;
   
   // compute axis of rotation
   mfCamRotXAxis =  mVdVec[1];
   mfCamRotYAxis = -mVdVec[0];
   mfCamRotZAxis = 0.0;
   mUseSingleRotAngle = true;

   ComputeUpAngleAxis(frame);
   
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("ComputeViewVectors() mfCamTransXYZ=%f, %f, %f, mfCamSingleRotAngle=%f\n"
       "   mfCamRotXYZ=%f, %f, %f mAxisLength=%f\n", mfCamTransX, mfCamTransY,
       mfCamTransZ, mfCamSingleRotAngle, mfCamRotXAxis, mfCamRotYAxis,
       mfCamRotZAxis, mAxisLength);
   #endif
} // end ComputeViewVectors(int frame)


//------------------------------------------------------------------------------
// void ComputeUpAngleAxis(int frame)
//------------------------------------------------------------------------------
void TrajPlotCanvas::ComputeUpAngleAxis(int frame)
{
   // calculate view up direction
   
   Rvector6 upOutState;
   
   upOutState = mUpState;
   
   if (mViewUpCoordSystem->GetName() != mViewCoordSystem->GetName())
   {
      mCoordConverter.Convert(mTime[frame], mUpState, mViewUpCoordSystem,
                              upOutState, mViewCoordSystem);
      
      //=========================== from Steve
      //Rvector6 originIn;
      //Rvector6 originOut;
      
      //mCoordConverter.Convert(mTime[frame], mUpState, mViewUpCoordSystem,
      //                        upOutState, mInternalCoordSystem);
      
      //mCoordConverter.Convert(mTime[frame], originIn, mViewUpCoordSystem,
      //                         originOut, mInternalCoordSystem);
      
      //Rvector6 r1 = upOutState - originOut;
      //Rvector6 r2;
      
      //mCoordConverter.Convert(mTime[frame], r1, mInternalCoordSystem,
      //                        r2, mViewCoordSystem);
         
      //mCoordConverter.Convert(mTime[frame], originIn, mInternalCoordSystem,
      //                        originOut, mViewCoordSystem);
      
      //upOutState = r2 - originOut;
      //=========================== from Steve
   }
   
   mUpVec.Set(upOutState(0), upOutState(1), upOutState(2));

   //loj: 12/7/05 Added
   // If view up and view direction is the same axis, change view direction,
   // so it can show up direction correctly even for gluLookAt.
   
   Rvector3 vdUnit = mVdVec.GetUnitVector();
   Real upDotView = mUpVec * vdUnit;
   //MessageInterface::ShowMessage("===> upDotView = %f\n", upDotView);
   
   if (Abs(upDotView) == 1.0)
   {
      //MessageInterface::ShowMessage("===> mUpVec and mVdVec are on the same axis.\n");
      
      if (Abs(mUpVec[0]) > 0.0)
         mVcVec = Cross(mUpVec, Rvector3(0.0, -1.0, 0.0));
      else if (Abs(mUpVec[1]) > 0.0)
         mVcVec = Cross(mUpVec, Rvector3(0.0, 0.0, -1.0));
      else
         mVcVec = Cross(mUpVec, Rvector3(-1.0, 0.0, 0.0));

      mVdVec = Cross(mVdVec, mVcVec);

      // if using gluLookAt, we don't want view point and view up direction line up
      if (mUseGluLookAt)
         mVpLocVec = -mVdVec;
   }
   
   // DJC added for "Up"   
   mfUpAngle = atan2(mVdVec[1],mVdVec[0]) * DEG_PER_RAD + 90.0;
   mfUpXAxis = mVdVec[0];
   mfUpYAxis = mVdVec[1];
   mfUpZAxis = mVdVec[2];
   
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::ComputeUpAngleAxis() mVpLocVec=%s, mVdVec=%s\n   "
       "mVcVec=%s, mUpVec=%s\n", mVpLocVec.ToString().c_str(),
       mVdVec.ToString().c_str(), mVcVec.ToString().c_str(),
       mUpVec.ToString().c_str());
   MessageInterface::ShowMessage
      ("   atan2(mVdVec[y],mVdVec[x])=%f, mfUpAngle=%f, mfUpXYZAxis=%f, %f, %f\n",
       atan2(mVdVec[1],mVdVec[0]), mfUpAngle, mfUpXAxis, mfUpYAxis, mfUpZAxis);
   #endif
} // end ComputeUpAngleAxis()


//------------------------------------------------------------------------------
// void TransformView(int frame)
//------------------------------------------------------------------------------
void TrajPlotCanvas::TransformView(int frame)
{
   if (frame < 0)
      return;
   
   //MessageInterface::ShowMessage
   //   ("===> TrajPlotCanvas::TransformView() mUseSingleRotAngle=%d, "
   //    "mUseGluLookAt=%d\n", mUseSingleRotAngle, mUseGluLookAt);
   
   if (mUseSingleRotAngle)
   {
      glLoadIdentity();
      
      if (mUseGluLookAt)
      {
         //-----------------------------------------------------------
         // To fix Earth Z-axis flipping when looking at from SC
         //-----------------------------------------------------------         
         if (mViewUpAxisName == "X")
         {
            if (mVpLocVec[1] < 0)
               mUpVec.Set(1.0, 0.0, 0.0);
         }
         else if (mViewUpAxisName == "-X")
         {
            if (mVpLocVec[1] < 0)
               mUpVec.Set(-1.0, 0.0, 0.0);
         }
         else if (mViewUpAxisName == "Y")
         {
            if (mVpLocVec[0] < 0)
               mUpVec.Set(0.0, 1.0, 0.0);
         }
         else if (mViewUpAxisName == "-Y")
         {
            if (mVpLocVec[0] < 0)
               mUpVec.Set(0.0, -1.0, 0.0);
         }
         
         //MessageInterface::ShowMessage
         //   ("===> mVpLocVec=%s, mVcVec=%s, mUpVec=%s\n",
         //    mVpLocVec.ToString().c_str(), mVcVec.ToString().c_str(),
         //    mUpVec.ToString().c_str());
         
         //-------------------------------------------------
         // use gluLookAt()
         //-------------------------------------------------
         gluLookAt(mVpLocVec[0], mVpLocVec[1], mVpLocVec[2],
                   mVcVec[0], mVcVec[1], mVcVec[2],
                   mUpVec[0], mUpVec[1], mUpVec[2]);

      }
      else
      {
         glTranslatef(mfCamTransX, mfCamTransY, mfCamTransZ);
         glRotatef(mfCamSingleRotAngle,
                   mfCamRotXAxis, mfCamRotYAxis, mfCamRotZAxis);
         
         // DJC added for Up
         glRotatef(-mfUpAngle, mfUpXAxis, mfUpYAxis, -mfUpZAxis);
                           
      } //if (mUseGluLookAt)

      //-------------------------------------------------
      // add current user mouse rotation
      //-------------------------------------------------
      #ifdef USE_TRACKBALL      
         //MessageInterface::ShowMessage
         //   ("===> mQuat=%f, %f, %f, %f\n", mQuat[0], mQuat[1], mQuat[2], mQuat[2]);
         GLfloat m[4][4];
         BuildRotMatrix(m, mQuat);
         glMultMatrixf(&m[0][0]);      
      #else      
         ApplyEulerAngles();      
      #endif

   } //if (mUseSingleRotAngle)
   
} // end TransformView()


//------------------------------------------------------------------------------
//  void DrawFrame()
//------------------------------------------------------------------------------
/**
 * Draws whole picture.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawFrame()
{
   #if DEBUG_TRAJCANVAS_ANIMATION
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::DrawFrame() mNumData=%d, mUsenitialViewPoint=%d\n"
       "   mViewCoordSysName=%s, mInitialCoordSysName=%s\n", mNumData,
       mUseInitialViewPoint, mViewCoordSysName.c_str(), mInitialCoordSysName.c_str());
   #endif
   
   if (mUseInitialViewPoint)
   {
      SetDefaultView();
      
      if (!mViewCoordSysName.IsSameAs(mInitialCoordSysName))
      {         
         if (mInitialCoordSysName.IsSameAs(mInternalCoordSysName))
         {
            mViewCoordSystem = mInternalCoordSystem;
            mViewCoordSysName = mInternalCoordSysName;
            mIsInternalCoordSystem = true;
         }
         else
         {
            mViewCoordSystem = mInitialCoordSystem;
            mViewCoordSysName = mInitialCoordSysName;
            mIsInternalCoordSystem = false;
         }
      }
      
      UpdateRotateFlags();
      ConvertObjectData();
      
      // set view center object
      mOriginName = wxString(mViewCoordSystem->GetOriginName().c_str());
      mOriginId = GetObjectId(mOriginName);
      
      mViewObjName = mOriginName;
      GotoObject(mViewObjName); //loj: 1/11/06 added
   }
   
   int numberOfData = mNumData;
   mIsEndOfData = false;
   mIsEndOfRun = false;
   
   for (int frame=1; frame<numberOfData; frame++)
   {
      // wxYield() yields control to pending messages in the windowing system.
      
      // wxSafeYield() is similar to wxYield() except it disables the user
      // input to all program windows before calling wxYield and re-enables
      // it again afterwards.
      
      //wxSafeYield();
      wxYield(); //loj: 8/16/05 to allow mouse event
      
      if (mHasUserInterrupted)
         break;
      
      Sleep(mUpdateInterval);
      
      mNumData = frame;
      
      // Set projection here, because DrawPlot() is called in OnPaint()
      if (mUseInitialViewPoint)
         ComputeViewVectors(mNumData-1);
      
      ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
      
      Refresh(false);
   }
   
   mNumData = numberOfData;
   mIsEndOfData = true;
   mIsEndOfRun = true;
   
} // end DrawFrame()


//------------------------------------------------------------------------------
//  void DrawPlot()
//------------------------------------------------------------------------------
/**
 * Draws whole plot.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawPlot()
{
   if (mNumData < 1) // to avoid 0.0 time
      return;
   
   #if DEBUG_TRAJCANVAS_DRAW
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::DrawPlot() mNumData=%d, mNeedOriginConversion=%d, "
       "mIsInternalCoordSystem=%d\n   mUseInitialViewPoint=%d, mAxisLength=%f\n",
       mNumData, mNeedOriginConversion, mIsInternalCoordSystem, mUseInitialViewPoint,
       mAxisLength);
   #endif
   
   if (mRedrawLastPointsOnly || mNumPointsToRedraw == 0)
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   else
      glClear(GL_DEPTH_BUFFER_BIT);
   
   DrawStatus("Frame#: ", mNumData-1, mTime[mNumData-1]);
   
   #if DEBUG_TRAJCANVAS_DRAW
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::DrawPlot() mIsEndOfData=%d, mIsEndOfRun=%d\n",
      mIsEndOfData, mIsEndOfRun);
   #endif
   
   // compute projection if not using initial viewpoint and not end of run.
   if (mUseInitialViewPoint && !mIsEndOfRun)
      ComputeViewVectors(mNumData-1);
   
   ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
   
   // change back to view projection
   SetProjection();

   TransformView(mNumData-1);

   // tilt Origin rotation axis if needed
   if (mNeedOriginConversion)
   {
      glPushMatrix();
      TiltOriginZAxis();
   }
   
   // draw equatorial plane
   if (mDrawXyPlane)
      DrawEquatorialPlane(mXyPlaneColor);

   // draw axes
   if (mDrawAxes)
   {
      if (!mCanRotateAxes)
      {
         DrawAxes();
      }
   }
   
   // draw ecliptic plane
   if (mDrawEcPlane)
      DrawEclipticPlane(mEcPlaneColor);
   
   if (mNeedOriginConversion)
      glPopMatrix();
   
   // draw object orbit
   DrawObjectOrbit(mNumData-1);
   
   // draw Earth-Sun line
   if (mDrawESLines)
      DrawESLines(mNumData-1);
   
   glFlush();
   SwapBuffers();
   
} // end DrawPlot()


//------------------------------------------------------------------------------
//  void DrawSphere()
//------------------------------------------------------------------------------
/**
 * Draws sphere.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawSphere(GLdouble radius, GLint slices, GLint stacks,
                                GLenum style, GLenum orientation)
{
   GLUquadricObj* qobj = gluNewQuadric();
   gluQuadricDrawStyle(qobj, style  );
   gluQuadricNormals  (qobj, GLU_SMOOTH);
   gluQuadricTexture  (qobj, GL_TRUE   );
   gluSphere(qobj, radius, slices, stacks);
   gluQuadricOrientation(qobj, orientation);
   gluDeleteQuadric(qobj);
}


//------------------------------------------------------------------------------
//  void DrawObject(const wxString &objName, int frame)
//------------------------------------------------------------------------------
/**
 * Draws object sphere and maps texture image.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawObject(const wxString &objName, int frame)
{
   int objId = GetObjectId(objName);
   
   #if DEBUG_TRAJCANVAS_DRAW > 1
   MessageInterface::ShowMessage
         ("TrajPlotCanvas::DrawObject() drawing:%s, objId:%d, frame=%d\n",
          objName.c_str(), objId, frame);
   #endif
   
   //loj: 11/4/05 Added for light
   #ifdef ENABLE_LIGHT_SOURCE
   //-------------------------------------------------------
   // enable light source on option
   //-------------------------------------------------------
   if (mEnableLightSource && mSunPresent)
   {
      //Why whole Sun is not lit?
      //float lightPos[4] = {0.0f, 0.0f, 0.0f, 1.0f};
      float lightPos[4];
      int sunId = GetObjectId("Sun");
      lightPos[0] = -mObjectTempPos[sunId][frame][0];
      lightPos[1] = -mObjectTempPos[sunId][frame][1];
      lightPos[2] = -mObjectTempPos[sunId][frame][2];
      // opposite direction with  sun earth line
      //lightPos[0] = mObjectTempPos[sunId][frame][0];
      //lightPos[1] = mObjectTempPos[sunId][frame][1];
      //lightPos[2] = mObjectTempPos[sunId][frame][2];
      lightPos[3] = 1.0;
      // If 4th value is zero, the light source is directional one, and
      // (x,y,z) values describes its direction.
      // If 4th value is nonzero, the light is positional, and the (x,y,z) values
      // specify the location of the light in homogeneous object coordinates.
      // By default, a positional light radiates in all directions.
      
      // reset the light position to reflect the transformations
      glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
      
      // enable the lighting
      glEnable(GL_LIGHTING);
   }
   #endif
   
   #if DEBUG_TRAJCANVAS_DRAW > 1
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::DrawObject() mObjectTextureIdMap[%s]=%d\n",
       objName.c_str(), mObjectTextureIdMap[objName]);
   #endif
   
   //-------------------------------------------------------
   // rotate Earth, need to rotate before texture mapping
   //-------------------------------------------------------
   if (objName == "Earth" && mCanRotateBody)
   {
      Real earthRotAngle = 0.0;
      Real initialLong = mInitialLongitude;
      Real offset = 40.0; // need this to line up earth texture with longitude
            
      //========== #ifdef
      #ifdef USE_MHA_TO_ROTATE_EARTH
      
      if (mSolarSystem)
      {         
         Real mha = 0.0;
         
         if (initialLong < 180.0)
            initialLong = -initialLong - offset;

         CelestialBody *earth = mSolarSystem->GetBody("Earth");
         if (earth)
            mha = earth->GetHourAngle(mTime[frame]);
         
         earthRotAngle = mha + initialLong + offset;
      
         #if DEBUG_TRAJCANVAS_DRAW > 1
         if (!mIsEndOfRun)
            MessageInterface::ShowMessage
               ("===> mha=%f, earthRotAngle=%f\n", mha, earthRotAngle);
         #endif
         
      }
      
      //========== #else
      #else
      
      if (initialLong < 180.0)
         initialLong = -initialLong - offset;
      
      earthRotAngle = (Abs(mTime[frame] - mTime[0])) * 361.0 + mInitialMha +
         initialLong + offset;
      
      #endif
      //========== #endif

      earthRotAngle =
         AngleUtil::PutAngleInDegRange(earthRotAngle, 0.0, 360.0);
      
      #if DEBUG_TRAJCANVAS_DRAW > 1
      if (!mIsEndOfRun)
         MessageInterface::ShowMessage
            ("===> backwards=%d, earthRotAngle=%f\n", backwards, earthRotAngle);
      #endif

      glRotatef(earthRotAngle, 0.0, 0.0, 1.0);
   }
   
   //-------------------------------------------------------
   // draw axes if it rotates with the body
   //-------------------------------------------------------
   if (mDrawAxes && objId == mOriginId && mCanRotateAxes)
   {
      DrawAxes();
   }

   //-------------------------------------------------------
   // draw object with texture on option
   //-------------------------------------------------------
   if (mObjectTextureIdMap[objName] != GmatPlot::UNINIT_TEXTURE)
   {
      //glColor4f(1.0, 1.0, 1.0, 1.0);
      glColor3f(1.0, 1.0, 1.0);
      
      glBindTexture(GL_TEXTURE_2D, mObjectTextureIdMap[objName]);
      glEnable(GL_TEXTURE_2D);

      if (objName == "Sun")
         DrawSphere(mObjectRadius[objId], 50, 50, GLU_FILL, GLU_INSIDE);
      else
         DrawSphere(mObjectRadius[objId], 50, 50, GLU_FILL);
         
      //----------------------------------------------------
      // draw grid on option
      //----------------------------------------------------
      if (mDrawGrid && objName == "Earth")
      {
         // Just draw a wireframe sphere little bigger to show grid(loj: 11/3/05)
         glColor3f(0.20, 0.20, 0.50);
         DrawSphere(mObjectRadius[objId]+50, 36, 18, GLU_LINE);
      }
      
      glDisable(GL_TEXTURE_2D);
   }
   else
   {
      #if DEBUG_TRAJCANVAS_DRAW
      MessageInterface::ShowMessage
         ("*** Warning *** TrajPlotCanvas::DrawObject() %s texture not found.\n",
          objName.c_str());
      #endif
      
      // Just draw a wireframe sphere if we get here
      glColor3f(0.20, 0.20, 0.50);
      DrawSphere(mObjectRadius[objId], 50, 50, GLU_LINE);      
      glDisable(GL_TEXTURE_2D);
   }
   
   #ifdef ENABLE_LIGHT_SOURCE
   if (mEnableLightSource && mSunPresent)
   {
      glDisable(GL_LIGHTING);
   }
   #endif
   
} // end DrawObject(const wxString &objName)


//------------------------------------------------------------------------------
//  void DrawObjectOrbit(int frame)
//------------------------------------------------------------------------------
/**
 * Draws object orbit and object at the last point.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawObjectOrbit(int frame)
{
   int objId;
   wxString objName;

   #ifdef ENABLE_LIGHT_SOURCE
   if (mEnableLightSource && mSunPresent)
   {
      // we don't want the orbit paths lit
      glDisable(GL_LIGHTING);
   }
   #endif
   
   for (int obj=0; obj<mObjectCount; obj++)
   {
      objName = mObjectNames[obj];
      objId = GetObjectId(objName);
      
      if (!mDrawOrbitFlag[objId][frame])
         continue;
      
      glPushMatrix();
      glBegin(GL_LINES);
      
      int beginFrame = 1;
      
      //---------------------------------------------------------
      // compute begin frame
      //---------------------------------------------------------
      if (mRedrawLastPointsOnly && !mIsEndOfRun)
      {
         beginFrame = frame - mNumPointsToRedraw;
         if (beginFrame < 1)
            beginFrame = 1;
      }
      else if (!mIsEndOfRun && mNumPointsToRedraw == -1)
      {
         beginFrame = frame - 2;
         if (beginFrame < 1)
            beginFrame = 1;
      }

      //---------------------------------------------------------
      // draw lines
      //---------------------------------------------------------
      for (int i=beginFrame; i<=frame; i++)
      {
         // Draw object orbit line based on points
         if ((mTime[i] > mTime[i-1]) ||
             (i>2 && mTime[i] < mTime[i-1]) && mTime[i-1] < mTime[i-2]) //for backprop
         {
            Rvector3 r1(mObjectTempPos[objId][i-1][0], mObjectTempPos[objId][i-1][1],
                        mObjectTempPos[objId][i-1][2]);
            
            Rvector3 r2(mObjectTempPos[objId][i][0], mObjectTempPos[objId][i][1],
                        mObjectTempPos[objId][i][2]);
            
            // if object position magnitude is 0, skip
            if (r1.GetMagnitude() == 0.0 || r2.GetMagnitude() == 0.0)
               continue;

            // if object position diff is over limit, skip (ScriptEx_TargetHohmann)
            #ifdef SKIP_OVER_LIMIT_DATA
            static Real sMaxDiffDist = 100000.0;
            // if difference is more than sMaxDiffDist skip
            if ((Abs(r2[0]- r1[0]) > sMaxDiffDist && (SignOf(r2[0]) != SignOf(r1[0]))) ||
                (Abs(r2[1]- r1[1]) > sMaxDiffDist && (SignOf(r2[1]) != SignOf(r1[1]))) ||
                (Abs(r2[2]- r1[2]) > sMaxDiffDist && (SignOf(r2[2]) != SignOf(r1[2]))))
            {
               #if DEBUG_SHOW_SKIP
               MessageInterface::ShowMessage
                  ("===> plot=%s, i1=%d, i2=%d, time1=%f, time2=%f\n   r1=%s, r2=%s\n",
                   GetName().c_str(), i-1, i, mTime[i-1], mTime[i], r1.ToString().c_str(),
                   r2.ToString().c_str());
               #endif
               continue;
            }
            #endif
            
            if (mObjectArray[obj]->IsOfType(Gmat::SPACECRAFT))
               *sIntColor = mObjectOrbitColor[objId][i];
            else
               *sIntColor = mObjectColorMap[objName].GetIntColor();
            
            glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);
            
            glVertex3f((-mObjectTempPos[objId][i-1][0]),
                       (-mObjectTempPos[objId][i-1][1]),
                       ( mObjectTempPos[objId][i-1][2]));
            
            glVertex3f((-mObjectTempPos[objId][i][0]),
                       (-mObjectTempPos[objId][i][1]),
                       ( mObjectTempPos[objId][i][2]));
            
            mObjLastFrame[objId] = i;            
            
         }
      }
      
      glEnd();
      glPopMatrix();
      
      //---------------------------------------------------------
      // draw object orbit normal vector
      // (loj: 6/13/05 Fow now it only draws spacecraft orbit normal vector.)
      //---------------------------------------------------------
      if (mShowOrbitNormalMap[objName])
      {
         int numSkip = frame/12;
         UnsignedInt color;
         
         for (int i=1; i<=frame; i++)
         {
            if (numSkip <= 0 || i % numSkip != 0)
               continue;
            
            if ((mTime[i] > mTime[i-1]) ||
                (i>2 && mTime[i] < mTime[i-1]) && mTime[i-1] < mTime[i-2])
            {
               Rvector3 r1(mObjectTempPos[objId][i-1][0],
                           mObjectTempPos[objId][i-1][1],
                           mObjectTempPos[objId][i-1][2]);
               
               Rvector3 r2(mObjectTempPos[objId][i][0],
                           mObjectTempPos[objId][i][1],
                           mObjectTempPos[objId][i][2]);
               
               // if object position magnitude is 0, skip
               if (r1.GetMagnitude() == 0.0 || r2.GetMagnitude() == 0.0)
                  continue;
               
               glPushMatrix();
               
               // move to origin
               glTranslatef(-mObjectTempPos[mOriginId][i][0],
                            -mObjectTempPos[mOriginId][i][1],
                             mObjectTempPos[mOriginId][i][2]);
               
//                glTranslatef(mObjectTempPos[mOriginId][i][0],
//                             mObjectTempPos[mOriginId][i][1],
//                             -mObjectTempPos[mOriginId][i][2]);
               
               if (mObjectArray[obj]->IsOfType(Gmat::SPACECRAFT))
                  color = mObjectOrbitColor[objId][i];
               else
                  color = mObjectColorMap[objName].GetIntColor();
               
               DrawObjectOrbitNormal(objId, i, color);
               glPopMatrix();
            }
         }
      }
      
      //---------------------------------------------------------
      //draw object with texture
      //---------------------------------------------------------
      //loj: 4/25/05 Why is spcacecraft rotaing itself?
      
      if (frame > 0)
      {         
         if (mShowObjectMap[objName])
         {
            #if DEBUG_TRAJCANVAS_DRAW
            MessageInterface::ShowMessage
               ("DrawObjectOrbit() objName=%s, objId=%d\n", objName.c_str(), objId);
            MessageInterface::ShowMessage
               ("DrawObjectOrbit() mObjectTempPos=%f, %f, %f\n",
                mObjectTempPos[objId][mObjLastFrame[objId]][0],
                mObjectTempPos[objId][mObjLastFrame[objId]][1],
                mObjectTempPos[objId][mObjLastFrame[objId]][2]);
            #endif
            
            glPushMatrix();
            
            // put object at final position
            glTranslatef(-mObjectTempPos[objId][mObjLastFrame[objId]][0],
                         -mObjectTempPos[objId][mObjLastFrame[objId]][1],
                          mObjectTempPos[objId][mObjLastFrame[objId]][2]);
            
//             glTranslatef(mObjectTempPos[objId][mObjLastFrame[objId]][0],
//                          mObjectTempPos[objId][mObjLastFrame[objId]][1],
//                          -mObjectTempPos[objId][mObjLastFrame[objId]][2]);
            
            if (mObjectArray[obj]->IsOfType(Gmat::SPACECRAFT))
               DrawSpacecraft(mObjectOrbitColor[objId][mObjLastFrame[obj]]);
            else
               DrawObject(objName, frame);
            
            glPopMatrix();
            
         }
      }
   }
   
} // end DrawObjectOrbit(int frame)


//------------------------------------------------------------------------------
//  void DrawObjectOrbitNormal(int obj, int objId, int frame, UnsignedInt color)
//------------------------------------------------------------------------------
/**
 * Draws object orbit normal vector.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawObjectOrbitNormal(int objId, int frame, UnsignedInt color)
{
   Real distance = (Real)mAxisLength/2.2;
   float endPos[3];
   
   Rvector3 r(mObjectTempPos[objId][frame][0], mObjectTempPos[objId][frame][1],
              mObjectTempPos[objId][frame][2]);
            
   Rvector3 v(mObjectTempVel[objId][frame][0], mObjectTempVel[objId][frame][1],
              mObjectTempVel[objId][frame][2]);
      
   Rvector3 normV = Cross(r, v);
   normV.Normalize();
   
   //--------------------------------
   // draw normal vector line
   //--------------------------------
   
   // set color
   *sIntColor = color;
   glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);
   
   glBegin(GL_LINES);
   
   // get orbit normal unit vector and multiply by distance
   // Add minus sign to x, y
   endPos[0] = -normV[0] * distance;
   endPos[1] = -normV[1] * distance;
   endPos[2] =  normV[2] * distance;
   
   glVertex3f(0.0, 0.0, 0.0);
   glVertex3f(endPos[0], endPos[1], endPos[2]);
   
   glEnd();

   // Draw a cone (loj: 6/13/05 need to compute cone up direction)
   //glPushMatrix();
   //glTranslatef(endPos[0], endPos[1], endPos[2]);
   //GLUquadricObj* qobj = gluNewQuadric();
   //gluQuadricDrawStyle(qobj, GLU_FILL  );
   //gluQuadricNormals  (qobj, GLU_SMOOTH);
   //gluCylinder(qobj, 200.0, 0.0, 250.0, 50, 50);
   //gluDeleteQuadric(qobj);
   //glPopMatrix();
   
   // Show Orbit Normal direction text
   //glColor3f(1.0, 1.0, 0.0); // yellow
   DrawStringAt(" +N", endPos[0], endPos[1], endPos[2]);
}


//------------------------------------------------------------------------------
//  void DrawSpacecraft(UnsignedInt scColor)
//------------------------------------------------------------------------------
/**
 * Draws spacecraft.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawSpacecraft(UnsignedInt scColor)
{
   // draw six faces of a long cube
   glBegin(GL_QUADS);
   *sIntColor = scColor;
   glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);
        
   glNormal3f( 0.0F, 0.0F, 1.0F);
   glVertex3f( mScRadius, mScRadius, mScRadius*2);
   glVertex3f(-mScRadius, mScRadius, mScRadius*2);
   glVertex3f(-mScRadius,-mScRadius, mScRadius*2);
   glVertex3f( mScRadius,-mScRadius, mScRadius*2);

   glNormal3f( 0.0F, 0.0F,-1.0F);
   glVertex3f(-mScRadius,-mScRadius,-mScRadius*2);
   glVertex3f(-mScRadius, mScRadius,-mScRadius*2);
   glVertex3f( mScRadius, mScRadius,-mScRadius*2);
   glVertex3f( mScRadius,-mScRadius,-mScRadius*2);

   glNormal3f( 0.0F, 1.0F, 0.0F);
   glVertex3f( mScRadius, mScRadius, mScRadius*2);
   glVertex3f( mScRadius, mScRadius,-mScRadius*2);
   glVertex3f(-mScRadius, mScRadius,-mScRadius*2);
   glVertex3f(-mScRadius, mScRadius, mScRadius*2);

   glNormal3f( 0.0F,-1.0F, 0.0F);
   glVertex3f(-mScRadius,-mScRadius,-mScRadius*2);
   glVertex3f( mScRadius,-mScRadius,-mScRadius*2);
   glVertex3f( mScRadius,-mScRadius, mScRadius*2);
   glVertex3f(-mScRadius,-mScRadius, mScRadius*2);

   glNormal3f( 1.0F, 0.0F, 0.0F);
   glVertex3f( mScRadius, mScRadius, mScRadius*2);
   glVertex3f( mScRadius,-mScRadius, mScRadius*2);
   glVertex3f( mScRadius,-mScRadius,-mScRadius*2);
   glVertex3f( mScRadius, mScRadius,-mScRadius*2);

   glNormal3f(-1.0F, 0.0F, 0.0F);
   glVertex3f(-mScRadius,-mScRadius,-mScRadius*2);
   glVertex3f(-mScRadius,-mScRadius, mScRadius*2);
   glVertex3f(-mScRadius, mScRadius, mScRadius*2);
   glVertex3f(-mScRadius, mScRadius,-mScRadius*2);
   glEnd();

   // spacecraft with same color, use Display List
   if (mGlList == 0)
   {
      mGlList = glGenLists( 1 );
      glNewList( mGlList, GL_COMPILE_AND_EXECUTE );
      
      // draw six faces of a thin wide cube
      glBegin(GL_QUADS);
      *sIntColor = GmatColor::YELLOW32;
      glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);

      glNormal3f( 0.0F, 0.0F, 1.0F);
      glVertex3f( mScRadius/4, mScRadius*4, mScRadius*1.5);
      glVertex3f(-mScRadius/4, mScRadius*4, mScRadius*1.5);
      glVertex3f(-mScRadius/4,-mScRadius*4, mScRadius*1.5);
      glVertex3f( mScRadius/4,-mScRadius*4, mScRadius*1.5);

      glNormal3f( 0.0F, 0.0F,-1.0F);
      glVertex3f(-mScRadius/4,-mScRadius*4,-mScRadius*1.5);
      glVertex3f(-mScRadius/4, mScRadius*4,-mScRadius*1.5);
      glVertex3f( mScRadius/4, mScRadius*4,-mScRadius*1.5);
      glVertex3f( mScRadius/4,-mScRadius*4,-mScRadius*1.5);

      glNormal3f( 0.0F, 1.0F, 0.0F);
      glVertex3f( mScRadius/4, mScRadius*4, mScRadius*1.5);
      glVertex3f( mScRadius/4, mScRadius*4,-mScRadius*1.5);
      glVertex3f(-mScRadius/4, mScRadius*4,-mScRadius*1.5);
      glVertex3f(-mScRadius/4, mScRadius*4, mScRadius*1.5);

      glNormal3f( 0.0F,-1.0F, 0.0F);
      glVertex3f(-mScRadius/4,-mScRadius*4,-mScRadius*1.5);
      glVertex3f( mScRadius/4,-mScRadius*4,-mScRadius*1.5);
      glVertex3f( mScRadius/4,-mScRadius*4, mScRadius*1.5);
      glVertex3f(-mScRadius/4,-mScRadius*4, mScRadius*1.5);

      glNormal3f( 1.0F, 0.0F, 0.0F);
      glVertex3f( mScRadius/4, mScRadius*4, mScRadius*1.5);
      glVertex3f( mScRadius/4,-mScRadius*4, mScRadius*1.5);
      glVertex3f( mScRadius/4,-mScRadius*4,-mScRadius*1.5);
      glVertex3f( mScRadius/4, mScRadius*4,-mScRadius*1.5);

      glNormal3f(-1.0F, 0.0F, 0.0F);
      glVertex3f(-mScRadius/4,-mScRadius*4,-mScRadius*1.5);
      glVertex3f(-mScRadius/4,-mScRadius*4, mScRadius*1.5);
      glVertex3f(-mScRadius/4, mScRadius*4, mScRadius*1.5);
      glVertex3f(-mScRadius/4, mScRadius*4,-mScRadius*1.5);
      glEnd();
      glEndList();
   }
   else
   {
      glCallList( mGlList );
   }

} // end DrawSpacecraft()


//------------------------------------------------------------------------------
//  void DrawEquatorialPlane(UnsignedInt color)
//------------------------------------------------------------------------------
/**
 * Draws equatorial plane circles.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawEquatorialPlane(UnsignedInt color)
{
   int i;
   float endPos[3];
   float distance;
   Real angle;
   
   static const Real RAD_PER_DEG =
      3.14159265358979323846264338327950288419716939937511 / 180.0;
   
   distance = (Real)mAxisLength;

   glPushMatrix();
   glBegin(GL_LINES);
   
   // set color
   *sIntColor = color;
   glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);

   //-----------------------------------
   // draw lines
   //-----------------------------------
   for (i=0; i<360; i+=15)
   {
      angle = RAD_PER_DEG * ((Real)i);
      
      endPos[0] = distance * cos(angle);
      endPos[1] = distance * sin(angle);
      endPos[2] = 0.0;
      
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(endPos[0], endPos[1], endPos[2]);
   }
   
   glEnd();
   glPopMatrix();

   //-----------------------------------
   // draw circles
   //-----------------------------------
   glPushMatrix();
   
   GLUquadricObj *qobj = gluNewQuadric();

   //==============================================================
   // Argosy code
   //==============================================================
   Real orthoDepth = distance;
   
   //orthoDepth = (half-size-of-image)*60/(half-FOV-degrees)
   if (mUsePerspectiveMode)
      orthoDepth = (mAxisLength*60) / (mFovDeg/2.0);

   Real ort = orthoDepth * 8;
   Real pwr = Floor(Log10(ort));
   Real size = Exp10(pwr)/100;
   Real imax = orthoDepth/size;
   
   //------------------------------------------
   // Draw MAJOR circles
   //------------------------------------------
   //SetCelestialColor (GlOptions.EquatorialPlaneColor);
   for (int i=1; i<=(int)imax; ++i)
      if (i%10==0)
         DrawCircle(qobj, i*size);
   
   //------------------------------------------
   // Draw MINOR circles
   //------------------------------------------
   //imax = minimum (imax, 100);
   imax = GmatMathUtil::Min(imax, 100);
   //ZColor color = GlOptions.EquatorialPlaneColor;
   Real factor = (size*100)/(ort);
   //color.Red *=factor;
   //color.Green *=factor;
   //color.Blue *=factor;
   //SetCelestialColor (color)

   GLubyte ubfactor = (GLubyte)(factor * 255);
   //MessageInterface::ShowMessage("===> ubfactor=%d, factor=%f\n", ubfactor, factor);

   // Why does alpha value have no effects?
   glColor4ub(sGlColor->red, sGlColor->green, sGlColor->blue, ubfactor);
   
   for (int i=1; i<=(int)imax; ++i)
      //if (i%10!=0 && (GlOptions.DrawDarkLines || factor > 0.5))
      //if (i%10!=0 && (factor > 0.5)) // why i%10!=0? not every 10th?
      if (i%10 == 0 || (factor > 0.5))
         DrawCircle(qobj, i*size);
   
   gluDeleteQuadric(qobj);
   
   glPopMatrix();
   
} // end DrawEquatorialPlane()


//------------------------------------------------------------------------------
//  void DrawEclipticPlane(UnsignedInt color)
//------------------------------------------------------------------------------
/**
 * Draws ecliptic plane circles.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawEclipticPlane(UnsignedInt color)
{
   // First rotate the grand coordinate system to obliquity of the ecliptic
   // (23.5) and draw equatorial plane
   
   glPushMatrix();
   glRotatef(23.5, -1, 0, 0);
   DrawEquatorialPlane(color);
   glPopMatrix();
} // end DrawEclipticPlane()


//------------------------------------------------------------------------------
//  void DrawESLines(int frame)
//------------------------------------------------------------------------------
/**
 * Draws Earth Sun lines.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawESLines(int frame)
{
   
   if (frame <= 0)
      return;
   
   int earthId = GetObjectId("Earth");
   int sunId = GetObjectId("Sun");
   
   // check for Earth and Sun
   if (earthId == UNKNOWN_OBJ_ID || sunId == UNKNOWN_OBJ_ID)
      return;
   
   Real earthPos[3], sunPos[3];
   Real distance = (Real)mAxisLength;
   Real mag;

   int numSkip = frame/12; // draw 24 lines (12*2)
   if (numSkip == 0)
      numSkip = 1;
   
   
   //--------------------------------
   // draw sun lines
   //--------------------------------
   
   // set color
   *sIntColor = mESLinecolor;
   glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);
   
   glBegin(GL_LINES);
   
   //for (int i=0; i<mNumData; i+=numSkip)
   for (int i=1; i<=frame; i+=numSkip)
   {      
      earthPos[0] = -mObjectTempPos[earthId][i][0];
      earthPos[1] = -mObjectTempPos[earthId][i][1];
      earthPos[2] =  mObjectTempPos[earthId][i][2];
      
      sunPos[0] = -mObjectTempPos[sunId][i][0];
      sunPos[1] = -mObjectTempPos[sunId][i][1];
      sunPos[2] =  mObjectTempPos[sunId][i][2];
      
      if (mOriginName == "Earth")
      {
         // show lines beyond the Sun
         mag = sqrt(sunPos[0]*sunPos[0] + sunPos[1]*sunPos[1] +
                    sunPos[2]*sunPos[2]);
         glVertex3f(sunPos[0]/mag*distance, sunPos[1]/mag*distance,
                    sunPos[2]/mag*distance);
         glVertex3f(-sunPos[0]/mag*distance, -sunPos[1]/mag*distance,
                    -sunPos[2]/mag*distance);
      }
      else if (mOriginName == "Sun")
      {
         // show lines beyond the Earth
         mag = sqrt(earthPos[0]*earthPos[0] + earthPos[1]*earthPos[1] +
                    earthPos[2]*earthPos[2]);
         glVertex3f(earthPos[0]/mag*distance, earthPos[1]/mag*distance,
                    earthPos[2]/mag*distance);
         glVertex3f(-earthPos[0]/mag*distance, -earthPos[1]/mag*distance,
                    -earthPos[2]/mag*distance);
      }
      else
      {
         // show lines between Sun and Earth and to -Sun
         glVertex3f(earthPos[0], earthPos[1], earthPos[2]);
         glVertex3f(sunPos[0], sunPos[1], sunPos[2]);
         glVertex3f(earthPos[0], earthPos[1], earthPos[2]);
         glVertex3f(-sunPos[0], -sunPos[1], -sunPos[2]);
      }
      
   }
   
   glEnd();
   
   // Show Sun direction text
   glColor3f(1.0, 1.0, 0.0); // yellow
   
   // get sun unit vector and multiply by distance
   mag = sqrt(sunPos[0]*sunPos[0] + sunPos[1]*sunPos[1] + sunPos[2]*sunPos[2]);
   DrawStringAt(" +S", sunPos[0]/mag*distance/2.2,
                sunPos[1]/mag*distance/2.2,
                sunPos[2]/mag*distance/2.2);
   
} // end DrawESLines()


//---------------------------------------------------------------------------
// void DrawAxes()
//---------------------------------------------------------------------------
void TrajPlotCanvas::DrawAxes()
{
   wxString axisLabel;
   GLfloat viewDist;

   glLineWidth(2.0); //loj: 11/2/05 Fix for L57
   
   //-----------------------------------
   // draw axes
   //-----------------------------------
   
   //viewDist = mCurrViewDist/2; //zooms in and out
   viewDist = mAxisLength/2.2; // stays the same
   glBegin(GL_LINES);
   
   glColor3f(0, 1, 0);   // x
   glVertex3f(-viewDist, 0, 0);
   glVertex3f( viewDist, 0, 0);
   
   glColor3f(0, 0, 1);   // y
   glVertex3f(0, -viewDist, 0);
   glVertex3f(0,  viewDist, 0);
   
   glColor3f(1, 1, 0);   // z
   glVertex3f(0, 0, -viewDist);
   glVertex3f(0, 0, viewDist);
   
   glEnd();
   
   //-----------------------------------
   // throw some text out...
   //-----------------------------------
   glColor3f(0, 1, 0);   // green
   axisLabel = "+X " + mViewCoordSysName;
   DrawStringAt(axisLabel, -viewDist, 0.0, 0.0);
   
   glColor3f(0, 0, 1);   // blue
   axisLabel = "+Y " + mViewCoordSysName;
   DrawStringAt(axisLabel, 0.0, -viewDist, 0.0);
   
   glColor3f(1, 1, 0);   // yellow
   axisLabel = "+Z " + mViewCoordSysName;
   DrawStringAt(axisLabel, 0.0, 0.0, viewDist);
   
   glLineWidth(1.0);
   
}


//---------------------------------------------------------------------------
// void DrawStatus(const wxString &label, int frame, double time)
//---------------------------------------------------------------------------
void TrajPlotCanvas::DrawStatus(const wxString &label, int frame, double time)
{
   //----------------------------------------------------
   // draw current frame number and time
   //----------------------------------------------------
   //loj: 5/23/05 I want to use glWindowPos2f but it is available in version 1.4
   // then I'll not need to set GL_PROJECTION mode
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0, (GLfloat)mCanvasSize.x, 0.0, (GLfloat)mCanvasSize.y);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   wxString str;
   wxString text;
   str.Printf("%d", frame);
   text = label + str;
   str.Printf("%f", time);
   text = text + "  Time: " + str;
   
   glColor3f(1, 1, 0); //yellow
   glRasterPos2i(0, 0);
   glCallLists(strlen(text.c_str()), GL_BYTE, (GLubyte*)text.c_str());
   
   //wxLogStatus(GmatAppData::GetMainFrame(), wxT("Frame#: %d, Time: %f"), frame,
   //            mTime[frame]);
   
}


//---------------------------------------------------------------------------
// void ApplyEulerAngles()
//---------------------------------------------------------------------------
void TrajPlotCanvas::ApplyEulerAngles()
{
   if (mRotateAboutXaxis)
   {
      glRotatef(mfCamRotYAngle, 0.0, 1.0, 0.0);
      glRotatef(mfCamRotZAngle, 0.0, 0.0, 1.0);
      glRotatef(mfCamRotXAngle, 1.0, 0.0, 0.0);
   }
   else if (mRotateAboutYaxis)
   {
      glRotatef(mfCamRotZAngle, 0.0, 0.0, 1.0);
      glRotatef(mfCamRotXAngle, 1.0, 0.0, 0.0);
      glRotatef(mfCamRotYAngle, 0.0, 1.0, 0.0);
   }
   else
   {
      glRotatef(mfCamRotXAngle, 1.0, 0.0, 0.0);
      glRotatef(mfCamRotYAngle, 0.0, 1.0, 0.0);
      glRotatef(mfCamRotZAngle, 0.0, 0.0, 1.0);
   }
}


//---------------------------------------------------------------------------
// void DrawStringAt(const wxString &str, GLfloat x, GLfloat y, GLfloat z)
//---------------------------------------------------------------------------
void TrajPlotCanvas::DrawStringAt(const wxString &str, GLfloat x, GLfloat y,
                                  GLfloat z)
{
   glRasterPos3f(x, y, z);
   glCallLists(strlen(str.c_str()), GL_BYTE, (GLubyte*)str.c_str());
}


//---------------------------------------------------------------------------
// void DrawCircle(GLUquadricObj *qobj, Real radius)
//---------------------------------------------------------------------------
void TrajPlotCanvas::DrawCircle(GLUquadricObj *qobj, Real radius)
{
   gluQuadricDrawStyle(qobj, GLU_LINE  );
   gluQuadricNormals  (qobj, GLU_SMOOTH);
   gluQuadricTexture  (qobj, GL_FALSE  );
   gluDisk(qobj, radius, radius, 50, 1);
}


//---------------------------------------------------------------------------
// int GetObjectId(const wxString &name)
//---------------------------------------------------------------------------
int TrajPlotCanvas::GetObjectId(const wxString &name)
{
   for (int i=0; i<mObjectCount; i++)
      if (mObjectNames[i] == name)
         return i;

   #if DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::GetObjectId() obj name: " + name +
       " not found in the object list\n");
   #endif
   
   return UNKNOWN_OBJ_ID;
}


//---------------------------------------------------------------------------
// bool TiltOriginZAxis()
//---------------------------------------------------------------------------
bool TrajPlotCanvas::TiltOriginZAxis()
{
   if (mNumData == 0)
      return false;
   
   if (mInternalCoordSystem == NULL || mViewCoordSystem == NULL)
      return false;

   std::string axisTypeName =
      mViewCoordSystem->GetRefObject(Gmat::AXIS_SYSTEM, "")->GetTypeName();
   
   #if DEBUG_TRAJCANVAS_DRAW
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::TiltOriginZAxis() AxisTypeName=%s\n", axisTypeName.c_str());
   #endif
   
   // rotate earth Z axis if view CS is EarthMJ2000Ec
   if (mViewCoordSystem->GetName() == "EarthMJ2000Ec")
   {
      Rvector6 inState, outState;
      
      inState.Set(0.0, 0.0, 1.0, 0.0, 0.0, 0.0);
      
      mCoordConverter.Convert(mTime[0], inState, mInternalCoordSystem,
                              outState, mViewCoordSystem);
      
      #if DEBUG_TRAJCANVAS_DRAW > 2
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::TiltOriginZAxis() in=%g, %g, %g, out=%g, %g, %g\n",
             inState[0], inState[1], inState[2], outState[0], outState[1], outState[2]);
         Rvector3 vecA(inState[0], inState[1], inState[2]);
         Rvector3 vecB(outState[0], outState[1], outState[2]);
         Real angDeg = AngleUtil::ComputeAngleInDeg(vecA, vecB);
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::TiltOriginZAxis() angDeg=%g\n", angDeg);
         //outState = 0, 0.397777, 0.917482
         //angDeg = 23.4393
      #endif
         
      // convert outState to rotation angle
      // How???
      
      // rotate Earth Z axis
      glRotatef(23.5, 1.0, 0.0, 0.0);
   }
   
   return true;
}


//---------------------------------------------------------------------------
// bool ConvertObjectData()
//---------------------------------------------------------------------------
bool TrajPlotCanvas::ConvertObjectData()
{
   if (mInternalCoordSystem == NULL || mViewCoordSystem == NULL)
      return false;
   
   Rvector6 inState, outState;
   
   #if DEBUG_TRAJCANVAS_CONVERT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::ConvertObjectData() internalCS=%s, viewCSName=%s, viewCS=%d\n",
       mInternalCoordSystem->GetName().c_str(), mViewCoordSystem->GetName().c_str(),
       mViewCoordSystem);
   #endif
   
   // do not convert if view CS is internal CS
   if (mIsInternalCoordSystem)
   {
      #if DEBUG_TRAJCANVAS_CONVERT
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::ConvertObjectData() No conversion is needed. "
          "Just copy MJ2000 pos\n");
      #endif
      for (int i=0; i<mObjectCount; i++)
      {
         int objId = GetObjectId(mObjectNames[i]);
         
         for (int i=0; i<mNumData; i++)
            CopyVector3(mObjectTempPos[objId][i], mObjectGciPos[objId][i]);
      }
   }
   else if (mViewCoordSysName == mInitialCoordSysName)
   {
      #if DEBUG_TRAJCANVAS_CONVERT
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::ConvertObjectData() No conversion is needed. "
          "Just copy Initial View CS pos\n");
      #endif
      for (int i=0; i<mObjectCount; i++)
      {
         int objId = GetObjectId(mObjectNames[i]);
         
         for (int i=0; i<mNumData; i++)
            CopyVector3(mObjectTempPos[objId][i], mObjectInitialPos[objId][i]);
      }
   }
   else
   {
      for (int i=0; i<mObjectCount; i++)
      {
         int objId = GetObjectId(mObjectNames[i]);
         
         #if DEBUG_TRAJCANVAS_CONVERT
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::ConvertObjectData() mObjectNames[%d]=%s\n", objId,
             mObjectNames[i].c_str());
         #endif
         
         for (int i=0; i<mNumData; i++)
            ConvertObject(objId, i);              
      }
   }
   
   return true;
} //ConvertObjectData()


//---------------------------------------------------------------------------
// void ConvertObject(int objId, int index)
//---------------------------------------------------------------------------
void TrajPlotCanvas::ConvertObject(int objId, int index)
{
   Rvector6 inState, outState;
   
   inState.Set(mObjectGciPos[objId][index][0], mObjectGciPos[objId][index][1],
               mObjectGciPos[objId][index][2], mObjectGciVel[objId][index][0],
               mObjectGciVel[objId][index][1], mObjectGciVel[objId][index][2]);
   
   mCoordConverter.Convert(mTime[index], inState, mInternalCoordSystem,
                           outState, mViewCoordSystem);
   
   mObjectTempPos[objId][index][0] = outState[0];
   mObjectTempPos[objId][index][1] = outState[1];
   mObjectTempPos[objId][index][2] = outState[2];
   
   mObjectTempVel[objId][index][0] = outState[3];
   mObjectTempVel[objId][index][1] = outState[4];
   mObjectTempVel[objId][index][2] = outState[5];
   
   #if DEBUG_TRAJCANVAS_CONVERT
   if (index < 10)
   {
      MessageInterface::ShowMessage
         ("   index=%d, inState=%16.9f, %16.9f, %16.9f\n"
          "           outState=%16.9f, %16.9f, %16.9f\n", index, inState[0],
          inState[1], inState[2], outState[0], outState[1], outState[2]);
   }
   #endif
}


//---------------------------------------------------------------------------
// void UpdateRotateFlags()
//---------------------------------------------------------------------------
void TrajPlotCanvas::UpdateRotateFlags()
{
   AxisSystem *axis =
      (AxisSystem*)mViewCoordSystem->GetRefObject(Gmat::AXIS_SYSTEM, "");

   if (axis->IsOfType("BodyFixedAxes") &&
       (mOriginName.IsSameAs(axis->GetStringParameter("Origin").c_str())))
   {
      mCanRotateBody = false;
      mCanRotateAxes = false;
   }
   else if (axis->IsOfType("InertialAxes"))
   {
      mCanRotateBody = true;
      mCanRotateAxes = false;
   }
   else
   {
      mCanRotateBody = false;
      mCanRotateAxes = false;
   }
   
   
   #if DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::UpdateRotateFlags() mCanRotateBody=%d, "
       "mCanRotateAxes=%d\n", mCanRotateBody, mCanRotateAxes);
   #endif
}


//------------------------------------------------------------------------------
// void MakeValidCoordSysList()
//------------------------------------------------------------------------------
/*
 * Make a list of coordinate system that can be converted from and to.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::MakeValidCoordSysList()
{
   StringArray csList =
      theGuiInterpreter->GetListOfConfiguredItems(Gmat::COORDINATE_SYSTEM);
   CoordinateSystem *cs;
   SpacePoint *sp;
   wxString csName;
   wxString origin;
   wxString primary;
   wxString secondary;
   
   mValidCSNames.Empty();
   
   for (unsigned int i=0; i<csList.size(); i++)
   {
      origin = "None";
      primary = "None";
      secondary = "None";
      
      cs = theGuiInterpreter->GetCoordinateSystem(csList[i]);
      csName = cs->GetName().c_str();
      
      //MessageInterface::ShowMessage("==> csName=%s\n", csName.c_str());
      
      sp = cs->GetOrigin();
      // cannot convert to CS with spacecraft as origin
      if (sp->IsOfType("Spacecraft"))
         continue;
      else
         origin = cs->GetOriginName().c_str();
      
      sp = cs->GetPrimaryObject();
      if (sp != NULL)
      {
         // cannot convert to CS with spacecraft as primary
         if (sp->IsOfType("Spacecraft"))
            continue;
         
         primary = sp->GetName().c_str();
      }
      
      sp = cs->GetSecondaryObject();
      if (sp != NULL)
      {
         // cannot convert to CS with spacecraft as secondary
         if (sp->IsOfType("Spacecraft"))
            continue;
         
         secondary = sp->GetName().c_str();
      }

      //MessageInterface::ShowMessage
      //   ("==> origin=%s, primary=%s, secondary=%s\n",
      //    origin.c_str(), primary.c_str(), secondary.c_str());

      if (origin == "None")
         continue;
      
      if (GetObjectId(origin) != UNKNOWN_OBJ_ID)
      {
         if (primary == "None" && secondary == "None")
         {
            mValidCSNames.Add(csName);
            continue;
         }

         if (primary != "None" && GetObjectId(primary) != UNKNOWN_OBJ_ID)
         {
            if (secondary == "None")
            {
               mValidCSNames.Add(csName);
               continue;
            }
            else if (GetObjectId(secondary) != UNKNOWN_OBJ_ID)
            {
               mValidCSNames.Add(csName);
               continue;               
            }
         }
      }
   }

   //for (unsigned int i=0; i<mValidCSNames.GetCount(); i++)
   //{
   //   MessageInterface::ShowMessage("==> validCSName=%s\n", mValidCSNames[i].c_str());
   //}
}


//---------------------------------------------------------------------------
// Rvector3 ComputeEulerAngles()
//---------------------------------------------------------------------------
Rvector3 TrajPlotCanvas::ComputeEulerAngles()
{
   Rvector3 modAngle;

   #ifndef COMPUTE_EULER_ANGLE
   return modAngle;

   
   #else
   
   bool error = false;
   Rvector3 eulerAngle;
   Rmatrix33 finalMat;

   #ifdef USE_MODELVIEW_MAT
   //if (mUseGluLookAt)
   {   
      // model view matrix
      static GLfloat sProjMat[16];
      static GLfloat sViewMat[16];
      
      glGetFloatv(GL_PROJECTION_MATRIX, sProjMat);
      glGetFloatv(GL_MODELVIEW_MATRIX, sViewMat);
      
      // OpenGL stores matrix in column major: ith column, jth row.
      
      Rmatrix33 pjmat(sProjMat[0], sProjMat[1], sProjMat[2],
                      sProjMat[4], sProjMat[5], sProjMat[6],
                      sProjMat[8], sProjMat[9], sProjMat[10]);
      
      Rmatrix33 mvmat(sViewMat[0], sViewMat[1], sViewMat[2],
                      sViewMat[4], sViewMat[5], sViewMat[6],
                      sViewMat[8], sViewMat[9], sViewMat[10]);
      
      //org:finalMat = pjmat * mvmat;
      //finalMat = mvmat * pjmat;
      finalMat = mvmat;
      
            
      #ifdef DEBUG_TRAJCANVAS_EULER
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::ComputeEulerAngles() sViewMat=\n"
          "   %f, %f, %f, %f\n   %f, %f, %f, %f\n"
          "   %f, %f, %f, %f\n   %f, %f, %f, %f\n",
          sViewMat[0], sViewMat[1], sViewMat[2], sViewMat[3],
          sViewMat[4], sViewMat[5], sViewMat[6], sViewMat[7],
          sViewMat[8], sViewMat[9], sViewMat[10], sViewMat[11],
          sViewMat[12], sViewMat[13], sViewMat[14], sViewMat[15]);
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::ComputeEulerAngles() pjmat=%s\n",
          pjmat.ToString().c_str());
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::ComputeEulerAngles() mvmat=%s\n",
          mvmat.ToString().c_str());
       MessageInterface::ShowMessage
          ("TrajPlotCanvas::ComputeEulerAngles() finalMat=%s\n",
           finalMat.ToString().c_str());
      #endif
   }
   #else
   //else
   {
   
      Rvector3 upAxis((Real)mfUpXAxis, (Real)mfUpYAxis, (Real)mfUpZAxis);
      Rvector3 rotAxis = Rvector3((Real)mfCamRotXAxis, (Real)mfCamRotYAxis,
                                  (Real)mfCamRotZAxis);

      #ifdef DEBUG_TRAJCANVAS_EULER
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::ComputeEulerAngles() mfUpAngle=%f, upAxis=%s\n",
          mfUpAngle, upAxis.ToString().c_str());
   
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::ComputeEulerAngles() mfCamSingleRotAngle=%f, rotAxis=%s\n",
          mfCamSingleRotAngle, rotAxis.ToString().c_str());
      #endif
      
      Rmatrix33 upMat, rotMat;
      
      try
      {
         // compute cosine matrix
         if (upAxis.GetMagnitude() > 0.0)
            upMat = GmatAttUtil::ToCosineMatrix(mfUpAngle * RAD_PER_DEG, upAxis);
         
         if (rotAxis.GetMagnitude() > 0.0)
            rotMat = GmatAttUtil::ToCosineMatrix(mfCamSingleRotAngle * RAD_PER_DEG, rotAxis);
         
         //finalMat = rotMat * upMat;
         finalMat = upMat * rotMat;
         
         #ifdef DEBUG_TRAJCANVAS_EULER
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::ComputeEulerAngles() \n  rotMat=%s  upMat=%s  "
             "finalMat=%s\n", rotMat.ToString().c_str(),
             upMat.ToString().c_str(), finalMat.ToString().c_str());
         #endif
         
      }
      catch (BaseException &e)
      {
         error = true;
         MessageInterface::ShowMessage
            ("*** ERROR *** TrajPlotCanvas::ComputeEulerAngles() %s\n",
             e.GetMessage().c_str());
      }
   }
   #endif
   
   if (error)
      return modAngle;

   
   try
   {
      // convert finalMat to Euler angle
      if (mRotateAboutXaxis)
         eulerAngle = GmatAttUtil::ToEulerAngles(finalMat, 2, 3, 1);
      else if (mRotateAboutYaxis)
         eulerAngle = GmatAttUtil::ToEulerAngles(finalMat, 3, 1, 2);
      else
         eulerAngle = GmatAttUtil::ToEulerAngles(finalMat, 1, 2, 3);
            
      eulerAngle = eulerAngle * DEG_PER_RAD;
      
      #ifdef DEBUG_TRAJCANVAS_EULER
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::ComputeEulerAngles() eulerAngle=%s\n",
          eulerAngle.ToString().c_str());
      #endif
      
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("*** ERROR *** TrajPlotCanvas::ComputeEulerAngles() %s\n",
          e.GetMessage().c_str());
   }
   
   modAngle = eulerAngle;
   
   //loj: 9/29/05
   // How can I compute modified rotation angle in general way?
   
   if (eulerAngle.GetMagnitude() == 0.0)
   {
      if (mRotateAboutXaxis) // 2-3-1 (Default)
      {
         modAngle[0] = -90 - 270;
         modAngle[1] = 90;
         modAngle[2] = 0;
      }
   }
   else
   {
      if (mRotateAboutXaxis) // 2-3-1 (Default)
      {
         if (eulerAngle(2) == 0.0)
         {
            if (mUseGluLookAt)
            {
               modAngle[0] = eulerAngle(0) + 90;
               modAngle[1] = eulerAngle(2);
               modAngle[2] = eulerAngle(1) + 180;
            }
            else
            {
               modAngle[0] = eulerAngle(0) - 270;
               modAngle[1] = eulerAngle(2);
               modAngle[2] = eulerAngle(1);
            }
         }
         else
         {
            if (mUseGluLookAt)
            {
               modAngle[0] = eulerAngle(0);
               modAngle[1] = eulerAngle(2) - 180;;
               modAngle[2] = eulerAngle(1);
            }
            else
            {
               modAngle[0] = eulerAngle(0);
               modAngle[1] = eulerAngle(2) - 90;
               modAngle[2] = eulerAngle(1);
            }
         }
      }
      else if (mRotateAboutZaxis) // 1-2-3
      {
         if (eulerAngle(2) == 0.0)
         {
            modAngle[0] = eulerAngle(0);
            modAngle[1] = eulerAngle(1) - 90;
            modAngle[2] = eulerAngle(2) + 90;
         }
         else
         {
            modAngle[0] = eulerAngle(0) - 180;
            modAngle[1] = eulerAngle(1);
            modAngle[2] = eulerAngle(2) - 90;
         }
      }
   }
   
   #ifdef DEBUG_TRAJCANVAS_EULER
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::ComputeEulerAngles() modAngle=%s\n",
       modAngle.ToString().c_str());
   #endif

   return modAngle;

   #endif
}


//---------------------------------------------------------------------------
// void ComputeLongitudeLst(Real time, Real x, Real y, Real *meanHourAngle,
//                          Real *longitude, Real *localSiderealTime)
//---------------------------------------------------------------------------
void TrajPlotCanvas::ComputeLongitudeLst(Real time, Real x, Real y,
                                         Real *meanHourAngle, Real *longitude,
                                         Real *localSiderealTime)
{
   Real mha = 0.0;
   Real lon = 0.0;
   Real lst = 0.0;
   *meanHourAngle = mha;
   *longitude = lon;
   *localSiderealTime = lst;

   if (mViewObjName != "Earth")
      return;
   
   // compute longitude of the first spacecraft
   if (mSolarSystem)
   {
      Real raRad = ATan(y, x);
      Real raDeg = RadToDeg(raRad, true);
      CelestialBody *earth = mSolarSystem->GetBody("Earth");
      if (earth)
         mha = earth->GetHourAngle(time);
      
      lon = raDeg - mha;
      lon = AngleUtil::PutAngleInDegRange(lon, 0.0, 360.0);
   }
   
   lst = mha + lon;
   lst = AngleUtil::PutAngleInDegRange(lst, 0.0, 360.0);
   *meanHourAngle = mha;
   *longitude = lon;
   *localSiderealTime = lst;
}


//---------------------------------------------------------------------------
//  void CopyVector3(Real to[3], Real from[3])
//---------------------------------------------------------------------------
void TrajPlotCanvas::CopyVector3(Real to[3], Real from[3])
{
   to[0] = from[0];
   to[1] = from[1];
   to[2] = from[2];
}


//---------------------------------------------------------------------------
// bool LoadImage(char *fileName)
//---------------------------------------------------------------------------
bool TrajPlotCanvas::LoadImage(char *fileName)
{
#ifndef SKIP_DEVIL
   return false;
   
#else
   #if DEBUG_TRAJCANVAS_INIT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::LoadImage() Not using DevIL. file=%s\n", fileName);
   #endif
   
   ::wxInitAllImageHandlers();
   wxImage image = wxImage(fileName);
   int width = image.GetWidth();
   int height = image.GetHeight();
   
   GLubyte *data = image.GetData();
   
   if (data == NULL)
      return false;
   
   #if DEBUG_TRAJCANVAS_INIT
   int size = width * height * 3;
   MessageInterface::ShowMessage("===> width=%d, height=%d, size=%d\n",
                                 width, height, size);
   #endif
   
   // Why is image upside down?
   // Get vertial mirror
   wxImage mirror = image.Mirror(false);
   GLubyte *data1 = mirror.GetData();
   
   //used for min and magnifying texture
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   
   //pass image to opengl
   gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB,
                     GL_UNSIGNED_BYTE, data1);
   
   return true;
#endif
}

