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
#include "GmatMainFrame.hpp"       // for EnableAnimation()
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

// always define USE_TRACKBALL, it works better for rotation
#define USE_TRACKBALL
#ifdef USE_TRACKBALL
#include "AttitudeUtil.hpp"
using namespace FloatAttUtil;
#endif

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

BEGIN_EVENT_TABLE(OrbitViewCanvas, ViewCanvas)
   EVT_SIZE(OrbitViewCanvas::OnSize)
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
//const int OrbitViewCanvas::UNKNOWN_OBJ_ID = -999;

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
   // extra data need
   mNeedAttitude = true;
   
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
      {
         #if DEBUG_INIT
         MessageInterface::ShowMessage
            ("   Setting new wxGLContext(this) to ModelManager::modelContext\n");
         #endif
         mm->modelContext = new wxGLContext(this);
      }
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
      
   //mAxisLength = mCurrViewDist;
   mAxisLength = DEFAULT_DIST;
   
   mOriginName = "";
   mOriginId = 0;
   
   //original value
   
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
   
   // drawing options
   mDrawXyPlane = false;
   mDrawEcPlane = false;
   mDrawAxes = false;
   mNeedAttitude = true;
   
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
   
   // Note:
   // deleting m_glContext is handled in wxGLCanvas
   
   #ifndef __WXMAC__
      ModelManager *mm = ModelManager::Instance();
      if (!mm->modelContext)
      {
         // delete modelContext since it was created in the constructor
         delete mm->modelContext;
         mm->modelContext = NULL;
      }
   #endif
   
   ClearObjectArrays();
   
   #ifdef DEBUG_RESOURCE_CLEARING
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::~OrbitViewCanvas() '%s' exiting\n", mPlotName.c_str());
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
      ("OrbitViewCanvas::SetGlObject() '%s' entered\n", mPlotName.c_str());
   #endif

   ViewCanvas::SetGlObject(objNames, objOrbitColors, objArray);
   
   // Initialize objects used in view
   SetDefaultViewPoint();
      
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetGlObject() '%s' leaving\n", mPlotName.c_str());
   #endif
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
      ("OrbitViewCanvas::SetGlCoordSystem() '%s' entered\n", mPlotName.c_str());
   #endif
   
   ViewCanvas::SetGlCoordSystem(internalCs, viewCs, viewUpCs);
   
   // if view coordinate system origin is spacecraft, make spacecraft radius smaller.
   // So that spacecraft won't overlap each other.
   //@todo: need better way to scale spacecraft size.  See Dunn's comments above.
   if (viewCs->GetOrigin()->IsOfType(Gmat::SPACECRAFT))
      mScRadius = 30;
   else if (viewCs->GetOrigin()->IsOfType(Gmat::CELESTIAL_BODY))
      mScRadius = mObjectRadius[mOriginId] * 0.03;
   
   mMaxZoomIn = mObjMaxZoomIn[mOriginId];
   
   if (mUseInitialViewPoint)
      mAxisLength = mMaxZoomIn;
   
   UpdateRotateFlags();
   
   #if DEBUG_CS
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetGlCoordSystem() '%s' leaving\n", mPlotName.c_str());
   #endif
   
} // end SetGlCoordSystem()


//------------------------------------------------------------------------------
// void SetGl3dDrawingOption(bool drawEcPlane, bool drawXyPlane, ...)
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetGl3dDrawingOption(bool drawEcPlane, bool drawXyPlane,
                                           bool drawWireFrame, bool drawAxes,
                                           bool drawGrid, bool drawSunLine,
                                           bool usevpInfo, bool drawStars,
                                           bool drawConstellations,
                                           Integer starCount)
{
   mDrawEcPlane = drawEcPlane;
   mDrawXyPlane = drawXyPlane;
   mDrawWireFrame = drawWireFrame;
   mDrawAxes = drawAxes;
   mDrawGrid = drawGrid;
   mDrawSunLine = drawSunLine;
   mUseInitialViewPoint = usevpInfo;
   mDrawStars = drawStars;
   mDrawConstellations = drawConstellations;
   mStarCount = starCount;
   Refresh(false);   
}


