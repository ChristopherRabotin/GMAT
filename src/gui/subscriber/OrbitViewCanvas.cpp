//$Id$
//------------------------------------------------------------------------------
//                              OrbitViewCanvas
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// ** Legal **
//
// Developed jointly by NASA/GSFC, Thinking Systems, Inc., and Schafer Corp.,
// under AFRL NOVA Contract #FA945104D03990003
//
// Author:   Linda Jun (wrote TrajPlotCanvas)
// Created:  2003/11/25
// Modified: Phillip Silvia, Jr. and Dunning Idle 5th - Schafer Corp.
// Date:     Summer 2010
// Changes:  Took out all minus signs from trajectories and vectors so attitude
//           would work correctly.  Added attitude calls to draw spacecraft in
//           the proper orientation.  Changed the ECI axis color convention to
//           Red-Green-Blue.
/**
* These classes and methods implement OpenGL display of the earth and
* orbit trajectories.  The upgrade by Schafer allows 3D spacecraft
* models to be drawn in the correct position and attitude.
 */
//------------------------------------------------------------------------------

#include "OrbitViewCanvas.hpp"
#include "Camera.hpp"
#include "ModelObject.hpp"
#include "ModelManager.hpp"
#include "GmatAppData.hpp"         // for GetGuiInterpreter()
#include "FileManager.hpp"         // for texture files
#include "ColorTypes.hpp"          // for namespace GmatColor::
#include "Rvector3.hpp"            // for Rvector3::GetMagnitude()
#include "RealUtilities.hpp"
#include "AngleUtil.hpp"           // for ComputeAngleInDeg()
#include "CelestialBody.hpp"
#include "MdiGlPlotData.hpp"
#include "FileUtil.hpp"            // for GmatFileUtil::DoesFileExist()
#include "MessageInterface.hpp"
#include "SubscriberException.hpp"
#include "TimeSystemConverter.hpp" // for ConvertMjdToGregorian()
#include "GmatDefaults.hpp"
#include "BodyFixedAxes.hpp"
#include "Rendering.hpp"
#include "GmatOpenGLSupport.hpp"   // for OpenGL support
#include <string.h>                // for strlen( )

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


// currently lighting is not working    - PS, working on it!
#define ENABLE_LIGHT_SOURCE
// Rotating Earth using attitude seems to work, so commented out (LOJ: 2010.11.22)
//#define USE_MHA_TO_ROTATE_EARTH
// GetAttitude() from SpacePoint has been implemented, so we can ust it
#define ENABLE_OBJECT_ATTITUDE

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
//#define DEBUG_INIT 1
//#define DEBUG_UPDATE 1
//#define DEBUG_SOLVER_DATA 1
//#define DEBUG_UPDATE_OBJECT 2
//#define DEBUG_ACTION 1
//#define DEBUG_CONVERT 1
//#define DEBUG_DRAW 1
//#define DEBUG_ZOOM 1
//#define DEBUG_OBJECT 2
//#define DEBUG_TEXTURE 2
//#define DEBUG_PERSPECTIVE 1
//#define DEBUG_PROJECTION 1
//#define DEBUG_CS 1
//#define DEBUG_ANIMATION 1
//#define DEBUG_LONGITUDE 1
//#define DEBUG_SHOW_SKIP 1
//#define DEBUG_ROTATE_BODY 2
//#define DEBUG_DATA_BUFFERRING
//#define DEBUG_ORBIT_LINES

#define MODE_CENTERED_VIEW 0
#define MODE_FREE_FLYING 1
#define MODE_ASTRONAUT_6DOF 2

BEGIN_EVENT_TABLE(OrbitViewCanvas, wxGLCanvas)
   EVT_SIZE(OrbitViewCanvas::OnTrajSize)
   EVT_PAINT(OrbitViewCanvas::OnPaint)
   EVT_MOUSE_EVENTS(OrbitViewCanvas::OnMouse)
   EVT_KEY_DOWN(OrbitViewCanvas::OnKeyDown)
END_EVENT_TABLE()

using namespace GmatPlot;
using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------
const int OrbitViewCanvas::LAST_STD_BODY_ID = 10;
const int OrbitViewCanvas::MAX_COORD_SYS = 10;
const Real OrbitViewCanvas::MAX_ZOOM_IN = 3700.0;
const Real OrbitViewCanvas::RADIUS_ZOOM_RATIO = 2.2;
const Real OrbitViewCanvas::DEFAULT_DIST = 30000.0;
const int OrbitViewCanvas::UNKNOWN_OBJ_ID = -999;

// color
static int *sIntColor = new int;
static GlColorType *sGlColor = (GlColorType*)sIntColor;

//------------------------------------------------------------------------------
// OrbitViewCanvas(wxWindow *parent, wxWindowID id, ...)
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
OrbitViewCanvas::OrbitViewCanvas(wxWindow *parent, wxWindowID id,
                                 const wxPoint& pos, const wxSize& size,
                                 const wxString& name, long style)
   : ViewCanvas(parent, id, pos, size, name, style)
{
   modelsAreLoaded = false;
   mGlInitialized = false;
   mViewPointInitialized = false;
   mOpenGLInitialized = false;
   mPlotName = name;
   mParent = parent;
   mFatalErrorFound = false;
   mInFunction = false;
   mWriteRepaintDisalbedInfo = true;
   
   // Linux specific
   #ifdef __WXGTK__
      hasBeenPainted = false;
   #endif
   
   #if DEBUG_INIT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas() name=%s, size.X=%d, size.Y=%d\n",
       name.c_str(), size.GetWidth(), size.GetHeight());
   #endif
   
   #ifdef __USE_WX280_GL__
   // Note:
   // Use wxGLCanvas::m_glContext, otherwise resize will not work
   //m_glContext = new wxGLContext(this);
   #endif
   ModelManager *mm = ModelManager::Instance();

   #ifndef __WXMAC__
      if (!mm->modelContext)
         mm->modelContext = new wxGLContext(this);
        #else
      if (!mm->modelContext)
          mm->modelContext = this->GetGLContext();
   #endif

   theContext = mm->modelContext;//new wxGLContext(this);

   mStars = GLStars::Instance();
   mStars->InitStars();
   mStars->SetDesiredStarCount(mStarCount);
   
   mCamera.Reset();
   mCamera.Relocate(DEFAULT_DIST, 0.0, 0.0, 0.0, 0.0, 0.0);
   
   // initialize data members
   GmatAppData *gmatAppData = GmatAppData::Instance();   
   theGuiInterpreter = gmatAppData->GetGuiInterpreter();
   theStatusBar = gmatAppData->GetMainFrame()->GetStatusBar();
   mTextTrajFile = NULL;
   mGlList = 0;
   mIsFirstRun = true;
   
   ResetPlotInfo();
   
   // projection
   mControlMode = MODE_CENTERED_VIEW;
   mInversion = 1;
   mUseInitialViewPoint = true;
   
   // viewpoint
   SetDefaultViewPoint();
   
   // default view
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
   
   // Note from Dunn.  Size of earth vs. spacecraft models will take lots of
   // work in the future.  Models need to be drawn in meters.  Cameras need to
   // be placed near models to "make big enough to see", and if camera is beyond
   // about 2000 meters, model should be drawn as dot.  More discussion to follow!
//   mEarthRadius = 6378.14f; //km
   // @todo - does this need a pointer to the actual Earth object, to get radius? (mEarthRadius does not
   // appear to be used, though)
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
   
   mXyPlaneColor = GmatColor::NAVY32;
   mEcPlaneColor = 0x00002266; //dark red
   mSunLineColor = GmatColor::YELLOW32;
   
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
   //   ("==> OrbitViewCanvas::OrbitViewCanvas() pSolarSystem=%p\n", pSolarSystem);
   
   // objects
   mObjectCount         = 0;
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
   
   #if DEBUG_INIT
   MessageInterface::ShowMessage
      ("   pInternalCoordSystem=%p, pViewCoordSystem=%p\n", pInternalCoordSystem,
       pViewCoordSystem);
   if (pInternalCoordSystem)
      MessageInterface::ShowMessage
         ("   pInternalCoordSystem=%s\n", pInternalCoordSystem->GetName().c_str());
   if (pViewCoordSystem)
      MessageInterface::ShowMessage
         ("   pViewCoordSystem=%s\n", pViewCoordSystem->GetName().c_str());
   MessageInterface::ShowMessage("OrbitViewCanvas() constructor exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// ~OrbitViewCanvas()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
OrbitViewCanvas::~OrbitViewCanvas()
{
   #ifdef DEBUG_RESOURCE_CLEARING
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::~OrbitViewCanvas() '%s' entered\n", mPlotName.c_str());
   #endif
   
   if (mTextTrajFile)
      delete mTextTrajFile;
   
   // Note:
   // deleting m_glContext is handled in wxGLCanvas
   
   ClearObjectArrays();
   
   #ifdef DEBUG_RESOURCE_CLEARING
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::~OrbitViewCanvas() '%s' exiting\n", mPlotName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// bool OrbitViewCanvas::InitOpenGL()
//------------------------------------------------------------------------------
/**
 * Initializes GL and IL.
 */
//------------------------------------------------------------------------------
bool OrbitViewCanvas::InitOpenGL()
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
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::InitOpenGL() '%s' entered, calling InitGL()\n",
       mPlotName.c_str());
   #endif
   
   InitGL();

   // remove back faces
   /*glDisable(GL_CULL_FACE);
   
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
   glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);*/
   
#ifndef SKIP_DEVIL
   
   // initalize devIL library
   ilInit();
   ilutInit();
   ilutRenderer(ILUT_OPENGL);
   
#endif

   #ifdef __USE_WX280_GL__
   SetCurrent(*theContext);
   #else
   SetCurrent();
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
   mOpenGLInitialized = true;
   
   #ifdef DEBUG_INIT_OPENGL
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::InitOpenGL() '%s' returning true\n", mPlotName.c_str());
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// wxString GetGotoObjectName()
//------------------------------------------------------------------------------
wxString OrbitViewCanvas::GetGotoObjectName()
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
wxGLContext* OrbitViewCanvas::GetGLContext()
{
   return theContext;
}


//------------------------------------------------------------------------------
// void SetEndOfRun(bool flag = true)
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetEndOfRun(bool flag)
{
   #if DEBUG_UPDATE
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetEndOfRun() OrbitViewCanvas::SetEndOfRun() flag=%d, "
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
      #if DEBUG_LONGITUDE
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::SetEndOfRun() mIsEndOfRun=%d, mNumData=%d\n",
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
      Real y = mObjectViewPos[index+1];
      
      // Dunn notes the variable "longitude" below is declared elsewhere in this
      // file.  Even if the other "longitude" is protected, it should probably
      // have a different name.  This is the FIRST place longitude is declared.
      Real lst, longitudeFinal, mha;
      
      ComputeLongitudeLst(time, x, y, &mha, &longitudeFinal, &lst);
      mFinalMha = mha;
      mFinalLongitude = longitudeFinal;
      mFinalLst = lst;
      
      #if DEBUG_LONGITUDE
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::SetEndOfRun() mInitialLongitude=%f, time=%f, x=%f,\n   "
          "y=%f, mFinalMha=%f, mFinalLongitude=%f, mFinalLst=%f\n",
          mInitialLongitude, time, x, y, mha, longitudeFinal, lst);
      #endif
   }
}


//------------------------------------------------------------------------------
// void SetUsePerspectiveMode(bool perspMode)
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetUsePerspectiveMode(bool perspMode)
{
   #if DEBUG_INIT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas()::SetUsePerspectiveMode() perspMode=%d\n", perspMode);
   #endif
   
   mUsePerspectiveMode = perspMode;
}


//------------------------------------------------------------------------------
// void SetObjectColors(const wxStringColorMap &objectColorMap)
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetObjectColors(const wxStringColorMap &objectColorMap)
{
   mObjectColorMap = objectColorMap;
}


//------------------------------------------------------------------------------
// void SetShowObjects(const wxStringColorMap &showObjMap)
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetShowObjects(const wxStringBoolMap &showObjMap)
{
   mShowObjectMap = showObjMap;
}

