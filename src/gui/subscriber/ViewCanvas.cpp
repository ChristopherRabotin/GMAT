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
#include "FileManager.hpp"         // for GetFullPathname()
#include "GmatOpenGLSupport.hpp"   // for InitGL()
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"       // for GetMainFrameStatusBar()
#include "GuiInterpreter.hpp"
#include "SubscriberException.hpp"
#include "MessageInterface.hpp"


//http://www.opengl.org/resources/features/KilgardTechniques/oglpitfall/
// When you desire high-quality texture mapping, you will typically specify a
// mipmapped texture filter. Mipmapping lets you specify multiple levels of
// detail for a texture image. Each level of detail is half the size of the
// previous level of detail in each dimension. So if your initial texture
// image is an image of size 32x32, the lower levels of detail will be of
// size 16x16, 8x8, 4x4, 2x2, and 1x1. Typically, you use the gluBuild2DMipmaps
// routine to automatically construct the lower levels of details from you
// original image. This routine re-samples the original image at each level
// of detail so that the image is available at each of the various smaller sizes.
#define __ENABLE_MIPMAPS__

//#define DEBUG_GL_INFO
//#define DEBUG_INIT 1
//#define DEBUG_LOAD_IMAGE 1
//#define DEBUG_TEXTURE 1
//#define DEBUG_LOAD_MODEL 1
//#define DEBUG_OBJECT 1
//#define DEBUG_DATA_BUFFERRING 1

//---------------------------------
// static data
//---------------------------------
const Real ViewCanvas::RADIUS_ZOOM_RATIO = 2.2;

