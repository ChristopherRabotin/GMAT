//$Id$
//------------------------------------------------------------------------------
//                              TrajPlotCanvas
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include "GmatAppData.hpp"         // for GetGuiInterpreter()
#include "FileManager.hpp"         // for texture files
#include "ColorTypes.hpp"          // for namespace GmatColor::
#include "Rvector3.hpp"            // for Rvector3::GetMagnitude()
#include "GmatConstants.hpp"
#include "AngleUtil.hpp"           // for ComputeAngleInDeg()
#include "CelestialBody.hpp"
#include "MdiGlPlotData.hpp"
#include "MessageInterface.hpp"
#include "SubscriberException.hpp"
#include "TimeSystemConverter.hpp" // for Convert()
#include "GmatDefaults.hpp"
#include <string.h>                // for strlen()

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

// define SKIP_DEVIL if devil is not supported
//#define SKIP_DEVIL
#ifndef SKIP_DEVIL
// Include without IL/ so different versions of IL can be used without modifying
// the directory
// On Linux, just use the installed versions of DevIL
#ifdef __WXGTK__
#define ILUT_USE_OPENGL
#  include <IL/il.h>
#  include <IL/ilu.h>
#  include <IL/ilut.h>
#else
#  include <il.h>
#  include <ilu.h>
#  include <ilut.h>
#endif

#endif


// always define USE_TRACKBALL, it works better for rotation
#define USE_TRACKBALL
#ifdef USE_TRACKBALL
#include "AttitudeUtil.hpp"
using namespace FloatAttUtil;
#endif


// currently lighting is not working
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
//#define COMPUTE_EULER_ANGLE
#ifdef COMPUTE_EULER_ANGLE
#include "AttitudeUtil.hpp"
//#define USE_MODELVIEW_MAT
//#define DEBUG_TRAJCANVAS_EULER 1
#endif

// For the newer (wx 2.7.x+) method, create a wxGLCanvas window using the
// constructor that does not create an implicit rendering context, create
// an explicit instance of a wxGLContext that is initialized with the
// wxGLCanvas yourself (highly recommended for future compatibility with
// wxWidgets and the flexibility of your own program!) then use either
// wxGLCanvas::SetCurrent with the instance of the wxGLContext or
// wxGLContext::SetCurrent with the instance of the wxGLCanvas to bind the
// OpenGL state that is represented by the rendering context to the canvas,
// and then call wxGLCanvas::SwapBuffers to swap the buffers of the OpenGL
// canvas and thus show your current output.
// This still not working so commented out. But there is a problem with
// showing object and status line in the wx 2.8.4 using implicit GLContext
//#define __USE_WX280_GL__

// skip over limit data
//#define SKIP_OVER_LIMIT_DATA

// debug
//#define DEBUG_TRAJCANVAS_INIT 1
//#define DEBUG_TRAJCANVAS_UPDATE 1
//#define DEBUG_TRAJCANVAS_UPDATE_OBJECT 2
//#define DEBUG_TRAJCANVAS_ACTION 1
//#define DEBUG_TRAJCANVAS_CONVERT 1
//#define DEBUG_TRAJCANVAS_DRAW 2
//#define DEBUG_TRAJCANVAS_ZOOM 1
//#define DEBUG_TRAJCANVAS_OBJECT 2
//#define DEBUG_TRAJCANVAS_TEXTURE 2
//#define DEBUG_TRAJCANVAS_PERSPECTIVE 1
//#define DEBUG_TRAJCANVAS_PROJ 1
//#define DEBUG_TRAJCANVAS_CS 1
//#define DEBUG_ANIMATION 1
//#define DEBUG_TRAJCANVAS_LONGITUDE 1
//#define DEBUG_SHOW_SKIP 1
//#define DEBUG_ROTATE 1
//#define DEBUG_DATA_BUFFERRING
//#define DEBUG_ORBIT_LINES

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
const int TrajPlotCanvas::LAST_STD_BODY_ID = 10;
const int TrajPlotCanvas::MAX_COORD_SYS = 10;
const float TrajPlotCanvas::MAX_ZOOM_IN = 3700.0f;
const float TrajPlotCanvas::RADIUS_ZOOM_RATIO = 2.2f;
const float TrajPlotCanvas::DEFAULT_DIST = -30000.0f;
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
// TrajPlotCanvas(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <parent>   parent window pointer
 * @param <id>       window id
 * @param <pos>      position (top, left) where the window to be placed 
 * @param <size>     size of the window
 * @param <name>     title of window
 * @param <style>    style of window
 *
 */
//------------------------------------------------------------------------------
TrajPlotCanvas::TrajPlotCanvas(wxWindow *parent, wxWindowID id,
                               const wxPoint& pos, const wxSize& size,
                               const wxString& name, long style)
   : ViewCanvas(parent, id, pos, size, name, style)
{
   mGlInitialized = false;
   mPlotName = name;
   mParent = parent;
   
   // Linux specific
   #ifdef __WXGTK__
      hasBeenPainted = false;
   #endif
   
   #if DEBUG_TRAJCANVAS_INIT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas() name=%s, size.X=%d, size.Y=%d\n",
       name.c_str(), size.GetWidth(), size.GetHeight());
   #endif
   
   #ifdef __USE_WX280_GL__
   // Note:
   // Use wxGLCanvas::m_glContext, otherwise resize will not work
   m_glContext = new wxGLContext(this);
   #endif
   
   // initalize data members
   GmatAppData *gmatAppData = GmatAppData::Instance();   
   theGuiInterpreter = gmatAppData->GetGuiInterpreter();
   theStatusBar = gmatAppData->GetMainFrame()->GetStatusBar();
   mTextTrajFile = NULL;
   mGlList = 0;
   mIsFirstRun = true;
   
   ResetPlotInfo();
   
   // projection
   mUsePerspectiveMode = false;
   mUseInitialViewPoint = true;
   
   // viewpoint
   InitializeViewPoint();
   
   // devault view
   mCanvasSize = size;
   mDefaultRotXAngle = 90.0;
   mDefaultRotYAngle = 0.0;
   mDefaultRotZAngle = 0.0;
   mDefaultViewDist = DEFAULT_DIST;
   
   // view model
   mUseGluLookAt = true;
   mUseSingleRotAngle = true;
   
   // performance
   // if mNumPointsToRedraw =  0 It redraws whole plot
   // if mNumPointsToRedraw = -1 It does not clear GL_COLOR_BUFFER
   mNumPointsToRedraw = 0;
   mRedrawLastPointsOnly = false;
   mUpdateFrequency = 50;
   
   //mAxisLength = mCurrViewDist;
   mAxisLength = DEFAULT_DIST;
   
   mOriginName = "";
   mOriginId = 0;
   
   //original value
   mRotateAboutXaxis = true;   // 2-3-1
   mRotateAboutYaxis = false;  // 3-1-2
   mRotateAboutZaxis = false;  // 1-2-3
   
   //mRotateAboutXaxis = false;   // 2-3-1
   //mRotateAboutYaxis = false ;  // 3-1-2
   //mRotateAboutZaxis = true;    // 1-2-3
   
   mRotateXy = true;
   mZoomAmount = 300.0;
   
   // projection
   ChangeProjection(size.x, size.y, mAxisLength);
   
//   mEarthRadius = 6378.14f; //km
   // @todo - does this need a pointer to the actual Earth object, to get radius? (mEarthRadius does not
   // appear to be used, though
   mEarthRadius = (float) GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH]; //km
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
   mSunLineColor = GmatColor::GOLDTAN;
   
   // animation
   mIsAnimationRunning = false;
   mHasUserInterrupted = false;
   mUpdateInterval = 1;
   mFrameInc = 1;
   
   // solver data
   mDrawSolverData = false;
   
   // message
   mShowMaxWarning = true;
   mOverCounter = 0;
   
   // solar system
   pSolarSystem = NULL;
   //MessageInterface::ShowMessage
   //   ("==> TrajPlotCanvas::TrajPlotCanvas() pSolarSystem=%p\n", pSolarSystem);
   
   // objects
   mObjectDefaultRadius = 200; //km: make big enough to see

   // Initialize arrays to NULL
   ClearObjectArrays(false);
   
   // Zoom
   mMaxZoomIn = MAX_ZOOM_IN;
   
   // Spacecraft
   mScCount = 0;
   
   // Coordinate System
   pInternalCoordSystem = theGuiInterpreter->GetInternalCoordinateSystem();
   mInternalCoordSysName = wxString(pInternalCoordSystem->GetName().c_str());
   
   mViewCoordSysName = "";
   pViewCoordSystem = NULL;
   
   // CoordinateSystem conversion
   mViewCsIsInternalCs = true;
   
   #if DEBUG_TRAJCANVAS_INIT
   MessageInterface::ShowMessage
      ("   pInternalCoordSystem=%p, pViewCoordSystem=%p\n", pInternalCoordSystem,
       pViewCoordSystem);
   if (pInternalCoordSystem)
      MessageInterface::ShowMessage
         ("   pInternalCoordSystem=%s\n", pInternalCoordSystem->GetName().c_str());
   if (pViewCoordSystem)
      MessageInterface::ShowMessage
         ("   pViewCoordSystem=%s\n", pViewCoordSystem->GetName().c_str());
   MessageInterface::ShowMessage("TrajPlotCanvas() constructor exiting\n");
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
   #ifdef DEBUG_RESOURCE_CLEARING
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::~TrajPlotCanvas() '%s' entered\n", mPlotName.c_str());
   #endif
   
   if (mTextTrajFile)
      delete mTextTrajFile;
   
   // Note:
   // deleting m_glContext is handled in wxGLCanvas
   
   ClearObjectArrays();
   
   #ifdef DEBUG_RESOURCE_CLEARING
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::~TrajPlotCanvas() '%s' exiting\n", mPlotName.c_str());
   #endif
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
   // get GL version
   #ifdef __GET_GL_INFO__
   const GLubyte *str = glGetString(GL_VENDOR);
   MessageInterface::ShowMessage("GL vendor = '%s'\n", (char*)str);
   str = glGetString(GL_VERSION);
   MessageInterface::ShowMessage("GL version = '%s'\n", (char*)str);
   str = glGetString(GL_EXTENSIONS);
   MessageInterface::ShowMessage("GL extensions = '%s'\n", (char*)str);
   str = gluGetString(GLU_VERSION);
   MessageInterface::ShowMessage("GLU version = '%s'\n", (char*)str);
   str = gluGetString(GLU_EXTENSIONS);
   MessageInterface::ShowMessage("GLU extensions = '%s'\n", (char*)str);
   #endif
   
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
   {
      //throw SubscriberException("SetPixelFormatDescriptor failed\n");
   }
   
   // font
   SetDefaultGLFont();

   mShowMaxWarning = true;
   mIsAnimationRunning = false;
   
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
// wxGLContext* GetGLContext()
//------------------------------------------------------------------------------
/*
 * Return current GLContext pointer.
 */
//------------------------------------------------------------------------------
wxGLContext* TrajPlotCanvas::GetGLContext()
{
   return m_glContext;
}