//------------------------------------------------------------------------------
// void SetGLContext(wxGLContext *glContext)
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetGLContext(wxGLContext *glContext)
{
   #ifdef __USE_WX280_GL__
   if (glContext == NULL)
      SetCurrent(*theContext);
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
void OrbitViewCanvas::ClearPlot()
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
 * Resets plotting information.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::ResetPlotInfo()
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
   mInFunction = false;
   mWriteRepaintDisalbedInfo = true;
   modelsAreLoaded = false;

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
void OrbitViewCanvas::RedrawPlot(bool viewAnimation)
{
   #ifdef DEBUG_REDRAW
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::RedrawPlot() entered, viewAnimation=%d\n", viewAnimation);
   #endif
   
   if (mAxisLength < mMaxZoomIn)
   {
      mAxisLength = mMaxZoomIn;
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::RedrawPlot() distance < max zoom in. distance set to %f\n",
          mAxisLength);
   }
   
   if (viewAnimation)
      ViewAnimation(mUpdateInterval, mFrameInc);
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
void OrbitViewCanvas::ShowDefaultView()
{
   int clientWidth, clientHeight;
   GetClientSize(&clientWidth, &clientHeight);

   SetDefaultView();
   ChangeView(mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);
   ChangeProjection(clientWidth, clientHeight, mAxisLength);
   Refresh(false);
}

//------------------------------------------------------------------------------
// void DrawWireFrame(bool flag)
//------------------------------------------------------------------------------
/**
 * Shows objects in wire frame.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawWireFrame(bool flag)
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
void OrbitViewCanvas::DrawXyPlane(bool flag)
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
void OrbitViewCanvas::DrawEcPlane(bool flag)
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
void OrbitViewCanvas::OnDrawAxes(bool flag)
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
void OrbitViewCanvas::OnDrawGrid(bool flag)
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
void OrbitViewCanvas::DrawInOtherCoordSystem(const wxString &csName)
{
   #if DEBUG_ACTION
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::DrawInOtherCoordSysName() viewCS=%s, newCS=%s\n",
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
void OrbitViewCanvas::GotoObject(const wxString &objName)
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
   
   #ifdef DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::GotoObject() objName=%s, mViewObjId=%d, mMaxZoomIn=%f\n"
       "   mAxisLength=%f\n", objName.c_str(), mViewObjId, mMaxZoomIn, mAxisLength);
   #endif

   mIsEndOfData = true;
   mIsEndOfRun = true;

}


//---------------------------------------------------------------------------
// void GotoOtherBody(const wxString &body)
//---------------------------------------------------------------------------
void OrbitViewCanvas::GotoOtherBody(const wxString &body)
{
   #ifdef DEBUG_OBJECT
      MessageInterface::ShowMessage("OrbitViewCanvas::GotoOtherBody() body=%s\n",
                                    body.c_str());
   #endif
}


//---------------------------------------------------------------------------
// void ViewAnimation(int interval, int frameInc)
//---------------------------------------------------------------------------
void OrbitViewCanvas::ViewAnimation(int interval, int frameInc)
{
   #ifdef DEBUG_ANIMATION
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::ViewAnimation() interval=%d, frameInc=%d\n",
       interval, frameInc);
   #endif
   
   if (mIsEndOfData && mInFunction)
      return;
   
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
void OrbitViewCanvas::SetGlObject(const StringArray &objNames,
                                  const UnsignedIntArray &objOrbitColors,
                                  const std::vector<SpacePoint*> &objArray)
{
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetGlObject() entered for %s, objCount=%d, colorCount=%d.\n",
       mPlotName.c_str(), objNames.size(), objOrbitColors.size());
   #endif
   
   // Initialize objects used in view
   SetDefaultViewPoint();
   
   mObjectArray = objArray;
   wxArrayString tempList;
   
   if (objNames.size() == objOrbitColors.size() &&
       objNames.size() == objArray.size())
   {      
      for (UnsignedInt i=0; i<objNames.size(); i++)
      {
         tempList.Add(objNames[i].c_str());
         
         #if DEBUG_OBJECT > 1
         MessageInterface::ShowMessage
            ("   objNames[%d]=%s, objPtr=<%p>%s\n", i, objNames[i].c_str(),
             mObjectArray[i], mObjectArray[i]->GetName().c_str());
         #endif
      }
      
      AddObjectList(tempList, objOrbitColors);
   }
   else
   {
      MessageInterface::ShowMessage("OrbitViewCanvas::SetGlObject() object sizes "
                                    "are not the same. No objects added.\n");
   }
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage("OrbitViewCanvas::SetGlObject() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetSolarSystem(SolarSystem *ss)
{
   pSolarSystem = ss;
}


//------------------------------------------------------------------------------
// void SetGlCoordSystem(CoordinateSystem *internalCs,CoordinateSystem *viewCs,
//                       CoordinateSystem *viewUpCs)
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetGlCoordSystem(CoordinateSystem *internalCs,
                                       CoordinateSystem *viewCs,
                                       CoordinateSystem *viewUpCs)
{
   #if DEBUG_CS
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetGlCoordSystem() for '%s', internalCs=<%p>, viewCs=<%p>, "
       " viweUpCs=%p\n",  mPlotName.c_str(), internalCs, viewCs, viewUpCs);
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
   // So that spacecraft won't overlap each other.
   //@todo: need better way to scale spacecraft size.  See Dunn's comments above.
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
   
   #if DEBUG_CS
   MessageInterface::ShowMessage
      ("   mViewCoordSysName=%s, pViewCoordSystem=%p, mOriginName=%s, "
       "mOriginId=%d\n", mViewCoordSysName.c_str(), pViewCoordSystem,
       mOriginName.c_str(),  mOriginId);
   MessageInterface::ShowMessage
      ("   mViewUpCoordSysName=%s, mViewObjName=%s, mViewObjId=%d\n",
       mViewUpCoordSysName.c_str(), mViewObjName.c_str(), mViewObjId);
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
void OrbitViewCanvas::SetGlViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
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
      
   #if DEBUG_PROJECTION
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetGlViewOption() pViewPointRefObj=%p, "
       "pViewPointVectorObj=%p\n   pViewDirectionObj=%p, mViewScaleFactor=%f   "
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
      mViewObjName = pViewDirectionObj->GetName().c_str();
      mViewPointRefObjName = pViewPointRefObj->GetName();
      
      mVpRefObjId = GetObjectId(pViewPointRefObj->GetName().c_str());
      
      if (mVpRefObjId == GmatPlot::UNKNOWN_BODY)
      {
         mUseViewPointRefVector = true;
         MessageInterface::ShowMessage
            ("*** Warning *** OrbitViewCanvas::SetGlViewOption() Cannot find "
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
            ("*** Warning *** OrbitViewCanvas::SetGlViewOption() "
             "ViewPointRefObject is NULL,"
             "so will use default Vector instead.\n");
   }
   
   // Set viewpoint vector object id
   if (!mUseViewPointVector && pViewPointVectorObj)
   {
      mVpVecObjId = GetObjectId(pViewPointVectorObj->GetName().c_str());
      
      if (mVpVecObjId == GmatPlot::UNKNOWN_BODY)
      {
         mUseViewPointVector = true;
         MessageInterface::ShowMessage
            ("*** Warning *** OrbitViewCanvas::SetGlViewOption() Cannot find "
             "pViewPointVectorObj name=%s, so using vector=%s\n",
             pViewPointVectorObj->GetName().c_str(),
             mViewPointVector.ToString().c_str());
      }
   }
   else
   {
      if (!mUseViewPointVector)
         MessageInterface::ShowMessage
            ("*** Warning *** OrbitViewCanvas::SetGlViewOption() "
             "ViewPointVectorObject is NULL, "
             "so will use default Vector instead.\n");
   }
   
   // Set view direction object id
   if (!mUseViewDirectionVector && pViewDirectionObj)
   {
      mVdirObjId = GetObjectId(pViewDirectionObj->GetName().c_str());
      
      if (mVdirObjId == GmatPlot::UNKNOWN_BODY)
      {
         mUseViewDirectionVector = true;
         MessageInterface::ShowMessage
            ("*** Warning *** OrbitViewCanvas::SetGlViewOption() Cannot find "
             "pViewDirectionObj name=%s, so using vector=%s\n",
             pViewDirectionObj->GetName().c_str(),
             mViewDirectionVector.ToString().c_str());
      }
   }
   else
   {
      if (!mUseViewDirectionVector)
         MessageInterface::ShowMessage
            ("*** Warning *** OrbitViewCanvas::SetGlViewOption() "
             "ViewDirectionObject is NULL,"
             "so will use default Vector instead.\n");
   }
   
} //end SetGlViewOption()


//------------------------------------------------------------------------------
// void SetGlDrawOrbitFlag(const std::vector<bool> &drawArray)
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetGlDrawOrbitFlag(const std::vector<bool> &drawArray)
{
   mDrawOrbitArray = drawArray;
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetGlDrawObjectFlag() mDrawOrbitArray.size()=%d, "
       "mObjectCount=%d\n", mDrawOrbitArray.size(), mObjectCount);
   
   bool draw;
   for (int i=0; i<mObjectCount; i++)
   {
      draw = mDrawOrbitArray[i] ? true : false;      
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::SetGlDrawObjectFlag() i=%d, mDrawOrbitArray[%s]=%d\n",
          i, mObjectNames[i].c_str(), draw);
   }
   #endif
}


//------------------------------------------------------------------------------
// void SetGlShowObjectFlag(const std::vector<bool> &showArray)
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetGlShowObjectFlag(const std::vector<bool> &showArray)
{
   mShowObjectArray = showArray;

   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetGlDrawObjectFlag() mDrawOrbitArray.size()=%d, "
       "mObjectCount=%d\n", mShowObjectArray.size(), mObjectCount);
   #endif
   
   bool show;
   mSunPresent = true;//false;
   
   for (int i=0; i<mObjectCount; i++)
   {
      show = mShowObjectArray[i] ? true : false;
      mShowObjectMap[mObjectNames[i]] = show;
      
      if (mObjectNames[i] == "Sun" && mShowObjectMap["Sun"])
         mSunPresent = true;
      
      #if DEBUG_OBJECT
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::SetGlShowObjectFlag() i=%d, mShowObjectMap[%s]=%d\n",
          i, mObjectNames[i].c_str(), show);
      #endif
   }
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetGlDrawObjectFlag() mEnableLightSource=%d, mSunPresent=%d\n",
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
      // from the viewer aren't drawn (for efficiency).
      glEnable(GL_CULL_FACE);
      
      // create a light:
      //float lightColor[4]={1.0f, 1.0f, 1.0f, 1.0f};
      
      //glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, lightColor);
      //glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
      
      // enable the light
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      
      // tell OpenGL to use glColor() to get material properties for..
      glEnable(GL_COLOR_MATERIAL);
      
      // ..the front face's ambient and diffuse components
      glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
      
      // Set the ambient lighting
      GLfloat ambient[4] = {0.4f, 0.4f, 0.4f, 1.0f};
      glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
      //----------------------------------------------------------------------
   }
   #endif
}


//------------------------------------------------------------------------------
// void SetNumPointsToRedraw(Integer numPoints)
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetNumPointsToRedraw(Integer numPoints)
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
void OrbitViewCanvas::SetUpdateFrequency(Integer updFreq)
{
   mUpdateFrequency = updFreq;
}


//------------------------------------------------------------------------------
// void UpdatePlot(const StringArray &scNames, const Real &time, ...
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
 * @param <inFunction> true if data is published inside a function
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::UpdatePlot(const StringArray &scNames, const Real &time,
                                 const RealArray &posX, const RealArray &posY,
                                 const RealArray &posZ, const RealArray &velX,
                                 const RealArray &velY, const RealArray &velZ,
                                 const UnsignedIntArray &scColors, bool solving,
                                 Integer solverOption, bool inFunction)
{
   if (IsFrozen())
      Thaw();

   // Commented out since spacecraft panel is not showing. (LOJ: 2011.04.25)
   // This code attempt to show spacecraft moel when total data points are
   // less than update frequency points (Bug 2380)
   // To load spacecraft model, OpenGL needs to be initialized first
   //if (!mOpenGLInitialized)
   //   Update();
   
   mTotalPoints++;
   mInFunction = inFunction;   
   mDrawSolverData = false;
   
   //-----------------------------------------------------------------
   // If showing current iteration only, handle solver iteration data
   // separately here since it will be shown temporarily during the run
   //-----------------------------------------------------------------
   if (solverOption == 1)
      UpdateSolverData(posX, posY, posZ, scColors, solving);
   
   // If drawing solver's current iteration and no run data has been
   // buffered up, save up to 2 points so that it will still go through
   // view projection transformation to show other objects.
   if (solverOption == 1 && solving && mNumData > 1)
      return;
   
   mScCount = scNames.size();
   mScNameArray = scNames;
   
   if (mNumData < MAX_DATA)
      mNumData++;
   
   if (mScCount > MAX_SCS)
      mScCount = MAX_SCS;
   
   #if DEBUG_UPDATE
   MessageInterface::ShowMessage
      ("===========================================================================\n");
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::UpdatePlot() plot=%s, time=%f, posX=%f, mNumData=%d, "
       "mScCount=%d, scColor=%u, solving=%d, solverOption=%d, inFunction=%d\n",
       GetName().c_str(), time, posX[0], mNumData, mScCount, scColors[0], solving,
       solverOption, inFunction);
   #endif
   
   
   //-----------------------------------------------------------------
   // Buffer data for plot
   //-----------------------------------------------------------------
   ComputeBufferIndex(time);
   mTime[mLastIndex] = time;
   
   // Dunn notes the variable "longitude" below is declared elsewhere in this
   // file.  Even if the other "longitude" is protected, it should probably
   // have a different name.  This is the SECOND place longitude is declared.
   Real mha, longitude2, lst;

   // Dunn notes that "longitude2" which is his new name for a variable that was
   // declared with the same name multiple places, is set below to mInitialLongitude.
   // In ComputerLongitudeLst, this variable is a function of BOTH earth orientation
   // and the location in ECI space of the spacecraft.
   ComputeLongitudeLst(mTime[mLastIndex], posX[0], posY[0], &mha, &longitude2, &lst);
   
   #if DEBUG_LONGITUDE
   MessageInterface::ShowMessage
      ("   time=%f, mLastIndex=%d, mha=%f, longitude2=%f, lst = %f\n",
       mTime[mLastIndex], mLastIndex, mha, longitude2, lst);
   #endif
   
   // if beginning of the plot
   if (mNumData == 0)
   {
      mInitialLongitude = longitude2;
      mInitialMha = mha;
      #if DEBUG_LONGITUDE
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::UpdatePlot() mInitialLongitude = %f, mInitialMha = %f\n",
          mInitialLongitude, mInitialMha);
      #endif
   }
   
   // update spacecraft position
   UpdateSpacecraftData(time, posX, posY, posZ, velX, velY, velZ, scColors,
                        solverOption);
   
   // update non-spacecraft objects position
   UpdateOtherData(time);
   
   // Initialize view point if not already initialized
   // We want users to change the view point during the run,
   // so mUseInitialViewPoint is removed (LOJ: 2011.02.08)
   //if (!mViewPointInitialized || mUseInitialViewPoint)
   if (!mViewPointInitialized)
      InitializeViewPoint();
   
}


//---------------------------------------------------------------------------
// void TakeAction(const std::string &action)
//---------------------------------------------------------------------------
void OrbitViewCanvas::TakeAction(const std::string &action)
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
void OrbitViewCanvas::AddObjectList(const wxArrayString &objNames,
                                    const UnsignedIntArray &objColors,
                                    bool clearList)
{
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::AddObjectList() entered, object count=%d, color count=%d\n",
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
         #if DEBUG_OBJECT
         MessageInterface::ShowMessage
            ("OrbitViewCanvas::AddObjectList() Bind new texture object=%s\n",
             objNames[i].c_str());
         #endif
         
         mObjectTextureIdMap[objNames[i]] = GmatPlot::UNINIT_TEXTURE;
      }
      
      // initialize show object
      mShowObjectMap[objNames[i]] = true;

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
      
      #if DEBUG_OBJECT > 1
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::AddObjectList() objNames[%d]=%s\n",
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
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage("OrbitViewCanvas::AddObjectList() leaving\n");
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
int OrbitViewCanvas::ReadTextTrajectory(const wxString &filename)
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
   if (!InitOpenGL())
   {
      wxMessageDialog msgDialog(this, _T("InitOpenGL() failed"),
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
void OrbitViewCanvas::OnPaint(wxPaintEvent& event)
{
   // must always be here
   wxPaintDC dc(this);
   
   if (mFatalErrorFound) return;
   
   #ifndef __WXMOTIF__
      #ifndef __USE_WX280_GL__
         if (!GetContext()) return;
      #endif
   #endif
   
   #ifdef __USE_WX280_GL__
   theContext->SetCurrent(*this);
   SetCurrent(*theContext);
   #else
   SetCurrent();
   #endif
   
   if (!mGlInitialized && mObjectCount > 0)
   {
      InitOpenGL();
      mGlInitialized = true;
   }
   
   // set OpenGL to recognize the counter clockwise defined side of a polygon
   // as its 'front' for lighting and culling purposes
   glFrontFace(GL_CCW);
   
   // enable face culling, so that polygons facing away (defines by front face)
   // from the viewer aren't drawn (for efficiency).
   glEnable(GL_CULL_FACE);
   
   // tell OpenGL to use glColor() to get material properties for..
   glEnable(GL_COLOR_MATERIAL);
   
   // ..the front face's ambient and diffuse components
   glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
   
   // Set the ambient lighting
   GLfloat ambient[4] = {0.4f, 0.4f, 0.4f, 1.0f};
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
   
   int nWidth, nHeight;
   GetClientSize(&nWidth, &nHeight);
   glViewport(0, 0, nWidth, nHeight);
   
   GLUquadricObj *qobj = gluNewQuadric();
   for (int i = 10; i < 110; i  += 10)
      DrawCircle(qobj, i);
   
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
   
   if (mIsEndOfRun && mInFunction)
   {
      if (mWriteRepaintDisalbedInfo)
      {
         Freeze();
         wxString msg = "*** WARNING *** This plot data was published inside a "
            "function, so repainting or drawing animation is disabled.\n";
         MessageInterface::ShowMessage(msg.c_str());
         GmatAppData::Instance()->GetMainFrame()->EnableAnimation(false);
         
         mWriteRepaintDisalbedInfo = false;
      }
      return;
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
void OrbitViewCanvas::OnTrajSize(wxSizeEvent& event)
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
      theContext->SetCurrent(*this);
      SetCurrent(*theContext);
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
void OrbitViewCanvas::OnMouse(wxMouseEvent& event)
{
   
   //MessageInterface::ShowMessage
   //   ("===> OnMouse() mUseInitialViewPoint=%d, mIsEndOfData=%d\n",
   //    mUseInitialViewPoint, mIsEndOfData);
   
   if (mIsEndOfData && mInFunction)
      return;
   
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
         
         #if DEBUG_EULER
         MessageInterface::ShowMessage
            ("OrbitViewCanvas::OnMouse() mCurrRotXAngle=%f, %f, %f\n",
             mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);
         MessageInterface::ShowMessage
            ("OrbitViewCanvas::OnMouse() mfCamRotXYZAngle=%f, %f, %f\n",
             mfCamRotXAngle, mfCamRotYAngle, mfCamRotZAngle);
         #endif
         
         
         // always look at from Z for rotation and zooming
         mfCamTransX = 0.0;
         mfCamTransY = 0.0;
         mfCamTransZ = -mVpLocVec.GetMagnitude();
      }
      #endif
      
   }
   //if mouse dragging
   if (event.Dragging())
   {
      //------------------------------
      // translating
      //------------------------------
      if ((mControlMode != MODE_ASTRONAUT_6DOF && event.ShiftDown() && event.LeftIsDown()) ||
         (mControlMode == MODE_ASTRONAUT_6DOF && event.LeftIsDown()))
      {
         // Do a X/Y Translate of the camera
         mfCamTransX = (fEndX - mfStartX) * mInversion;
         mfCamTransY = (fEndY - mfStartY) * mInversion;

         mCamera.Translate(mfCamTransX, mfCamTransY, 0.0, true);
         
         // repaint
         Refresh(false);
      }
      //------------------------------
      // rotating
      //------------------------------
      else if ((mControlMode != MODE_ASTRONAUT_6DOF && event.LeftIsDown()) ||
         (mControlMode == MODE_ASTRONAUT_6DOF && event.RightIsDown()))
      {
         // PS - Attempting a new form of view rotation
         //   Rather than apply a rotation based on quaternions and all of that
         //   complication, we move the camera position based
         //   on the mouse movement. 
         // The angles used are based on how far the mouse moved
         float angleX = (mLastMouseX - mouseX) / 400.0 * mInversion,
            angleY = (mLastMouseY - mouseY) / 400.0 * mInversion;
         if (mControlMode == MODE_CENTERED_VIEW)
         {
            mCamera.Rotate(angleX, angleY, 0.0, false, true);
         }
         else
         {
            mCamera.Rotate(angleX, angleY, 0.0, false, false);
         }
         
         // repaint
         Refresh(false); 
         
      }
      //------------------------------
      // translating forward and backward
      //------------------------------
      /*else if (mControlMode != MODE_ASTRONAUT_6DOF && event.ShiftDown() && event.RightIsDown())
      {
         // find the length
         Real length = mLastMouseY - mouseY;
         length *= 100;
         mCamera.Translate(0.0, 0.0, length, true);

         Refresh(false);
      }*/
      //------------------------------
      // FOV Zoom
      //------------------------------
      else if (event.ShiftDown() && event.RightIsDown()){
         Real x2 = Pow(mLastMouseX - mouseX, 2);
         Real y2 = Pow(mouseY - mLastMouseY, 2);
         Real length = sqrt(x2 + y2);
         
         Real distance = (mCamera.view_center - mCamera.position).GetMagnitude();
         
         mZoomAmount = length * distance / 1000000;
         if (mouseY > mLastMouseY)
            mCamera.ZoomOut(mZoomAmount);
         else
            mCamera.ZoomIn(mZoomAmount);
         
         Refresh(false);
      }
      //------------------------------
      // "zooming"
      //------------------------------
      else if (mControlMode != MODE_ASTRONAUT_6DOF && event.RightIsDown())
      {            
         // if end-of-run compute new mfCamRotXYZAngle by calling ChangeView()
         if (mIsEndOfRun)
            ChangeView(mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);
         
         //VC++ error C2668: 'pow' : ambiguous call to overloaded function
         //'long double pow(long double,int)' 'float pow(float,int)' 'double pow(double,int);
         // Changed pow to GmatMathUtil::Pow();
         
         // find the length
         Real x2 = Pow(mLastMouseX - mouseX, 2);
         Real y2 = Pow(mouseY - mLastMouseY, 2);
         Real length = sqrt(x2 + y2);
         
         Real distance = (mCamera.view_center - mCamera.position).GetMagnitude();
         
         mZoomAmount = length * distance / 500;
         
         if (mouseX < mLastMouseX && mouseY > mLastMouseY)
         {
            // dragging from upper right corner to lower left corner
            mCamera.Translate(0, 0, mZoomAmount, false);
            //mCamera.ZoomIn(mZoomAmount);
         }
         else if (mouseX > mLastMouseX && mouseY < mLastMouseY)
         {
            // dragging from lower left corner to upper right corner
            mCamera.Translate(0, 0, -mZoomAmount, false);
            //mCamera.ZoomOut(mZoomAmount);
         }
         else
         {
            // if mouse moves toward left then zoom in
            if (mouseX < mLastMouseX || mouseY < mLastMouseY)
               mCamera.Translate(0, 0, mZoomAmount, false);
               //mCamera.ZoomIn(mZoomAmount);
            else
               mCamera.Translate(0, 0, -mZoomAmount, false);
               //mCamera.ZoomOut(mZoomAmount);
         }
         
         Refresh(false);
      }
      //------------------------------
      // roll
      //------------------------------
      else if (event.MiddleIsDown())
      {
         float roll = (mouseY - mLastMouseY) / 400.0 * mInversion;
         if (mControlMode == MODE_CENTERED_VIEW)
         {
            mCamera.Rotate(0.0, 0.0, roll, false, true);
         }
         else
            mCamera.Rotate(0.0, 0.0, roll, false, false);
         Refresh(false);
      }
   }
   // Mousewheel movements
   else if (event.GetWheelRotation() != 0 && mControlMode == MODE_ASTRONAUT_6DOF)
   {
      float rot = event.GetWheelRotation();
      Real distance = (mCamera.view_center - mCamera.position).GetMagnitude();
      Real movement = rot * distance / 3000;

      if (event.ShiftDown() && rot > 0)
      {
         mCamera.ZoomIn(1);
      }
      else if (event.ShiftDown() && rot < 0)
      {
         mCamera.ZoomOut(1);
      }
      else if (rot > 0)
      {
         mCamera.Translate(0.0, 0.0, movement, true);
      }
      else if (rot < 0)
      {
         mCamera.Translate(0.0, 0.0, movement, true);
      }
      Refresh(false);
   } // end if (event.Dragging())
   
   // ensures the directional vectors for the viewpoint are still orthogonal
   mCamera.ReorthogonalizeVectors();
   
   // save last position
   mLastMouseX = mouseX;
   mLastMouseY = mouseY;
   
   mfStartX = fEndX;
   mfStartY = fEndY;
   
   #ifdef __WRITE_GL_MOUSE_POS__
   wxString mousePosStr;
   //mousePosStr.Printf("X = %g Y = %g", fEndX, fEndY);
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
void OrbitViewCanvas::OnKeyDown(wxKeyEvent &event)
{
   int keyDown = event.GetKeyCode();
   if (keyDown == 'w' || keyDown == 'W')
   {
      mCamera.Translate(0.0, 0.0, 300, true);
   }
   else if (keyDown == 's' || keyDown == 'S')
   {
      mCamera.Translate(0.0, 0.0, -300, true);
   }
   else if (keyDown == 'a' || keyDown == 'A')
   {
      mCamera.Translate(-300, 0.0, 0.0, true);
   }
   else if (keyDown == 'd' || keyDown == 'D')
   {
      mCamera.Translate(300, 0.0, 0.0, true);
   }
   else if (keyDown == 'z' || keyDown == 'Z')
   {
      if (event.ShiftDown())
      {
         mControlMode = MODE_ASTRONAUT_6DOF;
      }
      else
      {
         if (mControlMode == MODE_ASTRONAUT_6DOF)
            mControlMode = MODE_FREE_FLYING;
         else
         {
            mControlMode = 1 - mControlMode;
         }
      }
   }
   else if (keyDown == 'i' || keyDown == 'I')
   {
      mInversion *= -1;
   }
   /*else if (keyDown == 't' || keyDown == 'T')
    * {
      if (mCamera.TrackingMode() == 1)
         mCamera.Untrack();
      else
         mCamera.TrackStill(0);
   }
   else if (keyDown == 'f' || keyDown == 'F')
   {
      if (mCamera.TrackingMode() == 2)
         mCamera.Untrack();
      else
         mCamera.TrackFollow(0);
   }*/
   else if (keyDown == WXK_ESCAPE)
      mHasUserInterrupted = true;
   
   // ensures the directional vectors for the viewpoint are still orthogonal
   mCamera.ReorthogonalizeVectors();
   Refresh(false);
}