// color
static int *sIntColor = new int;
static GlColorType *sGlColor = (GlColorType*)sIntColor;

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
   // @note
   // I want to use wxFULL_REPAINT_ON_RESIZE for style so when window is resized
   // it automatically repaints, but it flickers too much. I tried to use
   // wxBufferedPaintDC() to reduce the flickering, but I see no difference.
   // Currently it repaints when I make window size larger, but not when I
   // reduce the size. (LOJ: 2011.07.01)
   #ifdef __USE_WX280_GL__
   : wxGLCanvas(parent, id, 0, pos, size, style, name)
   #else
   : wxGLCanvas(parent, id, pos, size, style, name)
   #endif
{
   // initialize pointers
   mParent = parent;
   GmatAppData *gmatAppData = GmatAppData::Instance();
   theGuiInterpreter = gmatAppData->GetGuiInterpreter();
   theStatusBar = gmatAppData->GetMainFrame()->GetMainFrameStatusBar();
   mCanvasSize = size;
   
   // initialization
   mPlotName = name;
   mGlInitialized = false;
   mViewPointInitialized = false;
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
   mNeedVelocity = false;
   mNeedAttitude = false;
   
   // drawing options
   mDrawWireFrame = false;
   mDrawGrid = false;
   
   // light source
   mSunPresent = false;
   mEnableLightSource = true;
   
   // view control
   mUseInitialViewPoint = true;
   mAxisLength = 30000.0;
   
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
   #ifdef DEBUG_VIEWCANVAS
   MessageInterface::ShowMessage("ViewCanvas::~ViewCanvas() '%s' entered\n", mPlotName.c_str());
   #endif
   
   // Cleanup textures
   for (std::map<wxString, GLuint>::iterator i = mTextureIdMap.begin();
        i != mTextureIdMap.end(); ++i)
   {
      GLuint texId = i->second;
      #ifdef DEBUG_VIEWCANVAS
      MessageInterface::ShowMessage("   texId = %3d, obj = '%s'\n", texId, i->first.c_str());
      #endif
      if (i->first != "" && texId != GmatPlot::UNINIT_TEXTURE)
      {
         glDeleteTextures(1, &texId);
      }
   }
   
   #ifdef DEBUG_VIEWCANVAS
   MessageInterface::ShowMessage("ViewCanvas::~ViewCanvas() '%s' leaviing\n", mPlotName.c_str());
   #endif
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
// bool InitializePlot()
//------------------------------------------------------------------------------
/**
 * Initializes GL and IL.
 */
//------------------------------------------------------------------------------
bool ViewCanvas::InitializePlot()
{
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("\nViewCanvas::InitializePlot() '%s' entered\n", mPlotName.c_str());
   #endif
   
   // Add things to do here
   wxPaintDC dc(this);
   
   #ifndef __WXMOTIF__
      #ifndef __USE_WX280_GL__
         if (!GetContext()) return false;
      #endif
   #endif
   
   #ifdef __USE_WX280_GL__
      theContext->SetCurrent(*this);
      SetCurrent(*theContext);
   #else
      SetCurrent();
   #endif
   
   InitOpenGL();
   
   // load body textures
   LoadBodyTextures();
   
   // load spacecraft models
   LoadSpacecraftModels();
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("ViewCanvas::InitializePlot() '%s' leaving\n\n", mPlotName.c_str());
   #endif
   
   return true;
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
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("ViewCanvas::InitOpenGL() '%s' entered, calling InitGL()\n",
       mPlotName.c_str());
   #endif
   
   #ifdef DEBUG_GL_INFO
   char *GL_version = (char*)glGetString(GL_VERSION);
   MessageInterface::ShowMessage("   GL version  = '%s'\n", GL_version);
   char *GL_vendor = (char*)glGetString(GL_VENDOR);
   MessageInterface::ShowMessage("   GL vendor   = '%s'\n", GL_vendor);
   char *GL_renderer = (char*)glGetString(GL_RENDERER);
   MessageInterface::ShowMessage("   GL renderer = '%s'\n", GL_renderer);
   char *GLU_version = (char*)gluGetString(GLU_VERSION);
   MessageInterface::ShowMessage("   GLU version = '%s'\n", GLU_version);
   #endif
   
   #ifdef DEBUG_GL_MORE_INFO
   char *GL_ext = (char*)glGetString(GL_EXTENSIONS);
   char *GLU_ext = (char*)gluGetString(GLU_EXTENSIONS);
   MessageInterface::ShowMessage("   GL extensions = '%s'\n", GL_ext);
   MessageInterface::ShowMessage("   GLU extensions = '%s'\n", GLU_ext);
   #endif
   
   InitGL();
   
   #ifdef __USE_WX280_GL__
   SetCurrent(*theContext);
   #else
   SetCurrent();
   #endif
   
   // set pixel format
   if (!SetPixelFormatDescriptor())
   {
      //throw SubscriberException("SetPixelFormatDescriptor failed\n");
   }
   
   // set font
   SetDefaultGLFont();
      
   mShowMaxWarning = true;
   mIsAnimationRunning = false;
   mGlInitialized = true;
   
   #ifdef DEBUG_INIT_OPENGL
   MessageInterface::ShowMessage
      ("ViewCanvas::InitOpenGL() '%s' returning true\n", mPlotName.c_str());
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// virtual void SetObjectColors(const wxStringColorMap &objectColorMap)
//------------------------------------------------------------------------------
void ViewCanvas::SetObjectColors(const wxStringColorMap &objectColorMap)
{
   mObjectColorMap = objectColorMap;
}


//------------------------------------------------------------------------------
// virtual void SetObjectColors(const wxStringColorMap &objectColorMap)
//------------------------------------------------------------------------------
void ViewCanvas::SetShowObjects(const wxStringBoolMap &showObjMap)
{
   mShowObjectMap = showObjMap;
}


//------------------------------------------------------------------------------
// void SetGlObject(const StringArray &objNames,
//                  const UnsignedIntArray &objOrbitColors,
//                  const std::vector<SpacePoint*> &objArray)
//------------------------------------------------------------------------------
void ViewCanvas::SetGlObject(const StringArray &objNames,
                             const UnsignedIntArray &objOrbitColors,
                             const std::vector<SpacePoint*> &objArray)
{
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::SetGlObject() '%s' entered, objCount=%d, colorCount=%d.\n",
       mPlotName.c_str(), objNames.size(), objOrbitColors.size());
   #endif
   
   #if 0
   // Initialize objects used in view
   SetDefaultViewPoint();
   #endif
   
   mObjectArray = objArray;
   wxArrayString tempList;
   int scCount = 0;
   
   if (objNames.size() == objOrbitColors.size() &&
       objNames.size() == objArray.size())
   {      
      for (UnsignedInt i=0; i<objNames.size(); i++)
      {
         tempList.Add(objNames[i].c_str());
         
         // Set spacecraft names and count
         if (mObjectArray[i]->IsOfType(Gmat::SPACECRAFT))
         {
            mScNameArray.push_back(objNames[i]);
            scCount++;
         }
         
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
      MessageInterface::ShowMessage("ViewCanvas::SetGlObject() object sizes "
                                    "are not the same. No objects added.\n");
   }
   
   mScCount = scCount;
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::SetGlObject() '%s' leaving\n", mPlotName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void ViewCanvas::SetSolarSystem(SolarSystem *ss)
{
   pSolarSystem = ss;
}


//------------------------------------------------------------------------------
// void SetGlCoordSystem(CoordinateSystem *internalCs,CoordinateSystem *viewCs,
//                       CoordinateSystem *viewUpCs)
//------------------------------------------------------------------------------
void ViewCanvas::SetGlCoordSystem(CoordinateSystem *internalCs,
                                  CoordinateSystem *viewCs,
                                  CoordinateSystem *viewUpCs)
{
   #if DEBUG_CS
   MessageInterface::ShowMessage
      ("ViewCanvas::SetGlCoordSystem() '%s' entered, internalCs=<%p>, viewCs=<%p>, "
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


//---------------------------------------------------------------------------
// void SetGl2dDrawingOption(const std::string &centralBodyName,
//         const std::string &textureMap, bool drawFootPrints)
//---------------------------------------------------------------------------
void ViewCanvas::SetGl2dDrawingOption(const std::string &centralBodyName,
                                      const std::string &textureMap,
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


//------------------------------------------------------------------------------
// void SetGlDrawOrbitFlag(const std::vector<bool> &drawArray)
//------------------------------------------------------------------------------
void ViewCanvas::SetGlDrawOrbitFlag(const std::vector<bool> &drawArray)
{
   mDrawOrbitArray = drawArray;
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::SetGlDrawObjectFlag() mDrawOrbitArray.size()=%d, "
       "mObjectCount=%d\n", mDrawOrbitArray.size(), mObjectCount);
   
   bool draw;
   for (int i=0; i<mObjectCount; i++)
   {
      draw = mDrawOrbitArray[i] ? true : false;      
      MessageInterface::ShowMessage
         ("ViewCanvas::SetGlDrawObjectFlag() i=%d, mDrawOrbitArray[%s]=%d\n",
          i, mObjectNames[i].c_str(), draw);
   }
   #endif
}


//------------------------------------------------------------------------------
// void SetGlShowObjectFlag(const std::vector<bool> &showArray)
//------------------------------------------------------------------------------
void ViewCanvas::SetGlShowObjectFlag(const std::vector<bool> &showArray)
{
   mShowObjectArray = showArray;

   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::SetGlDrawObjectFlag() mDrawOrbitArray.size()=%d, "
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
         ("ViewCanvas::SetGlShowObjectFlag() i=%d, mShowObjectMap[%s]=%d\n",
          i, mObjectNames[i].c_str(), show);
      #endif
   }
   
   // Handle light source
   HandleLightSource();
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::SetGlDrawObjectFlag() mEnableLightSource=%d, mSunPresent=%d\n",
       mEnableLightSource, mSunPresent);
   #endif
}


//------------------------------------------------------------------------------
// void SetNumPointsToRedraw(Integer numPoints)
//------------------------------------------------------------------------------
void ViewCanvas::SetNumPointsToRedraw(Integer numPoints)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("ViewCanvas::SetNumPointsToRedraw() entered, numPoints=%d\n", numPoints);
   #endif
   
   mNumPointsToRedraw = numPoints;
   mRedrawLastPointsOnly = false;
   
   // if mNumPointsToRedraw =  0 It redraws whole plot
   // if mNumPointsToRedraw = -1 It does not clear GL_COLOR_BUFFER
   if (mNumPointsToRedraw > 0)
      mRedrawLastPointsOnly = true;
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("ViewCanvas::SetNumPointsToRedraw() leaving, mRedrawLastPointsOnly=%d\n",
       mRedrawLastPointsOnly);
   #endif
}


//------------------------------------------------------------------------------
// void SetUpdateFrequency(Integer updFreq)
//------------------------------------------------------------------------------
void ViewCanvas::SetUpdateFrequency(Integer updFreq)
{
   mUpdateFrequency = updFreq;
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
// void SetEndOfRun(bool flag = true)
//------------------------------------------------------------------------------
void ViewCanvas::SetEndOfRun(bool flag)
{
   #if DEBUG_UPDATE
   MessageInterface::ShowMessage
      ("ViewCanvas::SetEndOfRun() ViewCanvas::SetEndOfRun() flag=%d, "
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
         ("ViewCanvas::SetEndOfRun() mIsEndOfRun=%d, mNumData=%d\n",
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
   }
}


//---------------------------------------------------------------------------
// void TakeAction(const std::string &action)
//---------------------------------------------------------------------------
/**
 * Perform any actions for GL plot
 */
//---------------------------------------------------------------------------
void ViewCanvas::TakeAction(const std::string &action)
{
   #ifdef DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("ViewCanvas::TakeAction() '%s' entered, action = '%s'\n", mPlotName.c_str(),
       action.c_str());
   #endif
   
   if (action == "ClearSolverData")
   {
      mSolverAllPosX.clear();
      mSolverAllPosY.clear();
      mSolverAllPosZ.clear();
   }
   else if (action == "ClearObjects")
   {
      mObjectCount = 0;
      mObjectArray.clear();
   }
   else if (action == "PenUp")
   {
      mIsDrawing[mLastIndex] = true;
   }
   else if (action == "PenDown")
   {
      // We don't want to connect to new point so set it to false
      mIsDrawing[mLastIndex] = false;
   }
   
   #ifdef DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("ViewCanvas::TakeAction() '%s' leaving, action = '%s'\n", mPlotName.c_str(),
       action.c_str());
   #endif
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
 * @param <drawing> true if drawing is enabled
 * @param <inFunction> true if data is published inside a function
 */
//------------------------------------------------------------------------------
void ViewCanvas::UpdatePlot(const StringArray &scNames, const Real &time,
                            const RealArray &posX, const RealArray &posY,
                            const RealArray &posZ, const RealArray &velX,
                            const RealArray &velY, const RealArray &velZ,
                            const UnsignedIntArray &scColors, bool solving,
                            Integer solverOption, bool drawing, bool inFunction)
{
   if (IsFrozen())
      Thaw();
   
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
       "mScCount=%d, scColor=%u, solving=%d, solverOption=%d, drawing=%d, inFunction=%d\n",
       GetName().c_str(), time, posX[0], mNumData, mScCount, scColors[0], solving,
       solverOption, drawing, inFunction);
   #endif
   
   
   //-----------------------------------------------------------------
   // Buffer data for plot
   //-----------------------------------------------------------------
   ComputeBufferIndex(time);
   mTime[mLastIndex] = time;
   mIsDrawing[mLastIndex] = drawing;
   
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
      
      if (mTextureIdMap.find(objNames[i]) == mTextureIdMap.end())
      {
         #if DEBUG_OBJECT
         MessageInterface::ShowMessage
            ("ViewCanvas::AddObjectList() Bind new texture object=%s\n",
             objNames[i].c_str());
         #endif
         
         mTextureIdMap[objNames[i]] = GmatPlot::UNINIT_TEXTURE;
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
// bool SetPixelFormatDescriptor()
//------------------------------------------------------------------------------
/**
 * Sets pixel format on Windows.
 */
//------------------------------------------------------------------------------
bool ViewCanvas::SetPixelFormatDescriptor()
{
#ifdef __WXMSW__
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("ViewCanvas::SetPixelFormatDescriptor() entered\n");
   #endif
   
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
      ("ViewCanvas::SetPixelFormatDescriptor() pixelFormatId = %d \n",
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
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("ViewCanvas::SetPixelFormatDescriptor() returning true\n");
   #endif
   
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
void ViewCanvas::SetDefaultGLFont()
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
// virtual void SetDrawingMode()
//------------------------------------------------------------------------------
void ViewCanvas::SetDrawingMode()
{
   // set OpenGL to recognize the counter clockwise defined side of a polygon
   // as its 'front' for lighting and culling purposes
   glFrontFace(GL_CCW);
   
   // enable face culling, so that polygons facing away (defines by front face)
   // from the viewer aren't drawn (for efficiency).
   glEnable(GL_CULL_FACE);
   
   // tell OpenGL to use glColor() to get material properties for..
   glEnable(GL_COLOR_MATERIAL);
   
   // the front face's ambient and diffuse components
   glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
   
   // Set the ambient lighting
   GLfloat ambient[4] = {0.4f, 0.4f, 0.4f, 1.0f};
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
   
   // Set viewport size
   int nWidth, nHeight;
   GetClientSize(&nWidth, &nHeight);
   glViewport(0, 0, nWidth, nHeight);
   
   // Set plygon drawng mode
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
}


//------------------------------------------------------------------------------
// virtual void HandleLightSource()
//------------------------------------------------------------------------------
void ViewCanvas::HandleLightSource()
{
   // do nothing here
}


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
      
      if (mIsDrawing)
         delete [] mIsDrawing;
      
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
      
      if (mObjectViewVel)
         delete [] mObjectViewVel;
      
      if (mObjectQuat)
         delete [] mObjectQuat;
      
      if (mCoordData)
         delete [] mCoordData;
   }
   
   mTime = NULL;
   mIsDrawing = NULL;
   mObjectRadius = NULL;
   mObjMaxZoomIn = NULL;
   mObjLastFrame = NULL;
   mDrawOrbitFlag = NULL;
   mObjectOrbitColor = NULL;
   mObjectGciPos = NULL;
   mObjectViewPos = NULL;
   mObjectViewVel = NULL;
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
      ("ViewCanvas::CreateObjectArrays() allocating object arrays with %d\n",
       mObjectCount);
   #endif
   
   if ((mTime = new Real[MAX_DATA]) == NULL)
      return false;
   
   if ((mIsDrawing = new bool[MAX_DATA]) == NULL)
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

   if (mNeedVelocity)
      if ((mObjectViewVel = new Real[mObjectCount*MAX_DATA*3]) == NULL)
         return false;
   
   if (mNeedAttitude)
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
   
   // Compute last buffer index
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
   #ifdef DEBUG_COMPUTE_ACTUAL_INDEX
   MessageInterface::ShowMessage
      ("ViewCanvas::ComputeActualIndex() mRedrawLastPointsOnly=%d, mIsEndOfRun=%d\n"
       "   mBeginIndex1=%d, mEndIndex1=%d, mBeginIndex2=%d, mEndIndex2=%d\n",
       mRedrawLastPointsOnly, mIsEndOfRun, mBeginIndex1, mEndIndex1, mBeginIndex2,
       mEndIndex2);
   #endif
   
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
   
   #ifdef DEBUG_COMPUTE_ACTUAL_INDEX
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
// virtual bool LoadBodyTextures()
//------------------------------------------------------------------------------
bool ViewCanvas::LoadBodyTextures()
{
   #if DEBUG_TEXTURE
   MessageInterface::ShowMessage
      ("ViewCanvas::LoadBodyTextures() entered, mObjectCount=%d\n", mObjectCount);
   #endif
   
   //--------------------------------------------------
   // load object texture if used
   //--------------------------------------------------
   for (int i=0; i<mObjectCount; i++)
   {
      //if (mObjectArray[i]->IsOfType(Gmat::SPACECRAFT))
      //   continue;
      
      if (mTextureIdMap[mObjectNames[i]] == GmatPlot::UNINIT_TEXTURE)
      {
         #if DEBUG_TEXTURE > 1
         MessageInterface::ShowMessage
            ("ViewCanvas::LoadBodyTextures() object=<%p>'%s'\n",
             mObjectArray[i], mObjectNames[i].c_str());
         #endif
         
         mTextureIdMap[mObjectNames[i]] =
            BindTexture(mObjectArray[i], mObjectNames[i]);
      }
   }
   
   #if DEBUG_TEXTURE
   MessageInterface::ShowMessage
      ("ViewCanvas::LoadBodyTextures() leaving, mObjectCount=%d\n", mObjectCount);
   #endif
   
   return true;   
}


//------------------------------------------------------------------------------
// virtual GLuint BindTexture(SpacePoint *obj, const wxString &objName)
//------------------------------------------------------------------------------
/**
 * Binds texture map for the body. It uses texture file set for this plot.
 * If it was not set then it uses texture file from the body.
 */
//------------------------------------------------------------------------------
GLuint ViewCanvas::BindTexture(SpacePoint *obj, const wxString &objName)
{
   GLuint texId = GmatPlot::UNINIT_TEXTURE;
   std::string textureFile;
   
   // if texture file specified then use it
   if (mTextureFileMap.find(objName) != mTextureFileMap.end())
      textureFile = mTextureFileMap[objName];
   
   #if DEBUG_TEXTURE
   MessageInterface::ShowMessage
      ("ViewCanvas::BindTexture() entered, objName='%s', textureFile = '%s'\n",
       objName.c_str(), textureFile.c_str());
   #endif
   
   try
   {
      if (obj->IsOfType(Gmat::CELESTIAL_BODY))
      {
         // use texture map set by user for this plot, if not set use from the celestial body
         if (textureFile == "" || !GmatFileUtil::DoesFileExist(textureFile))
         {
            CelestialBody *body = (CelestialBody*) obj;
            textureFile = body->GetStringParameter(body->GetParameterID("TextureMapFileName"));
         }
      }
      else if (obj->IsOfType(Gmat::SPACECRAFT))
      {
         FileManager *fm = FileManager::Instance();
         std::string iconLoc = fm->GetFullPathname("ICON_PATH");
         
         #ifdef DEBUG_TEXTURE
         MessageInterface::ShowMessage("   iconLoc = '%s'\n", loc.c_str());
         #endif
         
         // Check if icon file directory exist
         if (GmatFileUtil::DoesDirectoryExist(iconLoc.c_str(), false))
         {
            textureFile = iconLoc + "/Spacecraft.png";
         }
      }
      
      #if DEBUG_TEXTURE
      MessageInterface::ShowMessage
         ("   theContext=<%p>, textureFile='%s'\n", theContext, textureFile.c_str());
      #endif
      
      #ifdef __USE_WX280_GL__
         SetCurrent(*theContext);
      #else
         SetCurrent();
      #endif

      // Generate text id and bind it before loading image
      glGenTextures(1, &texId);
      glBindTexture(GL_TEXTURE_2D, texId);
      
      #if DEBUG_TEXTURE
      GLenum error = glGetError();
      MessageInterface::ShowMessage
         ("   texId = %3d, error = %d, GL_INVALID_VALUE = %d, UNINIT_TEXTURE = %d\n",
          texId, error, GL_INVALID_VALUE, GmatPlot::UNINIT_TEXTURE);
      #endif
      
      if (!LoadImage(textureFile))
      {
         if (obj->IsOfType(Gmat::CELESTIAL_BODY))
         {
            MessageInterface::ShowMessage
               ("*** WARNING *** ViewCanvas::BindTexture() Cannot load texture "
                "image for '%s' from '%s'\n", objName.c_str(), textureFile.c_str());
         }
         texId = GmatPlot::UNINIT_TEXTURE;
      }
   }
   catch (BaseException &e)
   {
      // Give warning for missing texture file for only CelestialBody object
      if (obj->IsOfType(Gmat::CELESTIAL_BODY))
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** ViewCanvas::BindTexture() Cannot bind texture "
             "image for %s.\n%s\n", objName.c_str(), e.GetFullMessage().c_str());
      }
   }
   
   #if DEBUG_TEXTURE
   MessageInterface::ShowMessage
      ("ViewCanvas::BindTexture() objName='%s', texId=%d\n", objName.c_str(),
       texId);
   #endif
   
   return texId;
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
   {
      #if DEBUG_LOAD_IMAGE
      MessageInterface::ShowMessage
         ("ViewCanvas::LoadImage() blank filename, so returning false\n");
      #endif
      return false;
   }
   
   wxImage image = wxImage(fileName.c_str());
   int width = image.GetWidth();
   int height = image.GetHeight();
   
   GLubyte *data = image.GetData();
   
   if (data == NULL)
   {
      #if DEBUG_LOAD_IMAGE
      MessageInterface::ShowMessage
         ("ViewCanvas::LoadImage() empty data, so returning false\n");
      #endif
      return false;
   }
   
   #if DEBUG_LOAD_IMAGE
   int size = width * height * 3;
   MessageInterface::ShowMessage
      ("   width=%d, height=%d, size=%d, hasAlpha=%d\n", width, height, size,
       image.HasAlpha());
   #endif
   
   // Why is image upside down?
   // Get vertial mirror
   wxImage mirror = image.Mirror(false);
   GLubyte *data1 = mirror.GetData();
   
   glEnable(GL_TEXTURE_2D);
   
   //=======================================================
   // Do we want to use mipmaps?
   // When you desire high-quality texture mapping, you will typically specify a
   // mipmapped texture filter. Mipmapping lets you specify multiple levels of
   // detail for a texture image. Each level of detail is half the size of the
   // previous level of detail in each dimension. So if your initial texture
   // image is an image of size 32x32, the lower levels of detail will be of
   // size 16x16, 8x8, 4x4, 2x2, and 1x1. Typically, you use the gluBuild2DMipmaps
   // routine to automatically construct the lower levels of details from you
   // original image. This routine re-samples the original image at each level
   // of detail so that the image is available at each of the various smaller sizes.
   //=======================================================
   #ifdef __ENABLE_MIPMAPS__
   //=======================================================
   
   //used for min and magnifying texture
   int mipmapsStatus = 0;
   
   //pass image to opengl
   #ifndef __WXGTK__
   
   // If small icon, set background color alpha value to 0 transparent
   // so that it will not be shown when drawing.
   // Why it's not working? Set to false (LOJ: 2011.07.06)
   bool removeBackground = false;
   if (removeBackground && width == 16 && height == 16)
   {
      unsigned char red, green, blue;
      
      for (int x = 0; x < width; x++)
      {
         for (int y = 0; y < height; y++)
         {
            red = mirror.GetRed(x, y);
            green = mirror.GetGreen(x, y);
            blue = mirror.GetBlue(x, y);
            
            #if 0
            MessageInterface::ShowMessage
               ("   mirror(%2d,%2d), red=%3d, green=%3d, blue=%3d\n", x, y, red, green, blue);
            #endif
            
            // Assuming background color is white
            if (red == 255 && green == 255 && blue == 255)
               mirror.SetAlpha(x, y, 0);
            else
               mirror.SetAlpha(x, y, 255);
            
            GLubyte *data2 = mirror.GetData();
            
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            
            // Set internal format and data type to GL_RGBA
            mipmapsStatus =
               gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA,
                                 GL_UNSIGNED_BYTE, data2);
            
         }
      }
   }
   else
   {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      
      // This call crashes GMAT on Linux, so it is excluded here. 
      mipmapsStatus =
         gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB,
                           GL_UNSIGNED_BYTE, data1);
   }
   
   #else
   
   // Try this on Linux
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                GL_UNSIGNED_BYTE, data1);
   
   #endif
   
   if (mipmapsStatus == 0)
   {
      #if DEBUG_LOAD_IMAGE
      MessageInterface::ShowMessage
         ("ViewCanvas::LoadImage() returning true, mipmapsStatus=%d\n", mipmapsStatus);
      #endif
      
      return true;
   }
   else
   {
      #if DEBUG_LOAD_IMAGE
      MessageInterface::ShowMessage
         ("ViewCanvas::LoadImage() returning false, mipmapsStatus=%d\n", mipmapsStatus);
      #endif
      return false;
   }
   
   //=======================================================
   #else
   //=======================================================
   
   //used for min and magnifying texture
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                GL_UNSIGNED_BYTE, data1);
   
   #if DEBUG_LOAD_IMAGE
   MessageInterface::ShowMessage
      ("ViewCanvas::LoadImage() returning true, using glTexImage2D\d\n");
   #endif
   
   return true;
   
   //=======================================================
   #endif
   //=======================================================
}


