//$Id$
//------------------------------------------------------------------------------
//                              ViewCanvas
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun
// Created: 2010/04/19
/**
 * Implements ViewCanvas for 3D visualization.
 */
//------------------------------------------------------------------------------
#include "ViewCanvas.hpp"
#include "MdiGlPlotData.hpp"       // for GmatPlot::MAX_SCS
#include "Spacecraft.hpp"
#include "ModelManager.hpp"
#include "Rendering.hpp"           // for GlColorType
#include "FileUtil.hpp"            // for GmatFileUtil::DoesFileExist()
#include "GmatOpenGLSupport.hpp"   // for InitGL()
#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"
#include "SubscriberException.hpp"
#include "MessageInterface.hpp"

#if 0
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
#endif

//#define DEBUG_LOAD_IMAGE 1

#if 0
// color
static int *sIntColor = new int;
static GlColorType *sGlColor = (GlColorType*)sIntColor;
#endif

//---------------------------------
// static data
//---------------------------------
const Real ViewCanvas::RADIUS_ZOOM_RATIO = 2.2;

//------------------------------------------------------------------------------
// ViewCanvas(wxWindow *parent, wxWindowID id, ...)
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
ViewCanvas::ViewCanvas(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos, const wxSize& size,
                       const wxString& name, long style)
   #ifdef __USE_WX280_GL__
   : wxGLCanvas(parent, id, 0, pos, size, style, name)
   #else
   : wxGLCanvas(parent, id, pos, size, style, name)
   #endif
{
   // initialize pointers
   mParent = parent;
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   mCanvasSize = size;
   
   // initialization
   mPlotName = name;
   mGlInitialized = false;
   mViewPointInitialized = false;
   mOpenGLInitialized = false;
   modelsAreLoaded = false;
   
   // performance
   // if mNumPointsToRedraw =  0 It redraws whole plot
   // if mNumPointsToRedraw = -1 It does not clear GL_COLOR_BUFFER
   mNumPointsToRedraw = 0;
   mRedrawLastPointsOnly = false;
   mUpdateFrequency = 50;
   
   // ring buffer index
   mBeginIndex1 = 0;
   mBeginIndex2 = -1;
   mEndIndex1 = -1;
   mEndIndex2 = -1;
   mRealBeginIndex1 = 0;
   mRealBeginIndex2 = -1;
   mRealEndIndex1 = -1;
   mRealEndIndex2 = -1;
   mLastIndex = 0;
   mCurrIndex = -1;
   
   // data points
   mNumData = 0;
   mTotalPoints = 0;
   
   // data control flags
   mOverCounter = 0;
   mIsEndOfData = false;
   mIsEndOfRun = false;
   mWriteWarning = true;
   
   // view control
   mUseInitialViewPoint = true;
   
   // animation
   mIsAnimationRunning = false;
   mHasUserInterrupted = false;
   mUpdateInterval = 1;
   mFrameInc = 1;
   
   // message
   mShowMaxWarning = true;
   mOverCounter = 0;
   
   // solver data
   mDrawSolverData = false;
   
   // error handling and function mode
   mFatalErrorFound = false;
   mWriteRepaintDisalbedInfo = false;
   mInFunction = false;
   
   // solar system
   pSolarSystem = NULL;
   
   // Spacecraft
   mScCount = 0;
   
   // objects
   mObjectCount = 0;
   mObjectDefaultRadius = 200; //km: make big enough to see
   
   // Coordinate System
   pInternalCoordSystem = theGuiInterpreter->GetInternalCoordinateSystem();
   mInternalCoordSysName = wxString(pInternalCoordSystem->GetName().c_str());
   mViewCoordSysName = "";
   pViewCoordSystem = NULL;
   mViewCsIsInternalCs = true;
   
   // Initialize dynamic arrays to NULL
   ClearObjectArrays(false);
   
}


//------------------------------------------------------------------------------
// ~ViewCanvas()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ViewCanvas::~ViewCanvas()
{
}

//------------------------------------------------------------------------------
// wxGLContext* GetGLContext()
//------------------------------------------------------------------------------
/*
 * Return current GLContext pointer.
 */
//------------------------------------------------------------------------------
wxGLContext* ViewCanvas::GetGLContext()
{
   return theContext;
}