//------------------------------------------------------------------------------
// bool SetPixelFormatDescriptor()
//------------------------------------------------------------------------------
/**
 * Sets pixel format on Windows.
 */
//------------------------------------------------------------------------------
bool OrbitViewCanvas::SetPixelFormatDescriptor()
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
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetPixelFormatDescriptor() pixelFormatId = %d \n",
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

#else
   // Should we return true for non-Window system?
   //return false;
   return true;
#endif
}


//------------------------------------------------------------------------------
//  void SetDefaultGLFont()
//------------------------------------------------------------------------------
/**
 * Sets default GL font.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetDefaultGLFont()
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
// void SetDefaultViewPoint()
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetDefaultViewPoint()
{
   mViewPointInitialized = false;
   mViewPointRefObjName = "UNKNOWN";
   
   pViewPointRefObj = NULL;
   pViewPointVectorObj = NULL;
   pViewDirectionObj = NULL;
   
   mViewPointRefVector.Set(0.0, 0.0, 0.0);
   mViewPointVector.Set(DEFAULT_DIST, 0.0, 0.0);
   mViewDirectionVector.Set(0.0, 0.0, -1.0);
   //mCamera.Reset();
   //mCamera.Relocate(DEFAULT_DIST, 0.0, 0.0, 0.0, 0.0, 0.0);
   
   mViewScaleFactor = 1.0;
   mUseViewPointRefVector = true;
   mUseViewPointVector = true;
   mUseViewDirectionVector = true;
   mVpRefObjId = UNKNOWN_OBJ_ID;
   mVpVecObjId = UNKNOWN_OBJ_ID;
   mVdirObjId = UNKNOWN_OBJ_ID;
}


//------------------------------------------------------------------------------
// void InitializeViewPoint()
//------------------------------------------------------------------------------
void OrbitViewCanvas::InitializeViewPoint()
{
   // Dunn took out minus signs below to position vectors correctly in the 
   // ECI reference frame.

   #if DEBUG_INIT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::InitializeViewPoint() entered\n");
   #endif
   
   wxString name;
   int objId;
   int index;
   Rvector3 refVec, viewpoint, direction;
   
   if (mUseViewPointRefVector)
   {
      refVec = mViewPointRefVector;
   }
   else
   {
      name = pViewPointRefObj->GetName().c_str();
      objId = GetObjectId(name);
      index = objId * MAX_DATA * 3 + (mLastIndex*3);
      refVec = Rvector3(mObjectViewPos[index+0], mObjectViewPos[index+1], mObjectViewPos[index+2]);
   }
   
   if (mUseViewPointVector)
   {
      viewpoint = mViewPointVector;
   }
   else
   {
      name = pViewPointVectorObj->GetName().c_str();
      objId = GetObjectId(name);
      index = objId * MAX_DATA * 3 + (mLastIndex*3);
      viewpoint = Rvector3(mObjectViewPos[index+0], mObjectViewPos[index+1], mObjectViewPos[index+2]);
   }
   viewpoint *= mViewScaleFactor;
      
   if (mUseViewDirectionVector)
   {
      direction = mViewDirectionVector;
   }
   else
   {
      name = pViewDirectionObj->GetName().c_str();
      objId = GetObjectId(name);
      index = objId * MAX_DATA * 3 + (mLastIndex*3);
      direction = Rvector3(mObjectViewPos[index+0], mObjectViewPos[index+1], mObjectViewPos[index+2]);
   }
   
   mCamera.Reset();
   if (mViewUpAxisName == "X")
      mCamera.up = Rvector3(1.0, 0.0, 0.0);
   else if (mViewUpAxisName == "-X")
      mCamera.up = Rvector3(-1.0, 0.0, 0.0);
   else if (mViewUpAxisName == "Y")
      mCamera.up = Rvector3(0.0, 1.0, 0.0);
   else if (mViewUpAxisName == "-Y")
      mCamera.up = Rvector3(0.0, -1.0, 0.0);
   else if (mViewUpAxisName == "Z")
      mCamera.up = Rvector3(0.0, 0.0, 1.0);
   else if (mViewUpAxisName == "-Z")
      mCamera.up = Rvector3(0.0, 0.0, -1.0);
   
   Rvector3 viewPos = refVec + viewpoint;
   Rvector3 viewDiff = viewPos - direction;
   
   #if DEBUG_INIT
   MessageInterface::ShowMessage("   refVec    = %s", refVec.ToString().c_str());
   MessageInterface::ShowMessage("   viewpoint = %s", viewpoint.ToString().c_str());
   MessageInterface::ShowMessage("   viewPos   = %s", viewPos.ToString().c_str());
   MessageInterface::ShowMessage("   direction = %s", viewPos.ToString().c_str());
   MessageInterface::ShowMessage("   viewDiff  = %s", viewDiff.ToString().c_str());
   #endif
   
   // If view difference is not zero then relocate camera
   if (!viewDiff.IsZeroVector())
   {
      mCamera.Relocate(viewPos, direction);
      // ReorthogonalizeVectors() is called from Camera::Relocate(), so commented out
      //mCamera.ReorthogonalizeVectors();
   }
   
   mViewPointInitialized = true;
   
   #if DEBUG_INIT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::InitializeViewPoint() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void ComputeBufferIndex(Real time)
//------------------------------------------------------------------------------
void OrbitViewCanvas::ComputeBufferIndex(Real time)
{
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
   
}


//------------------------------------------------------------------------------
// void ComputeActualIndex()
//------------------------------------------------------------------------------
void OrbitViewCanvas::ComputeActualIndex()
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
      ("OrbitView::ComputeActualIndex()     mBeginIndex1=%3d,     mEndIndex1=%3d, "
       "    mBeginIndex2=%3d,     mEndIndex2=%3d\n", mBeginIndex1, mEndIndex1,
       mBeginIndex2, mEndIndex2);
   MessageInterface::ShowMessage
      ("OrbitView::ComputeActualIndex() mRealBeginIndex1=%3d, mRealEndIndex1=%3d, "
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
bool OrbitViewCanvas::LoadGLTextures()
{
   #if DEBUG_TEXTURE
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::LoadGLTextures() mObjectCount=%d\n", mObjectCount);
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
         #if DEBUG_TEXTURE > 1
         MessageInterface::ShowMessage
            ("OrbitViewCanvas::LoadGLTextures() object=<%p>'%s'\n",
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
GLuint OrbitViewCanvas::BindTexture(SpacePoint *obj, const wxString &objName)
{
   GLuint ret = GmatPlot::UNINIT_TEXTURE;

   //MessageInterface::ShowMessage("===> OrbitViewCanvas::BindTexture() ret = %d\n", ret);
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
               ("*** WARNING *** OrbitViewCanvas::BindTexture() Unable to load "
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
         
         #ifdef __USE_WX280_GL__
            SetCurrent(*theContext);
         #else
            SetCurrent();
         #endif
            
         glGenTextures(1, &ret);
         glBindTexture(GL_TEXTURE_2D, ret);
         
         //if (textureFile == "")
         //   ret = GmatPlot::UNINIT_TEXTURE;
         //else
         // load image file
         if (!LoadImage(textureFile))
         {
            if (obj->IsOfType(Gmat::CELESTIAL_BODY))
            {
               MessageInterface::ShowMessage
                  ("*** WARNING *** OrbitViewCanvas::BindTexture() Cannot load texture "
                   "image for '%s' from '%s'\n", objName.c_str(), textureFile.c_str());
            }
            ret = GmatPlot::UNINIT_TEXTURE;
         }
         
      #endif
   }
   catch (BaseException &e)
   {
      // Give warning for missing texture file for only CelestialBody object
      if (obj->IsOfType(Gmat::CELESTIAL_BODY))
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** OrbitViewCanvas::BindTexture() Cannot bind texture "
             "image for %s.\n%s\n", objName.c_str(), e.GetFullMessage().c_str());
      }
   }
   
   #if DEBUG_TEXTURE
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::BindTexture() objName=%s ret=%d\n", objName.c_str(),
       ret);
   #endif
   
   return ret;
} //end BindTexture()


//------------------------------------------------------------------------------
// void SetDefaultView()
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetDefaultView()
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
void OrbitViewCanvas::SetProjection()
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
 * the viewing volume is a rectangular parallelepiped. Unlike perspective
 * projection, the size of the viewing volume doesn't change from one end to the
 * other, so distance from the camera doesn't affect how large a object appears.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetupWorld()
{

   #if DEBUG_PROJECTION > 2
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetupWorld() mUsePerspectiveMode=%d, mUseSingleRotAngle=%d\n",
       mUsePerspectiveMode, mUseSingleRotAngle);
   #endif
   
   #if DEBUG_PROJECTION > 2
   if (mUseSingleRotAngle)
   {
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::SetupWorld() mfLeftPos=%f, mfRightPos=%f\n   mfBottomPos=%f, "
          "mfTopPos=%f\n   mfViewNear=%f, mfViewFar=%f\n", mfLeftPos, mfRightPos,
          mfBottomPos, mfTopPos, mfViewNear, mfViewFar);
   }
   #endif 
      
   // Setup how we view the world
   GLfloat aspect = (GLfloat)mCanvasSize.x / (GLfloat)mCanvasSize.y;
   
   #if DEBUG_PERSPECTIVE
   if (mUseSingleRotAngle)
   {
      MessageInterface::ShowMessage
         ("   mAxisLength=%f, size=%f, dist=%f, mViewObjRadius=%f\n   "
          "mFovDeg=%f, ratio=%f, \n", mAxisLength, size, dist, mViewObjRadius,
          mFovDeg, ratio);
   }
   #endif
      
   // PS - Greatly simplified. Uses the FOV from the active camera, the aspect ratio of the screen,
   //       and a constant near-far plane
   float distance = (mCamera.position - mCamera.view_center).GetMagnitude() * 2;
   if (500000000.0f > distance)
      distance = 500000000.0f;
   
   gluPerspective(mCamera.fovDeg, aspect, 50.0f, distance);
   
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
void OrbitViewCanvas::ComputeView(GLfloat fEndX, GLfloat fEndY)
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
void OrbitViewCanvas::ChangeView(float viewX, float viewY, float viewZ)
{

   mfCamRotXAngle = (int)(viewX) % 360 + 270;
   mfCamRotYAngle = (int)(viewY) % 360;
   mfCamRotZAngle = (int)(viewZ) % 360;
   
   //MessageInterface::ShowMessage
   //   ("===> OrbitViewCanvas::ChangeView() mfCamRotXYZAngle = %f %f %f\n",
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
 * orthographic projection.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::ChangeProjection(int width, int height, float axisLength)
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
 * PS - Much of this is deprecated, since most of the vector usage is in the
 *       Camera class, which is external
 *      Consider removing quite a bit
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::ComputeViewVectors()
{   
   //int frame = mNumData - 1;
   int frame = mLastIndex;
   mIsFirstRun = false;
   int index = 0;
   
   #if DEBUG_PROJECTION
   MessageInterface::ShowMessage
      ("ComputeViewVectors() entered, frame=%d, time=%f\n", frame, mTime[frame]);
   #endif
   
   //-----------------------------------------------------------------
   // get viewpoint reference vector
   //-----------------------------------------------------------------
   mVpRefVec.Set(0.0, 0.0, 0.0);
   
   if (!mUseViewPointRefVector && pViewPointRefObj != NULL)
   {
      #if DEBUG_PROJECTION
      MessageInterface::ShowMessage
         ("ComputeViewVectors() pViewPointRefObj=<%p>, name='%s'\n",
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
            ("*** WARNING *** OrbitViewCanvas::ComputeViewVectors() Invalid "
             "mVpRefObjId=%d\n", mVpRefObjId);
      }
   }
   
   //-----------------------------------------------------------------
   // get viewpoint vector
   //-----------------------------------------------------------------
   //mVpVec = mViewPointVector;
   
   if (!mUseViewPointVector && pViewPointVectorObj != NULL)
   {
      #if DEBUG_PROJECTION
      MessageInterface::ShowMessage
         ("ComputeViewVectors() pViewPointVectorObj=<%p>, name='%s', mVpVecObjId=%d\n",
          pViewPointVectorObj, pViewPointVectorObj->GetName().c_str(),
          mVpVecObjId);
      #endif
      
      // if valid body id
      if (mVpVecObjId != UNKNOWN_OBJ_ID)
      {         
         if (mUseGluLookAt)
         {
            index = mVpVecObjId * MAX_DATA * 3 + frame * 3;
            
            #if DEBUG_PROJECTION
            MessageInterface::ShowMessage
               ("ComputeViewVectors() using GluLookAt, index=%d\n", index);
            #endif
            // if look at from object, negate it so that we can see the object
            /*mVpVec.Set(-mObjectViewPos[index+0],
                       -mObjectViewPos[index+1],
                       -mObjectViewPos[index+2]);

            // set z component to zero so that plane doesn't move up and down
            mVpVec[2] = 0.0;*/
            
         }
         else
         {
            index = mVpVecObjId * MAX_DATA * 3 + frame * 3;
            
            /*mVpVec.Set(mObjectViewPos[index+0],
                       mObjectViewPos[index+1],
                       mObjectViewPos[index+2]);*/
         }
      }
      else
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** OrbitViewCanvas::ComputeViewVectors() Invalid "
             "mVpVecObjId=%d\n", mVpVecObjId);
      }
   }
   
   //-----------------------------------------------------------------
   // get viewpoint location
   //-----------------------------------------------------------------
   //mVpLocVec = mVpRefVec + (mViewScaleFactor * mVpVec);
   
   //-----------------------------------------------------------------
   // get view direction and view center vector
   //-----------------------------------------------------------------
   //mVdVec = mViewDirectionVector;
   
   if (!mUseViewDirectionVector && pViewDirectionObj != NULL)
   {
      #if DEBUG_PROJECTION
      MessageInterface::ShowMessage
         ("ComputeViewVectors() pViewDirectionObj=<%p>, name='%s'\n",
          pViewDirectionObj, pViewDirectionObj->GetName().c_str());
      #endif
      
      // if viewpoint ref object is same as view direction object
      // just look opposite side
      if (pViewDirectionObj->GetName() == mViewPointRefObjName)
      {
         //mVdVec = -mVpLocVec;
      }
      else if (mVdirObjId != UNKNOWN_OBJ_ID)
      {
         index = mVdirObjId * MAX_DATA * 3 + frame * 3;
         
         // for efficiency, body data are computed in UpdatePlot() once.
         /*mVdVec.Set(mObjectViewPos[index+0],
                    mObjectViewPos[index+1],
                    mObjectViewPos[index+2]);
         
         // check for 0.0 direction 
         if (mVdVec.GetMagnitude() == 0.0)
            mVdVec = mViewDirectionVector;*/
      }
      else
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** OrbitViewCanvas::ComputeViewVectors() Invalid "
             "mVdirObjId=%d\n", mVdirObjId);
      }
   }
   
   // set view center vector for gluLookAt()
   //mVcVec = mVdVec;
   
   #if DEBUG_PROJECTION
   MessageInterface::ShowMessage
      ("ComputeViewVectors() mVpRefVec=%s, mVpVec=%s\n   mVpLocVec=%s, "
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
   //if (mNumData <= mUpdateFrequency)
      //mAxisLength = mVpLocVec.GetMagnitude();
   
   // if mAxisLength is too small, set to max zoom in value
   if (mAxisLength < mMaxZoomIn)
      mAxisLength = mMaxZoomIn;
   
   // compute camera rotation angle
   //Real vdMag = mVdVec.GetMagnitude();
   
   //mfCamSingleRotAngle = ACos(-(mVdVec[2]/vdMag)) * DEG_PER_RAD;
   
   // compute axis of rotation
   //mfCamRotXAxis =  mVdVec[1];
   //mfCamRotYAxis = -mVdVec[0];
   //mfCamRotZAxis = 0.0;
   mUseSingleRotAngle = true;
   
   ComputeUpAngleAxis();
   
   #if DEBUG_PROJECTION
   MessageInterface::ShowMessage
      ("ComputeViewVectors() leaving, mfCamTransXYZ=%f, %f, %f, mfCamSingleRotAngle=%f\n"
       "   mfCamRotXYZ=%f, %f, %f mAxisLength=%f\n", mfCamTransX, mfCamTransY,
       mfCamTransZ, mfCamSingleRotAngle, mfCamRotXAxis, mfCamRotYAxis,
       mfCamRotZAxis, mAxisLength);
   #endif
} // end ComputeViewVectors(int frame)