//------------------------------------------------------------------------------
// virtual bool LoadSpacecraftModels()
//------------------------------------------------------------------------------
bool ViewCanvas::LoadSpacecraftModels()
{
   #if DEBUG_LOAD_MODEL
   MessageInterface::ShowMessage
      ("ViewCanvas::LoadSpacecraftModels() entered, mGlInitialized = %d, "
       "modelsAreLoaded = %d, mScCount = %d\n", mGlInitialized, modelsAreLoaded,
       mScCount);
   #endif
   
   if (mGlInitialized)
   {
      if (!modelsAreLoaded)
      {
         ModelManager *mm = ModelManager::Instance();
         for (int sc = 0; sc < mScCount; sc++)
         {
            int satId = GetObjectId(mScNameArray[sc].c_str());
            if (satId != UNKNOWN_OBJ_ID)
            {
               Spacecraft *sat = (Spacecraft*)mObjectArray[satId];
               if (sat->modelFile != "" && sat->modelID == -1)
               {
                  wxString modelPath(sat->modelFile.c_str());
                  if (GmatFileUtil::DoesFileExist(modelPath.c_str()))
                  {
                     sat->modelID = mm->LoadModel(modelPath);
                     #ifdef DEBUG_LOAD_MODEL
                     MessageInterface::ShowMessage
                        ("UpdateSpacecraftData() loaded model '%s'\n", modelPath.c_str());
                     #endif
                  }
                  else
                  {
                     MessageInterface::ShowMessage
                        ("*** WARNING *** Cannot load the model file for spacecraft '%s'. "
                         "The file '%s' does not exist.\n", sat->GetName().c_str(),
                         modelPath.c_str());
                  }
               }
               
               // Set modelsAreLoaded to true if it went through all models
               if (sc == mScCount-1)
                  modelsAreLoaded = true;
            }
         }
      }
   }
   
   #if DEBUG_LOAD_MODEL
   MessageInterface::ShowMessage
      ("ViewCanvas::LoadSpacecraftModels() leaving, mGlInitialized = %d, "
       "modelsAreLoaded = %d\n", mGlInitialized, modelsAreLoaded);
   #endif
   
   return modelsAreLoaded;
}


