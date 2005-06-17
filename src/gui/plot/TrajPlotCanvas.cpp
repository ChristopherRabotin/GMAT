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
#include "FileManager.hpp"        // for Earth texture file
#include "ColorTypes.hpp"         // for namespace GmatColor::
#include "Rvector3.hpp"           // for Rvector3::GetMagnitude()
#include "AngleUtil.hpp"          // for ComputeAngleInDeg()
#include "MdiGlPlotData.hpp"
#include "MessageInterface.hpp"
#include <string.h>               // for strlen()

#include <math.h>

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
//#  include <GL/glut.h> //loj: 6/13/05 Added
#  include <IL/il.h>
#  include <IL/ilu.h>
#  include <IL/ilut.h>
#endif

//#define DEBUG_TRAJCANVAS_INIT 1
//#define DEBUG_TRAJCANVAS_UPDATE 1
//#define DEBUG_TRAJCANVAS_UPDATE_OBJECT 2
//#define DEBUG_TRAJCANVAS_ACTION 1
//#define DEBUG_TRAJCANVAS_CONVERT 2
//#define DEBUG_TRAJCANVAS_DRAW 2
//#define DEBUG_TRAJCANVAS_ZOOM 1
//#define DEBUG_TRAJCANVAS_OBJECT 2
//#define DEBUG_TRAJCANVAS_TEXTURE 2
//#define DEBUG_TRAJCANVAS_PROJ 1
//#define DEBUG_TRAJCANVAS_ANIMATION 1

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

struct GlColorType
{
   Byte red;
   Byte green;
   Byte blue;
   Byte not_used;
};


// texture
static int *sIntColor = new int;
static GlColorType *sGlColor = (GlColorType*)sIntColor;


//------------------------------------------------------------------------------
// TrajPlotCanvas(wxWindow *parent, wxWindowID id,
//                const wxPoint& pos, const wxSize& size, const wxString &csName,
//                SolarSystem *solarSys, long style, const wxString& name)
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
 * @param <style>    style of window
 * @param <name>     title of window
 *
 */
//------------------------------------------------------------------------------
TrajPlotCanvas::TrajPlotCanvas(wxWindow *parent, wxWindowID id,
                               const wxPoint& pos, const wxSize& size,
                               const wxString &csName, SolarSystem *solarSys,
                               long style, const wxString& name)
   : wxGLCanvas(parent, id, pos, size, style, name)
{    
   // initalize data members
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   mTextTrajFile = NULL;
   mGlList = 0;
   mNumData = 0;

   // projection
   mUsePerspectiveMode = false;
   
   // viewpoint
   mViewPointRefObjName = "UNKNOWN";
   mViewPointRefObj = NULL;
   mViewPointVectorObj = NULL;
   mViewDirectionObj = NULL;
   mViewPointRefVector.Set(0.0, 0.0, 0.0);
   mViewPointVector.Set(0.0, 0.0, 30000);
   mViewDirectionVector.Set(0.0, 0.0, -1.0);
   mVpLocVec.Set(0.0, 0.0, 30000);
   mViewScaleFactor = 1.0;
   mUseInitialViewPoint = true;
   mUseViewPointRefVector = true;
   mUseViewPointVector = true;
   mUseViewDirectionVector = true;
   mVpRefObjId = -1;
   mVpVecObjId = -1;
   mVdirObjId = -1;

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
   
   mAxisLength = mCurrViewDist;
   mOriginName = "";
   mOriginId = 0;

   //original value
   //mRotateAboutXaxis = true;
   //mRotateAboutYaxis = false;
   //mRotateAboutZaxis = false;

   mRotateAboutXaxis = false;
   mRotateAboutYaxis = false;
   mRotateAboutZaxis = true;
   
   mRotateXy = true;

   mZoomAmount = 300.0;
   
   // projection
   ChangeProjection(size.x, size.y, mAxisLength);
   
   mEarthRadius = 6378.14; //km
   mScRadius = 200;        //km: make big enough to see
   
   // view options
   mDrawWireFrame = false;
   mDrawEqPlane = false;
   mDrawEcPlane = false;
   mDrawAxes = false;
   mDrawOrbitNormal = true;
   mEqPlaneColor = GmatColor::GRAY32;
   mEcPlaneColor = GmatColor::TEAL32;
   mESLinecolor = GmatColor::D_BROWN32;
   
   // animation
   mViewAnimation = false;
   mHasUserInterrupted = false;
   mUpdateInterval = 50;
   
   // solar system
   mSolarSystem = solarSys;
      
   for (int i=0; i<MAX_BODIES; i++)
   {
      mObjMaxZoomIn[i] = MAX_ZOOM_IN;
   }
   
   // objects
   mObjectDefaultRadius = 200; //km: make big enough to see
   
   for (int obj=0; obj<MAX_OBJECT; obj++)
      mObjectRadius[obj] = 0.0;
   
   // Zoom
   mMaxZoomIn = 0;
   
   // Spacecraft
   mScCount = 0;
   
//    for (int i=0; i<MAX_SCS; i++)
//    {
//       mScLastFrame[i] = 0;
//    }
   
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
   glEnable(GL_DEPTH_TEST);

   glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
   glDepthFunc(GL_LESS);
   glDepthRange(0.0, 100.0); //loj: 5.20 just to try
   
   // speedups
   glEnable(GL_DITHER);
   glShadeModel(GL_SMOOTH);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
   glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
   
#ifdef __WXMSW__
   // initalize devIL library
   ilInit();
   ilutRenderer(ILUT_OPENGL);
   
   // try to load textures
   //5/17:commented out, it is called from AddObjectList()
   
   if (!LoadGLTextures())
      return false;
   
#endif

   //loj: 3/10/05 Actually I don't need this
   // pixel format
   //if (!SetPixelFormatDescriptor())
   //   return false;
   
   // font
   SetDefaultGLFont();

//    mInitialized = true;
   
   return true;
}


// //------------------------------------------------------------------------------
// // bool IsInitialized()
// //------------------------------------------------------------------------------
// /**
//  * Retrives initialized flag.
//  */
// //------------------------------------------------------------------------------
// bool TrajPlotCanvas::IsInitialized()
// {
//    return mInitialized;
// }