//------------------------------------------------------------------------------
// void ComputeUpAngleAxis()
//    PS - Also pretty deprecated
//------------------------------------------------------------------------------
void OrbitViewCanvas::ComputeUpAngleAxis()
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
   
   #if DEBUG_PROJECTION
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::ComputeUpAngleAxis() mVpLocVec=%s, mVdVec=%s\n   "
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
void OrbitViewCanvas::TransformView()
{
   #if DEBUG_OrbitViewCanvas_DRAW
   MessageInterface::ShowMessage
      ("==> OrbitViewCanvas::TransformView() mUseSingleRotAngle=%d, "
       "mUseGluLookAt=%d, mIsEndOfData=%d, mIsEndOfRun=%d\n", mUseSingleRotAngle,
       mUseGluLookAt, mIsEndOfData, mIsEndOfRun);
   #endif
   
   glLoadIdentity();
   
   //MessageInterface::ShowMessage("==> TransformView() call gluLookAt()\n");
      
   
   if (mUseGluLookAt)
   {
      gluLookAt(mCamera.position[0], mCamera.position[1], mCamera.position[2],
                mCamera.view_center[0], mCamera.view_center[1], mCamera.view_center[2],
                mCamera.up[0], mCamera.up[1], mCamera.up[2]);
   }
   else
   {
      glTranslatef(mfCamTransX, mfCamTransY, mfCamTransZ);
      glRotatef(mfCamSingleRotAngle, mfCamRotXAxis, mfCamRotYAxis, mfCamRotZAxis);
      
      // DJC added for Up
      glRotatef(-mfUpAngle, mfUpXAxis, mfUpYAxis, -mfUpZAxis);
      
   } //if (mUseGluLookAt)
   
} // end TransformView()


//------------------------------------------------------------------------------
//  void DrawFrame()
//------------------------------------------------------------------------------
/**
 * Draws whole picture.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawFrame()
{
   #if DEBUG_ANIMATION
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::DrawFrame() mNumData=%d, mUsenitialViewPoint=%d\n"
       "   mViewCoordSysName=%s\n", mNumData, mUseInitialViewPoint,
       mViewCoordSysName.c_str());
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
   
   int numberOfData = mNumData;
   mIsEndOfData = false;
   mIsEndOfRun = false;
   mCurrIndex = 0;
   
   // refresh every 50 points (Allow user to set frame this increment?)
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
void OrbitViewCanvas::DrawPlot()
{
   if (mTotalPoints == 0)
      return;
   
   #if DEBUG_DRAW
   MessageInterface::ShowMessage
      ("===========================================================================\n");
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::DrawPlot() mTotalPoints=%d, mNumData=%d, mTime[%d]=%f\n",
       mTotalPoints, mNumData, mLastIndex, mTime[mLastIndex]);
   #endif
   #if DEBUG_DRAW > 1
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
   
   /*Rmatrix converterMatrix = mCoordConverter.GetLastRotationMatrix();
     mCoordMatrix = Rmatrix(4,4);
     for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
           mCoordMatrix.SetElement(i, j, converterMatrix.GetElement(i,j));
     mCoordMatrix.SetElement(3, 3, 1);
     mCoordMatrix = mCoordMatrix.Transpose();*/
   
   // compute projection if using initial viewpoint and not end of run or
   // if not using initial viewpoint and not first run.
   // We need initial values for  gulLookAt()
   if ((mUseInitialViewPoint && !mIsEndOfRun) ||
       (!mUseInitialViewPoint && mIsFirstRun && mUseGluLookAt))
   {
      ComputeViewVectors();
   }
   
   ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
      
   glDisable(GL_LIGHTING);
   
   // draw stars
   if (mDrawStars)
   {
      // drawing the stars at infinity requires them to have their own projection
      glMatrixMode(GL_PROJECTION); 
      glLoadIdentity();
      GLfloat aspect = (GLfloat)mCanvasSize.x / (GLfloat)mCanvasSize.y;
      glMatrixMode(GL_MODELVIEW);
      gluPerspective(mCamera.fovDeg, aspect, 0.1f, 50000000.0f);
      // they stars also need to be drawn in their own world view to be drawn at infinity
      Rvector3 starPosition = mCamera.position;
      Rvector3 starCenter = mCamera.view_center - starPosition;
      Rvector3 starUp = mCamera.up;
      
      #if DEBUG_DRAW > 1
      MessageInterface::ShowMessage
         ("   starCenter  =%s   starPosition=%s\n", starCenter.ToString().c_str(),
          starPosition.ToString().c_str());
      #endif
      
      // if star position is not zero vector then normalize (bug 2367 fix)
      if (!starPosition.IsZeroVector())
         starPosition.Normalize();
      starCenter += starPosition;
      
      gluLookAt(starPosition[0], starPosition[1], starPosition[2],
                starCenter[0], starCenter[1], starCenter[2],
                starUp[0], starUp[1], starUp[2]);
      
      glMultMatrixd(mCoordMatrix.GetDataVector());
      
      // draw the stars
      mStars->DrawStarsVA(1.0f, mStarCount, mDrawConstellations);
   }
   
   SetProjection();
   TransformView();
   
   // draw axes
   if (mDrawAxes)
      if (!mCanRotateAxes)
         DrawAxes();
   
   // draw equatorial plane
   if (mDrawXyPlane)
      DrawEquatorialPlane(mXyPlaneColor);
   
   // draw ecliptic plane
   if (mDrawEcPlane)
      DrawEclipticPlane(mEcPlaneColor);
      
   // draw object orbit
   DrawObjectOrbit(mNumData-1);
   
   if (mDrawSolverData)
      DrawSolverData();
   
   // draw Earth-Sun line
   if (mDrawSunLine)
      DrawSunLine();
   
   glFlush();
   SwapBuffers();

} // end DrawPlot()