//------------------------------------------------------------------------------
// void SetEndOfRun(bool flag = true)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetEndOfRun(bool flag)
{
   #if DEBUG_TRAJCANVAS_UPDATE
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::SetEndOfRun() TrajPlotCanvas::SetEndOfRun() flag=%d, "
       "mNumData=%d\n",  flag, mNumData);
   #endif
   
   mIsEndOfRun = flag;
   mIsEndOfData = flag;
   
   if (mNumData < 1)
   {
      Refresh(false);
      return;
   }
   
   if (mIsEndOfRun)
   {
      #if DEBUG_TRAJCANVAS_LONGITUDE
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::SetEndOfRun() mIsEndOfRun=%d, mNumData=%d\n",
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
      
      //int index = objId * MAX_DATA * 3 + (mNumData-1) * 3;
      int index = objId * MAX_DATA * 3 + mLastIndex * 3;
      //Real time = mTime[mNumData-1];
      Real time = mTime[mLastIndex];
      Real x = mObjectViewPos[index+0];
      Real y = mObjectViewPos[index+1];;

      // Dunn notes the variable "longitude" below is declared elsewhere in this
      // file.  Even if the other "longitude" is protected, it should probably
      // have a different name.  This is the FIRST place longitude is declared.
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
// void SetGLContext(wxGLContext *glContext)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetGLContext(wxGLContext *glContext)
{
   #ifdef __USE_WX280_GL__
   if (glContext == NULL)
      SetCurrent(*m_glContext);
   else
      SetCurrent(*glContext);
   #else
   SetCurrent();
   #endif
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
   
   // In wxWidgets-2.8.4, this shows previous plot
   #ifndef __USE_WX280_GL__
   SwapBuffers();
   #endif
}


//------------------------------------------------------------------------------
// void ResetPlotInfo()
//------------------------------------------------------------------------------
/**
 * Resets ploting infomation.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::ResetPlotInfo()
{
   mNumData = 0;
   mTotalPoints = 0;
   mCurrIndex = -1;
   mBeginIndex1 = 0;
   mBeginIndex2 = -1;
   mEndIndex1 = -1;
   mEndIndex2 = -1;
   mRealBeginIndex1 = 0;
   mRealBeginIndex2 = -1;
   mRealEndIndex1 = -1;
   mRealEndIndex2 = -1;
   mLastIndex = 0;
   mOverCounter = 0;
   mIsEndOfData = false;
   mIsEndOfRun = false;
   mWriteWarning = true;
   
   // Initialize view
   if (mUseInitialViewPoint)
      SetDefaultView();
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
   #ifdef DEBUG_ANIMATION
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::RedrawPlot() entered, viewAnimation=%d\n", viewAnimation);
   #endif
   
   if (mAxisLength < mMaxZoomIn)
   {
      mAxisLength = mMaxZoomIn;
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::RedrawPlot() distance < max zoom in. distance set to %f\n",
          mAxisLength);
   }
   
   if (viewAnimation)
      ViewAnimation(mUpdateInterval, mFrameInc);
   else
      Refresh(false);
   
   #ifdef DEBUG_ANIMATION
   MessageInterface::ShowMessage("TrajPlotCanvas::RedrawPlot() leaving\n");
   #endif
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
// void RotatePlot(int width, int height, int mouseX, int mouseY)
//------------------------------------------------------------------------------
/**
 * Rotates plot
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::RotatePlot(int width, int height, int mouseX, int mouseY)
{
   //=======================================================
   #ifdef USE_TRACKBALL
   //=======================================================
   
   // drag in progress, simulate trackball
   float spinQuat[4];
   
   ToQuat(spinQuat,
          (2.0*mLastMouseX - width) / width,
          (     height - 2.0*mLastMouseY) / height,
          (     2.0*mouseX - width) / width,
          (     height - 2.0*mouseY) / height);
   
   AddQuats(spinQuat, mQuat, mQuat);
   
   #if DEBUG_ROTATE
   MessageInterface::ShowMessage
      ("OnMouse() w=%d, h=%d, mLastMouseX=%d, mLastMouseY=%d, "
       "mouseX=%d, mouseY=%d\n", width, height, mLastMouseX,
       mLastMouseY, mouseX, mouseY);
   MessageInterface::ShowMessage
      ("OnMouse=% f, % f, % f, % f\n", mQuat[0], mQuat[1], mQuat[2],
       mQuat[3]);
   #endif
   
   //=======================================================
   #else
   //=======================================================
   
   #if DEBUG_ROTATE
   MessageInterface::ShowMessage
      ("   event.LeftIsDown() mCurrRotXYZAngle=%f, %f, %f\n",
       mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);
   #endif
   
   // if end-of-run compute new mfCamRotXYZAngle by calling ChangeView()
   if (mIsEndOfRun)
      ChangeView(mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);
   
   ComputeView(fEndX, fEndY);
   ChangeView(mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);
   
   //MessageInterface::ShowMessage
   //   ("===> after ChangeView() mfCamRotXYZAngle=%f, %f, %f\n",
   //    mfCamRotXAngle, mfCamRotYAngle, mfCamRotZAngle);
   
   //=======================================================
   #endif
   //=======================================================
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

   if (csName == "")
      return;
   
   
   // if current view CS name is different from the new CS name
   if (!mViewCoordSysName.IsSameAs(csName))
   {
      mViewCoordSysName = csName;
      
      pViewCoordSystem =
         (CoordinateSystem*)theGuiInterpreter->GetConfiguredObject(csName.c_str());
      
      if (pViewCoordSystem->GetName() == pInternalCoordSystem->GetName())
         mViewCsIsInternalCs = true;
      else
         mViewCsIsInternalCs = false;
      
      wxString oldOriginName = mOriginName;
      mOriginName = pViewCoordSystem->GetOriginName().c_str();
      mOriginId = GetObjectId(mOriginName);
      
      UpdateRotateFlags();
      
      if (!mOriginName.IsSameAs(oldOriginName))
         GotoObject(mOriginName);
      
      ConvertObjectData();
      Refresh(false);
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
      //int index = objId * MAX_DATA * 3 + (mNumData-1) * 3;
      int index = objId * MAX_DATA * 3 + mLastIndex * 3;
      // compute mAxisLength
      Rvector3 pos(mObjectViewPos[index+0], mObjectViewPos[index+1],
                   mObjectViewPos[index+2]);
      
      mAxisLength = pos.GetMagnitude();
      
      if (mAxisLength == 0.0)
         mAxisLength = mMaxZoomIn;
   }
   
   #ifdef DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::GotoObject() objName=%s, mViewObjId=%d, mMaxZoomIn=%f\n"
       "   mAxisLength=%f\n", objName.c_str(), mViewObjId, mMaxZoomIn, mAxisLength);
   #endif

   mIsEndOfData = true;
   mIsEndOfRun = true;

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
// void ViewAnimation(int interval, int frameInc)
//---------------------------------------------------------------------------
void TrajPlotCanvas::ViewAnimation(int interval, int frameInc)
{
   #ifdef DEBUG_ANIMATION
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::ViewAnimation() interval=%d, frameInc=%d\n",
       interval, frameInc);
   #endif
   
   this->SetFocus(); // so that it can get key interrupt
   mIsAnimationRunning = true;
   mUpdateInterval = interval;
   mFrameInc = frameInc;
   mHasUserInterrupted = false;
   
   GmatAppData *gmatAppData = GmatAppData::Instance();
   gmatAppData->GetMainFrame()->EnableMenuAndToolBar(false, false, true);
   
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   DrawFrame();
   
   gmatAppData->GetMainFrame()->EnableMenuAndToolBar(true, false, true);   
   
   mIsAnimationRunning = false;
   
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
      ("TrajPlotCanvas::SetGlObject() entered for %s, objCount=%d, colorCount=%d.\n",
       mPlotName.c_str(), objNames.size(), objOrbitColors.size());
   #endif
   
   // Initialize objects used in view
   InitializeViewPoint();
   
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
            ("   objNames[%d]=%s, objPtr=<%p>%s\n", i, objNames[i].c_str(),
             mObjectArray[i], mObjectArray[i]->GetName().c_str());
         #endif
      }
      
      AddObjectList(tempList, objOrbitColors);
   }
   else
   {
      MessageInterface::ShowMessage("TrajPlotCanvas::SetGlObject() object sizes "
                                    "are not the same. No ojbects added.\n");
   }
   
   #if DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage("TrajPlotCanvas::SetGlObject() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetSolarSystem(SolarSystem *ss)
{
   pSolarSystem = ss;
}


//------------------------------------------------------------------------------
// void SetGlCoordSystem(CoordinateSystem *internalCs,CoordinateSystem *viewCs,
//                       CoordinateSystem *viewUpCs)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetGlCoordSystem(CoordinateSystem *internalCs,
                                      CoordinateSystem *viewCs,
                                      CoordinateSystem *viewUpCs)
{
   #if DEBUG_TRAJCANVAS_CS
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::SetGlCoordSystem() '%s' entered, internalCs=<%p>, viewCs=<%p>, "
       "viweUpCs=%p\n",  mPlotName.c_str(), internalCs, viewCs, viewUpCs);
   #endif
   
   if (internalCs == NULL || viewCs == NULL || viewUpCs == NULL)
   {
      throw SubscriberException
         ("Internal or View or View Up CoordinateSystem is NULL\n"); 
   }
   
   pInternalCoordSystem = internalCs;
   mInternalCoordSysName = internalCs->GetName().c_str();
   
   pViewCoordSystem = viewCs;
   mViewCoordSysName = viewCs->GetName().c_str();
   
   pViewUpCoordSystem = viewUpCs;
   mViewUpCoordSysName = viewUpCs->GetName().c_str();
   
   // see if we need data conversion
   if (mViewCoordSysName.IsSameAs(mInternalCoordSysName))
      mViewCsIsInternalCs = true;
   else
      mViewCsIsInternalCs = false;
   
   // set view center object
   mOriginName = viewCs->GetOriginName().c_str();
   mOriginId = GetObjectId(mOriginName);
   
   mViewObjName = mOriginName;
   mViewObjId = mOriginId;
   
   // if view coordinate system origin is spacecraft, make spacecraft radius smaller.
   // So that spapcecraft won't overlap each other.
   //@todo: need better way to scale spacecraft size.
   if (viewCs->GetOrigin()->IsOfType(Gmat::SPACECRAFT))
      mScRadius = 30;
   else if (viewCs->GetOrigin()->IsOfType(Gmat::CELESTIAL_BODY))
      mScRadius = mObjectRadius[mOriginId] * 0.03;
   
   mMaxZoomIn = mObjMaxZoomIn[mOriginId];
   
   if (mUseInitialViewPoint)
   {
      mAxisLength = mMaxZoomIn;
   }
   
   UpdateRotateFlags();
   
   #if DEBUG_TRAJCANVAS_CS
   MessageInterface::ShowMessage
      ("   mViewCoordSysName=%s, pViewCoordSystem=%p, mOriginName=%s, "
       "mOriginId=%d\n", mViewCoordSysName.c_str(), pViewCoordSystem,
       mOriginName.c_str(),  mOriginId);
   MessageInterface::ShowMessage
      ("   mViewUpCoordSysName=%s, mViewObjName=%s, mViewObjId=%d\n",
       mViewUpCoordSysName.c_str(), mViewObjName.c_str(), mViewObjId);
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::SetGlCoordSystem() '%s' leaving\n",  mPlotName.c_str());
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
   pViewPointRefObj = vpRefObj;
   pViewPointVectorObj = vpVecObj;
   pViewDirectionObj = vdObj;
   
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
      ("TrajPlotCanvas::SetGlViewOption() entered, pViewPointRefObj=<%p>, "
       "pViewPointVectorObj=<%p>\n   pViewDirectionObj=<%p>, mViewScaleFactor=%f   "
       "mViewPointRefVector=%s\n   mViewPointVector=%s, mViewDirectionVector=%s, "
       "mViewUpAxisName=%s\n   mUseViewPointRefVector=%d, mUseViewDirectionVector=%d, "
       "mUseFixedFov=%d, mFixedFovAngle=%f\n",  pViewPointRefObj, pViewPointVectorObj,
       pViewDirectionObj, mViewScaleFactor, lvpRefVec.ToString(10).c_str(),
       lvpVec.ToString(10).c_str(), lvdVec.ToString(10).c_str(), mViewUpAxisName.c_str(),
       mUseViewPointRefVector, mUseViewDirectionVector, mUseFixedFov, mFixedFovAngle);
   #endif
   
   // Set viewpoint ref. object id
   if (!mUseViewPointRefVector && pViewPointRefObj)
   {
      mViewPointRefObjName = pViewPointRefObj->GetName();
      mVpRefObjId = GetObjectId(mViewPointRefObjName.c_str());
      
      if (mVpRefObjId == GmatPlot::UNKNOWN_BODY)
      {
         mUseViewPointRefVector = true;
         MessageInterface::ShowMessage
            ("*** WARNING *** TrajPlotCanvas::SetGlViewOption() Cannot find "
             "pViewPointRefObj name=%s, so using vector=%s\n",
             pViewPointRefObj->GetName().c_str(),
             mViewPointRefVector.ToString().c_str());
      }
   }
   else
   {
      mViewPointRefObjName = "Earth";
      
      if (!mUseViewPointRefVector)
         MessageInterface::ShowMessage
            ("*** WARNING *** TrajPlotCanvas::SetGlViewOption() "
             "ViewPointRefObject is NULL, "
             "so will use default Vector instead.\n");
   }
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("   mViewObjName='%s', mViewPointRefObjName='%s', mVpRefObjId=%d\n",
       mViewObjName.c_str(), mViewPointRefObjName.c_str(), mVpRefObjId);
   #endif
   
   // Set viewpoint vector object id
   if (!mUseViewPointVector && pViewPointVectorObj)
   {
      mVpVecObjId = GetObjectId(pViewPointVectorObj->GetName().c_str());
      
      if (mVpVecObjId == GmatPlot::UNKNOWN_BODY)
      {
         mUseViewPointVector = true;
         MessageInterface::ShowMessage
            ("*** WARNING *** TrajPlotCanvas::SetGlViewOption() Cannot find "
             "pViewPointVectorObj name=%s, so using vector=%s\n",
             pViewPointVectorObj->GetName().c_str(),
             mViewPointVector.ToString().c_str());
      }
   }
   else
   {
      if (!mUseViewPointVector)
         MessageInterface::ShowMessage
            ("*** WARNING *** TrajPlotCanvas::SetGlViewOption() "
             "ViewPointVectorObject is NULL, "
             "so will use default Vector instead.\n");
   }
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("   mVpVecObjId=%d, mUseViewPointVector=%d\n", mVpVecObjId,
       mUseViewPointVector);
   #endif
   
   // Set view direction object id
   if (!mUseViewDirectionVector && pViewDirectionObj)
   {
      mVdirObjId = GetObjectId(pViewDirectionObj->GetName().c_str());
      
      if (mVdirObjId == GmatPlot::UNKNOWN_BODY)
      {
         mUseViewDirectionVector = true;
         MessageInterface::ShowMessage
            ("*** WARNING *** TrajPlotCanvas::SetGlViewOption() Cannot find "
             "pViewDirectionObj name=%s, so using vector=%s\n",
             pViewDirectionObj->GetName().c_str(),
             mViewDirectionVector.ToString().c_str());
      }
   }
   else
   {
      if (!mUseViewDirectionVector)
         MessageInterface::ShowMessage
            ("*** WARNING *** TrajPlotCanvas::SetGlViewOption() "
             "ViewDirectionObject is NULL, "
             "so will use default Vector instead.\n");
   }
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("   mVdirObjId=%d, mUseViewDirectionVector=%d\n", mVdirObjId,
       mUseViewDirectionVector);
   #endif
   
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
   
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage("TrajPlotCanvas::SetGlViewOption() leaving\n");
   #endif
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
   
   bool draw;
   for (int i=0; i<mObjectCount; i++)
   {
      draw = mDrawOrbitArray[i] ? true : false;      
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::SetGlDrawObjectFlag() i=%d, mDrawOrbitArray[%s]=%d\n",
          i, mObjectNames[i].c_str(), draw);
   }
   #endif
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
         ("TrajPlotCanvas::SetGlShowObjectFlag() i=%d, mShowObjectMap[%s]=%d\n",
          i, mObjectNames[i].c_str(), show);
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
 * Updates spacecraft trajectory. Position and velocity should be in view
 * coordinate system.
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
 * @param <solving> true if GMAT is running solver
 * @param <solverOption> solver iteration drawing option can be one of following:
 *           0 (Subscriber::SI_ALL)     = Draw all iteration
 *           1 (Subscriber::SI_CURRENT) = Draw current iteration only
 *           2 (Subscriber::SI_NONE)    = Draw no iteration
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::UpdatePlot(const StringArray &scNames, const Real &time,
                                const RealArray &posX, const RealArray &posY,
                                const RealArray &posZ, const RealArray &velX,
                                const RealArray &velY, const RealArray &velZ,
                                const UnsignedIntArray &scColors, bool solving,
                                Integer solverOption)
{
   mScCount = scNames.size();
   mScNameArray = scNames;
   mTotalPoints++;
   
   if (mNumData < MAX_DATA)
      mNumData++;
   
   if (mScCount > MAX_SCS)
      mScCount = MAX_SCS;
   
   #if DEBUG_TRAJCANVAS_UPDATE
   MessageInterface::ShowMessage
      ("=====================================================\n");
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::UpdatePlot() plot=%s, time=%f, posX=%f, mNumData=%d, "
       "mScCount=%d, scColor=%u, solving=%d, solverOption=%d\n", GetName().c_str(),
       time, posX[0], mNumData, mScCount, scColors[0], solving, solverOption);
   #endif
   
   
   mDrawSolverData = false;
   
   //-----------------------------------------------------------------
   // If showing current iteration only, handle solver iteration data
   // separately here since it will be shown temporarily during the run
   //-----------------------------------------------------------------
   if (solverOption == 1)
      UpdateSolverData(posX, posY, posZ, scColors, solving);
   
   // If drawing solver's current iteration and no run data has been
   // bufferred up, save up to 2 points so that it will still go through
   // view projection transformation to show other objects.
   if (solverOption == 1 && solving && mNumData > 1)
      return;
   
   //-----------------------------------------------------------------
   // Buffer data for plot
   //-----------------------------------------------------------------
   
   mCurrIndex++;
   
   if (mCurrIndex < MAX_DATA)
   {
      mEndIndex1 = mNumData - 1;
      if (mEndIndex2 != -1)
      {
         mBeginIndex1++;
         if (mBeginIndex1 + 1 > MAX_DATA)
            mBeginIndex1 = 0;
         
         mEndIndex2++;
         if (mEndIndex2 + 1 > MAX_DATA)
            mEndIndex2 = 0;
      }
   }
   else
   {
      // Write buffer maxed out message only once
      if (mWriteWarning)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** '%s' exceed the maximum data points, now "
             "showing %d most recent data points.\n", mPlotName.c_str(), MAX_DATA);
         mWriteWarning = false;
      }
      
      mBeginIndex1++;
      if (mBeginIndex1 + 1 > MAX_DATA)
         mBeginIndex1 = 0;
      
      mEndIndex1 = MAX_DATA - 1;
      
      mBeginIndex2 = 0;
      mEndIndex2++;
      if (mEndIndex2 + 1 > MAX_DATA)
         mEndIndex2 = 0;
      mCurrIndex = 0;
   }
   
   // find buffer index
   mLastIndex = mEndIndex1;
   if (mEndIndex2 != -1)
      mLastIndex = mEndIndex2;
   
   #ifdef DEBUG_DATA_BUFFERRING
   MessageInterface::ShowMessage
      ("===> time=%f, mTotalPoints=%4d, mNumData=%3d, mCurrIndex=%3d, mLastIndex=%3d\n   "
       "mBeginIndex1=%3d, mEndIndex1=%3d, mBeginIndex2=%3d, mEndIndex2=%3d\n",
       time, mTotalPoints, mNumData, mCurrIndex, mLastIndex, mBeginIndex1, mEndIndex1,
       mBeginIndex2, mEndIndex2);
   #endif
   
   mTime[mLastIndex] = time;

   // Dunn notes the variable "longitude" below is declared elsewhere in this
   // file.  Even if the other "longitude" is protected, it should probably
   // have a different name.  This is the SECOND place longitude is declared.
   Real mha, longitude, lst;
   ComputeLongitudeLst(mTime[mLastIndex], posX[0], posY[0], &mha, &longitude, &lst);
   
   #if DEBUG_TRAJCANVAS_LONGITUDE
   MessageInterface::ShowMessage
      ("   time=%f, mLastIndex=%d, mha=%f, longitude=%f, lst = %f\n",
       mTime[mLastIndex], mLastIndex, mha, longitude, lst);
   #endif
   
   // if beginning of the plot
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
   
   // update spacecraft position
   UpdateSpacecraftData(time, posX, posY, posZ, velX, velY, velZ, scColors,
                        solverOption);
   
   // update non-spacecraft objects position
   UpdateOtherData(time);
   
//    if (mNumData < MAX_DATA)
//       mNumData++;
   
}


//---------------------------------------------------------------------------
// void TakeAction(const std::string &action)
//---------------------------------------------------------------------------
void TrajPlotCanvas::TakeAction(const std::string &action)
{
   if (action == "ClearSolverData")
   {
      //MessageInterface::ShowMessage("===> clearing solver data");
      mSolverAllPosX.clear();
      mSolverAllPosY.clear();
      mSolverAllPosZ.clear();
   }
   else if (action == "ClearObjects")
   {
      mObjectCount = 0;
      mObjectArray.clear();
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
      ("TrajPlotCanvas::AddObjectList() entered, object count=%d, color count=%d\n",
       objNames.GetCount(), objColors.size());
   #endif
   
   // clear bodies
   if (clearList)
   {
      mObjectNames.Empty();
   }
   
   RgbColor rgb;
   
   mObjectCount = objNames.GetCount();
   ClearObjectArrays();
   
   if (!CreateObjectArrays())
      throw SubscriberException("There is not enough memory to allocate\n");
   
   for (int i=0; i<mObjectCount; i++)
   {
      // add object names
      mObjectNames.Add(objNames[i]);
      
      if (mObjectTextureIdMap.find(objNames[i]) == mObjectTextureIdMap.end())
      {
         #if DEBUG_TRAJCANVAS_OBJECT
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::AddObjectList() Bind new texture object=%s\n",
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
   
   // Always initialize GL before run, InitGL() is called in OnPaint()
   // if using 2.6.3 or later version
   // For 2.6.3 version initialize GL here
   #ifndef __USE_WX280_GL__
   InitGL();
   #endif
   
   ResetPlotInfo();
   ClearPlot();
   
   #if DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage("TrajPlotCanvas::AddObjectList() leaving\n");
   #endif
   
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
      int index = 0;
      
      for(int i=0; i<numDataPoints && i < MAX_DATA; i++)
      {
         index = sc * MAX_DATA * 3 + mNumData * 3;
         mTime[mNumData] = mTrajectoryData[i].time;
         mObjectOrbitColor[sc*MAX_DATA+mNumData] = GmatColor::RED32;
         mObjectViewPos[index+0] = mTrajectoryData[i].x;
         mObjectViewPos[index+1] = mTrajectoryData[i].y;
         mObjectViewPos[index+2] = mTrajectoryData[i].z;
         mNumData++;
      }
      
      mTextTrajFile->Close();
      
      #ifdef __WRITE_GL_MOUSE_POS__
      // Write out to status bar
      wxString text;
      text.Printf("Number of data points: %d", numDataPoints);
      theStatusBar->SetStatusText(text, 2);
      //wxLogStatus(GmatAppData::Instance()->GetMainFrame(),
      //            wxT("Number of data points: %d"), numDataPoints);
      #endif
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
   // must always be here
   wxPaintDC dc(this);
   
   #ifndef __WXMOTIF__
      #ifndef __USE_WX280_GL__
         if (!GetContext()) return;
      #endif
   #endif
   
   #ifdef __USE_WX280_GL__
   SetCurrent(*m_glContext);
   #else
   SetCurrent();
   #endif
   
   #ifdef __USE_WX280_GL__
   if (!mGlInitialized)
   {
      InitGL();
      mGlInitialized = true;
   }
   #endif
   
   if (mDrawWireFrame)
   {
      glPolygonMode(GL_FRONT, GL_LINE);
      glPolygonMode(GL_BACK, GL_LINE);
   }
   else
   {
      glPolygonMode(GL_FRONT, GL_FILL);
      glPolygonMode(GL_BACK, GL_FILL);
   }
   
   // Linux specific
   #ifdef __WXGTK__
      hasBeenPainted = true;
   #endif
   
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
   // Linux specific handler for sizing
   #ifdef __WXGTK__
      if (hasBeenPainted == false)
         return;
   #endif
   
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
      
      #ifdef __USE_WX280_GL__
      SetCurrent(*m_glContext);
      #else
      SetCurrent();
      #endif
      
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
     
   //MessageInterface::ShowMessage
   //   ("===> OnMouse() mUseInitialViewPoint=%d, mIsEndOfData=%d\n",
   //    mUseInitialViewPoint, mIsEndOfData);
   
   int flippedY;
   int width, height;
   int mouseX, mouseY;
   
   mIsAnimationRunning = false;
   
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
         
         if (mUsePerspectiveMode)
         {
            // always look at from Z for rotation and zooming
            mfCamTransX = 0.0;
            mfCamTransY = 0.0;
            mfCamTransZ = -mVpLocVec.GetMagnitude();
         }
      }
      #endif
      
      
      //loj: 3/22/06
      // When USE_TRACKBALL: This code is trying to compute quaternion of last
      // plot transformation so that the plot can stay in the same orientation
      // when user clicks it after run completes.
      #ifdef USE_TRACKBALL
      if (!mUseSingleRotAngle)
      {
         // Get last modelview matrix and compute quat
         GLfloat mvMat[16];//, mvQ[4];
         glGetFloatv(GL_MODELVIEW_MATRIX, mvMat);
         FloatAttUtil::ToQuat(mQuat, mvMat);
         
         #if DEBUG_ROTATE
         MessageInterface::ShowMessage
            ("OnMouse() Final mvMat=\n%f, %f, %f, %f\n%f, %f, %f, %f\n"
             "%f, %f, %f, %f\n%f, %f, %f, %f\n",
             mvMat[0], mvMat[1], mvMat[2], mvMat[3], mvMat[4], mvMat[5],
             mvMat[6], mvMat[8], mvMat[8], mvMat[9], mvMat[10], mvMat[11],
             mvMat[12], mvMat[13], mvMat[14], mvMat[15]);
         MessageInterface::ShowMessage
            ("OnMouse=% f, % f, % f, % f\n", mQuat[0], mQuat[1], mQuat[2],
             mQuat[3]);
         #endif
         
      }
      #endif
      
   }
   
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
         RotatePlot(width, height, mouseX, mouseY);
         
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
         
         //VC++ error C2668: 'pow' : ambiguous call to overloaded function
         //'long double pow(long double,int)' 'float pow(float,int)' 'double pow(double,int);
         // Changed pow to GmatMathUtil::Pow();
         
         // find the length
         Real x2 = Pow(mouseX - mLastMouseX, 2);
         Real y2 = Pow(mouseY - mLastMouseY, 2);
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
   
   #ifdef __WRITE_GL_MOUSE_POS__
   wxString mousePosStr;
   mousePosStr.Printf("X = %g Y = %g mouseX = %d, mouseY = %d",
                      fEndX, fEndY, mouseX, mouseY);   
   theStatusBar->SetStatusText(mousePosStr, 2);
   //wxLogStatus(MdiGlPlot::mdiParentGlFrame,
   //            wxT("X = %d Y = %d lastX = %f lastY = %f Zoom amount = %f Distance = %f"),
   //            event.GetX(), event.GetY(), mfStartX, mfStartY, mZoomAmount, mAxisLength);
   #endif
   
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
      //32,                    // 32-bit z-buffer
      16,                    // 32-bit z-buffer
      0,                     // no stencil buffer
      0,                     // no auxiliary buffer
      PFD_MAIN_PLANE,        // main layer
      0,                     // reserved
      0, 0, 0                // layer masks ignored
   };
   
   // get the device context's best-available-match pixel format
   int pixelFormatId = ChoosePixelFormat(hdc, &pfd);
   
   #ifdef DEBUG_TRAJCANVAS_INIT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::SetPixelFormatDescriptor() pixelFormatId = %d \n",
       pixelFormatId);
   #endif
   
   if(pixelFormatId == 0)
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** Failed to find a matching pixel format\n");
      return false;
   }
   
   // set the pixel format of the device context
   if (!SetPixelFormat(hdc, pixelFormatId, &pfd))
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** Failed to set pixel format id %d\n", pixelFormatId);
      return false;
   }
   
   return true;
   
#endif

   // Should we return true for non-Window system?
   //return false;
   return true;
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
// void InitializeViewPoint()
//------------------------------------------------------------------------------
void TrajPlotCanvas::InitializeViewPoint()
{
   mViewPointRefObjName = "UNKNOWN";
   
   pViewPointRefObj = NULL;
   pViewPointVectorObj = NULL;
   pViewDirectionObj = NULL;
   
   mViewPointRefVector.Set(0.0, 0.0, 0.0);
   mViewPointVector.Set(0.0, 0.0, 30000.0);
   mViewDirectionVector.Set(0.0, 0.0, -1.0);
   mVpLocVec.Set(0.0, 0.0, 30000.0);
   mViewScaleFactor = 1.0;
   mUseViewPointRefVector = true;
   mUseViewPointVector = true;
   mUseViewDirectionVector = true;
   mVpRefObjId = UNKNOWN_OBJ_ID;
   mVpVecObjId = UNKNOWN_OBJ_ID;
   mVdirObjId = UNKNOWN_OBJ_ID;
}


//------------------------------------------------------------------------------
// void ComputeActualIndex()
//------------------------------------------------------------------------------
void TrajPlotCanvas::ComputeActualIndex()
{
   mRealBeginIndex1 = mBeginIndex1;
   mRealEndIndex1   = mEndIndex1;
   mRealBeginIndex2 = mBeginIndex2;
   mRealEndIndex2   = mEndIndex2;
   
   // if re-drawing last few points only
   if (mRedrawLastPointsOnly && !mIsEndOfRun)
   {
      // if ring buffer not over run
      if (mEndIndex2 == -1)
      {
         mRealBeginIndex1 = mEndIndex1 - mNumPointsToRedraw;
         if (mRealBeginIndex1 < 0)
            mRealBeginIndex1 = 0;
      }
      else
      {
         mRealBeginIndex1 = mEndIndex2 - mNumPointsToRedraw;
         if (mRealBeginIndex1 >= 0)
         {
            mRealEndIndex1 = mEndIndex2;
            mRealBeginIndex2 = -1;
            mRealEndIndex2 = -1;
         }
         else
         {
            mRealBeginIndex1 = MAX_DATA + mRealBeginIndex1;
            mRealEndIndex1 = MAX_DATA - 1;
            mRealBeginIndex2 = 0;
            mRealEndIndex2 = mEndIndex2;
         }
      }
   }
   
   #ifdef DEBUG_DATA_BUFFERRING
   MessageInterface::ShowMessage
      ("TrajPltCanvas::ComputeActualIndex()     mBeginIndex1=%3d,     mEndIndex1=%3d, "
       "    mBeginIndex2=%3d,     mEndIndex2=%3d\n", mBeginIndex1, mEndIndex1,
       mBeginIndex2, mEndIndex2);
   MessageInterface::ShowMessage
      ("TrajPltCanvas::ComputeActualIndex() mRealBeginIndex1=%3d, mRealEndIndex1=%3d, "
       "mRealBeginIndex2=%3d, mRealEndIndex2=%3d\n", mRealBeginIndex1, mRealEndIndex1,
       mRealBeginIndex2,  mRealEndIndex2);
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
            ("TrajPlotCanvas::LoadGLTextures() object=<%p>'%s'\n",
             mObjectArray[i], mObjectNames[i].c_str());
         #endif
         
         mObjectTextureIdMap[mObjectNames[i]] =
            BindTexture(mObjectArray[i], mObjectNames[i]);
      }
   }
   
   return true;
   
} //end LoadGLTextures()


//------------------------------------------------------------------------------
// GLuint BindTexture(SpacePoint *obj, const wxString &objName)
//------------------------------------------------------------------------------
/**
 * Loads textures and returns binding index.
 */
//------------------------------------------------------------------------------
GLuint TrajPlotCanvas::BindTexture(SpacePoint *obj, const wxString &objName)
{
   GLuint ret = GmatPlot::UNINIT_TEXTURE;

   //MessageInterface::ShowMessage("===> TrajPlotCanvas::BindTexture() ret = %d\n", ret);
   // texture map file names now stored with the CelestialBody  wcs 2009.01.06
   //FileManager *fm = FileManager::Instance();
   std::string textureFile;  
   //std::string name = std::string(objName.Upper().c_str());
   //std::string filename = name + "_TEXTURE_FILE";
   
   try
   {
      //textureFile = fm->GetFullPathname(filename);
      CelestialBody *body = (CelestialBody*) obj;
      textureFile = body->GetStringParameter(body->GetParameterID("TextureMapFileName"));
      
      #ifndef SKIP_DEVIL
         ILboolean status = ilLoadImage((char*)textureFile.c_str());
         if (!status)
         {
            MessageInterface::ShowMessage
               ("*** WARNING *** TrajPlotCanvas::BindTexture() Unable to load "
                "texture file for %s\nfile name:%s\n", objName.c_str(),
                textureFile.c_str());
         }
         else
         {
            //ilutGLLoadImage((char*)textureFile.c_str());
            //ret = 1;
            ret = ilutGLBindTexImage();
         }
      #else
         
         glGenTextures(1, &ret);
         glBindTexture(GL_TEXTURE_2D, ret);
         
         //if (textureFile == "")
         //   ret = GmatPlot::UNINIT_TEXTURE;
         //else
         // load image file
         if (!LoadImage(textureFile))
            ret = GmatPlot::UNINIT_TEXTURE;
         
      #endif
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** TrajPlotCanvas::BindTexture() Cannot bind texture "
          "image for %s.\n%s\n", objName.c_str(), e.GetFullMessage().c_str());
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
   
   #ifdef USE_TRACKBALL
   ToQuat(mQuat, 0.0f, 0.0f, 0.0f, 0.0);
   #endif   
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
      
      Real size = GmatMathUtil::Sqrt(mfRightPos * mfRightPos +
                                     mfTopPos   * mfTopPos);
      
      Real dist = mVpLocVec.GetMagnitude();      
      mViewObjRadius = mObjectDefaultRadius;
      
      if (mUseFixedFov && mUseSingleRotAngle)
      {
         mFovDeg = mFixedFovAngle;
      }
      else
      {
         if (!mUseViewDirectionVector && pViewDirectionObj != NULL)
         {
            // if showing view object
            if (mShowObjectMap[mViewObjName])
               mViewObjRadius = mObjectRadius[mViewObjId];
         }
         
         // compute fov angle
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
      
      //Add ratio to prevent near side clipping
      gluPerspective(mFovDeg, aspect, mAxisLength/(mFovDeg*15), mAxisLength * mFovDeg * ratio);
      
      //glFrustum(mfLeftPos, mfRightPos, mfViewBottom, mfTopPos, mAxisLength, mfViewFar*mAxisLength);
   }
   else
   {
      // Setup how we view the world
      glOrtho(mfLeftPos, mfRightPos, mfBottomPos, mfTopPos, mfViewNear, mfViewFar);
   }
   
   //-----------------------------------------------------------------
   // Note: mouse rotation is applied in TransformView as MODELVIEW mode
   //-----------------------------------------------------------------
   
   //camera moves opposite direction to center on object
   //this is the point of rotation
   
   //int index = mViewObjId * MAX_DATA * 3 + (mNumData-1) * 3;
   int index = mViewObjId * MAX_DATA * 3 + mLastIndex * 3;
   glTranslatef(mObjectViewPos[index+0], mObjectViewPos[index+1],
                -mObjectViewPos[index+2]);
   
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
//  void ComputeViewVectors()
//------------------------------------------------------------------------------
/**
 * Computes viewing vectors using viewing options.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::ComputeViewVectors()
{   
   //int frame = mNumData - 1;
   int frame = mLastIndex;
   mIsFirstRun = false;
   int index = 0;
   
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("ComputeViewVectors() frame=%d, time=%f\n", frame, mTime[frame]);
   #endif
   
   //-----------------------------------------------------------------
   // get viewpoint reference vector
   //-----------------------------------------------------------------
   mVpRefVec.Set(0.0, 0.0, 0.0);
   
   if (!mUseViewPointRefVector && pViewPointRefObj != NULL)
   {
      #if DEBUG_TRAJCANVAS_PROJ
      MessageInterface::ShowMessage
         ("ComputeViewVectors() pViewPointRefObj=<%p>, name=%s\n",
          pViewPointRefObj, pViewPointRefObj->GetName().c_str());
      #endif
      
      // if valid body id
      if (mVpRefObjId != UNKNOWN_OBJ_ID)
      {
         index = mVpRefObjId * MAX_DATA * 3 + frame * 3;
         // for efficiency, body data are computed in UpdatePlot() once.
         mVpRefVec.Set(mObjectViewPos[index+0],
                       mObjectViewPos[index+1],
                       mObjectViewPos[index+2]);
      }
      else
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** TrajPlotCanvas::ComputeViewVectors() Invalid "
             "mVpRefObjId=%d\n", mVpRefObjId);
      }
   }
   
   //-----------------------------------------------------------------
   // get viewpoint vector
   //-----------------------------------------------------------------
   mVpVec = mViewPointVector;
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("ComputeViewVectors() initial mVpVec=%s\n", mVpVec.ToString().c_str());
   #endif
   
   if (!mUseViewPointVector && pViewPointVectorObj != NULL)
   {
      #if DEBUG_TRAJCANVAS_PROJ
      MessageInterface::ShowMessage
         ("ComputeViewVectors() pViewPointVectorObj=<%p>, name=%s, "
          "mVpVecObjId=%d, mUseGluLookAt=%d\n", pViewPointVectorObj,
          pViewPointVectorObj->GetName().c_str(), mVpVecObjId, mUseGluLookAt);
      #endif
      
      // if valid body id
      if (mVpVecObjId != UNKNOWN_OBJ_ID)
      {
         if (mUseGluLookAt)
         {
            index = mVpVecObjId * MAX_DATA * 3 + frame * 3;
            
            // if look at from object, negate it so that we can see the object
            mVpVec.Set(-mObjectViewPos[index+0],
                       -mObjectViewPos[index+1],
                       -mObjectViewPos[index+2]);
            
            // set z component to zero so that plane doesn't move up and down
            ////mVpVec[2] = 0.0; //commented out LOJ: 2010.05.11
         }
         else
         {
            index = mVpVecObjId * MAX_DATA * 3 + frame * 3;
            
            mVpVec.Set(mObjectViewPos[index+0],
                       mObjectViewPos[index+1],
                       mObjectViewPos[index+2]);
         }
      }
      else
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** TrajPlotCanvas::ComputeViewVectors() Invalid "
             "mVpVecObjId=%d\n", mVpVecObjId);
      }
   }
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("ComputeViewVectors() final mVpVec=%s\n", mVpVec.ToString().c_str());
   #endif
   
   //-----------------------------------------------------------------
   // get viewpoint location
   //-----------------------------------------------------------------
   mVpLocVec = mVpRefVec + (mViewScaleFactor * mVpVec);
   
   if (mVpLocVec.IsZeroVector())
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** Viewpoint location vector is zero, so setting "
          "vector to [ 0 0 30000]\n");
      mVpLocVec.Set(0.0, 0.0, 30000.0);
   }
   
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("ComputeViewVectors() mVpLocVec=%s, mVpRefVec=%s, mViewScaleFactor=%f\n",
       mVpLocVec.ToString().c_str(), mVpRefVec.ToString().c_str(),
       mViewScaleFactor);
   #endif
   
   //-----------------------------------------------------------------
   // get view direction and view center vector
   //-----------------------------------------------------------------
   mVdVec = mViewDirectionVector;
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("ComputeViewVectors() initial mVdVec=%s\n", mVdVec.ToString().c_str());
   #endif
   
   if (!mUseViewDirectionVector && pViewDirectionObj != NULL)
   {
      #if DEBUG_TRAJCANVAS_PROJ
      MessageInterface::ShowMessage
         ("ComputeViewVectors() pViewDirectionObj=<%p>, name=%s\n",
          pViewDirectionObj, pViewDirectionObj->GetName().c_str());
      #endif
      
      // if viewpoint ref object is same as view direction object
      // just look opposite side
      if (pViewDirectionObj->GetName() == mViewPointRefObjName)
      {
         mVdVec = -mVpLocVec;
      }
      else if (mVdirObjId != UNKNOWN_OBJ_ID)
      {
         index = mVdirObjId * MAX_DATA * 3 + frame * 3;
         
         // for efficiency, body data are computed in UpdatePlot() once.
         mVdVec.Set(mObjectViewPos[index+0],
                    mObjectViewPos[index+1],
                    mObjectViewPos[index+2]);
         
         // check for 0.0 direction 
         if (mVdVec.GetMagnitude() == 0.0)
            mVdVec = mViewDirectionVector;
      }
      else
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** TrajPlotCanvas::ComputeViewVectors() Invalid "
             "mVdirObjId=%d\n", mVdirObjId);
      }
   }
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("ComputeViewVectors() final mVdVec=%s\n", mVdVec.ToString().c_str());
   #endif
   
   if (mVdVec.IsZeroVector())
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** View direction vector is zero, so setting "
          "vector to [ 0 0 -30000]\n");
      mVdVec.Set(0.0, 0.0, -30000.0);
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
   
   ComputeUpAngleAxis();
   
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("==> ComputeViewVectors() mfCamTransXYZ=%f, %f, %f, mfCamSingleRotAngle=%f\n"
       "   mfCamRotXYZ=%f, %f, %f mAxisLength=%f\n", mfCamTransX, mfCamTransY,
       mfCamTransZ, mfCamSingleRotAngle, mfCamRotXAxis, mfCamRotYAxis,
       mfCamRotZAxis, mAxisLength);
   #endif
} // end ComputeViewVectors(int frame)


//------------------------------------------------------------------------------
// void ComputeUpAngleAxis()
//------------------------------------------------------------------------------
void TrajPlotCanvas::ComputeUpAngleAxis()
{
   // calculate view up direction
   //int frame = mNumData - 1;
   int frame = mLastIndex;
   Rvector6 upOutState;
   upOutState = mUpState;
   
   if (pViewUpCoordSystem->GetName() != pViewCoordSystem->GetName())
   {
      mCoordConverter.Convert(mTime[frame], mUpState, pViewUpCoordSystem,
                              upOutState, pViewCoordSystem);
   }
   
   mUpVec.Set(upOutState(0), upOutState(1), upOutState(2));
   
   // If view up and view direction is the same axis, change view direction,
   // so it can show up direction correctly even for gluLookAt.
   
   Rvector3 vdUnit = mVdVec.GetUnitVector();
   Real upDotView = mUpVec * vdUnit;
   
   if (Abs(upDotView) == 1.0)
   {
      #if DEBUG_TRAJCANVAS_PROJ
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::ComputeUpAngleAxis() mUpVec and mVdVec are on the "
          "same axis.\n");
      #endif
      
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
// void TransformView()
//------------------------------------------------------------------------------
void TrajPlotCanvas::TransformView()
{
   #if DEBUG_TRAJPLOTCANVAS_DRAW
   MessageInterface::ShowMessage
      ("==> TrajPlotCanvas::TransformView() mUseSingleRotAngle=%d, "
       "mUseGluLookAt=%d, mIsEndOfData=%d, mIsEndOfRun=%d\n", mUseSingleRotAngle,
       mUseGluLookAt, mIsEndOfData, mIsEndOfRun);
   #endif
   
   glLoadIdentity();
   
   if (mUseSingleRotAngle)
   {
      //MessageInterface::ShowMessage("==> TransformView() call gluLookAt()\n");
      
      //glLoadIdentity();
      
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

   } //if (mUseSingleRotAngle)
   
   //-------------------------------------------------
   // add current user mouse rotation
   //-------------------------------------------------
   #ifdef USE_TRACKBALL      
      GLfloat m[4][4];
      BuildRotMatrix(m, mQuat);
      glMultMatrixf(&m[0][0]);      
   #else      
      ApplyEulerAngles();      
   #endif
      
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
   #if DEBUG_ANIMATION
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::DrawFrame() mNumData=%d, mUsenitialViewPoint=%d\n"
       "   mViewCoordSysName=%s, mInternalCoordSysName=%s\n", mNumData,
       mUseInitialViewPoint, mViewCoordSysName.c_str(), mInternalCoordSysName.c_str());
   #endif
   
   if (mUseInitialViewPoint)
   {
      #ifdef USE_TRACKBALL
         ToQuat(mQuat, 0.0f, 0.0f, 0.0f, 0.0);
      #endif
         
      SetDefaultView();            
      UpdateRotateFlags();
      
      // set view center object
      mOriginName = wxString(pViewCoordSystem->GetOriginName().c_str());
      mOriginId = GetObjectId(mOriginName);
      
      mViewObjName = mOriginName;
      GotoObject(mViewObjName);
   }

   // Save mNumData since it is updated inside a loop
   int numberOfData = mNumData;
   mIsEndOfData = false;
   mIsEndOfRun = false;
   mCurrIndex = 0;
   
   // Go through number of data points, incrementng mFrameInc
   for (int frame=1; frame<numberOfData; frame+=mFrameInc)
   {
      mIsAnimationRunning = true;
      
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
      mCurrIndex++;
      
      if (mCurrIndex < MAX_DATA)
      {
         mEndIndex1 = mNumData - 1;
         if (mEndIndex2 != -1)
         {
            mBeginIndex1++;
            if (mBeginIndex1 + 1 > MAX_DATA)
               mBeginIndex1 = 0;
            
            mEndIndex2++;
            if (mEndIndex2 + 1 > MAX_DATA)
               mEndIndex2 = 0;
         }
      }
      
      mLastIndex = mEndIndex1;
      if (mEndIndex2 != -1)
         mLastIndex = mEndIndex2;
      
      // Set projection here, because DrawPlot() is called in OnPaint()
      if (mUseInitialViewPoint)
         ComputeViewVectors();
      
      ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
      
      Refresh(false);
   }
   
   // final refresh, in case number of points is less than 50
   Refresh(false);
   
   // Reset values
   mNumData = numberOfData;
   mIsEndOfData = true;
   mIsEndOfRun = true;
   
   #if DEBUG_ANIMATION
   MessageInterface::ShowMessage("TrajPlotCanvas::DrawFrame() leaving\n");
   #endif
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
   #if DEBUG_TRAJCANVAS_DRAW
   MessageInterface::ShowMessage
      ("===========================================================================\n");
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::DrawPlot() mTotalPoints=%d, mNumData=%d, mTime[%d]=%f\n",
       mTotalPoints, mNumData, mLastIndex, mTime[mLastIndex]);
   #endif
   #if DEBUG_TRAJCANVAS_DRAW > 1
   MessageInterface::ShowMessage
      ("   mRedrawLastPointsOnly=%d, mNumPointsToRedraw=%d, mViewCsIsInternalCs=%d, "
       "mUseInitialViewPoint=%d, mAxisLength=%f\n", mRedrawLastPointsOnly, mNumPointsToRedraw,
       mViewCsIsInternalCs, mUseInitialViewPoint, mAxisLength);
   MessageInterface::ShowMessage
      ("   mUseInitialViewPoint=%d, mIsEndOfData=%d, mIsEndOfRun=%d, mDrawSolverData=%d\n",
       mUseInitialViewPoint, mIsEndOfData, mIsEndOfRun, mDrawSolverData);
   #endif
   
   if (mRedrawLastPointsOnly || mNumPointsToRedraw == 0)
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   else
      glClear(GL_DEPTH_BUFFER_BIT);
   
   DrawStatus("Frame#: ", mTotalPoints, "  Epoch: ", mTime[mLastIndex], 0, 5);
   
   // Plot is not refreshed when another panel is opened, so add glFlush()
   // and SwapBuffers() (loj: 4/5/06)
   //if (mNumData < 1) // to avoid 0.0 time
   if (mNumData < 1 && !mDrawSolverData) // to avoid 0.0 time
   {
      glFlush();
      SwapBuffers();
      return;
   }
   
   // compute projection if using initial viewpoint and not end of run or
   // if not using initial viewpoint and not first run.
   // We need initial values for  gulLookAt()
   if ((mUseInitialViewPoint && !mIsEndOfRun) ||
       (!mUseInitialViewPoint && mIsFirstRun && mUseGluLookAt))
      ComputeViewVectors();
   
   ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
   
   // change back to view projection
   SetProjection();
   TransformView();
   
   #ifdef __TILT_ORIGIN__
   // tilt Origin rotation axis if needed
   if (mNeedOriginConversion)
   {
      glPushMatrix();
      TiltOriginZAxis();
   }
   #endif
   
   // draw equatorial plane
   if (mDrawXyPlane)
      DrawEquatorialPlane(mXyPlaneColor);
   
   // draw axes
   if (mDrawAxes)
      if (!mCanRotateAxes)
         DrawAxes();
   
   // draw ecliptic plane
   if (mDrawEcPlane)
      DrawEclipticPlane(mEcPlaneColor);
   
   #ifdef __TILT_ORIGIN__
   if (mNeedOriginConversion)
      glPopMatrix();
   #endif
   
   // draw object orbit
   DrawObjectOrbit();
   
   if (mDrawSolverData)
      DrawSolverData();
   
   // draw Earth-Sun line
   if (mDrawSunLine)
      DrawSunLine();
   
   glFlush();
   SwapBuffers();
   
} // end DrawPlot()


//------------------------------------------------------------------------------
// void DrawSphere(GLdouble radius, GLint slices, GLint stacks, GLenum style,
//                 GLenum orientation = GLU_OUTSIDE, GLenum normals = GL_NONE,
//                 GLenum textureCoords = GL_FALSE)
//------------------------------------------------------------------------------
/**
 * Draws sphere.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawSphere(GLdouble radius, GLint slices, GLint stacks,
                                GLenum style, GLenum orientation, GLenum normals,
                                GLenum textureCoords)
{
   GLUquadricObj* qobj = gluNewQuadric();
   gluQuadricDrawStyle(qobj, style);
   gluQuadricNormals(qobj, normals);
   gluQuadricTexture(qobj, textureCoords);
   gluSphere(qobj, radius, slices, stacks);
   gluQuadricOrientation(qobj, orientation);
   gluDeleteQuadric(qobj);
}


//------------------------------------------------------------------------------
//  void DrawObject(const wxString &objName)
//------------------------------------------------------------------------------
/**
 * Draws object sphere and maps texture image.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawObject(const wxString &objName)
{
   int frame = mLastIndex;
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
      int index = sunId * MAX_DATA * 3 + frame * 3;
      
      lightPos[0] = -mObjectViewPos[index+0];
      lightPos[1] = -mObjectViewPos[index+1];
      lightPos[2] = -mObjectViewPos[index+2];
      // opposite direction with  sun earth line
      //lightPos[0] = mObjectViewPos[index+0];
      //lightPos[1] = mObjectViewPos[index+1];
      //lightPos[2] = mObjectViewPos[index+2];
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
      ("   mObjectTextureIdMap[%s]=%d\n", objName.c_str(),
       mObjectTextureIdMap[objName]);
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
      
      if (pSolarSystem)
      {         
         Real mha = 0.0;
         
         if (initialLong < 180.0)
            initialLong = -initialLong - offset;
         
         CelestialBody *earth = pSolarSystem->GetBody("Earth");
         if (earth)
            mha = earth->GetHourAngle(mTime[frame]);
         
         earthRotAngle = mha + initialLong + offset;
         
         #if DEBUG_TRAJCANVAS_DRAW > 1
         if (!mIsEndOfRun)
            MessageInterface::ShowMessage
               ("   frame=%3d, mha=%12.6f, initialLong=%12.6f, earthRotAngle=%12.6f\n",
                frame, mha, initialLong, earthRotAngle);
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
         MessageInterface::ShowMessage("   earthRotAngle=%f\n", earthRotAngle);
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
      
      glDisable(GL_TEXTURE_2D);
      
      //----------------------------------------------------
      // draw grid on option
      //----------------------------------------------------
      if (mDrawGrid && objName == "Earth")
      {
         // This makes lines thicker
         //glEnable(GL_LINE_SMOOTH);
         //glLineWidth(1.5);
         
         // Just draw a wireframe sphere little bigger to show grid
         //glColor3f(0.20, 0.20, 0.50); // dark blue
         glColor3f(0.0, 0.0, 0.0);      // black
         //glColor3f(0.50, 0.10, 0.20); // maroon
         GLdouble radius = mObjectRadius[objId] + mObjectRadius[objId] * 0.03;
         DrawSphere(radius, 36, 18, GLU_LINE, GLU_OUTSIDE, GL_NONE, GL_FALSE);
      }
   }
   else
   {
      #if DEBUG_TRAJCANVAS_DRAW
      MessageInterface::ShowMessage
         ("*** WARNING *** TrajPlotCanvas::DrawObject() %s texture not found.\n",
          objName.c_str());
      #endif
      
      // Just draw a wireframe sphere if we get here
      glColor3f(0.20f, 0.20f, 0.50f);
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
//  void DrawObjectOrbit()
//------------------------------------------------------------------------------
/**
 * Draws object orbit and object at the frame number. The frame is the index
 * of the data buffer which starts at 0.
 *
 * @param  frame  Frame number to be used for drawing
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawObjectOrbit()
{
   int endFrame = mLastIndex;
   int objId;
   wxString objName;
   
   #ifdef ENABLE_LIGHT_SOURCE
   if (mEnableLightSource && mSunPresent)
   {
      // we don't want the orbit paths lit
      glDisable(GL_LIGHTING);
   }
   #endif
   
   ComputeActualIndex();
   
   for (int obj=0; obj<mObjectCount; obj++)
   {
      objName = mObjectNames[obj];
      objId = GetObjectId(objName);
      mObjLastFrame[objId] = 0;
      
      #if DEBUG_TRAJCANVAS_DRAW
      MessageInterface::ShowMessage
         ("DrawObjectOrbit() obj=%d, objId=%d, objName=%s\n", obj, objId,
          objName.c_str());
      #endif
      
      // If not showing orbit or object, continue to next one
      if (!mDrawOrbitFlag[objId*MAX_DATA+endFrame] &&
          !mShowOrbitNormalMap[objName])
         continue;
      
      // always draw orbit trajectory
      DrawOrbit(objName, obj, objId);
      
      //---------------------------------------------------------
      // draw object orbit normal vector
      // (loj: 6/13/05 For now it only draws spacecraft orbit normal vector.)
      //---------------------------------------------------------
      if (mShowOrbitNormalMap[objName])
      {
         DrawOrbitNormal(objName, obj, objId);
      }
      
      //---------------------------------------------------------
      //draw object with texture
      //---------------------------------------------------------      
      if (mShowObjectMap[objName])
      {            
         DrawObjectTexture(objName, obj, objId);
      }
   }
} // end DrawObjectOrbit()


//------------------------------------------------------------------------------
// void DrawOrbit(const wxString &objName, int obj, int objId)
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawOrbit(const wxString &objName, int obj, int objId)
{   
   glPushMatrix();
   glBegin(GL_LINES);
   
   #ifdef DEBUG_ORBIT_LINES
   MessageInterface::ShowMessage
      ("==========> TrajPlotCanvas::DrawOrbit() objName='%s', drawing first part\n",
       objName.c_str());
   #endif
   
   // Draw first part from the ring buffer
   for (int i = mRealBeginIndex1 + 1; i <= mRealEndIndex1; i++)
   {
      DrawOrbitLines(i, objName, obj, objId);
   }
   
   // Draw second part from the ring buffer
   if (mEndIndex2 != -1 && mBeginIndex1 != mBeginIndex2)
   {
      #ifdef DEBUG_ORBIT_LINES
      MessageInterface::ShowMessage
         ("==========> TrajPlotCanvas::DrawOrbit() objName='%s', drawing second part\n",
          objName.c_str());
      #endif
      
      for (int i = mRealBeginIndex2 + 1; i <= mRealEndIndex2; i++)
      {
         DrawOrbitLines(i, objName, obj, objId);
      }
   }
   
   glEnd();
   glPopMatrix();
}


//------------------------------------------------------------------------------
// void DrawOrbitLines(int i, const wxString &objName, int obj, int objId)
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawOrbitLines(int i, const wxString &objName, int obj,
                                    int objId)
{
   int index1 = 0, index2 = 0;
   
   #ifdef DEBUG_ORBIT_LINES
   MessageInterface::ShowMessage
      ("==> mTime[%3d]=%f, mTime[%3d]=%f\n", i, mTime[i], i-1, mTime[i-1]);
   #endif
   
   // Draw object orbit line based on points
   if ((mTime[i] > mTime[i-1]) ||
       (i>2 && mTime[i] < mTime[i-1]) && mTime[i-1] < mTime[i-2]) //for backprop
   {
      index1 = objId * MAX_DATA * 3 + (i-1) * 3;
      index2 = objId * MAX_DATA * 3 + i * 3;
      
      Rvector3 r1(mObjectViewPos[index1+0], mObjectViewPos[index1+1],
                  mObjectViewPos[index1+2]);
      
      Rvector3 r2(mObjectViewPos[index2+0], mObjectViewPos[index2+1],
                  mObjectViewPos[index2+2]);
      
      // if object position magnitude is 0, skip
      if (r1.GetMagnitude() == 0.0 || r2.GetMagnitude() == 0.0)
         return;
      
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
            ("   plot=%s, i1=%d, i2=%d, time1=%f, time2=%f\n   r1=%s, r2=%s\n",
             GetName().c_str(), i-1, i, mTime[i-1], mTime[i], r1.ToString().c_str(),
             r2.ToString().c_str());
         #endif
         return;
      }
      #endif
      
      // If drawing orbit lines
      int colorIndex = objId * MAX_DATA + i;
      if (mDrawOrbitFlag[colorIndex])
      {
         if (mObjectArray[obj]->IsOfType(Gmat::SPACECRAFT))
            *sIntColor = mObjectOrbitColor[colorIndex];
         else
            *sIntColor = mObjectColorMap[objName].GetIntColor();
         
         glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);
         
         // Dunn recommends removing these minus signs to correct attitude!
         //glVertex3f((-mObjectViewPos[index1+0]),
         //           (-mObjectViewPos[index1+1]),
         //           ( mObjectViewPos[index1+2]));
         glVertex3f((mObjectViewPos[index1+0]),
                    (mObjectViewPos[index1+1]),
                    (mObjectViewPos[index1+2]));
         
         // Dunn recommends removing these minus signs to correct attitude!
         //glVertex3f((-mObjectViewPos[index2+0]),
         //           (-mObjectViewPos[index2+1]),
         //           ( mObjectViewPos[index2+2]));
         glVertex3f((mObjectViewPos[index2+0]),
                    (mObjectViewPos[index2+1]),
                    (mObjectViewPos[index2+2]));
         
         #ifdef DEBUG_ORBIT_LINES
         MessageInterface::ShowMessage
            ("==> index2=%d, color=%u, %f, %f, %f\n", index2, *sIntColor,
             mObjectViewPos[index2+0], mObjectViewPos[index2+1],
             mObjectViewPos[index2+2]);
         #endif
      }
      
      // save last valid frame to show object at final frame
      mObjLastFrame[objId] = i;
      
      #ifdef DEBUG_ORBIT_LINES
      MessageInterface::ShowMessage("==> mObjLastFrame[%d] = %d\n", objId, i);
      #endif
   }
}


//------------------------------------------------------------------------------
// void DrawOrbitNormal(const wxString &objName, int obj, int objId)
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawOrbitNormal(const wxString &objName, int obj, int objId)
{
   int numSkip = mTotalPoints / 50;
   
   // Draw first part from the ring buffer
   for (int i = mRealBeginIndex1; i <= mRealEndIndex1; i++)
   {
      if (numSkip <= 0 || i % numSkip != 0)
         continue;
      
      DrawOrbitNormalLines(i, objName, obj, objId);
   }
   
   // Draw second part from the ring buffer
   if (mEndIndex2 != -1 && mBeginIndex1 != mBeginIndex2)
   {
      for (int i = mRealBeginIndex2; i <= mRealEndIndex2; i++)
      {
         if (numSkip <= 0 || i % numSkip != 0)
            continue;
         
         DrawOrbitNormalLines(i, objName, obj, objId);
      }
   }
}


//------------------------------------------------------------------------------
// void DrawOrbitNormalLines()
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawOrbitNormalLines(int i, const wxString &objName,
                                          int obj, int objId)
{
   int index1 = 0, index2 = 0, index3 = 0;
   UnsignedInt color;
   
   if ((mTime[i] > mTime[i-1]) ||
       (i>2 && mTime[i] < mTime[i-1]) && mTime[i-1] < mTime[i-2])
   {
      index1 = objId * MAX_DATA * 3 + (i-1) * 3;
      index2 = objId * MAX_DATA * 3 + i * 3;
      
      Rvector3 r1(mObjectViewPos[index1+0],
                  mObjectViewPos[index1+1],
                  mObjectViewPos[index1+2]);
      
      Rvector3 r2(mObjectViewPos[index2+0],
                  mObjectViewPos[index2+1],
                  mObjectViewPos[index2+2]);
      
      // if object position magnitude is 0, skip
      if (r1.GetMagnitude() == 0.0 || r2.GetMagnitude() == 0.0)
         return;
      
      glPushMatrix();
      
      // move to origin
      index3 = mOriginId * MAX_DATA * 3 + i * 3;
      
      // Dunn recommends removing these minus signs to correct attitude!
      //glTranslatef(-mObjectViewPos[index3+0],
      //             -mObjectViewPos[index3+1],
      //              mObjectViewPos[index3+2]);
      glTranslatef(mObjectViewPos[index3+0],
                   mObjectViewPos[index3+1],
                   mObjectViewPos[index3+2]);
      
      if (mObjectArray[obj]->IsOfType(Gmat::SPACECRAFT))
         color = mObjectOrbitColor[objId*MAX_DATA+i];
      else
         color = mObjectColorMap[objName].GetIntColor();
      
      DrawObjectOrbitNormal(objId, i, color);
      glPopMatrix();
   }
}


//------------------------------------------------------------------------------
// void DrawObjectTexture(const wxString &objName, int obj, int objId)
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawObjectTexture(const wxString &objName, int obj, int objId)
{
   if (mNumData < 1)
      return;
   
   int index1 = objId * MAX_DATA * 3 + mObjLastFrame[objId] * 3;
   
   #if DEBUG_TRAJCANVAS_DRAW
   MessageInterface::ShowMessage
      ("   objName=%s, objId=%d, lastFrame=%d\n", objName.c_str(), objId,
       mObjLastFrame[objId]);
   MessageInterface::ShowMessage
      ("   mObjectViewPos=%f, %f, %f\n",
       mObjectViewPos[index1+0], mObjectViewPos[index1+1],
       mObjectViewPos[index1+2]);
   #endif
   
   glPushMatrix();
   
   // put object at final position
   // Dunn recommends removing these minus signs to correct attitude!
   //glTranslatef(-mObjectViewPos[index1+0],
   //             -mObjectViewPos[index1+1],
   //              mObjectViewPos[index1+2]);
   glTranslatef(mObjectViewPos[index1+0],
                mObjectViewPos[index1+1],
                mObjectViewPos[index1+2]);
   
   // first disable GL_TEXTURE_2D to show lines clearly
   // without this, lines are drawn dim (loj: 2007.06.11)
   glDisable(GL_TEXTURE_2D);
   
   if (mObjectArray[obj]->IsOfType(Gmat::SPACECRAFT))
      DrawSpacecraft(mObjectOrbitColor[objId*MAX_DATA+mObjLastFrame[objId]]);
   else
      DrawObject(objName);
   
   glPopMatrix();
}


//------------------------------------------------------------------------------
//  void DrawSolverData()
//------------------------------------------------------------------------------
/**
 * Draws solver iteration data
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawSolverData()
{

   int numPoints = mSolverAllPosX.size();
   //MessageInterface::ShowMessage("==========> solver points = %d\n", numPoints);
   
   if (numPoints == 0)
      return;
   
   
   glPushMatrix();
   glBegin(GL_LINES);
   
   for (int i=1; i<numPoints; i++)
   {
      int numSc = mSolverAllPosX[i].size();      
      //MessageInterface::ShowMessage("==========> sc count = %d\n", numSc);
      
      //---------------------------------------------------------
      // draw lines
      //---------------------------------------------------------
      for (int sc=0; sc<numSc; sc++)
      {
         *sIntColor = mSolverIterColorArray[sc];         
         glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);
         
         // Dunn recommends removing these minus signs to correct attitude!
         //glVertex3f(-mSolverAllPosX[i-1][sc], -mSolverAllPosY[i-1][sc], mSolverAllPosZ[i-1][sc]); 
         //glVertex3f(-mSolverAllPosX[i][sc], -mSolverAllPosY[i][sc], mSolverAllPosZ[i][sc]);
         glVertex3f(mSolverAllPosX[i-1][sc], mSolverAllPosY[i-1][sc], mSolverAllPosZ[i-1][sc]); 
         glVertex3f(mSolverAllPosX[i][sc], mSolverAllPosY[i][sc], mSolverAllPosZ[i][sc]);
      }
   }
   
   glEnd();
   glPopMatrix();
   
}


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
   
   int index = objId * MAX_DATA * 3 + frame * 3;
   Rvector3 r(mObjectViewPos[index+0], mObjectViewPos[index+1], mObjectViewPos[index+2]);            
   Rvector3 v(mObjectViewVel[index+0], mObjectViewVel[index+1], mObjectViewVel[index+2]);
   
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
   // Dunn recommends removing these minus signs to correct attitude!
   // Also multiply by 1.2 to get end of normal a little above earth surface.
   //endPos[0] = -normV[0] * distance;
   //endPos[1] = -normV[1] * distance;
   //endPos[2] =  normV[2] * distance;
   endPos[0] = normV[0] * distance * 1.2;
   endPos[1] = normV[1] * distance * 1.2;
   endPos[2] = normV[2] * distance * 1.2;
   
   glVertex3f(0.0, 0.0, 0.0);
   glVertex3f(endPos[0], endPos[1], endPos[2]);
   
   glEnd();
   
   // Show Orbit Normal direction text
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
         GmatMathUtil::PI / 180.0;
   
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

   // Dunn recommends changing 23.5 to -23.5.  When he changed -1 to 1 or +1 he 
   // got an RVector3 error.  This negative obliquity of the ecliptic around the
   // negative ECI X-Axis aligns the plane of the ecliptic with the sun line
   // after all minus signs for position have been removed.
   //glRotatef(23.5, -1, 0, 0);
   glRotatef(-23.5, -1, 0, 0);
   DrawEquatorialPlane(color);
   glPopMatrix();
} // end DrawEclipticPlane()


//------------------------------------------------------------------------------
//  void DrawSunLine()
//------------------------------------------------------------------------------
/**
 * Draws Origin to Sun lines.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawSunLine()
{
   //int frame = mNumData - 1;
   int frame = mLastIndex;
   if (frame <= 0)
      return;
   
   int sunId = GetObjectId("Sun");
   
   // check for Sun
   if (sunId == UNKNOWN_OBJ_ID)
      return;
   
   Real originPos[3], sunPos[3];
   Real distance = (Real)mAxisLength;
   Real mag;
   
   //--------------------------------
   // draw sun line
   //--------------------------------
   
   // set color
   *sIntColor = mSunLineColor;
   glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);
   
   glBegin(GL_LINES);
   int index = 0;
   
   // draw one line from origin to Sun
   // Dunn recommends setting originPos to zero.  Have sun line only come out
   // the side of the earth in the direction of the sun.
   index = mOriginId * MAX_DATA * 3 + frame * 3;
   //originPos[0] = -mObjectViewPos[index+0];
   //originPos[1] = -mObjectViewPos[index+1];
   //originPos[2] =  mObjectViewPos[index+2];
   originPos[0] = 0.0;
   originPos[1] = 0.0;
   originPos[2] = 0.0;
   
   index = sunId * MAX_DATA * 3 + frame * 3;
   // Dunn recommends removing these minus signs to correct attitude!
   //sunPos[0] = -mObjectViewPos[index+0];
   //sunPos[1] = -mObjectViewPos[index+1];
   //sunPos[2] =  mObjectViewPos[index+2];
   sunPos[0] = mObjectViewPos[index+0];
   sunPos[1] = mObjectViewPos[index+1];
   sunPos[2] = mObjectViewPos[index+2];
   
   // show lines between Sun and Earth and to -Sun
   glVertex3f(originPos[0], originPos[1], originPos[2]);
   glVertex3f(sunPos[0], sunPos[1], sunPos[2]);

   // Dunn recommends removing following two lines if we decide we don't need
   // negative sunline.
   //glVertex3f(originPos[0], originPos[1], originPos[2]);
   //glVertex3f(-sunPos[0], -sunPos[1], -sunPos[2]);
   
   glEnd();
   
   // Show Sun direction text
   glColor3f(1.0, 1.0, 0.0); // yellow
   
   // get sun unit vector and multiply by distance
   mag = sqrt(sunPos[0]*sunPos[0] + sunPos[1]*sunPos[1] + sunPos[2]*sunPos[2]);
   DrawStringAt(" +S", sunPos[0]/mag*distance/2.2,
                sunPos[1]/mag*distance/2.2,
                sunPos[2]/mag*distance/2.2);
   
} // end DrawSunLine()


//---------------------------------------------------------------------------
// void DrawAxes()
//---------------------------------------------------------------------------
void TrajPlotCanvas::DrawAxes()
{
   wxString axisLabel;
   GLfloat viewDist;

   glLineWidth(2.0);
   
   //-----------------------------------
   // draw axes
   //-----------------------------------
   
   //viewDist = mCurrViewDist/2; //zooms in and out
   viewDist = mAxisLength/2.2; // stays the same
   glBegin(GL_LINES);
   
   // Dunn recommends changing colors as follows.
   // X=red, Y=green, Z=blue
   //glColor3f(0, 1, 0);   // x
   glColor3f(1, 0, 0);   // x (red)
   glVertex3f(-viewDist, 0, 0);
   glVertex3f( viewDist, 0, 0);
   
   //glColor3f(0, 0, 1);   // y
   glColor3f(0, 1, 0);   // y (green)
   glVertex3f(0, -viewDist, 0);
   glVertex3f(0,  viewDist, 0);
   
   //glColor3f(1, 1, 0);   // z
   glColor3f(0, 0, 1);   // z (blue)
   glVertex3f(0, 0, -viewDist);
   glVertex3f(0, 0, viewDist);
   
   glEnd();
   
   //-----------------------------------
   // throw some text out...
   //-----------------------------------
   // Dunn recommends changing colors as follows.
   // X=red, Y=green, Z=blue
   // Dunn recommends making "viewDist" positive in all calls below.  This will
   // get axis labels on the correct, positive, end of each axis.
   //glColor3f(0, 1, 0);   // green
   glColor3f(1, 0, 0);   // red
   axisLabel = "+X " + mViewCoordSysName;
   //DrawStringAt(axisLabel, -viewDist, 0.0, 0.0);
   DrawStringAt(axisLabel, viewDist, 0.0, 0.0);
   
   //glColor3f(0, 0, 1);   // blue
   glColor3f(0, 1, 0);   // green
   axisLabel = "+Y " + mViewCoordSysName;
   //DrawStringAt(axisLabel, 0.0, -viewDist, 0.0);
   DrawStringAt(axisLabel, 0.0, viewDist, 0.0);
   
   //glColor3f(1, 1, 0);   // yellow
   glColor3f(0, 0, 1);   // blue
   axisLabel = "+Z " + mViewCoordSysName;
   DrawStringAt(axisLabel, 0.0, 0.0, viewDist);
   
   glLineWidth(1.0);
   
}


//---------------------------------------------------------------------------
// void DrawStatus(const wxString &label1, int frame, const wxString &label2,
//                 double time, int xpos = 0, int ypos = 0,
//                 const wxString &label3 = "")
//---------------------------------------------------------------------------
/*
 * Writes status at the bottom of the frame
 */
//---------------------------------------------------------------------------
void TrajPlotCanvas::DrawStatus(const wxString &label1, int frame,
                                const wxString &label2, double time,
                                int xpos, int ypos, const wxString &label3)
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
   wxString str, str1;
   wxString text;
   
   #ifdef DEBUG_DRAW_STATUS
   str.Printf("%d", frame);
   text = label1 + str;
   str1.Printf("%f - ", time);
   #endif
   
   if (time > 0)
   {
      Real toMjd = -999;
      std::string utcGregorian;
      TimeConverterUtil::Convert("A1ModJulian", time, "", "UTCGregorian",
                                 toMjd, utcGregorian, 1);
      str = utcGregorian.c_str();
   }
   
   text = text + label2 + str1 + str;
   
   glColor3f(1, 1, 0); //yellow
   glRasterPos2i(xpos, ypos);
   glCallLists(strlen(text.c_str()), GL_BYTE, (GLubyte*)text.c_str());
   
   if (label3 != "")
   {
      glRasterPos2i(xpos, 50);
      glCallLists(strlen(label3.c_str()), GL_BYTE, (GLubyte*)label3.c_str());
   }
   
   //wxLogStatus(GmatAppData::Instance()->GetMainFrame(), wxT("Frame#: %d, Time: %f"), frame,
   //            mTime[frame]);
   
}


//---------------------------------------------------------------------------
// void ApplyEulerAngles()
//---------------------------------------------------------------------------
void TrajPlotCanvas::ApplyEulerAngles()
{
   //MessageInterface::ShowMessage("==> TrajPlotCanvas::ApplyEulerAngles()\n");
   
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


//------------------------------------------------------------------------------
// void TrajPlotCanvas::ClearObjectArrays(bool deleteArrays = true)
//------------------------------------------------------------------------------
void TrajPlotCanvas::ClearObjectArrays(bool deleteArrays)
{
   #if DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage("TrajPlotCanvas::ClearObjectArrays() entered\n");
   #endif

   if (deleteArrays)
   {
      if (mObjectRadius)
         delete [] mObjectRadius;
      
      if (mObjMaxZoomIn)
         delete [] mObjMaxZoomIn;
      
      if (mObjLastFrame)
         delete [] mObjLastFrame;
      
      if (mDrawOrbitFlag)
         delete [] mDrawOrbitFlag;
      
      if (mObjectOrbitColor)
         delete [] mObjectOrbitColor;
      
      if (mObjectGciPos)
         delete [] mObjectGciPos;
      
      if (mObjectGciVel)
         delete [] mObjectGciVel;
      
      if (mObjectViewPos)
         delete [] mObjectViewPos;
      
      if (mObjectViewVel)
         delete [] mObjectViewVel;
   }
   
   mObjectRadius = NULL;
   mObjMaxZoomIn = NULL;
   mObjLastFrame = NULL;
   mDrawOrbitFlag = NULL;
   mObjectOrbitColor = NULL;
   mObjectGciPos = NULL;
   mObjectGciVel= NULL;
   mObjectViewPos = NULL;
   mObjectViewVel= NULL;
   
   #if DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage("TrajPlotCanvas::ClearObjectArrays() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// bool TrajPlotCanvas::CreateObjectArrays()
//------------------------------------------------------------------------------
/*
 * Allocates arrays for objects.
 */
//------------------------------------------------------------------------------
bool TrajPlotCanvas::CreateObjectArrays()
{
   #if DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage
      ("CreateObjectArrays() allocating object arrays with %d\n", mObjectCount);
   #endif
   
   if ((mObjectRadius = new Real[mObjectCount]) == NULL)
      return false;
   
   for (int i=0; i<mObjectCount; i++)
      mObjectRadius[i] = 0.0;
   
   if ((mObjMaxZoomIn = new Real[mObjectCount]) == NULL)
      return false;
   
   for (int i=0; i<mObjectCount; i++)
      mObjMaxZoomIn[i] = 0.0;
   
   if ((mObjLastFrame = new int[mObjectCount]) == NULL)
      return false;
   
   if ((mDrawOrbitFlag = new bool[mObjectCount*MAX_DATA]) == NULL)
      return false;
   
   if ((mObjectOrbitColor = new UnsignedInt[mObjectCount*MAX_DATA]) == NULL)
      return false;
   
   if ((mObjectGciPos = new Real[mObjectCount*MAX_DATA*3]) == NULL)
      return false;
   
   if ((mObjectGciVel = new Real[mObjectCount*MAX_DATA*3]) == NULL)
      return false;
   
   if ((mObjectViewPos = new Real[mObjectCount*MAX_DATA*3]) == NULL)
      return false;
   
   if ((mObjectViewVel = new Real[mObjectCount*MAX_DATA*3]) == NULL)
      return false;
   
   #if DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage("TrajPlotCanvas::CreateObjectArrays() exiting\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void UpdateSolverData(RealArray posX, RealArray posY, RealArray posZ, ...)
//------------------------------------------------------------------------------
void TrajPlotCanvas
::UpdateSolverData(const RealArray &posX, const RealArray &posY, const RealArray &posZ,
                   const UnsignedIntArray &scColors, bool solving)
{
   //-----------------------------------------------------------------
   // If showing current iteration only, handle solver iteration data
   // separately here since it will be shown temporarily during the run
   //-----------------------------------------------------------------
   if (solving)
   {
      mDrawSolverData = true;
      RealArray tempSolverX, tempSolverY, tempSolverZ;
      
      for (int sc=0; sc<mScCount; sc++)
      {
         int satId = GetObjectId(mScNameArray[sc].c_str());
         if (satId != UNKNOWN_OBJ_ID)
         {
            // if we are not drawing this spacecraft, skip
            if (!mDrawOrbitArray[satId])
               continue;
            
            tempSolverX.push_back(posX[sc]);
            tempSolverY.push_back(posY[sc]);
            tempSolverZ.push_back(posZ[sc]);
         }
      }
      
      mSolverAllPosX.push_back(tempSolverX);
      mSolverAllPosY.push_back(tempSolverY);
      mSolverAllPosZ.push_back(tempSolverZ);
      mSolverIterColorArray = scColors;
   }
   else
   {
      mSolverAllPosX.clear();
      mSolverAllPosY.clear();
      mSolverAllPosZ.clear();
   }
}


//---------------------------------------------------------------------------
// void UpdateSpacecraftData(const RealArray &posX, const RealArray &posY, ...)
//---------------------------------------------------------------------------
void TrajPlotCanvas
::UpdateSpacecraftData(const Real &time,
                       const RealArray &posX, const RealArray &posY,
                       const RealArray &posZ, const RealArray &velX,
                       const RealArray &velY, const RealArray &velZ,
                       const UnsignedIntArray &scColors,
                       Integer solverOption)
{
   #if DEBUG_TRAJCANVAS_UPDATE
   static int sNumDebugOutput = 1000;
   #endif
   
   //-------------------------------------------------------
   // update spacecraft position
   //-------------------------------------------------------
   for (int sc=0; sc<mScCount; sc++)
   {
      int satId = GetObjectId(mScNameArray[sc].c_str());
      
      #if DEBUG_TRAJCANVAS_UPDATE
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::UpdateSpacecraftData() satId=%d, scName=%s\n", satId,
          mObjectNames[satId].c_str());
      #endif
      
      if (satId != UNKNOWN_OBJ_ID)
      {
         int colorIndex = satId * MAX_DATA + mLastIndex;
         
         if (!mDrawOrbitArray[satId])
         {
            mDrawOrbitFlag[colorIndex] = false;
            MessageInterface::ShowMessage("===> mDrawOrbitArray[satId] is NULL\n");
            continue;
         }
         
         mDrawOrbitFlag[colorIndex] = true;
         
         // If drawing solver's current iteration only, we don't want to draw
         // first 3 points since these points have solver data.
         if (mDrawSolverData || (solverOption == 1 && mNumData == 2))
            mDrawOrbitFlag[colorIndex] = false;
         
         mObjectOrbitColor[colorIndex] = scColors[sc];
         
         int posIndex = satId * MAX_DATA * 3 + (mLastIndex*3);
         mObjectViewPos[posIndex+0] = posX[sc];
         mObjectViewPos[posIndex+1] = posY[sc];
         mObjectViewPos[posIndex+2] = posZ[sc];            
         mObjectViewVel[posIndex+0] = velX[sc];
         mObjectViewVel[posIndex+1] = velY[sc];
         mObjectViewVel[posIndex+2] = velZ[sc];
         
         #if DEBUG_TRAJCANVAS_UPDATE
         if (mNumData < sNumDebugOutput)
         {
            MessageInterface::ShowMessage
               ("   satId=%d, object=%s, colorIndex=%u, color=%u\n", satId,
                mObjectNames[satId].c_str(), colorIndex, mObjectOrbitColor[colorIndex]);
            MessageInterface::ShowMessage
               ("   satId:%d posIndex=%d, gcipos = %f, %f, %f\n", satId,
                posIndex, mObjectViewPos[posIndex+0], mObjectViewPos[posIndex+1],
                mObjectViewPos[posIndex+2]);
         }
         #endif
         
         // if need to convert to internal coordiante system (EarthMJ2000Eq)
         if (mViewCsIsInternalCs)
         {
            CopyVector3(&mObjectGciPos[posIndex], &mObjectViewPos[posIndex]);
            CopyVector3(&mObjectGciVel[posIndex], &mObjectViewVel[posIndex]);
         }
         else
         {
            Rvector6 satState(posX[sc], posY[sc], posZ[sc], velX[sc], velY[sc], velZ[sc]);
            Rvector6 outState;
            
            mCoordConverter.Convert(time, satState, pViewCoordSystem,
                                    outState, pInternalCoordSystem);
            
            mObjectGciPos[posIndex+0] = outState[0];
            mObjectGciPos[posIndex+1] = outState[1];
            mObjectGciPos[posIndex+2] = outState[2];                  
            mObjectGciVel[posIndex+0] = outState[3];
            mObjectGciVel[posIndex+1] = outState[4];
            mObjectGciVel[posIndex+2] = outState[5];
         }
         
         #if DEBUG_TRAJCANVAS_UPDATE
         if (mNumData < sNumDebugOutput)
         {
            MessageInterface::ShowMessage
               ("   satId:%d posIndex=%d, tmppos = %f, %f, %f\n", satId, posIndex,
                mObjectViewPos[posIndex+0], mObjectViewPos[posIndex+1],
                mObjectViewPos[posIndex+2]);
         }
         #endif
         
         #if DEBUG_TRAJCANVAS_UPDATE > 1
         if (mNumData < sNumDebugOutput)
         {
            MessageInterface::ShowMessage
               ("   satId:%d tmpvel = %f, %f, %f\n", satId,
                mObjectViewVel[posIndex+0], mObjectViewVel[posIndex+1],
                mObjectViewVel[posIndex+2]);
         }
         #endif
      }
   }
}


//------------------------------------------------------------------------------
// void UpdateOtherData(const Real &time)
//------------------------------------------------------------------------------
void TrajPlotCanvas::UpdateOtherData(const Real &time)
{
   for (int obj=0; obj<mObjectCount; obj++)
   {
      // if object pointer is not NULL and not a spacecraft
      if (mObjectArray[obj] && mObjectArray[obj]->GetType() != Gmat::SPACECRAFT)
      {
         int objId = GetObjectId(mObjectNames[obj]);
         
         #if DEBUG_TRAJCANVAS_UPDATE_OBJECT
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::UpdateOtherData() objId=%d, obj=%s\n", objId,
             mObjectNames[objId].c_str());
         #endif
         
         // if object id found, get object's state
         if (objId != UNKNOWN_OBJ_ID)
         {
            // Commented out continue since we still need to get object's
            // position for viewpoint or view direction object (LOJ: 2010.05.11)
            if (!mDrawOrbitArray[objId])
            {
               mDrawOrbitFlag[objId * MAX_DATA + mLastIndex] = false;
               //continue;
            }
            
            int colorIndex = objId * MAX_DATA + mLastIndex;
            mDrawOrbitFlag[colorIndex] = true;               
            Rvector6 objState = mObjectArray[obj]->GetMJ2000State(time);
            
            int posIndex = objId * MAX_DATA * 3 + (mLastIndex*3);
            mObjectGciPos[posIndex+0] = objState[0];
            mObjectGciPos[posIndex+1] = objState[1];
            mObjectGciPos[posIndex+2] = objState[2];
            mObjectGciVel[posIndex+0] = objState[3];
            mObjectGciVel[posIndex+1] = objState[4];
            mObjectGciVel[posIndex+2] = objState[5];
            
            #if DEBUG_TRAJCANVAS_UPDATE_OBJECT > 1
            MessageInterface::ShowMessage
               ("TrajPlotCanvas::UpdateOtherData() %s, posIndex=%d, objState=%s\n",
                mObjectNames[obj].c_str(), posIndex, objState.ToString().c_str());
            #endif
            
            // convert objects to view CoordinateSystem
            if (mViewCsIsInternalCs)
            {
               CopyVector3(&mObjectViewPos[posIndex], &mObjectGciPos[posIndex]);
               CopyVector3(&mObjectViewVel[posIndex], &mObjectGciVel[posIndex]);
            }
            else
            {
               Rvector6 outState;
                  
               mCoordConverter.Convert(time, objState, pInternalCoordSystem,
                                       outState, pViewCoordSystem);
                  
               mObjectViewPos[posIndex+0] = outState[0];
               mObjectViewPos[posIndex+1] = outState[1];
               mObjectViewPos[posIndex+2] = outState[2];                  
               mObjectViewVel[posIndex+0] = outState[3];
               mObjectViewVel[posIndex+1] = outState[4];
               mObjectViewVel[posIndex+2] = outState[5];
            }
            
            #if DEBUG_TRAJCANVAS_UPDATE_OBJECT > 1
            MessageInterface::ShowMessage
               ("    %s posIndex=%d, tmppos = %f, %f, %f\n", mObjectNames[obj].c_str(),
                posIndex, mObjectViewPos[posIndex+0], mObjectViewPos[posIndex+1],
                mObjectViewPos[posIndex+2]);
            #endif
            
         }
         else
         {
            #if DEBUG_TRAJCANVAS_UPDATE_OBJECT > 1
            MessageInterface::ShowMessage
               ("TrajPlotCanvas::UpdateOtherData() Cannot Add data. Invalid objId=%d\n",
                objId);
            #endif
         }
      }
      else
      {
         #if DEBUG_TRAJCANVAS_UPDATE_OBJECT > 1
         if (mObjectArray[obj] == NULL)
         {
            MessageInterface::ShowMessage
               ("TrajPlotCanvas::UpdateOtherData() Cannot add data. %s is NULL\n",
                mObjectNames[obj].c_str());
         }
         #endif
      }
   }
}


//---------------------------------------------------------------------------
// bool TiltOriginZAxis()
//---------------------------------------------------------------------------
bool TrajPlotCanvas::TiltOriginZAxis()
{
   if (mNumData == 0)
      return false;
   
   if (pInternalCoordSystem == NULL || pViewCoordSystem == NULL)
      return false;

   std::string axisTypeName =
      pViewCoordSystem->GetRefObject(Gmat::AXIS_SYSTEM, "")->GetTypeName();
   
   #if DEBUG_TRAJCANVAS_DRAW
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::TiltOriginZAxis() AxisTypeName=%s\n", axisTypeName.c_str());
   #endif
   
   // rotate earth Z axis if view CS is EarthMJ2000Ec
   if (pViewCoordSystem->GetName() == "EarthMJ2000Ec")
   {
      Rvector6 inState, outState;
      
      inState.Set(0.0, 0.0, 1.0, 0.0, 0.0, 0.0);
      
      mCoordConverter.Convert(mTime[0], inState, pInternalCoordSystem,
                              outState, pViewCoordSystem);
      
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
// void UpdateRotateFlags()
//---------------------------------------------------------------------------
void TrajPlotCanvas::UpdateRotateFlags()
{
   AxisSystem *axis =
      (AxisSystem*)pViewCoordSystem->GetRefObject(Gmat::AXIS_SYSTEM, "");

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


//---------------------------------------------------------------------------
// bool ConvertObjectData()
//---------------------------------------------------------------------------
bool TrajPlotCanvas::ConvertObjectData()
{
   if (pInternalCoordSystem == NULL || pViewCoordSystem == NULL)
      return false;
   
   Rvector6 inState, outState;
   
   #if DEBUG_TRAJCANVAS_CONVERT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::ConvertObjectData() internalCS=%s, viewCSName=%s, viewCS=%d\n",
       pInternalCoordSystem->GetName().c_str(), pViewCoordSystem->GetName().c_str(),
       pViewCoordSystem);
   #endif
   
   // do not convert if view CS is internal CS
   if (mViewCsIsInternalCs)
   {
      #if DEBUG_TRAJCANVAS_CONVERT
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::ConvertObjectData() No conversion is needed. "
          "Just copy MJ2000 pos\n");
      #endif
      for (int obj=0; obj<mObjectCount; obj++)
      {
         int objId = GetObjectId(mObjectNames[obj]);
         int index = 0;
         
         // Draw first part from the ring buffer
         for (int i = mRealBeginIndex1 + 1; i <= mRealEndIndex1; i++)
         {
            index = objId * MAX_DATA * 3 + i * 3;
            CopyVector3(&mObjectViewPos[index], &mObjectGciPos[index]);
         }
         
         // Draw second part from the ring buffer
         if (mEndIndex2 != -1 && mBeginIndex1 != mBeginIndex2)
         {
            for (int i = mRealBeginIndex2 + 1; i <= mRealEndIndex2; i++)
            {
               index = objId * MAX_DATA * 3 + i * 3;
               CopyVector3(&mObjectViewPos[index], &mObjectGciPos[index]);
            }
         }
      }
   }
   else
   {
      for (int obj=0; obj<mObjectCount; obj++)
      {
         int objId = GetObjectId(mObjectNames[obj]);
         
         #if DEBUG_TRAJCANVAS_CONVERT
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::ConvertObjectData() mObjectNames[%d]=%s\n", objId,
             mObjectNames[obj].c_str());
         #endif         
         
         // Draw first part from the ring buffer
         for (int i = mRealBeginIndex1 + 1; i <= mRealEndIndex1; i++)
            ConvertObject(objId, i);
         
         // Draw second part from the ring buffer
         if (mEndIndex2 != -1 && mBeginIndex1 != mBeginIndex2)
         {
            for (int i = mRealBeginIndex2 + 1; i <= mRealEndIndex2; i++)
               ConvertObject(objId, i);
         }
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
   int start = objId*MAX_DATA*3+index*3;
   inState.Set(mObjectGciPos[start+0], mObjectGciPos[start+1],
               mObjectGciPos[start+2], mObjectGciVel[start+0],
               mObjectGciVel[start+1], mObjectGciVel[start+2]);
   
   mCoordConverter.Convert(mTime[index], inState, pInternalCoordSystem,
                           outState, pViewCoordSystem);
   
   mObjectViewPos[index+0] = outState[0];
   mObjectViewPos[index+1] = outState[1];
   mObjectViewPos[index+2] = outState[2];
   mObjectViewVel[index+0] = outState[3];
   mObjectViewVel[index+1] = outState[4];
   mObjectViewVel[index+2] = outState[5];
   
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
   {   
      // model view matrix
      static GLfloat sViewMat[16];
      
      glGetFloatv(GL_MODELVIEW_MATRIX, sViewMat);
      
      // OpenGL stores matrix in column major: ith column, jth row.
      
      Rmatrix33 mvmat(sViewMat[0], sViewMat[1], sViewMat[2],
                      sViewMat[4], sViewMat[5], sViewMat[6],
                      sViewMat[8], sViewMat[9], sViewMat[10]);
      
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
         ("TrajPlotCanvas::ComputeEulerAngles() mvmat=%s\n",
          mvmat.ToString().c_str());
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::ComputeEulerAngles() finalMat=%s\n",
          finalMat.ToString().c_str());
      #endif
   }
   #else
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
             e.GetFullMessage().c_str());
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
          e.GetFullMessage().c_str());
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
   if (pSolarSystem)
   {
      Real raRad = ATan(y, x);
      Real raDeg = RadToDeg(raRad, true);
      CelestialBody *earth = pSolarSystem->GetBody("Earth");
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
// bool LoadImage(const std::string &fileName)
//---------------------------------------------------------------------------
bool TrajPlotCanvas::LoadImage(const std::string &fileName)
{
#ifndef SKIP_DEVIL
   return false;
   
#else
   #if DEBUG_TRAJCANVAS_INIT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::LoadImage() Not using DevIL. file='%s'\n", fileName.c_str());
   #endif
   
   if (fileName == "")
      return false;

   // Moved to GmatApp (loj: 2009.01.08)
   //::wxInitAllImageHandlers();
   
   wxImage image = wxImage(fileName.c_str());
   int width = image.GetWidth();
   int height = image.GetHeight();
   
   GLubyte *data = image.GetData();
   
   if (data == NULL)
      return false;
   
   #if DEBUG_TRAJCANVAS_INIT
   int size = width * height * 3;
   MessageInterface::ShowMessage
      ("   width=%d, height=%d, size=%d\n", width, height, size);
   #endif
   
   // Why is image upside down?
   // Get vertial mirror
   wxImage mirror = image.Mirror(false);
   GLubyte *data1 = mirror.GetData();
   
   //used for min and magnifying texture
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   
   //pass image to opengl
   #ifndef __WXGTK__
      // This call crashes GMAT on Linux, so it is excluded here. 
      gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB,
                     GL_UNSIGNED_BYTE, data1);
   #endif
   
   return true;
#endif
}