//------------------------------------------------------------------------------
//  void SetupProjection()
//------------------------------------------------------------------------------
/**
 * Sets up how object is viewed
 */
//------------------------------------------------------------------------------
void ViewCanvas::SetupProjection()
{
   #if DEBUG_PROJECTION > 2
   MessageInterface::ShowMessage("ViewCanvas::SetupProjection() entered\n");
   #endif
   
   // Setup the world view
   glMatrixMode(GL_PROJECTION); // first go to projection mode
   glLoadIdentity();            // clear all previous information
   SetupWorld();                // set it up
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();            // clear all previous information
   
   #if DEBUG_PROJECTION > 2
   MessageInterface::ShowMessage("ViewCanvas::SetupProjection() leaving\n");
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
       "mGlInitialized=%d, modelsAreLoaded=%d\n", time, mScCount,
       mGlInitialized, modelsAreLoaded);
   #endif
   
   // Load spacecraft models
   if (!modelsAreLoaded)
      LoadSpacecraftModels();
   
   //-------------------------------------------------------
   // update spacecraft position
   //-------------------------------------------------------
   for (int sc = 0; sc < mScCount; sc++)
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
         
         // Buffer velocity if needed
         if (mNeedVelocity)
         {
            mObjectViewVel[posIndex+0] = velX[sc];
            mObjectViewVel[posIndex+1] = velY[sc];
            mObjectViewVel[posIndex+2] = velZ[sc];            
         }
         
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
         
         if (mNeedAttitude)
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
            if (mNeedAttitude)
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