//------------------------------------------------------------------------------
//  void DrawObject(const wxString &objName, int obj)
//------------------------------------------------------------------------------
/**
 * Draws object sphere and maps texture image.
 *
 * @param  objName  Name of the object
 * @param  obj      Index of the object for mObjectArray
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawObject(const wxString &objName, int obj)
{
   int frame = mLastIndex;
   int objId = GetObjectId(objName);
   
   #if DEBUG_DRAW > 1
   MessageInterface::ShowMessage
         ("OrbitViewCanvas::DrawObject() drawing:%s, obj=%d, objId:%d, frame:%d\n",
          objName.c_str(), obj, objId, frame);
   #endif
   
   //loj: 11/4/05 Added for light
   #ifdef ENABLE_LIGHT_SOURCE
   //-------------------------------------------------------
   // enable light source on option
   //-------------------------------------------------------
   if (mEnableLightSource && mSunPresent)
   {
      int sunId = GetObjectId("Sun");
      int index = sunId * MAX_DATA * 3 + frame * 3;
      
      if (sunId == UNKNOWN_OBJ_ID)
      {
         mLight.SetPosition(0.01f, 1.0f, 0.3f);
      }
      else
      {
         index = sunId * MAX_DATA * 3 + frame * 3;
         mLight.SetPosition(mObjectViewPos[index+0],mObjectViewPos[index+1],mObjectViewPos[index+2]);
      }
      mLight.SetDirectional(true);

      // Dunn is setting sun level a little dimmer to avoid washing out the models.
      mLight.SetColor(0.8f,0.8f,0.8f,1.0f);  
      // If 4th value is zero, the light source is directional one, and
      // (x,y,z) values describes its direction.
      // If 4th value is nonzero, the light is positional, and the (x,y,z) values
      // specify the location of the light in homogeneous object coordinates.
      // By default, a positional light radiates in all directions.
      
      float lpos[4];
      mLight.GetPositionf(lpos);
      glLightfv(GL_LIGHT0, GL_POSITION, lpos);
      glLightfv(GL_LIGHT0, GL_SPECULAR, mLight.GetColor());
      
      // enable the lighting
      glEnable(GL_LIGHTING);
   }
   #endif
   
   #if DEBUG_DRAW > 1
   MessageInterface::ShowMessage
      ("   mObjectTextureIdMap[%s]=%d\n", objName.c_str(),
       mObjectTextureIdMap[objName]);
   #endif
   
   // Rotate body before drawing texture
   if (mObjectArray[obj]->IsOfType(Gmat::CELESTIAL_BODY))
      RotateBody(objName, frame, objId);
   
   #if DEBUG_DRAW > 1
   MessageInterface::ShowMessage
      ("   After rotate body, mDrawAxes=%d, objId=%d, mOriginId=%d, mCanRotateAxes=%d\n",
       mDrawAxes, objId, mOriginId, mCanRotateAxes);
   #endif
   
   //-------------------------------------------------------
   // draw axes if it rotates with the body
   //-------------------------------------------------------
   // Note from Dunn.  This is for earth fixed axes that rotate with the earth.
   // If this is true, you do get axes that rotate with the earth, but you also
   // get +X and +Y ECI axis labels.  The DrawAxes function needs to be told
   // which labels to use, so it can show Earth Fixed labels.
   if (mDrawAxes && objId == mOriginId && mCanRotateAxes)
   //if(true)
   {
      // Before debugging the Earth Rotation Angle, and getting the texture map
      // to be correctly oriented in ECI space, Dunn has noticed that the ECF
      // axes seem to be rotated 90 degrees to the east.  To fix this we will
      // call an OpenGL rotate command here before and after drawing the axes in
      // order to get them correctly oriented wrt the prime meridian.
      glRotatef(-90.0, 0.0, 0.0, 1.0);
      
      // This next line is the NASA call that draws the ECF axes with ECI labels.
      // Dunn has commented it out and added the code to draw with correct labels.
      // This is a kludge that needs to be fixed.
      DrawAxes();
      
      //// Here is Dunn's temporary kludge.
      //glDisable(GL_LIGHTING);
      //glDisable(GL_LIGHT0);
      //wxString axisLabel;
      //GLfloat viewDist;
      
      //glLineWidth(2.5); // Thicker for ECF
      
      ////-----------------------------------
      //// draw axes
      ////-----------------------------------
      
      ////viewDist = mCurrViewDist/2; //zooms in and out
      //viewDist = mAxisLength;///1.8; // stays the same
      //Rvector3 axis;
      //Rvector3 origin;
      //origin.Set(0, 0, 0);
      
      //// PS - See Rendering.cpp
      //axis.Set(viewDist, 0, 0);
      //DrawLine(1, 0, 0, origin, axis);
      
      //axis.Set(0, viewDist, 0);
      //DrawLine(0, 1, 0, origin, axis);
      
      //axis.Set(0, 0, viewDist);
      //DrawLine(0, 0, 1, origin, axis);
      
      ////-----------------------------------
      //// throw some text out...
      ////-----------------------------------
      //// Dunn took out old minus signs to get axis labels at the correct end of
      //// each axis and thus make attitude correct.
      //glColor3f(1, 0, 0);     // red
      //axisLabel = "+X Earth Fixed";
      //DrawStringAt(axisLabel, +viewDist, 0.0, 0.0, 1.0);

      //glColor3f(0, 1, 0);     // green
      //axisLabel = "+Y Earth Fixed";
      //DrawStringAt(axisLabel, 0.0, +viewDist, 0.0, 1.0);

      //glColor3f(0, 0, 1);     // blue
      //axisLabel = "+Z Earth Fixed";
      //// 0.82 multiplier below so this label doesn't sit on top of ECI
      //DrawStringAt(axisLabel, 0.0, 0.0, +viewDist*0.82, 1.0); 

      //glLineWidth(1.0);

      //glEnable(GL_LIGHTING);
      //glEnable(GL_LIGHT0);

      // After rotating -90 to get the axes lined up wrt the texture map, it is
      // time to rotate back +90.  This is from Dunn.
      glRotatef(90.0, 0.0, 0.0, 1.0);
   }
   
   // Trying this, but why 90 degree offset ? (LOJ: 2010.11.19)
   glRotatef(90.0, 0.0, 0.0, 1.0);
   
   //-------------------------------------------------------
   // draw object with texture on option
   //-------------------------------------------------------
   if (mObjectTextureIdMap[objName] != GmatPlot::UNINIT_TEXTURE)
   {
      //glColor4f(1.0, 1.0, 1.0, 1.0);
      glColor3f(1.0, 1.0, 1.0);
      
      glMultMatrixd(mCoordMatrix.GetDataVector());
      
      /*Rmatrix coordMatrix = mCoordConverter.GetLastRotationMatrix();
        Rmatrix dataMatrix = Rmatrix(4,4);
        for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
        dataMatrix.SetElement(i, j, coordMatrix.GetElement(i,j));
        dataMatrix.SetElement(3, 3, 1);
        dataMatrix = dataMatrix.Transpose();
        glMultMatrixd(dataMatrix.GetDataVector());*/
      // Commented out to make body fixed to work (2010.11.19)
      ////loj:glMultMatrixd(mCoordMatrix.GetDataVector());
      
      glBindTexture(GL_TEXTURE_2D, mObjectTextureIdMap[objName]);
      glEnable(GL_TEXTURE_2D);

      if (objName == "Sun")
      {
         glDisable(GL_LIGHTING);
         DrawSphere(mObjectRadius[objId], 50, 50, GLU_FILL, GLU_INSIDE);
         glEnable(GL_LIGHTING);
      }
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
      #if DEBUG_DRAW
      MessageInterface::ShowMessage
         ("*** WARNING *** OrbitViewCanvas::DrawObject() %s texture not found.\n",
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
void OrbitViewCanvas::DrawObjectOrbit(int frame)
{
   #if DEBUG_DRAW
   MessageInterface::ShowMessage
      ("==========> DrawObjectOrbit() entered, frame=%d, mLastIndex=%d\n", frame,
       mLastIndex);
   #endif
   
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
      
      int index = objId * MAX_DATA + mLastIndex;
      
      #if DEBUG_DRAW
      MessageInterface::ShowMessage
         ("DrawObjectOrbit() obj=%d, objId=%d, objName='%s', index=%d\n",
          obj, objId, objName.c_str(), index);
      #endif
      
      // If not showing orbit just draw object, continue to next one
      //if (!mDrawOrbitFlag[objId*MAX_DATA+mLastIndex])
      if (!mDrawOrbitFlag[index])
      {
         #if DEBUG_DRAW
         MessageInterface::ShowMessage
            ("==> Not drawing orbit of '%s', so skip\n", objName.c_str());
         #endif
         
         // just draw object texture and continue
         if (mShowObjectMap[objName])
            DrawObjectTexture(objName, obj, objId, frame);
         
         continue;
      }
      
      // always draw orbit trajectory
      DrawOrbit(objName, obj, objId);
      
      //---------------------------------------------------------
      //draw object with texture
      //---------------------------------------------------------      
      if (mShowObjectMap[objName])
      {            
         DrawObjectTexture(objName, obj, objId, frame);
      }
   }
   
   #if DEBUG_DRAW
   MessageInterface::ShowMessage("==========> DrawObjectOrbit() leaving, frame=%d\n", frame);
   #endif
} // end DrawObjectOrbit()