//------------------------------------------------------------------------------
// void SetGLContext(wxGLContext *glContext)
//------------------------------------------------------------------------------
void ViewCanvas::SetGLContext(wxGLContext *glContext)
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
// bool InitOpenGL()
//------------------------------------------------------------------------------
/**
 * Initializes GL and IL.
 */
//------------------------------------------------------------------------------
bool ViewCanvas::InitOpenGL()
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
      ("ViewCanvas::InitOpenGL() '%s' entered, calling InitGL()\n",
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
   
   #ifdef __USE_WX280_GL__
   SetCurrent(*theContext);
   #else
   SetCurrent();
   #endif
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("ViewCanvas::InitOpenGL() calling LoadGLTextures()\n");
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
      ("ViewCanvas::InitOpenGL() '%s' returning true\n", mPlotName.c_str());
   #endif
   
   return true;
}


//---------------------------------------------------------------------------
// void SetGl2dDrawingOption(const std::string &textureMap, bool drawFootPrints)
//---------------------------------------------------------------------------
void ViewCanvas::SetGl2dDrawingOption(const std::string &textureMap,
                                      Integer footPrintOption)
{
   // do nothing here
}


//---------------------------------------------------------------------------
// void SetGl3dDrawingOption(bool drawEcPlane, bool drawXyPlane, ...)
//---------------------------------------------------------------------------
void ViewCanvas::SetGl3dDrawingOption(bool drawEcPlane, bool drawXyPlane,
                                      bool drawWireFrame, bool drawAxes,
                                      bool drawGrid, bool drawSunLine,
                                      bool usevpInfo, bool drawStars,
                                      bool drawConstellations,
                                      Integer starCount)
{
   // do nothing here
}


//---------------------------------------------------------------------------
// void SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj, ...)
//---------------------------------------------------------------------------
void ViewCanvas::SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                                   SpacePoint *vdObj, Real vscaleFactor,
                                   const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                                   const Rvector3 &vdVec, const std::string &upAxis,
                                   bool usevpRefVec, bool usevpVec, bool usevdVec)
{
   // do nothing here
}


//---------------------------------------------------------------------------
// wxString GetGotoObjectName()
//---------------------------------------------------------------------------
wxString ViewCanvas::GetGotoObjectName()
{
   // return blank name
   return "";
}