//------------------------------------------------------------------------------
// void DrawOrbit(const wxString &objName, int obj, int objId)
//------------------------------------------------------------------------------
void ViewCanvas::DrawOrbit(const wxString &objName, int obj, int objId)
{
   #ifdef DEBUG_DRAW
   MessageInterface::ShowMessage
      ("==========> DrawOrbit() objName='%s', drawing first part, mRealBeginIndex1=%d, "
       "mRealEndIndex1=%d\n", objName.c_str(), mRealBeginIndex1, mRealEndIndex1);
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
   
   #if DEBUG_DRAW_DEBUG
   DrawDebugMessage(" Leaving DrawOrbit  --- ", GmatColor::RED32, 0, 240);
   #endif
}


//------------------------------------------------------------------------------
//  void DrawSolverData()
//------------------------------------------------------------------------------
/**
 * Draws solver iteration data
 * This is only called when drawing "current" solver data.  For drawing all
 * solver passes at the same time, see UpdatePlot()
 */
//------------------------------------------------------------------------------
void ViewCanvas::DrawSolverData()
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


//---------------------------------------------------------------------------
// void DrawStatus(const wxString &label1, unsigned int textColor, ...)
//---------------------------------------------------------------------------
/*
 * Writes status at the bottom of the frame
 */