//------------------------------------------------------------------------------
// void DrawOrbit(const wxString &objName, int obj, int objId)
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawOrbit(const wxString &objName, int obj, int objId)
{   
   glPushMatrix();
   glBegin(GL_LINES);
   
   #ifdef DEBUG_DRAW
   MessageInterface::ShowMessage
      ("==========> DrawOrbit() objName='%s', drawing first part\n",
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
      #ifdef DEBUG_DRAW
      MessageInterface::ShowMessage
         ("==========> DrawOrbit() objName='%s', drawing second part\n",
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
void OrbitViewCanvas::DrawOrbitLines(int i, const wxString &objName, int obj,
                                     int objId)
{
   int index1 = 0, index2 = 0;
   
   #ifdef DEBUG_ORBIT_LINES
   MessageInterface::ShowMessage
      ("DrawOrbitLines() entered, i=%d, objName='%s', obj=%d, objId=%d, "
       "mTime[%3d]=%f, mTime[%3d]=%f\n", i, objName.c_str(), obj, objId, i,
       mTime[i], i-1, mTime[i-1]);
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
         if (mObjectArray[obj]->IsOfType(Gmat::SPACECRAFT)){
            // We are drawing a spacecraft orbit.  This includes solver passes.
            *sIntColor = mObjectOrbitColor[colorIndex];}
         else {
            // We are drawing some other trajectory, say for a planet.
            *sIntColor = mObjectColorMap[objName].GetIntColor();}
         
         // PS - Rendering.cpp
         DrawLine(sGlColor, r1, r2);
      }
      
      // save last valid frame to show object at final frame
      mObjLastFrame[objId] = i;
      
      #ifdef DEBUG_ORBIT_LINES
      MessageInterface::ShowMessage
         ("DrawOrbitLines() leaving, mObjLastFrame[%d] = %d\n", objId, i);
      #endif
   }
}


//------------------------------------------------------------------------------
// void DrawObjectTexture(const wxString &objName, int obj, int objId, int frame)
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawObjectTexture(const wxString &objName, int obj,
                                        int objId, int frame)
{
   if (mNumData < 1)
      return;
   
   int index1 = objId * MAX_DATA * 3 + frame * 3;
   
   #if DEBUG_DRAW
   MessageInterface::ShowMessage
      ("DrawObjectTexture() entered, objName='%s', obj=%d, objId=%d, frame=%d, "
       "index1=%d, mObjLastFrame[%d]=%d\n", objName.c_str(), obj, objId, frame,
       index1, objId, mObjLastFrame[objId]);
   MessageInterface::ShowMessage
      ("   mObjectViewPos=%f, %f, %f\n", mObjectViewPos[index1+0],
       mObjectViewPos[index1+1], mObjectViewPos[index1+2]);
   #endif
   
   glPushMatrix();
   
   // put object at final position
   // Dunn took out old minus signs to make attitude correct.  Even though this
   // section was already commented out.  Just in case someone comments it back
   // in!
   //glTranslatef(mObjectViewPos[index1+0],
   //             mObjectViewPos[index1+1],
   //              mObjectViewPos[index1+2]);
   
   // first disable GL_TEXTURE_2D to show lines clearly
   // without this, lines are drawn dim (loj: 2007.06.11)
   glDisable(GL_TEXTURE_2D);
   
   #ifdef ENABLE_LIGHT_SOURCE
      //-------------------------------------------------------
      // enable light source on option
      //-------------------------------------------------------
      if (mEnableLightSource && mSunPresent)
      {
         int sunId = GetObjectId("Sun");
         int index;
         if (sunId == UNKNOWN_OBJ_ID)
         {
            mLight.SetPosition(0.01f, 1.0f, 0.3f);
            mLight.SetDirectional(true);
         }
         else
         {
            index = sunId * MAX_DATA * 3 + frame * 3;
            mLight.SetPosition(mObjectViewPos[index+0], mObjectViewPos[index+1], mObjectViewPos[index+2]);
            mLight.SetDirectional(false);
         }

         // Dunn is setting sunlight to be a little dimmer.
         mLight.SetColor(0.8f,0.8f,0.8f,1.0f);

         // opposite direction with  sun earth line
         //lightPos[0] = mObjectViewPos[index+0];
         //lightPos[1] = mObjectViewPos[index+1];
         //lightPos[2] = mObjectViewPos[index+2];
         // If 4th value is zero, the light source is directional one, and
         // (x,y,z) values describes its direction.
         // If 4th value is nonzero, the light is positional, and the (x,y,z) values
         // specify the location of the light in homogeneous object coordinates.
         // By default, a positional light radiates in all directions.
         
         // reset the light position to reflect the transformations
         float lpos[4], *color = mLight.GetColor();
         mLight.GetPositionf(lpos);
         glLightfv(GL_LIGHT0, GL_POSITION, lpos);
         glLightfv(GL_LIGHT0, GL_SPECULAR, color);
         //glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
         
         // enable the lighting
         glEnable(GL_LIGHTING);
         glEnable(GL_LIGHT0);
      }
   #endif
   
   // Draw spacecraft
   if (mObjectArray[obj]->IsOfType(Gmat::SPACECRAFT))
   {
      #if DEBUG_DRAW
      MessageInterface::ShowMessage("==> Drawing spacecraft '%s'\n", objName.c_str());
      #endif
      
      Spacecraft *spac = (Spacecraft*)mObjectArray[obj];
      ModelManager *mm = ModelManager::Instance();
      ModelObject *model = mm->GetModel(spac->modelID);
      // This was to darken the model when it is behind the Earth,
      // but was not pleasing visually and caused more problems
      // than it solved
      /*#ifdef ENABLE_LIGHT_SOURCE
        Rvector3 toCenter, cardinal;
        
        // Calculate whether or not the craft is in darkness
        toCenter = mLight.GetPosition();
        
        //toCenter.Set(lightPos[0], lightPos[1], lightPos[2]);
        if (toCenter.GetMagnitude() < (model->position[frame] - toCenter).GetMagnitude())
        isLit = true;
        else {
        cardinal = Cross(toCenter, Cross(model->position[frame], toCenter) / toCenter.GetMagnitude())
        / toCenter.GetMagnitude();
        isLit = cardinal.GetMagnitude() > mObjectRadius[GetObjectId("Earth")];
        }
        #endif*/
      
      if (spac->modelID != -1)
      {
         float     RTD         = (float)GmatMathConstants::DEG_PER_RAD;
         
         int attIndex = objId * MAX_DATA * 4 + mObjLastFrame[objId] * 4;
         Rvector quat = Rvector(4, mObjectQuat[attIndex+0], mObjectQuat[attIndex+1],
                                mObjectQuat[attIndex+2], mObjectQuat[attIndex+3]);
         Rvector3 EARad = Attitude::ToEulerAngles(quat, 1,2,3);
         
         // old code commented out (LOJ: 2010.11.22)
         // Extract spacecraft attitude from saved data files using subscriber
         //   Attitude  *scAttitude  = (Attitude*) spac->GetRefObject(Gmat::ATTITUDE, "");
         //   Rmatrix33 AMat         = scAttitude->GetCosineMatrix(mTime[frame]);
         //   Rvector3  EARad        = Attitude::ToEulerAngles(AMat,1,2,3);
         
         #ifdef DEBUG_SC_ATTITUDE
         MessageInterface::ShowMessage
            ("===> '%s', EARad=%s\n", objName.c_str(),EARad.ToString().c_str());
         #endif
         
         float     EAng1Deg     = float(EARad(0))*RTD;
         float     EAng2Deg     = float(EARad(1))*RTD;
         float     EAng3Deg     = float(EARad(2))*RTD;
         
         // Get offset rotation and scale from Spacecraft Visualization Tab in GUI.
         float     offset[3];
         float     rotation[3];
         float     scale;
         offset[0] = spac->GetRealParameter(spac->GetParameterID("ModelOffsetX"));
         offset[1] = spac->GetRealParameter(spac->GetParameterID("ModelOffsetY"));
         offset[2] = spac->GetRealParameter(spac->GetParameterID("ModelOffsetZ"));
         rotation[0] = spac->GetRealParameter(spac->GetParameterID("ModelRotationX"));
         rotation[1] = spac->GetRealParameter(spac->GetParameterID("ModelRotationY"));
         rotation[2] = spac->GetRealParameter(spac->GetParameterID("ModelRotationZ"));
         scale = spac->GetRealParameter(spac->GetParameterID("ModelScale"));
         model->SetBaseOffset(offset[0], offset[1], offset[2]);
         model->SetBaseRotation(true, rotation[0], rotation[1], rotation[2]);
         model->SetBaseScale(scale, scale, scale);
         
         // Dunn's new attitude call.  Need to change to quaternions.  Also need
         // to concatenate with BaseRotation.  Also need this to work for replay
         // animation buttons.
         model->Rotate(true, EAng1Deg, EAng2Deg, EAng3Deg);
         
         // The line above is where the object model gets its orientation.  This
         // also seems to be a good place to give the model its ECI position.
         // That call is actually in ModelObject.cpp on line 682.
         
         // Draw model
         glTranslatef(mObjectViewPos[index1+0],
                      mObjectViewPos[index1+1],
                      mObjectViewPos[index1+2]);
         model->Draw(frame, true); //isLit
         
         // Old code that may be worth saving
         //SetCurrent(*theContext);
         // ModelManager *mm = ModelManager::Instance();
         // SetCurrent(*mm->modelContext);
         // mm->modelContext->SetCurrent(*this);
         
         // Maybe delete this
         // Rvector3  eulersRad   = scAttitude->GetEulerAngles(mTime[frame],1,2,3);  // Uses the 123 Euler Sequence
         
         
         // Save line below for when Phil modifies model->Rotate to accept quats
         // Rvector   quaternion  = scAttitude->GetQuaternion(mTime[frame]);

         // Save line below for when I need to transpose a matrix.
         // Rmatrix33 TMat        = AMat.Transpose();
         
         // Neither of the calls below works.  The model is being positioned somewhere else!
         // PosX = 0.0;
         // PosY = 0.0;
         // PosZ = 15000.0;
         // model->Reposition(PosX,PosY,PosZ);
         // model->TranslateW(PosX,PosY,PosZ);
      }
      else
      {
         // Dunn took out old minus signs to make attitude correct.
         glTranslatef(mObjectViewPos[index1+0],
                      mObjectViewPos[index1+1],
                      mObjectViewPos[index1+2]);
         GlColorType *yellow = (GlColorType*)&GmatColor::YELLOW32;
         GlColorType *red = (GlColorType*)&GmatColor::RED32;
         *sIntColor = mObjectOrbitColor[objId*MAX_DATA+mObjLastFrame[objId]];
         // We want to differenciate spacecraft by orbit color (LOJ: 2011.02.16)
         //DrawSpacecraft(mScRadius, yellow, red);
         DrawSpacecraft(mScRadius, yellow, sGlColor);
      }
   }
   else
   {
      #if DEBUG_DRAW
      MessageInterface::ShowMessage("==> Drawing body '%s'\n", objName.c_str());
      #endif
      
      //put object at final position
      //
      // Dunn took out minus signs
      glTranslatef(mObjectViewPos[index1+0],mObjectViewPos[index1+1],mObjectViewPos[index1+2]);
      DrawObject(objName, obj);
   }
   
   if (mEnableLightSource && mSunPresent)
   {
      glDisable(GL_LIGHTING);
   }
   
   glPopMatrix();
}


//------------------------------------------------------------------------------
//  void DrawSolverData()
//------------------------------------------------------------------------------
/**
 * Draws solver iteration data
 * This is only called when drawing "current" solver data.  For drawing all
 * solver passes at the same time, see TrajPlotCanvas::UpdatePlot()
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawSolverData()
{
   Rvector3 start, end;
   int numPoints = mSolverAllPosX.size();
   
   #if DEBUG_SOLVER_DATA
   MessageInterface::ShowMessage
      ("==========> DrawSolverData() entered, solver points = %d\n", numPoints);
   #endif
   
   if (numPoints == 0)
      return;
   
   // Note that we're starting at 2 here rather than at 1.  There is a bug that
   // looks like a bad pointer when starting from 1 when the plot running in
   // "Current" mode.  We need to investigate this issue after the 2011a release
   // is out the door.  This TEMPORARY fix is in place so that the Mac, Linux
   // and Visual Studio builds won't crash for the "Current" setting.
   for (int i=2; i<numPoints; i++)
   {
      int numSc = mSolverAllPosX[i].size();      
      //MessageInterface::ShowMessage("==========> sc count = %d\n", numSc);
      
      //---------------------------------------------------------
      // draw lines
      //---------------------------------------------------------
      for (int sc=0; sc<numSc; sc++)
      {
         *sIntColor = mSolverIterColorArray[sc];         
         // Dunn took out old minus signs to make attitude correct.
         // Examining GMAT functionality in the debugger, this is only to show
         // the current solver iteration.  Somewhere else the multiple iterations 
         // are drawn.
         start.Set(mSolverAllPosX[i-1][sc],mSolverAllPosY[i-1][sc],mSolverAllPosZ[i-1][sc]);
         end.Set  (mSolverAllPosX[i]  [sc],mSolverAllPosY[i]  [sc],mSolverAllPosZ[i]  [sc]);
         
         // PS - See Rendering.cpp
         DrawLine(sGlColor, start, end);
      }
   }
   
   #if DEBUG_SOLVER_DATA
   MessageInterface::ShowMessage("==========> DrawSolverData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
//  void DrawEquatorialPlane(UnsignedInt color)
//------------------------------------------------------------------------------
/**
 * Draws equatorial plane circles.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawEquatorialPlane(UnsignedInt color)
{
   int i;
   float endPos[3];
   float distance;
   Real angle;
   
   glDisable(GL_LIGHTING);
   glDisable(GL_LINE_SMOOTH);
   glLineWidth(1.0f);
   
   //distance = (Real)mAxisLength;
   distance = (mCamera.position - mCamera.view_center).GetMagnitude();
   
   // set color
   *sIntColor = color;
   Rvector3 start, end;
   start.Set(0, 0, 0);

   //-----------------------------------
   // draw lines
   //-----------------------------------
   for (i=7; i<368; i+=15)
   {
      angle = GmatMathConstants::RAD_PER_DEG * ((Real)i);

      endPos[0] = distance * cos(angle);
      endPos[1] = distance * sin(angle);
      endPos[2] = 0.0;

      end.Set(endPos[0], endPos[1], endPos[2]);
      // PS - See Rendering.cpp
      DrawLine(sGlColor, start, end); 
   }

   //-----------------------------------
   // draw circles
   //-----------------------------------
   glPushMatrix();
   
   GLUquadricObj *qobj = gluNewQuadric();

   //==============================================================
   // Argosy code
   //==============================================================
   Real orthoDepth = distance;
   
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
   sGlColor->not_used = 255;
   
   glPopMatrix();
   glLineWidth(1.0f);
   glEnable(GL_LINE_SMOOTH);
   
} // end DrawEquatorialPlane()


//------------------------------------------------------------------------------
//  void DrawEclipticPlane(UnsignedInt color)
//------------------------------------------------------------------------------
/**
 * Draws ecliptic plane circles.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawEclipticPlane(UnsignedInt color)
{
   // First rotate the grand coordinate system to obliquity of the ecliptic
   // (23.5) and draw equatorial plane
   glPushMatrix();

   // Dunn changed 23.5 to -23.5.  When he changed -1 to 1 or +1 he got an 
   // RVector3 error.  This negative obliquity of the ecliptic around the
   // negative ECI X-Axis aligns the plane of the ecliptic with the sunline
   // after all minus signs for position have been removed.
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
void OrbitViewCanvas::DrawSunLine()
{
   //int frame = mNumData - 1;
   int frame = mLastIndex;
   if (frame <= 0)
      return;
   
   int sunId = GetObjectId("Sun");
   
   // check for Sun
   if (sunId == UNKNOWN_OBJ_ID)
      return;
   
   Rvector3 originPos, sunPos;
   Real distance = (Real)mAxisLength;
   Real mag;
   
   //--------------------------------
   // draw sun line
   //--------------------------------
   
   // set color
   *sIntColor = mSunLineColor;
   int index = 0;
   
   // draw one line from origin to Sun
   // Dunn took out old minus signs to make attitude correct.
   index = mOriginId * MAX_DATA * 3 + frame * 3;
   originPos.Set(mObjectViewPos[index+0], 
                 mObjectViewPos[index+1], 
                 mObjectViewPos[index+2]);
   
   index = sunId * MAX_DATA * 3 + frame * 3;
   sunPos.Set(mObjectViewPos[index+0], 
              mObjectViewPos[index+1], 
              mObjectViewPos[index+2]);
   
   // show lines between Sun and Earth and to -Sun
   // Dunn set it so sunline is only from origin out from earth in direction of
   // sun.
   // PS - See Rendering.cpp
   DrawLine(sGlColor, originPos, sunPos);
   //DrawLine(sGlColor, originPos, -sunPos);
   
   // Show Sun direction text
   glColor3f(1.0, 1.0, 0.0); // yellow
   
   // get sun unit vector and multiply by distance
   // Dunn changed the division factor from 2.2 to 2.0
   mag = sqrt(sunPos[0]*sunPos[0] + sunPos[1]*sunPos[1] + sunPos[2]*sunPos[2]);
   DrawStringAt(" +S", sunPos[0]/mag*distance/2.0,
                       sunPos[1]/mag*distance/2.0,
                       sunPos[2]/mag*distance/2.0,
                1.0);
   
} // end DrawSunLine()


//---------------------------------------------------------------------------
// void DrawAxes()
//---------------------------------------------------------------------------
void OrbitViewCanvas::DrawAxes()
{
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   wxString axisLabel;
   GLfloat viewDist;

   glLineWidth(2.0);
   
   //-----------------------------------
   // draw axes
   //-----------------------------------
   
   //viewDist = mCurrViewDist/2; //zooms in and out
   viewDist = mAxisLength;///1.8; // stays the same
   Rvector3 axis;
   Rvector3 origin;
   origin.Set(0, 0, 0);

   // PS - See Rendering.cpp
   axis.Set(viewDist, 0, 0);
   DrawLine(1, 0, 0, origin, axis);

   axis.Set(0, viewDist, 0);
   DrawLine(0, 1, 0, origin, axis);

   axis.Set(0, 0, viewDist);
   DrawLine(0, 0, 1, origin, axis);
   
   //-----------------------------------
   // throw some text out...
   //-----------------------------------
   // Dunn took out old minus signs to get axis labels at the correct end of
   // each axis and thus make attitude correct.
   glColor3f(1, 0, 0);     // red
   axisLabel = "+X ";
   DrawStringAt(axisLabel, +viewDist, 0.0, 0.0, 1.0);
   
   glColor3f(0, 1, 0);     // green
   axisLabel = "+Y ";
   DrawStringAt(axisLabel, 0.0, +viewDist, 0.0, 1.0);
   
   glColor3f(0, 0, 1);     // blue
   axisLabel = "+Z ";
   DrawStringAt(axisLabel, 0.0, 0.0, +viewDist, 1.0);
   
   glLineWidth(1.0);
   
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
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
void OrbitViewCanvas::DrawStatus(const wxString &label1, int frame,
                                 const wxString &label2, double time,
                                 int xpos, int ypos, const wxString &label3)
{
   #ifdef DEBUG_DRAW_STATUS
   MessageInterface::ShowMessage
      ("DrawStatus() entered, frame=%d, time=%.9f\n", frame, time);
   #endif
   
   //----------------------------------------------------
   // draw current frame number and time
   //----------------------------------------------------
   //loj: 5/23/05 I want to use glWindowPos2f but it is available in version 1.4
   // then I'll not need to set GL_PROJECTION mode
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
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
      #ifdef DEBUG_DRAW_STATUS
      MessageInterface::ShowMessage("  after convert time=%s\n", str.c_str());
      #endif
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
   
   /*text.Printf("Current Control Mode: ");
   switch (mControlMode)
   {
      case MODE_CENTERED_VIEW:
         str.Printf("Centered View ");
         break;
      case MODE_FREE_FLYING:
         str.Printf("Free-Flying ");
         break;
      case MODE_ASTRONAUT_6DOF:
         str.Printf("Astronaut 6DOF ");
         break;
   };
   text+=str;

   if (mInversion < 0)
      str.Printf("Inverted");
   else
      str.Printf("");
   text+=str;*/
   
   // Prepend space before coordinate system name (Bug 2318 fix)
   wxString viewCsName = "  " + mViewCoordSysName;
   glRasterPos2i(xpos, ypos+20);
   glCallLists(strlen(viewCsName.c_str()), GL_BYTE, (GLubyte*)viewCsName.c_str());
   
   // Do we use this? Commented out (LOJ: 2011.02.07)
   //wxLogStatus(GmatAppData::Instance()->GetMainFrame(),
   //            wxT("Frame#: %d, Time: %f"), frame, time);
   
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   
   #ifdef DEBUG_DRAW_STATUS
   MessageInterface::ShowMessage("DrawStatus() leaving\n");
   #endif
}