//---------------------------------------------------------------------------
// void GotoObject(const wxString &objName)
//---------------------------------------------------------------------------
void ViewCanvas::GotoObject(const wxString &objName)
{
   // do nothing here
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
void ViewCanvas::UpdatePlot(const StringArray &scNames, const Real &time,
                            const RealArray &posX, const RealArray &posY,
                            const RealArray &posZ, const RealArray &velX,
                            const RealArray &velY, const RealArray &velZ,
                            const UnsignedIntArray &scColors, bool solving,
                            Integer solverOption, bool inFunction)
{
   if (IsFrozen())
      Thaw();
   
   // Commented out since spacecraft solar panel is not showing. (LOJ: 2011.04.25)
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
   
   if (mScCount > GmatPlot::MAX_SCS)
      mScCount = GmatPlot::MAX_SCS;
   
   #if DEBUG_UPDATE
   MessageInterface::ShowMessage
      ("===========================================================================\n");
   MessageInterface::ShowMessage
      ("ViewCanvas::UpdatePlot() plot=%s, time=%f, posX=%f, mNumData=%d, "
       "mScCount=%d, scColor=%u, solving=%d, solverOption=%d, inFunction=%d\n",
       GetName().c_str(), time, posX[0], mNumData, mScCount, scColors[0], solving,
       solverOption, inFunction);
   #endif
   
   
   //-----------------------------------------------------------------
   // Buffer data for plot
   //-----------------------------------------------------------------
   ComputeBufferIndex(time);
   mTime[mLastIndex] = time;
   
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
   {
      #if DEBUG_UPDATE
      MessageInterface::ShowMessage
         ("ViewCanvas::UpdatePlot() Calling InitializeViewPoint()\n");
      #endif
      InitializeViewPoint();
   }
   
   #if DEBUG_UPDATE
   MessageInterface::ShowMessage("ViewCanvas::UpdatePlot() leaving\n");
   #endif
}


//---------------------------------------------------------------------------
// void AddObjectList(wxArrayString &objNames, UnsignedIntArray &objColors,
//                    bool clearList=true)
//---------------------------------------------------------------------------
void ViewCanvas::AddObjectList(const wxArrayString &objNames,
                               const UnsignedIntArray &objColors,
                               bool clearList)
{
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::AddObjectList() entered, object count=%d, color count=%d\n",
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
            ("ViewCanvas::AddObjectList() Bind new texture object=%s\n",
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
         ("ViewCanvas::AddObjectList() objNames[%d]=%s\n",
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
   MessageInterface::ShowMessage("ViewCanvas::AddObjectList() leaving\n");
   #endif
   
} //AddObjectList()

//-----------------------
// protected methods
//-----------------------

//------------------------------------------------------------------------------
// void ResetPlotInfo()
//------------------------------------------------------------------------------
/**
 * Resets plotting information.
 */
//------------------------------------------------------------------------------
void ViewCanvas::ResetPlotInfo()
{
   mCurrIndex = -1;
   mNumData = 0;
   mTotalPoints = 0;
   
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

//---------------------------------------------------------------------------
// int GetObjectId(const wxString &name)
//---------------------------------------------------------------------------
int ViewCanvas::GetObjectId(const wxString &name)
{
   for (int i=0; i<mObjectCount; i++)
      if (mObjectNames[i] == name)
         return i;
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::GetObjectId() obj name: " + name +
       " not found in the object list\n");
   #endif
   
   return UNKNOWN_OBJ_ID;
}


//------------------------------------------------------------------------------
// void ClearObjectArrays(bool deleteArrays = true)
//------------------------------------------------------------------------------
void ViewCanvas::ClearObjectArrays(bool deleteArrays)
{
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage("ViewCanvas::ClearObjectArrays() entered\n");
   #endif

   if (deleteArrays)
   {
      if (mTime)
         delete [] mTime;
      
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

   mTime = NULL;
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
   MessageInterface::ShowMessage("ViewCanvas::ClearObjectArrays() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// bool CreateObjectArrays()
//------------------------------------------------------------------------------
/*
 * Allocates arrays for objects.
 */
//------------------------------------------------------------------------------
bool ViewCanvas::CreateObjectArrays()
{
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("CreateObjectArrays() allocating object arrays with %d\n", mObjectCount);
   #endif
   
   if ((mTime = new Real[MAX_DATA]) == NULL)
      return false;
   
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
   MessageInterface::ShowMessage("ViewCanvas::CreateObjectArrays() exiting\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void ComputeBufferIndex(Real time)
//------------------------------------------------------------------------------
void ViewCanvas::ComputeBufferIndex(Real time)
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
void ViewCanvas::ComputeActualIndex()
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
      ("ViewCanvas::ComputeActualIndex()     mBeginIndex1=%3d,     mEndIndex1=%3d, "
       "    mBeginIndex2=%3d,     mEndIndex2=%3d\n", mBeginIndex1, mEndIndex1,
       mBeginIndex2, mEndIndex2);
   MessageInterface::ShowMessage
      ("ViewCanvas::ComputeActualIndex() mRealBeginIndex1=%3d, mRealEndIndex1=%3d, "
       "mRealBeginIndex2=%3d, mRealEndIndex2=%3d\n", mRealBeginIndex1, mRealEndIndex1,
       mRealBeginIndex2,  mRealEndIndex2);
   #endif
}


//------------------------------------------------------------------------------
// void UpdateSolverData(RealArray posX, RealArray posY, RealArray posZ, ...)
//------------------------------------------------------------------------------
void ViewCanvas::UpdateSolverData(const RealArray &posX, const RealArray &posY,
                       const RealArray &posZ, const UnsignedIntArray &scColors,
                       bool solving)
{
   #if DEBUG_SOLVER_DATA
   MessageInterface::ShowMessage
      ("ViewCanvas::UpdateSolverData() entered, solving=%d\n", solving);
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
      ("ViewCanvas::UpdateSolverData() leaving\n");
   #endif
}


//---------------------------------------------------------------------------
// void UpdateSpacecraftData(const RealArray &posX, const RealArray &posY, ...)
//---------------------------------------------------------------------------
void ViewCanvas::UpdateSpacecraftData(const Real &time,
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
      ("ViewCanvas::UpdateSpacecraftData() entered, time=%f, mScCount=%d, "
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
         ("ViewCanvas::UpdateSpacecraftData() satId=%d, scName=%s\n", satId,
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
      ("ViewCanvas::UpdateSpacecraftData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void UpdateOtherData(const Real &time)
//------------------------------------------------------------------------------
void ViewCanvas::UpdateOtherData(const Real &time)
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
            ("ViewCanvas::UpdateOtherData() objId=%d, obj=%s\n", objId,
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
               ("ViewCanvas::UpdateOtherData() %s, posIndex=%d, objState=%s\n",
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
               ("ViewCanvas::UpdateOtherData() Cannot Add data. Invalid objId=%d\n",
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
               ("ViewCanvas::UpdateOtherData() Cannot add data. %s is NULL\n",
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
// void UpdateSpacecraftAttitude(Real time, Spacecraft *sat, int satId)
//------------------------------------------------------------------------------
void ViewCanvas::UpdateSpacecraftAttitude(Real time, Spacecraft *sat,
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
// void UpdateOtherObjectAttitude(Real time, SpacePoint *sp, int objId)
//------------------------------------------------------------------------------
void ViewCanvas::UpdateOtherObjectAttitude(Real time, SpacePoint *sp,
                                                int objId)
{
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
}


//---------------------------------------------------------------------------
//  void CopyVector3(Real to[3], Real from[3])
//---------------------------------------------------------------------------
void ViewCanvas::CopyVector3(Real to[3], Real from[3])
{
   to[0] = from[0];
   to[1] = from[1];
   to[2] = from[2];
}


//---------------------------------------------------------------------------
// bool LoadImage(const std::string &fileName)
//---------------------------------------------------------------------------
bool ViewCanvas::LoadImage(const std::string &fileName)
{
   #if DEBUG_LOAD_IMAGE
   MessageInterface::ShowMessage
      ("ViewCanvas::LoadImage() file='%s'\n", fileName.c_str());
   #endif
   
   if (fileName == "")
      return false;
   
   wxImage image = wxImage(fileName.c_str());
   int width = image.GetWidth();
   int height = image.GetHeight();
   
   GLubyte *data = image.GetData();
   
   if (data == NULL)
      return false;
   
   #if DEBUG_LOAD_IMAGE
   int size = width * height * 3;
   MessageInterface::ShowMessage
      ("   width=%d, height=%d, size=%d\n", width, height, size);
   #endif
   
   // Why is image upside down?
   // Get vertial mirror
   wxImage mirror = image.Mirror(false);
   GLubyte *data1 = mirror.GetData();
   
   //used for min and magnifying texture
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      
   //pass image to opengl
   #ifndef __WXGTK__
      // This call crashes GMAT on Linux, so it is excluded here. 
      gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB,
                        GL_UNSIGNED_BYTE, data1);
   #endif
   
   return true;
}


//---------------------------------------------------------------------------
// void DrawStatus(const wxString &label1, unsigned int textColor, ...)
//---------------------------------------------------------------------------
/*
 * Writes status at the bottom of the frame
 */
//---------------------------------------------------------------------------
void ViewCanvas::DrawStatus(const wxString &label1, unsigned int textColor,
                            const wxString &label2, double time,
                            int xpos, int ypos, const wxString &label3)
{
   int frame = mTotalPoints;
   
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
   
   //glColor3f(1, 0, 0); //red
   GlColorType *color = (GlColorType*)&textColor;
   glColor3ub(color->red, color->green, color->blue);
   glRasterPos2i(xpos, ypos);
   glCallLists(strlen(text.c_str()), GL_BYTE, (GLubyte*)text.c_str());
   
   if (label3 != "")
   {
      glRasterPos2i(xpos, 50);
      glCallLists(strlen(label3.c_str()), GL_BYTE, (GLubyte*)label3.c_str());
   }
   
   // Prepend space before coordinate system name (Bug 2318 fix)
   wxString viewCsName = "  " + mViewCoordSysName;
   glRasterPos2i(xpos, ypos+20);
   glCallLists(strlen(viewCsName.c_str()), GL_BYTE, (GLubyte*)viewCsName.c_str());
   
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   
   #ifdef DEBUG_DRAW_STATUS
   MessageInterface::ShowMessage("DrawStatus() leaving\n");
   #endif
}