//---------------------------------------------------------------------------
void ViewCanvas::DrawStatus(const wxString &label1, unsigned int textColor,
                            const wxString &label2, double time, int xpos,
                            int ypos, bool showCS, const wxString &label3)
{   
   #ifdef DEBUG_DRAW_STATUS
   int frame = mTotalPoints;
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
   
   if (label1 != "")
      text = label1;      
   
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
   
   if (showCS)
   {
      // Prepend space before coordinate system name (Bug 2318 fix)
      wxString viewCsName = "  " + mViewCoordSysName;
      glRasterPos2i(xpos, ypos+20);
      glCallLists(strlen(viewCsName.c_str()), GL_BYTE, (GLubyte*)viewCsName.c_str());
   }
   
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   
   #ifdef DEBUG_DRAW_STATUS
   MessageInterface::ShowMessage("DrawStatus() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void DrawDebugMessage(const wxString &msg, unsigned int textColor, int xpos, int ypos)
//------------------------------------------------------------------------------
void ViewCanvas::DrawDebugMessage(const wxString &msg, unsigned int textColor,
                                  int xpos, int ypos)
{
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0, (GLfloat)mCanvasSize.x, 0.0, (GLfloat)mCanvasSize.y);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
   GlColorType *color = (GlColorType*)&textColor;
   glColor3ub(color->red, color->green, color->blue);
   glRasterPos2i(xpos, ypos);
   glCallLists(strlen(msg.c_str()), GL_BYTE, (GLubyte*)msg.c_str());
   
   SetupProjection();
}