//---------------------------------------------------------------------------
// void RotateEarthUsingMha(const wxString &objName, int frame)
//---------------------------------------------------------------------------
/**
 * This method is old way of computing Earth rotation angle using MHA and
 * will be removed when rotation using Earth's attitude matrix is completely
 * tested.
 */
//---------------------------------------------------------------------------
void OrbitViewCanvas::RotateEarthUsingMha(const wxString &objName, int frame)
{
   #if DEBUG_ROTATE_BODY
   MessageInterface::ShowMessage
      ("RotateEarthUsingMha() entered, frame=%d\n", objName.c_str(), frame);
   #endif
   
   Real earthRotAngle = 0.0;
   
   // Dunn would like to note that the mInitialLongitude being used to initialize
   // the variable "initialLong" was calculated in ComputeLongitudeLst and is
   // a function of spacecraft position.  Its value comes from the following
   // equation - "lon = raDeg - mha;"
   Real initialLong = mInitialLongitude;
   
   // Dunn will try different offsets.  Need to understand where initialLong
   // comes from.
   //Real offset = 40.0; // need this to line up earth texture with longitude
   Real offset = 90.0;
   
   if (pSolarSystem)
   {         
      Real mha = 0.0;
      
      if (initialLong < 180.0)
         initialLong = -initialLong - offset;
      
      CelestialBody *earth = pSolarSystem->GetBody("Earth");
      if (earth)
         mha = earth->GetHourAngle(mTime[frame]);
      
      // Dunn would like to note that in the equation below, initialLong has
      // the value "-mha" in it which was calculated in ComputeLongitudeLst.
      // The variable earthRotAngle does continue to grow because initialLong
      // is a constant while mha continues to grow.  But really this equation
      // should be a function of GMST and nothing to do with spacecraft
      // longitude.
      //earthRotAngle = mha + initialLong + offset;
      earthRotAngle = mha + offset;
      
      #if DEBUG_ROTATE_BODY
      if (!mIsEndOfRun)
         MessageInterface::ShowMessage
            ("   frame=%3d, mha=%12.6f, initialLong=%12.6f, earthRotAngle=%12.6f\n",
             frame, mha, initialLong, earthRotAngle);
      #endif
   }
   
   earthRotAngle =
      AngleUtil::PutAngleInDegRange(earthRotAngle, 0.0, 360.0);
   
   #if DEBUG_ROTATE_BODY
   if (!mIsEndOfRun)
      MessageInterface::ShowMessage("   earthRotAngle=%f\n", earthRotAngle);
   #endif
   
   glRotatef(earthRotAngle, 0.0, 0.0, 1.0);
   
   #if DEBUG_ROTATE_BODY 
   MessageInterface::ShowMessage
      ("RotateEarthUsingMha() After rotate body, mDrawAxes=%d, mOriginId=%d, mCanRotateAxes=%d\n",
       mDrawAxes, mOriginId, mCanRotateAxes);
   #endif
}


//---------------------------------------------------------------------------
// void RotateBodyUsingAttitude(const wxString &objName, int objId)
//---------------------------------------------------------------------------
void OrbitViewCanvas::RotateBodyUsingAttitude(const wxString &objName, int objId)
{
   //=======================================================
   #ifdef ENABLE_OBJECT_ATTITUDE
   //=======================================================
   
   #if DEBUG_ROTATE_BODY > 1
   MessageInterface::ShowMessage
      ("RotateBodyUsingAttitude() '%s' entered, objId=%d, mLastIndex=%d, time=%f\n",
       objName.c_str(), objId, mLastIndex, mTime[mLastIndex]);
   #endif
   
   if (mTime[mLastIndex] == 0.0)
      return;
   
   // Any object that has an attitude (spacecraft, celestial sphere, and celestial bodies)
   // needs to be oriented correctly in the  coordinate system in which the Orbit View is drawn.    
   //
   // Define the following matrices:
   // R_IP:  the rotation matrix from the coordinate system of the plot to the inertial coordinate system 
   // R_IB:  the rotation matrix from celestial body fixed to inertial, for the body to be drawn in the Orbit View
   //
   // We can calculate the rotation matrix from celestial body fixed to the plot coordinate system, R_BP, using:
   //
   // R_BP = R_IB^T*R_IP;
   //
   // R_PB defines the rotation that must be applied to the celestial body before drawing in the Orbit View.
   // I don't know what OpenGL method is being used for this so we can talk about that this afternoon.
   // We may need to convert R_PB to quaternion or Euler angles  but the most efficient way would be to
   // just pass in R_PB.
   
   // Rotate body
   int attIndex = objId * MAX_DATA * 4 + mLastIndex * 4;
   
   Rvector quat = Rvector(4, mObjectQuat[attIndex+0], mObjectQuat[attIndex+1],
                          mObjectQuat[attIndex+2], mObjectQuat[attIndex+3]);
   
   #if DEBUG_ROTATE_BODY > 1
   MessageInterface::ShowMessage("==================================\n");
   MessageInterface::ShowMessage
      ("==> attIndex=%d, quat=%s\n", attIndex, quat.ToString(16, 1).c_str());
   #endif
   
   if (quat.IsZeroVector())
      return;
   
   // the rotation matrix from celestial body fixed to inertial
   Rmatrix33 matIB = Attitude::ToCosineMatrix(quat);
   
   #if DEBUG_ROTATE_BODY > 1
   MessageInterface::ShowMessage("==> matIB=\n%s", matIB.ToString(16, 25).c_str());
   #endif
   
   // Get the rotation matrix from the coordinate system of the plot to the inertial coordinate system
   Rvector6 inState, outState;
   int posIndex = objId * MAX_DATA * 3 + mLastIndex * 3;
   inState.Set(mObjectGciPos[posIndex+0], mObjectGciPos[posIndex+1],
               mObjectGciPos[posIndex+2], 0.0, 0.0, 0.0);
   
   #if DEBUG_ROTATE_BODY > 1
   MessageInterface::ShowMessage
      ("   posIndex=%d, inState=%s\n", posIndex, inState.ToString(16, 1).c_str());
   MessageInterface::ShowMessage
      ("   pViewCoordSystem=<%p>'%s', pInternalCoordSystem=<%p>'%s'\n",
       pViewCoordSystem,
       pViewCoordSystem ? pViewCoordSystem->GetName().c_str() : "NULL",
       pInternalCoordSystem,
       pInternalCoordSystem ? pInternalCoordSystem->GetName().c_str() : "NULL");
   #endif

   mCoordConverter.Convert(mTime[mLastIndex], inState, pViewCoordSystem,
                           outState, pInternalCoordSystem);
   
   Rmatrix33 matIP = mCoordConverter.GetLastRotationMatrix();
   
   #if DEBUG_ROTATE_BODY > 1
   MessageInterface::ShowMessage("==> matIP=\n%s", matIP.ToString(16, 25).c_str());
   #endif
   
   Rmatrix33 matBP = matIB.Transpose() * matIP;
   
   #if DEBUG_ROTATE_BODY > 1
   MessageInterface::ShowMessage("==> matPB=\n%s", matBP.ToString(16, 25).c_str());
   #endif
   
   // Compute angle and axis
   Rvector3 eAxis;
   Real eAngle;
   Attitude::DCMToEulerAxisAndAngle(matBP, eAxis, eAngle);
   
   // Convert to degree
   Real angInDeg = GmatMathUtil::RadToDeg(eAngle, true);
   
   #if DEBUG_ROTATE_BODY > 1
   MessageInterface::ShowMessage
      ("RotateBodyUsingAttitude() '%s', epoch=%f, attIndex=%d, eAngle=%f, angInDeg=%f\n"
       "   eAxis=%s\n", objName.c_str(), mTime[mLastIndex], attIndex, eAngle, angInDeg,
       eAxis.ToString().c_str());
   #endif
   
   // Now rotate
   glRotated(angInDeg, eAxis[0], eAxis[1], eAxis[2]);

   //=======================================================
   #endif
   //=======================================================
}


//---------------------------------------------------------------------------
// void RotateBody(const wxString &objName, int frame, int objId)
//---------------------------------------------------------------------------
void OrbitViewCanvas::RotateBody(const wxString &objName, int frame, int objId)
{
   #ifdef DEBUG_ROTATE_BODY
   MessageInterface::ShowMessage
      ("RotateBody() '%s' entered, objName='%s', objId=%d, mOriginId=%d, "
       "mOriginName='%s'\n", mPlotName.c_str(), objName.c_str(),
       objId, mOriginId, mOriginName.c_str());
   #endif
   
   #ifdef USE_MHA_TO_ROTATE_EARTH
   bool useMhaToRotateEarth = true;
   #else
   bool useMhaToRotateEarth = false;
   #endif
   
   if (objName == "Earth")
   {
      if (useMhaToRotateEarth)
         RotateEarthUsingMha(objName, frame);
      else
         RotateBodyUsingAttitude(objName, objId);
      
      return;
   }
   
   // Rotate other body
   RotateBodyUsingAttitude(objName, objId);
}


//---------------------------------------------------------------------------
// void ApplyEulerAngles()
//---------------------------------------------------------------------------
void OrbitViewCanvas::ApplyEulerAngles()
{
   //MessageInterface::ShowMessage("==> OrbitViewCanvas::ApplyEulerAngles()\n");
   
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
// int GetObjectId(const wxString &name)
//---------------------------------------------------------------------------
int OrbitViewCanvas::GetObjectId(const wxString &name)
{
   for (int i=0; i<mObjectCount; i++)
      if (mObjectNames[i] == name)
         return i;
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::GetObjectId() obj name: " + name +
       " not found in the object list\n");
   #endif
   
   return UNKNOWN_OBJ_ID;
}