//------------------------------------------------------------------------------
// wxString GetGotoObjectName()
//------------------------------------------------------------------------------
wxString TrajPlotCanvas::GetGotoObjectName()
{
   //return mObjectNames[mOriginId];
   return mObjectNames[mViewObjId];
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
   }
   else
   {
      mfCamTransX = 0;
      mfCamTransY = 0;
      mfCamTransZ = 0;
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
   //mDrawEqPlane = false;
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
   
   //loj: 5/23/05 Why mVpVecObjId changed?, so need to reset.
   if (!mUseViewPointVector && mViewPointVectorObj != NULL)
      mVpVecObjId = GetObjectId(mViewPointVectorObj->GetName().c_str());
   
   ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
      
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

//    mCurrRotXAngle = mDefaultRotXAngle;
//    mCurrRotYAngle = mDefaultRotYAngle;
//    mCurrRotZAngle = mDefaultRotZAngle;
//    mCurrViewDist = mDefaultViewDist;
//    mAxisLength = mCurrViewDist;
//    mfCamTransX = 0;
//    mfCamTransY = 0;
//    mfCamTransZ = 0;

//    mOriginId = GetObjectId("Earth");
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

   //5.23
//    if (mUsePerspectiveMode)
//    {
//       if (mAxisLength > mMaxZoomIn)
//       {
//          mAxisLength = mAxisLength - realDist;
         
//          if (mAxisLength < mMaxZoomIn/2.0)
//             mAxisLength = mMaxZoomIn/2.0;
         
//          ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
//       }
//    }
   
   if (mUsePerspectiveMode)
   {
      //5.20if (mAxisLength > mMaxZoomIn/2)
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
// void DrawEqPlane(bool flag)
//------------------------------------------------------------------------------
/**
 * Draws equatorial plane
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawEqPlane(bool flag)
{
   mDrawEqPlane = flag;
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
      ("TrajPlotCanvas::DrawInNewCoordSysName() viewCS=%s, newCS=%s\n",
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
      
      mOriginName = mViewCoordSystem->GetOriginName().c_str();
      mOriginId = GetObjectId(mOriginName);
      //mMaxZoomIn = mObjMaxZoomIn[mOriginId]; //loj: 5/24/05 commented out
      
      mNeedSpacecraftConversion = true;
      mNeedOriginConversion = true;
      mNeedObjectConversion = true;
      
      ConvertObjectData(); //loj: 5/24/05 Added
      
      Refresh(false);
      GotoObject(mOriginName);
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
   //loj:5.20mOriginId = objId;
   mMaxZoomIn = mObjMaxZoomIn[objId];
   
   // if goto Object is center(0,0,0), zoom out to see the object,
   // otherwise, set to final position of the object
   //loj:5.20 if (objName == mOriginName)
   if (objName == mViewObjName)
   {
      mAxisLength = mMaxZoomIn;
   }
   else
   {
      Rvector3 pos(mObjectTempPos[objId][mNumData-1][0],
                   mObjectTempPos[objId][mNumData-1][1],
                   mObjectTempPos[objId][mNumData-1][2]);
      
      mAxisLength = pos.GetMagnitude();
      
      if (mAxisLength == 0.0)
         mAxisLength = mMaxZoomIn;
   }
   
   #ifdef DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::GotoObject() objName=%s, objId=%d, mMaxZoomIn=%f\n"
       "                             mAxisLength=%f\n", objName.c_str(),
       objId, mMaxZoomIn, mAxisLength);
   #endif

   if (mUsePerspectiveMode)
   {
      // move camera position to object
      mfCamTransX = -mObjectTempPos[objId][mNumData-1][0];
      mfCamTransY = -mObjectTempPos[objId][mNumData-1][1];
      mfCamTransZ = -mObjectTempPos[objId][mNumData-1][2];
   }
   
   Refresh(false);
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
   DrawFrame();
}


//------------------------------------------------------------------------------
// void SetGlObject(const StringArray &objNames,
//                  const UnsignedIntArray &objOrbitColors,
//                  const std::vector<SpacePoint*> objArray)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetGlObject(const StringArray &objNames,
                                 const UnsignedIntArray &objOrbitColors,
                                 const std::vector<SpacePoint*> objArray)
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
   
   // set center view object as origin of the CoordinateSystem if view direction
   // is not an object
   if (!mUseViewDirectionVector && mViewDirectionObj != NULL) //loj: 5/20
   {
      mViewObjName = wxString(mViewDirectionObj->GetName().c_str());
      mViewObjId = GetObjectId(mViewObjName);
   }

   //5.20
   if (mUseInitialViewPoint)
   {
      mMaxZoomIn = mObjMaxZoomIn[mOriginId];
      mAxisLength = mMaxZoomIn;
   }
   
   //    mMaxZoomIn = mObjMaxZoomIn[mViewObjId];
   //    mAxisLength = mMaxZoomIn;
   
   #if DEBUG_TRAJCANVAS_OBJECT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::SetGlCoordSystem() mViewCoordSystem=%s, originName=%s, "
       "mOriginId=%d\n", mViewCoordSysName.c_str(), mOriginName.c_str(),
       mOriginId);
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::SetGlCoordSystem() mViewObjName=%s, "
       "mViewObjId=%d\n", mViewObjName.c_str(), mViewObjId);
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::SetGlCoordSystem() mViewUpCoordSysName=%s\n",
       mViewUpCoordSysName.c_str());
   #endif

}


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
      ("TrajPlotCanvas::SetGlViewOption() vpRefObj=%d, vpVecObj=%d, "
       "vdObj=%d, vsFactor=%f\nvpRefVec=%s, vpVec=%s, vdVec=%s, upAxis=%s, "
       "usevpRefVec=%d, usevpVec=%d, usevdVec=%d\n",  vpRefObj, vpVecObj, vdObj,
       vsFactor, lvpRefVec.ToString().c_str(), lvpVec.ToString().c_str(),
       lvdVec.ToString().c_str(), upAxis.c_str(), usevpRefVec, usevpVec, usevdVec);
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
   
} //end SetGlViewOption()


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
      wxLogStatus(GmatAppData::GetMainFrame(),
                  wxT("Number of data points: %d"), numDataPoints);
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
   //      else
   //      {
   //          wxMessageDialog msgDialog(this, _T("InitGL() successful"),
   //                                   _T("ReadTextTrajectory File"));
   //          msgDialog.ShowModal();
   //      }

//    mInitialized = true;
    
   return numDataPoints;
    
} //end ReadTextTrajectory()


//loj: 5/16/05
// Removed mScGciPos[] and mScTempPos[] and use mObjectGciPos[] and mObjectTempPos[]

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
   #if DEBUG_TRAJCANVAS_UPDATE
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::UpdatePlot() time=%f, mNumData=%d\n", time, mNumData);
   #endif
   
   mScCount = posX.size();
   if (mScCount > MAX_SCS)
      mScCount = MAX_SCS;
   
   mScNameArray = scNames;
   
   if (mNumData < MAX_DATA)
   {
      mTime[mNumData] = time;
      
      //-------------------------------------------------------
      // update spacecraft position
      //-------------------------------------------------------
      for (int sc=0; sc<mScCount; sc++)
      {
         int objId = GetObjectId(mScNameArray[sc].c_str());
         
         if (objId != -1)
         {
            mObjectOrbitColor[objId][mNumData]  = scColors[sc];
            mObjectGciPos[objId][mNumData][0] = posX[sc];
            mObjectGciPos[objId][mNumData][1] = posY[sc];
            mObjectGciPos[objId][mNumData][2] = posZ[sc];
            
            //loj: 6/13/05 Added
            mObjectGciVel[objId][mNumData][0] = velX[sc];
            mObjectGciVel[objId][mNumData][1] = velY[sc];
            mObjectGciVel[objId][mNumData][2] = velZ[sc];
            
            if (mNeedInitialConversion)
            {
               Rvector6 inState, outState;

               // convert position and velocity (loj: 6/13/05 convert velocity also)
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
               
            }
            else
            {
               CopyVector3(mObjectTempPos[objId][mNumData],
                           mObjectGciPos[objId][mNumData]);
               CopyVector3(mObjectTempVel[objId][mNumData],
                           mObjectGciVel[objId][mNumData]);
            
            }
            
            #if DEBUG_TRAJCANVAS_UPDATE
            MessageInterface::ShowMessage
               ("TrajPlotCanvas::UpdatePlot() Sc%d pos = %f, %f, %f, color=%d\n", objId,
                mObjectTempPos[objId][mNumData][0], mObjectTempPos[objId][mNumData][1],
                mObjectTempPos[objId][mNumData][2], mObjectOrbitColor[objId][mNumData]);
            MessageInterface::ShowMessage
               ("TrajPlotCanvas::UpdatePlot() Sc%d vel = %f, %f, %f\n", objId,
                mObjectTempVel[objId][mNumData][0], mObjectTempVel[objId][mNumData][1],
                mObjectTempVel[objId][mNumData][2]);
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
            if (objId != -1)
            {
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
                  ("TrajPlotCanvas::UpdatePlot() %s pos = %f, %f, %f\n",
                   mObjectNames[obj].c_str(), mObjectTempPos[objId][mNumData][0],
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
      
   }   

   // Set projection here, because DrawPlot() is called in OnPaint()
   if (mUseInitialViewPoint)
   {
      ComputeProjection(mNumData-1);
      ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
      SetProjection();
   }
   else
   {
      ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
      SetProjection();
   }
   
   //DrawPlot(); //loj: This will make slow plotting.
   Refresh(false);
   
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
      //mObjectTextureIdMap.clear();
      mObjectNames.Empty();
   }
   
   RgbColor rgb;

   mObjectCount = objNames.GetCount();
   
   for (int i=0; i<mObjectCount; i++)
   {
      // add object names
      mObjectNames.Add(objNames[i]);
      
      // initialize object texture
      //mObjectTextureIdMap[objNames[i]] = UNINIT_TEXTURE;
      
      if (mObjectTextureIdMap.find(objNames[i]) == mObjectTextureIdMap.end())
      {
         #if DEBUG_TRAJCANVAS_OBJECT
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::AddObjectList()  Bind new texture object=%s\n",
             objNames[i].c_str());
         #endif
         
         mObjectTextureIdMap[objNames[i]] = UNINIT_TEXTURE;
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
   
} //AddObjectList()


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

   SetProjection();
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
   int flippedY;
   int clientWidth, clientHeight;
   int mouseX, mouseY;

   mViewAnimation = false;
   
   GetClientSize(&clientWidth, &clientHeight);
   ChangeProjection(clientWidth, clientHeight, mAxisLength);
   
   mouseX = event.GetX();
   mouseY = event.GetY();
   
   // First, flip the mouseY value so it is oriented right (bottom left is 0,0)
   flippedY = clientHeight - mouseY;
   
   GLfloat fEndX = mfLeftPos + ((GLfloat)mouseX /(GLfloat)clientWidth) *
      (mfRightPos - mfLeftPos);
   GLfloat fEndY = mfBottomPos + ((GLfloat)flippedY /(GLfloat)clientHeight)*
      (mfTopPos - mfBottomPos);
   
   if (mUseSingleRotAngle)
   {
      mUseSingleRotAngle = false;
      
      // How do I compute mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle?
      //mCurrRotXAngle = 0.0;
      //mCurrRotYAngle = 0.0;
      //mCurrRotZAngle = 0.0;
      //ChangeView(mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);
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
         ComputeView(fEndX, fEndY);
         ChangeView(mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);
         
         // repaint
         Refresh(false);
      }
      //------------------------------
      // zooming
      //------------------------------
      else if (event.RightIsDown())
      {            
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
   
   //wxLogStatus(MdiGlPlot::mdiParentGlFrame,
   //            wxT("X = %d Y = %d"), event.GetX(), event.GetY());
   
   //wxLogStatus(MdiGlPlot::mdiParentGlFrame,
   //            wxT("distance=%f"), mAxisLength);

   wxLogStatus(GmatAppData::GetMainFrame(),
               wxT("X = %g Y = %g"), fEndX, fEndY);

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
   
#ifdef __WXMSW__

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
      
      if (mObjectTextureIdMap[mObjectNames[i]] == UNINIT_TEXTURE)
      {
         #if DEBUG_TRAJCANVAS_TEXTURE > 1
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::LoadGLTextures() object=%s\n", mObjectNames[i].c_str());
         #endif
         
         mObjectTextureIdMap[mObjectNames[i]] = BindTexture(mObjectNames[i]);
      }
   }
   
   return true;
#else
   return false;
#endif
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
   GLuint ret = UNINIT_TEXTURE;
   
   FileManager *fm = FileManager::Instance();
   std::string textureFile;
   ILboolean status;
   
   //@todo - Change FileManager to have Luna
   // special case for Luna, FileManager has Moon
   std::string filename;
   if (objName == "Luna")
   {
      filename = "FULL_MOON_TEXTURE_FILE";
   }
   else
   {
      std::string name = std::string(objName.Upper().c_str());
      filename = "FULL_" + name + "_TEXTURE_FILE";
   }
   
   textureFile = fm->GetStringParameter(filename);
   if (textureFile != "UNKNOWN_ID")
   {
      status = ilLoadImage((char*)textureFile.c_str());
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
         //glBindTexture(GL_TEXTURE_2D, ret);
      }
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
   //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_PROJECTION); // first go to projection mode
   //glPushMatrix();
   glLoadIdentity();
   SetupWorld();                // set it up
   glMatrixMode(GL_MODELVIEW);
   //glPopMatrix();
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

   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::SetupWorld() mUsePerspectiveMode=%d, mUseSingleRotAngle=%d\n",
       mUsePerspectiveMode, mUseSingleRotAngle);
   #endif
   
   if (mUsePerspectiveMode)
   {
      //MessageInterface::ShowMessage
      //   ("mfLeftPos=%f, mfRightPos=%f, mfBottomPos=%f, mfTopPos=%f, "
      //    "mfViewNear=%f, mfViewFar=%f\n", mfLeftPos, mfRightPos, mfBottomPos,
      //    mfTopPos, mfViewNear, mfViewFar);
      
      // Setup how we view the world
      GLfloat aspect = (GLfloat)mCanvasSize.x / (GLfloat)mCanvasSize.y;
      Real size = GmatMathUtil::Sqrt(mfRightPos * mfRightPos +
                                     mfTopPos   * mfTopPos +
                                     mfViewFar  * mfViewFar);
      
      Real dist = mVpLocVec.GetMagnitude();
      mViewObjRadius = mObjectDefaultRadius*50; //loj:5/23/05 multiplied by 50
      
      if (mUseFixedFov && mUseSingleRotAngle)
      {
         mFovDeg = mFixedFovAngle;
      }
      else
      {
         if (!mUseViewDirectionVector && mViewDirectionObj != NULL)
         {
            int objId = GetObjectId(mViewDirectionObj->GetName().c_str());
            mViewObjRadius = mObjectRadius[objId];
         }
         
         // compute fov angle
         mFovDeg = 2.0 * ATan(size/2.0, dist - mViewObjRadius) * DEG_PER_RAD;
         //mFovDeg = 2.0 *
         //   GmatMathUtil::ATan(size/2.0, dist - mObjectRadius[mOriginId]) *
         //   GmatMathUtil::DEG_PER_RAD;
      }
      
      #if DEBUG_TRAJCANVAS_PROJ > 1
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::SetupWorld() size=%f, dist=%f, mViewObjRadius=%f\n",
          size, dist, mViewObjRadius);
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::SetupWorld() mAxisLength=%f, fov=%f\n",
          mAxisLength, mFovDeg);
      #endif
      
      //gluPerspective(mFovDeg, aspect, 1.0, mAxisLength * mFovDeg);
      gluPerspective(mFovDeg, aspect, mAxisLength/(mFovDeg*10), mAxisLength * mFovDeg);
   }
   else
   {
      // Setup how we view the world
      glOrtho(mfLeftPos, mfRightPos, mfBottomPos, mfTopPos, mfViewNear,
              mfViewFar);
   }
   
   // if using mouse to rotate the object
   if (!mUseSingleRotAngle)
   {
      
      //Translate Camera
      glTranslatef(mfCamTransX, mfCamTransY, mfCamTransZ);
      
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
   
   //camera moves opposite direction to center on object
   //this is the point of rotation
   //loj: 4/15/05 Added minus sign to x, y

   //loj:5.20
//    glTranslatef( mObjectTempPos[mOriginId][mNumData-1][0],
//                  mObjectTempPos[mOriginId][mNumData-1][1],
//                  -mObjectTempPos[mOriginId][mNumData-1][2]);
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
   //   ("%f %f %f\n", mfCamRotXAngle, mfCamRotYAngle, mfCamRotZAngle);
   
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
   mfViewNear   = -axisLength/2;
   mfViewFar    =  axisLength/2;
   
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
//  void ComputeProjection(int frame)
//------------------------------------------------------------------------------
/**
 * Computes view projection using viewing options.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::ComputeProjection(int frame)
{
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage("ComputeProjection() frame=%d, time=%f\n",
                                 frame, mTime[frame]);
   #endif
   
   //-----------------------------------------------------------------
   // get viewpoint reference vector
   //-----------------------------------------------------------------
   //5.19Rvector3 vpRefVec(0.0, 0.0, 0.0);
   mVpRefVec.Set(0.0, 0.0, 0.0);

   if (!mUseViewPointRefVector && mViewPointRefObj != NULL)
   {
      #if DEBUG_TRAJCANVAS_PROJ
      MessageInterface::ShowMessage
         ("ComputeProjection() mViewPointRefObj=%d, name=%s\n",
          mViewPointRefObj, mViewPointRefObj->GetName().c_str());
      #endif

      // if valid body id
      if (mVpRefObjId != -1)
      {
         // for efficiency, body data are computed in UpdatePlot() once.
         mVpRefVec.Set(mObjectTempPos[mVpRefObjId][frame][0],
                      mObjectTempPos[mVpRefObjId][frame][1],
                      mObjectTempPos[mVpRefObjId][frame][2]);
      }
      else
      {
         MessageInterface::ShowMessage
            ("*** Warning *** TrajPlotCanvas::ComputeProjection() Invalid mVpRefObjId=%d\n",
             mVpRefObjId);
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
         ("ComputeProjection() mViewPointVectorObj=%d, name=%s, mVpVecObjId=%d\n",
          mViewPointVectorObj, mViewPointVectorObj->GetName().c_str(),
          mVpVecObjId);
      #endif
      
      // if valid body id
      if (mVpVecObjId != -1)
      {
         // for efficiency, body data are computed in UpdatePlot() once.
         mVpVec.Set(mObjectTempPos[mVpVecObjId][frame][0],
                    mObjectTempPos[mVpVecObjId][frame][1],
                    mObjectTempPos[mVpVecObjId][frame][2]);
      }
      else
      {
         MessageInterface::ShowMessage
            ("*** Warning *** TrajPlotCanvas::ComputeProjection() Invalid mVpVecObjId=%d\n",
             mVpVecObjId);
      }
   }
   
   //-----------------------------------------------------------------
   // get viewpoint location
   //-----------------------------------------------------------------
   mVpLocVec = mVpRefVec + (mViewScaleFactor * mVpVec);
   
   //-----------------------------------------------------------------
   // get view direction and view center vector
   //-----------------------------------------------------------------
   //Rvector3 vdVec(mViewDirectionVector);
   mVdVec = mViewDirectionVector;
   mVcVec = mVdVec;
   
   if (!mUseViewDirectionVector && mViewDirectionObj != NULL)
   {
      #if DEBUG_TRAJCANVAS_PROJ
      MessageInterface::ShowMessage
         ("ComputeProjection() mViewDirectionObj=%d, name=%s\n",
          mViewDirectionObj, mViewDirectionObj->GetName().c_str());
      #endif
      
      // if viewpoint ref object is same as view direction object
      // just look opposite side
      if (!mUsePerspectiveMode &&
          mViewDirectionObj->GetName() == mViewPointRefObjName)
      {
         mVdVec = -mVpLocVec;
      }
      else if (mVdirObjId != -1)
      {
         // for efficiency, body data are computed in UpdatePlot() once.
         mVdVec.Set(mObjectTempPos[mVdirObjId][frame][0],
                    mObjectTempPos[mVdirObjId][frame][1],
                    mObjectTempPos[mVdirObjId][frame][2]);

         // view center vector
         mVcVec = mVdVec;
         
         // check for 0.0 direction (loj: 5/17/05 Added)
         if (mVdVec.GetMagnitude() == 0.0)
            mVdVec = mViewDirectionVector;
      }
      else
      {
         MessageInterface::ShowMessage
            ("*** Warning *** TrajPlotCanvas::ComputeProjection() Invalid mVdirObjId=%d\n",
             mVdirObjId);
      }
   }
   
   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("ComputeProjection() mVpRefVec=%s, mVpVec=%s\nmVpLocVec=%s, "
       " mVdVec=%s, mVcVec=%s\n", mVpRefVec.ToString().c_str(),
       mVpVec.ToString().c_str(), mVpLocVec.ToString().c_str(),
       mVdVec.ToString().c_str(), mVcVec.ToString().c_str());
   #endif
   
   
   //-----------------------------------------------------------------
   // set view center object
   //-----------------------------------------------------------------
   //loj:5.20 it's already set in SetGlCoordSystem()
   //mOriginId = GetObjectId(mOriginName);
   //mMaxZoomIn = mObjMaxZoomIn[mOriginId];
   
   if (mUsePerspectiveMode)
   {
      // set camera location
      if (!mUseGluLookAt) //(5/20)
      {
         mfCamTransX = -mVpLocVec[0];
         mfCamTransY = -mVpLocVec[1];
         mfCamTransZ = -mVpLocVec[2];
      }
   }
   else
   {     
      // compute axis length (this tells how far zoom out is)
      mAxisLength = mVpLocVec.GetMagnitude();

      // if mAxisLength is too small, set to max zoom in value
      if (mAxisLength < mMaxZoomIn)
         mAxisLength = mMaxZoomIn;
   }
   
   // compute camera rotation angle
   Real vdMag = mVdVec.GetMagnitude();
   
   mfCamSingleRotAngle = ACos(-(mVdVec[2]/vdMag)) * DEG_PER_RAD;
   
   // compute axis of rotation
   mfCamRotXAxis =  mVdVec[1];
   mfCamRotYAxis = -mVdVec[0];
   mfCamRotZAxis = 0.0;
   mUseSingleRotAngle = true;

   // DJC added for "Up"   
   mfUpAngle = atan2(mVdVec[1],mVdVec[0]) * DEG_PER_RAD + 90.0;
   mfUpXAxis = mVdVec[0];
   mfUpYAxis = mVdVec[1];
   mfUpZAxis = mVdVec[2];
   
   #if DEBUG_TRAJCANVAS_PROJ
      MessageInterface::ShowMessage
         ("ComputeProjection() mfCamTransXYZ=%f, %f, %f, mfCamSingleRotAngle=%f\n"
          "mfCamRotXYZ=%f, %f, %f mAxisLength=%f\n", mfCamTransX, mfCamTransY, mfCamTransZ,
          mfCamSingleRotAngle, mfCamRotXAxis, mfCamRotYAxis, mfCamRotZAxis, mAxisLength);
   #endif
}


//------------------------------------------------------------------------------
// void ComputeViewMatrix()
//------------------------------------------------------------------------------
void TrajPlotCanvas::ComputeViewMatrix()
{
   //MessageInterface::ShowMessage
   //   ("===> TrajPlotCanvas::ComputeViewMatrix() mUseGluLookAt=%d\n", mUseGluLookAt);
   
   if (mUseSingleRotAngle)
   {
      glLoadIdentity();
      
      // calculate view up direction
      
      Rvector6 inState, outState;
      
      if (mViewUpAxisName == "X")
         inState.Set(1.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      else if (mViewUpAxisName == "-X")
         inState.Set(-1.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      else if (mViewUpAxisName == "Y")
         inState.Set(0.0, 1.0, 0.0, 0.0, 0.0, 0.0);
      else if (mViewUpAxisName == "-Y")
         inState.Set(0.0, -1.0, 0.0, 0.0, 0.0, 0.0);
      else if (mViewUpAxisName == "Z")
         inState.Set(0.0, 0.0, 1.0, 0.0, 0.0, 0.0);
      else if (mViewUpAxisName == "-Z")
         inState.Set(0.0, 0.0, -1.0, 0.0, 0.0, 0.0);
      
      outState = inState;
      Rvector6 originIn;
      Rvector6 originOut;
      
      if (mViewUpCoordSystem->GetName() != mViewCoordSystem->GetName())
      {
         mCoordConverter.Convert(mTime[0], inState, mViewUpCoordSystem,
                                 outState, mInternalCoordSystem);
            
         mCoordConverter.Convert(mTime[0], originIn, mViewUpCoordSystem,
                                 originOut, mInternalCoordSystem);
         
         Rvector6 r1 = outState - originOut;
         Rvector6 r2;
         
         mCoordConverter.Convert(mTime[0], r1, mInternalCoordSystem,
                                 r2, mViewCoordSystem);
            
         mCoordConverter.Convert(mTime[0], originIn, mInternalCoordSystem,
                                 originOut, mViewCoordSystem);
            
         outState = r2 - originOut;
         
         //mCoordConverter.Convert(mTime[0], inState, mViewUpCoordSystem,
         //                        outState, mViewCoordSystem);
      }
      
      #if DEBUG_TRAJCANVAS_PROJ
      MessageInterface::ShowMessage
         ("===> mVpLocVec=%s, mVcVec=%s\n", mVpLocVec.ToString().c_str(),
          mVcVec.ToString().c_str());
      MessageInterface::ShowMessage
         ("===> outState= %f, %f, %f\n", outState[0], outState[1], outState[2]);
      #endif
      
      if (mUsePerspectiveMode)
      {
         if (mUseGluLookAt)
         {
            //-------------------------------------------------
            // use gluLookAt()
            //-------------------------------------------------
            gluLookAt(mVpLocVec[0], mVpLocVec[1], mVpLocVec[2],
                      mVcVec[0], mVcVec[1], mVcVec[2],
                      outState[0], outState[1], outState[2]);
            
            //Rvector3 upVec(outState[0], outState[1], outState[2]);
            //Real rotAngle = ACos(-(upVec[2]/upVec.GetMagnitude())) * DEG_PER_RAD;
            //glRotatef(rotAngle, upVec[1], -upVec[0], 0.0);
         }
         else
         {
            glTranslatef(mfCamTransX, mfCamTransY, mfCamTransZ);
            glRotatef(mfCamSingleRotAngle, mfCamRotXAxis, mfCamRotYAxis, mfCamRotZAxis);
         }
      }
      else
      {
         glTranslatef(mfCamTransX, mfCamTransY, mfCamTransZ);
         glRotatef(mfCamSingleRotAngle, mfCamRotXAxis, mfCamRotYAxis, mfCamRotZAxis);

         // DJC added for Up
         glRotatef(-mfUpAngle, mfUpXAxis, mfUpYAxis, -mfUpZAxis);
      }
   }
   
} // end ComputeViewMatrix()


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
      ("TrajPlotCanvas::DrawFrame() mNumData=%d, mUsenitialViewPoint=%d\n",
       mNumData, mUseInitialViewPoint);
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
      
      // set view center object
      mOriginName = wxString(mViewCoordSystem->GetOriginName().c_str());
      mOriginId = GetObjectId(mOriginName);
         
      mViewObjName = mOriginName;
      mViewObjId = mOriginId;
         
      mMaxZoomIn = mObjMaxZoomIn[mOriginId];
      mAxisLength = mMaxZoomIn;
         
      ConvertObjectData();
   }
   
   for (int frame=1; frame<mNumData; frame++)
   {
      // Yield control to pending message KeyEvent for user interrupt.
      // wxSafeYield() is similar to wxYield() except it disables the user
      // input to all program windows before calling wxYield and re-enables
      // it again afterwards.
      wxSafeYield();
      
      if (mHasUserInterrupted)
         break;
      
      Sleep(mUpdateInterval);
      
      //loj: If It doesn't clear, it shows trace
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      DrawStatus(frame);
      
      if (mUseInitialViewPoint)
      {
         ComputeProjection(frame);
         ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
         SetProjection();
      }
      else
      {
         SetProjection();
      }
      
      ComputeViewMatrix();     
      
      // tilt Origin rotation axis if needed
      if (mNeedOriginConversion)
      {
         glPushMatrix();
         TiltOriginZAxis();
      }
      
      if (mDrawEqPlane)
      {
         if (mOriginName == "Sun")
            DrawEclipticPlane(mEqPlaneColor);
         else
            DrawEquatorialPlane(mEqPlaneColor);
      }
      
      // draw axes
      if (mDrawAxes)
         DrawAxes(true);
      
      // draw ecliptic plane
      if (mDrawEcPlane)
      {
         if (mOriginName == "Earth")
            DrawEclipticPlane(mEcPlaneColor);
         else if(mOriginName == "Sun")
            DrawEquatorialPlane(mEcPlaneColor);
      }
      
      if (mNeedOriginConversion)
      {
         glPopMatrix();
      }
      
      // draw object orbit
      DrawObjectOrbit(frame);
      
      // draw Earth-Sun line
      if (mDrawESLine)
         DrawESLine();
      
      // draw axes in other coord. system
      if (!mIsInternalCoordSystem)
      {
         if (mDrawAxes)
            DrawAxes(false);
      }
      
      glFlush();
      SwapBuffers();
   }
   
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
      ("TrajPlotCanvas::DrawPlot() mNumData=%d, mNeedOriginConversion=%d, "
       "mIsInternalCoordSystem=%d, mUseInitialViewPoint=%d, mAxisLength=%f\n",
       mNumData, mNeedOriginConversion, mIsInternalCoordSystem, mUseInitialViewPoint,
       mAxisLength);
   #endif
   
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   DrawStatus(mNumData-1);
   SetProjection();
   
   ComputeViewMatrix();
   
   // tilt Origin rotation axis if needed
   if (mNeedOriginConversion)
   {
      glPushMatrix();
      TiltOriginZAxis();
   }
   
   // draw equatorial plane
   if (mDrawEqPlane)
      DrawEquatorialPlane(mEqPlaneColor);
   
   // draw axes
   if (mDrawAxes)
      DrawAxes(true);
   
   // draw ecliptic plane
   if (mDrawEcPlane)
      DrawEclipticPlane(mEcPlaneColor);
   
   if (mNeedOriginConversion)
   {
      glPopMatrix();
   }
   
   // draw object orbit
   DrawObjectOrbit(mNumData-1);
      
   // draw Earth-Sun line
   if (mDrawESLine)
      DrawESLine();
   
   // draw axes in other coord. system
   if (!mIsInternalCoordSystem)
   {
      if (mDrawAxes)
         DrawAxes(false);
   }
   
   glFlush();
   SwapBuffers();
   
} // end DrawPlot()


//------------------------------------------------------------------------------
//  void DrawObject(const wxString &objName)
//------------------------------------------------------------------------------
/**
 * Draws object shpere and maps texture image.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawObject(const wxString &objName)
{
   int objId = GetObjectId(objName);
   
   #if DEBUG_TRAJCANVAS_DRAW > 1
   MessageInterface::ShowMessage
         ("TrajPlotCanvas::DrawObject() drawing:%s, objId:%d\n",
          objName.c_str(), objId);
   #endif
   
   //-------------------------------------------------------
   // draw object with texture on option
   //-------------------------------------------------------
   #if DEBUG_TRAJCANVAS_DRAW > 1
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::DrawObject() mObjectTextureIdMap[%s]=%d\n",
       objName.c_str(), mObjectTextureIdMap[objName]);
   #endif
   
   if (mObjectTextureIdMap[objName] != UNINIT_TEXTURE)
   {
      //glColor4f(1.0, 1.0, 1.0, 1.0);
      glColor3f(1.0, 1.0, 1.0);
      
      glBindTexture(GL_TEXTURE_2D, mObjectTextureIdMap[objName]);
      glEnable(GL_TEXTURE_2D);
      GLUquadricObj* qobj = gluNewQuadric();
      gluQuadricDrawStyle(qobj, GLU_FILL  );
      gluQuadricNormals  (qobj, GLU_SMOOTH);
      gluQuadricTexture  (qobj, GL_TRUE   );
      gluSphere(qobj, mObjectRadius[objId], 50, 50);
      gluDeleteQuadric(qobj);
      
      glDisable(GL_TEXTURE_2D);
   }
   else
   {
      #if DEBUG_TRAJCANVAS_DRAW
      MessageInterface::ShowMessage
         ("*** Warning *** TrajPlotCanvas::DrawObject() %s texture not found.\n",
          objName.c_str());
      #endif
      
      // just draw small sphere?
   }
   
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
   
   for (int obj=0; obj<mObjectCount; obj++)
   {
      objName = mObjectNames[obj];
      objId = GetObjectId(objName);
      
      glPushMatrix();
      glBegin(GL_LINES);
      
      for (int i=1; i<=frame; i++)
      {
         // Draw object orbit line based on points
         if ((mTime[i] > mTime[i-1]) ||
             (i>2 && mTime[i] < mTime[i-1]) && mTime[i-1] < mTime[i-2]) //loj: 5/26/05 for backprop
         {
            Rvector3 r1(mObjectTempPos[objId][i-1][0], mObjectTempPos[objId][i-1][1],
                        mObjectTempPos[objId][i-1][2]);
            
            Rvector3 r2(mObjectTempPos[objId][i][0], mObjectTempPos[objId][i][1],
                        mObjectTempPos[objId][i][2]);
            
            // if object position magnitude is 0, skip
            if (r1.GetMagnitude() == 0.0 || r2.GetMagnitude() == 0.0)
               continue;
            
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
      
      //-------------------------------------------------------
      // draw object orbit normal vector
      // (loj: 6/13/05 Fow now it only draws spacecraft orbit normal vector.)
      //-------------------------------------------------------
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
               
               if (mObjectArray[obj]->IsOfType(Gmat::SPACECRAFT))
                  color = mObjectOrbitColor[objId][i];
               else
                  color = mObjectColorMap[objName].GetIntColor();
               
               DrawObjectOrbitNormal(objId, i, color);
               glPopMatrix();
            }
         }
      }
      
      //-------------------------------------------------------
      //draw object with texture
      //-------------------------------------------------------
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
            
            if (mObjectArray[obj]->IsOfType(Gmat::SPACECRAFT))
               DrawSpacecraft(mObjectOrbitColor[objId][mObjLastFrame[obj]]);
            else
               DrawObject(objName);
            
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


   Real orthoDepth = distance;
   
   if (mUsePerspectiveMode)
      orthoDepth = (mAxisLength*60) / (mFovDeg/2.0);

   bool useArgosyCode = true;

   if (useArgosyCode)
   {
      //==============================================================
      // Argosy code
      //==============================================================
      //orthoDepth = (half-size-of-image)*60/(half-FOV-degrees)
      
      Real ort = orthoDepth * 8;
      //int pwr = gile(log10 (ort));
      Real pwr = Floor(Log10(ort)); //loj: 6/17/05 Changed to use Log10()
      //Real size = exp10(pwr)/100;
      Real size = Exp10(pwr)/100;   //loj: 6/17/05 Changed to use Exp10()
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
      //SetCelestialColor (color);
      for (int i=1; i<=(int)imax; ++i)
         //if (i%10!=0 && (GlOptions.DrawDarkLines || factor > 0.5))
         if (i%10!=0 && (factor > 0.5))
            DrawCircle(qobj, i*size);
   }
   else
   {
      //==============================================================
      // GMAT code
      //==============================================================
      int maxCircle = (int)(distance/5000);
      Real radius;
      
      if (maxCircle > 50)
         maxCircle = 50;
      
      for(i=1; i<maxCircle; i++)
      {
         radius = i*distance/maxCircle; // equal distance
         radius = radius + (radius /100.0 / log10(radius) * exp(Real(i)));
         DrawCircle(qobj, radius);
      }
   }
   
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
//  void DrawESLine()
//------------------------------------------------------------------------------
/**
 * Draws ecliptic plane circles.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawESLine()
{
   Real objPos[3], endPos[3];
   Real distance = (Real)mAxisLength;
   Real mag;
   int numSkip = mNumData/12; // draw 24 lines (12*2)
   
   int objId = GetObjectId("Sun");
   
   // if origin is Sun, get Earth position
   if (mOriginName == "Sun")
      objId = GetObjectId("Earth");
   
   //--------------------------------
   // draw sun lines
   //--------------------------------
   
   // set color
   *sIntColor = mESLinecolor;
   glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);
   
   glBegin(GL_LINES);
   
   for (int i=0; i<mNumData; i+=numSkip)
   {
      //loj: 4/15/05 Added minus sign to x, y
      objPos[0] = -mObjectTempPos[objId][i][0];
      objPos[1] = -mObjectTempPos[objId][i][1];
      objPos[2] =  mObjectTempPos[objId][i][2];
      
      // get sun unit vector and multiply by distance
      mag = sqrt(objPos[0]*objPos[0] + objPos[1]*objPos[1] + objPos[2]*objPos[2]);
      endPos[0] = objPos[0]/mag * distance;
      endPos[1] = objPos[1]/mag * distance;
      endPos[2] = objPos[2]/mag * distance;
      
      //loj: 6/10/05 Why setting alpha doesn't work?
      //*sIntColor = mESLinecolor;
      //glColor4ub(sGlColor->red, sGlColor->green, sGlColor->blue, 255);
      
      //glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(endPos[0], endPos[1], endPos[2]);
      //glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(-endPos[0], -endPos[1], -endPos[2]);
   }
   
   glEnd();
   
   // Show Sun direction text(loj: 6/10/05)
   glColor3f(1.0, 1.0, 0.0); // yellow
   if (mOriginName == "Earth")
      DrawStringAt(" +S", endPos[0]/2.2, endPos[1]/2.2, endPos[2]/2.2);
   else if (mOriginName == "Sun")
      DrawStringAt(" -S", endPos[0]/2.2, endPos[1]/2.2, endPos[2]/2.2);
   
} // end DrawESLine()


//---------------------------------------------------------------------------
// void DrawAxes(bool gci = false)
//---------------------------------------------------------------------------
void TrajPlotCanvas::DrawAxes(bool gci)
{
   GLfloat viewDist;

   //-----------------------------------
   // draw axes
   //-----------------------------------
   
   //viewDist = mCurrViewDist/2; //zooms in and out
   viewDist = mAxisLength/2.2; // stays the same
   glBegin(GL_LINES);
   
   glColor3f(0, 1, 0);   // x
   glTranslatef(viewDist, 0.0, 0.0);
   glVertex3f(-viewDist, 0, 0);
   glVertex3f( viewDist, 0, 0);
   glTranslatef(-viewDist, 0.0, 0.0);

   glColor3f(0, 0, 1);   // y
   glTranslatef(0.0, -viewDist, 0.0);
   glVertex3f(0, -viewDist, 0);
   glVertex3f(0,  viewDist, 0);

   glColor3f(1, 1, 0);   // z
   glTranslatef(0.0, 0.0, viewDist);
   glVertex3f(0, 0, -viewDist);
   glVertex3f(0, 0, viewDist);

   glEnd();
   
   //-----------------------------------
   // throw some text out...
   //-----------------------------------
   glColor3f(0, 1, 0);   // x
   if (gci)
      DrawStringAt("+xMJ2000Eq", -viewDist, 0.0, 0.0);
   else
      DrawStringAt("+x", -viewDist, 0.0, 0.0);

   glColor3f(0, 0, 1);   // y
   if (gci)
      DrawStringAt("+yMJ2000Eq", 0.0, -viewDist, 0.0);
   else
      DrawStringAt("+y", 0.0, -viewDist, 0.0);
      
   glColor3f(1, 1, 0);   // z
   if (gci)
      DrawStringAt("+zMJ2000Eq", 0.0, 0.0, viewDist);
   else
      DrawStringAt("+z", 0.0, 0.0, viewDist);
   
}


//---------------------------------------------------------------------------
// void DrawStatus(int frame)
//---------------------------------------------------------------------------
void TrajPlotCanvas::DrawStatus(int frame)
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
   text = "Frame#: " + str;
   str.Printf("%f", mTime[frame]);
   text = text + "  Time: " + str;
   
   glColor3f(1, 1, 0); //yellow
   glRasterPos2i(0, 0);
   glCallLists(strlen(text.c_str()), GL_BYTE, (GLubyte*)text.c_str());
   
   //wxLogStatus(GmatAppData::GetMainFrame(), wxT("Frame#: %d, Time: %f"), frame,
   //            mTime[frame]);
   
}


//---------------------------------------------------------------------------
// void DrawStringAt(char* inMsg, GLfloat x, GLfloat y, GLfloat z)
//---------------------------------------------------------------------------
void TrajPlotCanvas::DrawStringAt(char* inMsg, GLfloat x, GLfloat y, GLfloat z)
{
   glRasterPos3f(x, y, z);
   glCallLists(strlen(inMsg), GL_BYTE, (GLubyte*)inMsg);
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

   MessageInterface::PopupMessage
      (Gmat::ERROR_, "TrajPlotCanvas::GetObjectId() obj name: " + name +
       " not found in the object list\n");

   return -1;
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
   
   // rotate earth Z axis if view CS is MJ2000Ec
   //5.10if (mViewCoordSystem->GetName() == "EarthMJ2000Ec")
   if (axisTypeName == "MJ2000Ec")
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
// bool ConvertSpacecraftData()
//---------------------------------------------------------------------------
bool TrajPlotCanvas::ConvertSpacecraftData()
{
   if (mInternalCoordSystem == NULL || mViewCoordSystem == NULL)
      return false;
   
   Rvector6 inState, outState;
   
   #if DEBUG_TRAJCANVAS_CONVERT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::ConvertSpacecraftData() internalCS=%s, viewCS=%s\n",
       mInternalCoordSystem->GetName().c_str(), mViewCoordSystem->GetName().c_str());
   #endif
   
   int objId;
   
   // do not convert if view CS is internal CS
   //if (mViewCoordSystem->GetName() == mInternalCoordSystem->GetName())
   if (mIsInternalCoordSystem)
   {
      for (int sc=0; sc<mScCount; sc++)
      {
         objId = GetObjectId(mScNameArray[sc].c_str());
         
         for (int i=0; i<mNumData; i++)
         {
            mObjectTempPos[objId][i][0] = mObjectGciPos[objId][i][0];
            mObjectTempPos[objId][i][1] = mObjectGciPos[objId][i][1];
            mObjectTempPos[objId][i][2] = mObjectGciPos[objId][i][2];
         }
      }
   }
   else
   {
      for (int sc=0; sc<mScCount; sc++)
      {
         objId = GetObjectId(mScNameArray[sc].c_str());
         
         for (int i=0; i<mNumData; i++)
         {
            inState.Set(mObjectGciPos[objId][i][0], mObjectGciPos[objId][i][1],
                        mObjectGciPos[objId][i][2], 0.0, 0.0, 0.0);
                        
            mCoordConverter.Convert(mTime[i], inState, mInternalCoordSystem,
                                    outState, mViewCoordSystem);
            
            mObjectTempPos[objId][i][0] = outState[0];
            mObjectTempPos[objId][i][1] = outState[1];
            mObjectTempPos[objId][i][2] = outState[2];
            
            #if DEBUG_TRAJCANVAS_CONVERT > 1
            MessageInterface::ShowMessage
               ("TrajPlotCanvas::ConvertSpacecraftData() in=%g, %g, %g, out=%g, %g, %g\n",
                inState[0], inState[1], inState[2], outState[0], outState[1], outState[2]);
            #endif
         }
      }
   }
   return true;
}


//---------------------------------------------------------------------------
// bool ConvertSpacecraftData(int frame)
//---------------------------------------------------------------------------
bool TrajPlotCanvas::ConvertSpacecraftData(int frame)
{
   if (mInternalCoordSystem == NULL || mViewCoordSystem == NULL)
      return false;
   
   Rvector6 inState, outState;
   
   #if DEBUG_TRAJCANVAS_CONVERT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::ConvertSpacecraftData() internalCS=%s, viewCS=%s\n",
       mInternalCoordSystem->GetName().c_str(), mViewCoordSystem->GetName().c_str());
   #endif

   int objId;
   
   // do not convert if view CS is internal CS
   //if (mViewCoordSystem->GetName() == mInternalCoordSystem->GetName())
   if (mIsInternalCoordSystem)
   {
      for (int sc=0; sc<mScCount; sc++)
      {
         objId = GetObjectId(mScNameArray[sc].c_str());
         
         mObjectTempPos[objId][frame][0] = mObjectGciPos[objId][frame][0];
         mObjectTempPos[objId][frame][1] = mObjectGciPos[objId][frame][1];
         mObjectTempPos[objId][frame][2] = mObjectGciPos[objId][frame][2];
      }
   }
   else
   {
      for (int sc=0; sc<mScCount; sc++)
      {
         objId = GetObjectId(mScNameArray[sc].c_str());
                  
         inState.Set(mObjectGciPos[objId][frame][0], mObjectGciPos[objId][frame][1],
                     mObjectGciPos[objId][frame][2],
                     0.0, 0.0, 0.0);
         
         mCoordConverter.Convert(mTime[frame], inState, mInternalCoordSystem,
                                 outState, mViewCoordSystem);
         
         mObjectTempPos[objId][frame][0] = outState[0];
         mObjectTempPos[objId][frame][1] = outState[1];
         mObjectTempPos[objId][frame][2] = outState[2];
         
         #if DEBUG_TRAJCANVAS_CONVERT > 1
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::ConvertSpacecraftData() in=%g, %g, %g, out=%g, %g, %g\n",
             inState[0], inState[1], inState[2], outState[0], outState[1], outState[2]);
         #endif
      }
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
      ("TrajPlotCanvas::ConvertObjectData() internalCS=%s, viewCS=%s\n",
       mInternalCoordSystem->GetName().c_str(), mViewCoordSystem->GetName().c_str());
   #endif
   
   // do not convert if view CS is internal CS
   //if (mViewCoordSystem->GetName() == mInternalCoordSystem->GetName())
   if (mIsInternalCoordSystem)
   {
      for (int i=0; i<mObjectCount; i++)
      {
         int objId = GetObjectId(mObjectNames[i]);
         
         for (int i=0; i<mNumData; i++)
            CopyVector3(mObjectTempPos[objId][i], mObjectGciPos[objId][i]);
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
               mObjectGciPos[objId][index][2], 0.0, 0.0, 0.0);
   
   mCoordConverter.Convert(mTime[index], inState, mInternalCoordSystem,
                           outState, mViewCoordSystem);
   
   mObjectTempPos[objId][index][0] = outState[0];
   mObjectTempPos[objId][index][1] = outState[1];
   mObjectTempPos[objId][index][2] = outState[2];
   
   #if DEBUG_TRAJCANVAS_CONVERT > 1
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::ConvertObject() in=%g, %g, %g, "
       "out=%g, %g, %g\n", inState[0], inState[1], inState[2],
       outState[0], outState[1], outState[2]);
   #endif
}


//---------------------------------------------------------------------------
//  void CopyVector3(float to[3], Real from[3])
//---------------------------------------------------------------------------
void TrajPlotCanvas::CopyVector3(float to[3], Real from[3])
{
   to[0] = from[0];
   to[1] = from[1];
   to[2] = from[2];
}


//---------------------------------------------------------------------------
//  void CopyVector3(float to[3], float from[3])
//---------------------------------------------------------------------------
void TrajPlotCanvas::CopyVector3(float to[3], float from[3])
{
   to[0] = from[0];
   to[1] = from[1];
   to[2] = from[2];
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
//  void CopyVector3(Real to[3], float from[3])
//---------------------------------------------------------------------------
void TrajPlotCanvas::CopyVector3(Real to[3], float from[3])
{
   to[0] = from[0];
   to[1] = from[1];
   to[2] = from[2];
}