//------------------------------------------------------------------------------
// void SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
//                      SpacePoint *vdObj, Real vsFactor,
//                      const Rvector3 &vpRefVec, const Rvector3 &vpVec,
//                      const Rvector3 &vdVec, bool usevpRefVec,
//                      bool usevpVec, bool usevdVec)
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
void OrbitViewCanvas::SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                                        SpacePoint *vdObj, Real vsFactor,
                                        const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                                        const Rvector3 &vdVec, const std::string &upAxis,
                                        bool usevpRefVec, bool usevpVec, bool usevdVec)
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
   
   Rvector3 lvpRefVec(vpRefVec);
   Rvector3 lvpVec(vpVec);
   Rvector3 lvdVec(vdVec);
      
   #if DEBUG_PROJECTION
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetGlViewOption() pViewPointRefObj=%p, "
       "pViewPointVectorObj=%p\n   pViewDirectionObj=%p, mViewScaleFactor=%f   "
       "mViewPointRefVector=%s\n   mViewPointVector=%s, mViewDirectionVector=%s, "
       "mViewUpAxisName=%s\n   mUseViewPointRefVector=%d, mUseViewDirectionVector=%d\n",
       pViewPointRefObj, pViewPointVectorObj, pViewDirectionObj, mViewScaleFactor,
       lvpRefVec.ToString(10).c_str(), lvpVec.ToString(10).c_str(),
       lvdVec.ToString(10).c_str(), mViewUpAxisName.c_str(), mUseViewPointRefVector,
       mUseViewDirectionVector);
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
   
} //end SetGl3dViewOption()


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