//------------------------------------------------------------------------------
// void OrbitViewCanvas::ClearObjectArrays(bool deleteArrays = true)
//------------------------------------------------------------------------------
void OrbitViewCanvas::ClearObjectArrays(bool deleteArrays)
{
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage("OrbitViewCanvas::ClearObjectArrays() entered\n");
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
      
      if (mObjectViewPos)
         delete [] mObjectViewPos;
      
      if (mObjectQuat)
         delete [] mObjectQuat;
      
      if (mCoordData)
         delete [] mCoordData;
   }
   
   mObjectRadius = NULL;
   mObjMaxZoomIn = NULL;
   mObjLastFrame = NULL;
   mDrawOrbitFlag = NULL;
   mObjectOrbitColor = NULL;
   mObjectGciPos = NULL;
   mObjectViewPos = NULL;
   mObjectQuat = NULL;
   mCoordData = NULL;
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage("OrbitViewCanvas::ClearObjectArrays() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// bool OrbitViewCanvas::CreateObjectArrays()
//------------------------------------------------------------------------------
/*
 * Allocates arrays for objects.
 */
//------------------------------------------------------------------------------
bool OrbitViewCanvas::CreateObjectArrays()
{
   #if DEBUG_OBJECT
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
   
   if ((mObjectViewPos = new Real[mObjectCount*MAX_DATA*3]) == NULL)
      return false;
   
   if ((mObjectQuat = new Real[mObjectCount*MAX_DATA*4]) == NULL)
      return false;

   if ((mCoordData = new Real[MAX_DATA*16]) == NULL)
      return false;
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage("OrbitViewCanvas::CreateObjectArrays() exiting\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void UpdateSolverData(RealArray posX, RealArray posY, RealArray posZ, ...)
//------------------------------------------------------------------------------
void OrbitViewCanvas
::UpdateSolverData(const RealArray &posX, const RealArray &posY, const RealArray &posZ,
                   const UnsignedIntArray &scColors, bool solving)
{
   #if DEBUG_SOLVER_DATA
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::UpdateSolverData() entered, solving=%d\n", solving);
   #endif
   
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
   
   #if DEBUG_SOLVER_DATA
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::UpdateSolverData() leaving\n");
   #endif
}


//---------------------------------------------------------------------------
// void UpdateSpacecraftData(const RealArray &posX, const RealArray &posY, ...)
//---------------------------------------------------------------------------
void OrbitViewCanvas
::UpdateSpacecraftData(const Real &time,
                       const RealArray &posX, const RealArray &posY,
                       const RealArray &posZ, const RealArray &velX,
                       const RealArray &velY, const RealArray &velZ,
                       const UnsignedIntArray &scColors,
                       Integer solverOption)
{
   #if DEBUG_UPDATE
   static int sNumDebugOutput = 1000;
   #endif

   #if DEBUG_UPDATE
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::UpdateSpacecraftData() entered, time=%f, mScCount=%d, "
       "mOpenGLInitialized=%d, modelsAreLoaded=%d\n", time, mScCount,
       mOpenGLInitialized, modelsAreLoaded);
   #endif
   
   //-------------------------------------------------------
   // update spacecraft position
   //-------------------------------------------------------
   for (int sc=0; sc<mScCount; sc++)
   {
      int satId = GetObjectId(mScNameArray[sc].c_str());
      
      #if DEBUG_UPDATE
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::UpdateSpacecraftData() satId=%d, scName=%s\n", satId,
          mObjectNames[satId].c_str());
      #endif
      
      if (satId != UNKNOWN_OBJ_ID)
      {
         Spacecraft *spac = (Spacecraft*)mObjectArray[satId];
         int colorIndex = satId * MAX_DATA + mLastIndex;
         
         if (mOpenGLInitialized)
         {
            ModelManager *mm = ModelManager::Instance();
            if (!modelsAreLoaded)
            {
               if (spac->modelFile != "" && spac->modelID == -1)
               {
                  wxString modelPath(spac->modelFile.c_str());
                  if (GmatFileUtil::DoesFileExist(modelPath.c_str()))
                  {
                     spac->modelID = mm->LoadModel(modelPath);
                     #ifdef DEBUG_MODEL
                     MessageInterface::ShowMessage
                        ("UpdateSpacecraftData() loaded model '%s'\n", modelPath.c_str());
                     #endif
                  }
                  else
                  {
                     MessageInterface::ShowMessage
                        ("*** WARNING *** Cannot load the model file for spacecraft '%s'. "
                         "The file '%s' does not exist.\n", spac->GetName().c_str(),
                         modelPath.c_str());
                  }
               }
               
               // Set modelsAreLoaded to true if it went through all models
               if (sc == mScCount-1)
                  modelsAreLoaded = true;
            }
         }
         
         if (!mDrawOrbitArray[satId])
         {
            mDrawOrbitFlag[colorIndex] = false;
            #ifdef DEBUG_UPDATE
            MessageInterface::ShowMessage("===> mDrawOrbitArray[satId] is NULL\n");
            #endif
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
         
         #if DEBUG_UPDATE
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
         
         // if need to convert to internal coordinate system (EarthMJ2000Eq)
         if (mViewCsIsInternalCs)
         {
            CopyVector3(&mObjectGciPos[posIndex], &mObjectViewPos[posIndex]);
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
         }
         
         #if DEBUG_UPDATE
         if (mNumData < sNumDebugOutput)
         {
            MessageInterface::ShowMessage
               ("   satId:%d posIndex=%d, tmppos = %f, %f, %f\n", satId, posIndex,
                mObjectViewPos[posIndex+0], mObjectViewPos[posIndex+1],
                mObjectViewPos[posIndex+2]);
         }
         #endif
         
         // Update spacecraft attitude
         #if DEBUG_UPDATE
         MessageInterface::ShowMessage
            ("   Now updating spacecraft attitude of %d\n", satId);
         #endif
         
         UpdateSpacecraftAttitude(time, spac, satId);
      }
   }
   
   #if DEBUG_UPDATE
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::UpdateSpacecraftData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void UpdateSpacecraftAttitude(Real time, Spacecraft *sat, int satId)
//------------------------------------------------------------------------------
void OrbitViewCanvas::UpdateSpacecraftAttitude(Real time, Spacecraft *sat,
                                               int satId)
{
   if (sat == NULL)
      return;
   
   int attIndex = satId * MAX_DATA * 4 + (mLastIndex*4);
   
   Rmatrix33 cosMat = sat->GetAttitude(time);
   Rvector quat = Attitude::ToQuaternion(cosMat);
   mObjectQuat[attIndex+0] = quat[0];
   mObjectQuat[attIndex+1] = quat[1];
   mObjectQuat[attIndex+2] = quat[2];
   mObjectQuat[attIndex+3] = quat[3];
}


//------------------------------------------------------------------------------
// void UpdateOtherData(const Real &time)
//------------------------------------------------------------------------------
void OrbitViewCanvas::UpdateOtherData(const Real &time)
{
   for (int obj = 0; obj < mObjectCount; obj++)
   {
      SpacePoint *otherObj = mObjectArray[obj];
      
      // if object pointer is not NULL and not a spacecraft
      if (otherObj != NULL && otherObj->GetType() != Gmat::SPACECRAFT)
      {
         int objId = GetObjectId(mObjectNames[obj]);
         
         #if DEBUG_UPDATE_OBJECT
         MessageInterface::ShowMessage
            ("OrbitViewCanvas::UpdateOtherData() objId=%d, obj=%s\n", objId,
             mObjectNames[objId].c_str());
         #endif
         
         // if object id found
         if (objId != UNKNOWN_OBJ_ID)
         {
            int colorIndex = objId * MAX_DATA + mLastIndex;
            if (!mDrawOrbitArray[objId])
               mDrawOrbitFlag[colorIndex] = false;
            else
               mDrawOrbitFlag[colorIndex] = true;
            
            Rvector6 objState;
            try
            {
               objState = otherObj->GetMJ2000State(time);
            }
            catch (BaseException &)
            {
               mFatalErrorFound = true;
               throw;
            }
            
            int posIndex = objId * MAX_DATA * 3 + (mLastIndex*3);
            mObjectGciPos[posIndex+0] = objState[0];
            mObjectGciPos[posIndex+1] = objState[1];
            mObjectGciPos[posIndex+2] = objState[2];
            
            #if DEBUG_UPDATE_OBJECT > 1
            MessageInterface::ShowMessage
               ("OrbitViewCanvas::UpdateOtherData() %s, posIndex=%d, objState=%s\n",
                mObjectNames[obj].c_str(), posIndex, objState.ToString().c_str());
            #endif
            
            // convert objects to view CoordinateSystem
            if (mViewCsIsInternalCs)
            {
               CopyVector3(&mObjectViewPos[posIndex], &mObjectGciPos[posIndex]);
            }
            else
            {
               Rvector6 outState;
               
               mCoordConverter.Convert(time, objState, pInternalCoordSystem,
                                       outState, pViewCoordSystem);
               
               mObjectViewPos[posIndex+0] = outState[0];
               mObjectViewPos[posIndex+1] = outState[1];
               mObjectViewPos[posIndex+2] = outState[2];                  
            }
            
            #if DEBUG_UPDATE_OBJECT > 1
            MessageInterface::ShowMessage
               ("    %s posIndex=%d, tmppos = %f, %f, %f\n", mObjectNames[obj].c_str(),
                posIndex, mObjectViewPos[posIndex+0], mObjectViewPos[posIndex+1],
                mObjectViewPos[posIndex+2]);
            #endif
            
            // Update object's attitude
            UpdateOtherObjectAttitude(time, otherObj, objId);
         }
         else
         {
            #if DEBUG_UPDATE_OBJECT > 1
            MessageInterface::ShowMessage
               ("OrbitViewCanvas::UpdateOtherData() Cannot Add data. Invalid objId=%d\n",
                objId);
            #endif
         }
      }
      else
      {
         #if DEBUG_UPDATE_OBJECT > 1
         if (mObjectArray[obj] == NULL)
         {
            MessageInterface::ShowMessage
               ("OrbitViewCanvas::UpdateOtherData() Cannot add data. %s is NULL\n",
                mObjectNames[obj].c_str());
         }
         #endif
      }
   }
   
   int cIndex = mLastIndex*16;
   Rmatrix converterMatrix = mCoordConverter.GetLastRotationMatrix();
   for (int i = 0; i < 4; i++)
   {
      for (int j = 0; j < 4; j++)
      {
         if (j < 3 && i < 3)
            mCoordData[cIndex+(i*4)+j] = converterMatrix.GetElement(i,j);
         else
            mCoordData[cIndex+(i*4)+j] = 0;
      }
   }
   mCoordData[cIndex+15] = 1;
   
   mCoordMatrix = Rmatrix(4,4);
   for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
         mCoordMatrix.SetElement(i, j, converterMatrix.GetElement(i,j));
   mCoordMatrix.SetElement(3, 3, 1);
   mCoordMatrix = mCoordMatrix.Transpose();
}


//------------------------------------------------------------------------------
// void UpdateOtherObjectAttitude(Real time, SpacePoint *sp, int objId)
//------------------------------------------------------------------------------
void OrbitViewCanvas::UpdateOtherObjectAttitude(Real time, SpacePoint *sp,
                                                int objId)
{
   //=======================================================
   #ifdef ENABLE_OBJECT_ATTITUDE
   //=======================================================
   
   if (sp == NULL)
      return;
   
   int attIndex = objId * MAX_DATA * 4 + (mLastIndex*4);
   
   // Get attitude matrix   
   Rmatrix33 cosMat = sp->GetAttitude(time);
   Rvector quat = Attitude::ToQuaternion(cosMat);
   #ifdef DEBUG_ATTITUDE
   MessageInterface::ShowMessage
      ("UpdateOtherObjectAttitude() '%s', attIndex=%d, quat=%s", sp->GetName().c_str(),
       attIndex, quat.ToString().c_str());
   #endif
   mObjectQuat[attIndex+0] = quat[0];
   mObjectQuat[attIndex+1] = quat[1];
   mObjectQuat[attIndex+2] = quat[2];
   mObjectQuat[attIndex+3] = quat[3];
   
   //=======================================================
   #endif
   //=======================================================
}


//---------------------------------------------------------------------------
// void UpdateRotateFlags()
//---------------------------------------------------------------------------
/**
 * Updates flag for rotating axes when body rotates.
 */
//---------------------------------------------------------------------------
void OrbitViewCanvas::UpdateRotateFlags()
{
   AxisSystem *axis =
      (AxisSystem*)pViewCoordSystem->GetRefObject(Gmat::AXIS_SYSTEM, "");
   
   if (axis->IsOfType("BodyFixedAxes") &&
       (mOriginName.IsSameAs(axis->GetStringParameter("Origin").c_str())))
   {
      mCanRotateAxes = true;
   }
   else
   {
      mCanRotateAxes = false;
   }
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::UpdateRotateFlags() mCanRotateAxes=%d\n",
       mCanRotateAxes);
   #endif
}


//---------------------------------------------------------------------------
// bool ConvertObjectData()
//---------------------------------------------------------------------------
bool OrbitViewCanvas::ConvertObjectData()
{
   if (pInternalCoordSystem == NULL || pViewCoordSystem == NULL)
      return false;
   
   Rvector6 inState, outState;
   
   #if DEBUG_CONVERT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::ConvertObjectData() internalCS=%s, viewCSName=%s, viewCS=%d\n",
       pInternalCoordSystem->GetName().c_str(), pViewCoordSystem->GetName().c_str(),
       pViewCoordSystem);
   #endif
   
   // do not convert if view CS is internal CS
   if (mViewCsIsInternalCs)
   {
      #if DEBUG_CONVERT
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::ConvertObjectData() No conversion is needed. "
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
         
         #if DEBUG_CONVERT
         MessageInterface::ShowMessage
            ("OrbitViewCanvas::ConvertObjectData() mObjectNames[%d]=%s\n", objId,
             mObjectNames[i].c_str());
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
void OrbitViewCanvas::ConvertObject(int objId, int index)
{
   Rvector6 inState, outState;
   int start = objId*MAX_DATA*3+index*3;
   inState.Set(mObjectGciPos[start+0], mObjectGciPos[start+1],
               mObjectGciPos[start+2], 0.0, 0.0, 0.0);
   
   mCoordConverter.Convert(mTime[index], inState, pInternalCoordSystem,
                           outState, pViewCoordSystem);
   
   mObjectViewPos[index+0] = outState[0];
   mObjectViewPos[index+1] = outState[1];
   mObjectViewPos[index+2] = outState[2];
   
   #if DEBUG_CONVERT
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
Rvector3 OrbitViewCanvas::ComputeEulerAngles()
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
      
      #ifdef DEBUG_EULER
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::ComputeEulerAngles() sViewMat=\n"
          "   %f, %f, %f, %f\n   %f, %f, %f, %f\n"
          "   %f, %f, %f, %f\n   %f, %f, %f, %f\n",
          sViewMat[0], sViewMat[1], sViewMat[2], sViewMat[3],
          sViewMat[4], sViewMat[5], sViewMat[6], sViewMat[7],
          sViewMat[8], sViewMat[9], sViewMat[10], sViewMat[11],
          sViewMat[12], sViewMat[13], sViewMat[14], sViewMat[15]);
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::ComputeEulerAngles() mvmat=%s\n",
          mvmat.ToString().c_str());
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::ComputeEulerAngles() finalMat=%s\n",
          finalMat.ToString().c_str());
      #endif
   }
   #else
   {   
      Rvector3 upAxis((Real)mfUpXAxis, (Real)mfUpYAxis, (Real)mfUpZAxis);
      Rvector3 rotAxis = Rvector3((Real)mfCamRotXAxis, (Real)mfCamRotYAxis,
                                  (Real)mfCamRotZAxis);
      
      #ifdef DEBUG_EULER
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::ComputeEulerAngles() mfUpAngle=%f, upAxis=%s\n",
          mfUpAngle, upAxis.ToString().c_str());
      
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::ComputeEulerAngles() mfCamSingleRotAngle=%f, rotAxis=%s\n",
          mfCamSingleRotAngle, rotAxis.ToString().c_str());
      #endif
      
      Rmatrix33 upMat, rotMat;
      
      try
      {
         // compute cosine matrix
         if (upAxis.GetMagnitude() > 0.0)
            upMat = GmatAttUtil::ToCosineMatrix(mfUpAngle * GmatMathUtil::RAD_PER_DEG, upAxis);
         
         if (rotAxis.GetMagnitude() > 0.0)
            rotMat = GmatAttUtil::ToCosineMatrix(mfCamSingleRotAngle * GmatMathUtil::RAD_PER_DEG, rotAxis);
         
         //finalMat = rotMat * upMat;
         finalMat = upMat * rotMat;
         
         #ifdef DEBUG_EULER
         MessageInterface::ShowMessage
            ("OrbitViewCanvas::ComputeEulerAngles() \n  rotMat=%s  upMat=%s  "
             "finalMat=%s\n", rotMat.ToString().c_str(),
             upMat.ToString().c_str(), finalMat.ToString().c_str());
         #endif
         
      }
      catch (BaseException &e)
      {
         error = true;
         MessageInterface::ShowMessage
            ("*** ERROR *** OrbitViewCanvas::ComputeEulerAngles() %s\n",
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
      
      #ifdef DEBUG_EULER
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::ComputeEulerAngles() eulerAngle=%s\n",
          eulerAngle.ToString().c_str());
      #endif
      
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("*** ERROR *** OrbitViewCanvas::ComputeEulerAngles() %s\n",
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
   
   #ifdef DEBUG_EULER
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::ComputeEulerAngles() modAngle=%s\n",
       modAngle.ToString().c_str());
   #endif

   return modAngle;

   #endif
}


//---------------------------------------------------------------------------
// void ComputeLongitudeLst(Real time, Real x, Real y, Real *meanHourAngle,
//                          Real *longitude, Real *localSiderealTime)
//---------------------------------------------------------------------------
void OrbitViewCanvas::ComputeLongitudeLst(Real time, Real x, Real y,
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
   //
   // Dunn would like to note that in the code below, the variable "lon" is 
   // calculated using the position of the spacecraft combined with the hour
   // angle of the earth.  This is then used to computer "lst" which likely 
   // stands for "Local Sidereal Time".  Local Sidereal Time has to do with
   // longitude of a ground site and is not related at all to the location of
   // a spacecraft.  Dunn thinks this code is used here to figure out how much
   // to rotate the earth.  What really should be used is GMST or Greenwhich
   // Mean Sidereal Time, which only has to do with the epoch time being used
   // by the sim.  It has NOTHING to do with spacecraft location.
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
void OrbitViewCanvas::CopyVector3(Real to[3], Real from[3])
{
   to[0] = from[0];
   to[1] = from[1];
   to[2] = from[2];
}


//---------------------------------------------------------------------------
// bool LoadImage(const std::string &fileName)
//---------------------------------------------------------------------------
bool OrbitViewCanvas::LoadImage(const std::string &fileName)
{
#ifndef SKIP_DEVIL
   return false;
   
#else
   #if DEBUG_INIT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::LoadImage() Not using DevIL. file='%s'\n", fileName.c_str());
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
   
   #if DEBUG_INIT
   int size = width * height * 3;
   MessageInterface::ShowMessage
      ("   width=%d, height=%d, size=%d\n", width, height, size);
   #endif
   
   // Why is image upside down?
   // Get virtual mirror
   wxImage mirror = image.Mirror(false);
   GLubyte *data1 = mirror.GetData();
   
   //used for min and magnifying texture
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   
   //pass image to OpenGL
   gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB,
                  GL_UNSIGNED_BYTE, data1);
   
   return true;
#endif
}