//------------------------------------------------------------------------------
// void OnPaint(wxPaintEvent& event)
//------------------------------------------------------------------------------
/**
 * Processes wxPaintEvent.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::OnPaint(wxPaintEvent& event)
{
   #ifdef DEBUG_ON_PAINT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::OnPaint() entered, mFatalErrorFound=%d, mGlInitialized=%d, mObjectCount=%d\n",
       mFatalErrorFound, mGlInitialized, mObjectCount);
   #endif
   
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
      #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("OrbitViewCanvas::OnPaint() Calling InitOpenGL()\n");
      #endif
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
// void OnSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
/**
 * Processes wxSizeEvent.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::OnSize(wxSizeEvent& event)
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
   
   mouseX = event.GetX();
   mouseY = event.GetY();
   
   // First, flip the mouseY value so it is oriented right (bottom left is 0,0)
   flippedY = height - mouseY;
   
   GLfloat fEndX = mfLeftPos + ((GLfloat)mouseX /(GLfloat)width) *
      (mfRightPos - mfLeftPos);
   GLfloat fEndY = mfBottomPos + ((GLfloat)flippedY /(GLfloat)height)*
      (mfTopPos - mfBottomPos);
   
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
//  void SetupProjection()
//------------------------------------------------------------------------------
/**
 * Sets up how object is viewed
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetupProjection()
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
   // Setup how we view the world
   GLfloat aspect = (GLfloat)mCanvasSize.x / (GLfloat)mCanvasSize.y;
         
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
// void TransformView()
//------------------------------------------------------------------------------
void OrbitViewCanvas::TransformView()
{
   #if DEBUG_OrbitViewCanvas_DRAW
   MessageInterface::ShowMessage
      ("==> OrbitViewCanvas::TransformView() mUseGluLookAt=%d, mIsEndOfData=%d, "
       "mIsEndOfRun=%d\n", mUseGluLookAt, mIsEndOfData, mIsEndOfRun);
   #endif
   
   glLoadIdentity();
      
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
// virtual void HandleLightSource()
//------------------------------------------------------------------------------
void OrbitViewCanvas::HandleLightSource()
{
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
}


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
   
   // Set background color to black
   glClearColor(0.0, 0.0, 0.0, 1.0);
   
   if (mRedrawLastPointsOnly || mNumPointsToRedraw == 0)
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   else
      glClear(GL_DEPTH_BUFFER_BIT);
   
   DrawStatus("", GmatColor::YELLOW32, "  Epoch: ", mTime[mLastIndex], 0, 5);
   
   // Plot is not refreshed when another panel is opened, so add glFlush()
   // and SwapBuffers() (loj: 4/5/06)
   //if (mNumData < 1) // to avoid 0.0 time
   if (mNumData < 1 && !mDrawSolverData) // to avoid 0.0 time
   {
      glFlush();
      SwapBuffers();
      return;
   }
   
   ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
   
   glDisable(GL_LIGHTING);
   
   // draw stars
   if (mDrawStars)
      DrawStars();
   
   SetupProjection();
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
   
   if (mEnableLightSource && mSunPresent)
   {
      // we don't want the orbit paths lit
      glDisable(GL_LIGHTING);
   }
   
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
   
   // Draw spacecraft
   if (mObjectArray[obj]->IsOfType(Gmat::SPACECRAFT))
   {
      #if DEBUG_DRAW
      MessageInterface::ShowMessage("==> Drawing spacecraft '%s'\n", objName.c_str());
      #endif
      
      Spacecraft *spac = (Spacecraft*)mObjectArray[obj];
      //ModelManager *mm = ModelManager::Instance();
      //ModelObject *model = mm->GetModel(spac->modelID);
      
      if (spac->modelID != -1)
      {
         DrawSpacecraft3dModel(spac, objId, frame);
      }
      else
      {
         // Dunn took out old minus signs to make attitude correct.
         glTranslatef(mObjectViewPos[index1+0],
                      mObjectViewPos[index1+1],
                      mObjectViewPos[index1+2]);
         GlColorType *yellow = (GlColorType*)&GmatColor::YELLOW32;
         //GlColorType *red = (GlColorType*)&GmatColor::RED32;
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
   
   #if DEBUG_DRAW > 1
   MessageInterface::ShowMessage
      ("   mTextureIdMap[%s]=%d\n", objName.c_str(),
       mTextureIdMap[objName]);
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
   if (mTextureIdMap[objName] != GmatPlot::UNINIT_TEXTURE)
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
      
      glBindTexture(GL_TEXTURE_2D, mTextureIdMap[objName]);
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
   
   if (mEnableLightSource && mSunPresent)
   {
      glDisable(GL_LIGHTING);
   }
   
} // end DrawObject(const wxString &objName)


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


//------------------------------------------------------------------------------
// void DrawSpacecraft3dModel(Spacecraft *sc, int objId, int frame)
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawSpacecraft3dModel(Spacecraft *sc, int objId, int frame)
{
   ModelManager *mm = ModelManager::Instance();
   ModelObject *model = mm->GetModel(sc->modelID);
   
   float RTD = (float)GmatMathConstants::DEG_PER_RAD;
   
   int index1 = objId * MAX_DATA * 3 + frame * 3;
   int attIndex = objId * MAX_DATA * 4 + mObjLastFrame[objId] * 4;
   
   Rvector quat = Rvector(4, mObjectQuat[attIndex+0], mObjectQuat[attIndex+1],
                          mObjectQuat[attIndex+2], mObjectQuat[attIndex+3]);
   Rvector3 EARad = Attitude::ToEulerAngles(quat, 1,2,3);
   
   #ifdef DEBUG_SC_ATTITUDE
   MessageInterface::ShowMessage
      ("===> '%s', EARad=%s\n", objName.c_str(),EARad.ToString().c_str());
   #endif
   
   float EAng1Deg = float(EARad(0)) * RTD;
   float EAng2Deg = float(EARad(1)) * RTD;
   float EAng3Deg = float(EARad(2)) * RTD;
   
   // Get offset rotation and scale from Spacecraft Visualization Tab in GUI.
   float     offset[3];
   float     rotation[3];
   float     scale;
   offset[0]   = sc->GetRealParameter(sc->GetParameterID("ModelOffsetX"));
   offset[1]   = sc->GetRealParameter(sc->GetParameterID("ModelOffsetY"));
   offset[2]   = sc->GetRealParameter(sc->GetParameterID("ModelOffsetZ"));
   rotation[0] = sc->GetRealParameter(sc->GetParameterID("ModelRotationX"));
   rotation[1] = sc->GetRealParameter(sc->GetParameterID("ModelRotationY"));
   rotation[2] = sc->GetRealParameter(sc->GetParameterID("ModelRotationZ"));
   scale = sc->GetRealParameter(sc->GetParameterID("ModelScale"));
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
   
   model->Draw(true); //isLit
   
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


//---------------------------------------------------------------------------
// void DrawStars()
//---------------------------------------------------------------------------
void OrbitViewCanvas::DrawStars()
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


//---------------------------------------------------------------------------
// void RotateBodyUsingAttitude(const wxString &objName, int objId)
//---------------------------------------------------------------------------
void OrbitViewCanvas::RotateBodyUsingAttitude(const wxString &objName, int objId)
{
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
   
   // Rotate other body
   RotateBodyUsingAttitude(objName, objId);
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


