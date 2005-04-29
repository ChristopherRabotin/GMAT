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
#  include <IL/il.h>
#  include <IL/ilu.h>
#  include <IL/ilut.h>
#endif

//#define DEBUG_TRAJCANVAS_INIT 1
//#define DEBUG_TRAJCANVAS_UPDATE 1
//#define DEBUG_TRAJCANVAS_ACTION 1
//#define DEBUG_TRAJCANVAS_CONVERT 2
//#define DEBUG_TRAJCANVAS_DRAW 1
//#define DEBUG_TRAJCANVAS_BODY 1
//#define DEBUG_TRAJCANVAS_PROJ 1
//#define DEBUG_TRAJCANVAS_ANIMATION 1

BEGIN_EVENT_TABLE(TrajPlotCanvas, wxGLCanvas)
   EVT_SIZE(TrajPlotCanvas::OnTrajSize)
   EVT_PAINT(TrajPlotCanvas::OnPaint)
   EVT_MOUSE_EVENTS(TrajPlotCanvas::OnMouse)
   EVT_KEY_DOWN(TrajPlotCanvas::OnKeyDown)
END_EVENT_TABLE()

//---------------------------------
// static data
//---------------------------------

enum TFrameMode
{
   GCI_FRAME,
   SOL_ROT_FRAME,
   SELENOCENTRIC_FRAME,
   EARTH_MOON_ROT_FRAME,
   HELIOCENTRIC_FRAME,
   ANY_BODY_FRAME,
   ANY_BODY_ROT_FRAME
};

struct GlColorType
{
   Byte red;
   Byte green;
   Byte blue;
   Byte not_used;
};


static int *sIntColor = new int;
static GlColorType *sGlColor = (GlColorType*)sIntColor;

using namespace GmatPlot;

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
   mInitialized = false;
   mNumData = 0;

   // projection
   mUsePerspectiveMode = false;
   
   // viewpoint
   mViewPointRefObj = NULL;
   mViewPointVectorObj = NULL;
   mViewDirectionObj = NULL;
   mViewPointRefVector.Set(0.0, 0.0, 0.0);
   mViewPointVector.Set(0.0, 0.0, 30000);
   mViewDirectionVector.Set(0.0, 0.0, -1.0);
   mViewPointLocVector.Set(0.0, 0.0, 30000);
   mViewScaleFactor = 1.0;
   mUseViewPointInfo = true;
   mUseViewPointRefVector = true;
   mUseViewPointVector = true;
   mUseViewDirectionVector = true;
   mVptRefScId = -1;
   mVptVecScId = -1;
   mVdirScId = -1;
   mVptRefBodyId = -1;
   mVptVecBodyId = -1;
   mVdirBodyId = -1;

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
   
   mAxisLength = mCurrViewDist;
   mCurrViewFrame = GCI_FRAME;
   mCenterViewBody = GmatPlot::EARTH;
   mCurrBody = GmatPlot::EARTH;
   mRotateAboutXaxis = true;
   mRotateAboutYaxis = false;
   mRotateAboutZaxis = false;
   mRotateXy = true;

   mZoomAmount = 300.0;
   
   // projection
   ChangeProjection(size.x, size.y, mAxisLength);
   
   mEarthRadius = 6378.14; //km
   mScRadius = 200;        //km: make big enough to see

   // view options
   mDrawSpacecraft = true;
   mDrawWireFrame = false;
   mDrawEqPlane = false;
   mDrawEcPlane = false;
   mUseTexture = true;
   mDrawAxes = false;
   mEqPlaneColor = GmatColor::GRAY32;
   mEcPlaneColor = GmatColor::TEAL32;
   mEcLineColor = GmatColor::D_BROWN32;

   // animation
   mViewAnimation = false;
   mHasUserInterrupted = false;
   mUpdateInterval = 10;
   
   // solar system
   mSolarSystem = solarSys;
   
   // bodies
   mOtherBodyCount = 0;
   
   for (int i=0; i<MAX_BODIES; i++)
   {
      mBodyTextureIndex[i] = UNINIT_TEXTURE;
      mBodyMaxZoomIn[i] = MAX_ZOOM_IN;
      mBodyInUse[i] = false;
      mBodyHasData[i] = false;
   }
   
   // set Sun, Earth, Moon in use as default,
   // bodies' position is updated in UpdadateSpacecraft()
   mBodyInUse[SUN]   = true;
   mBodyInUse[EARTH] = true;
   mBodyInUse[MOON]  = true;
   mBodyHasData[SUN]   = true;
   mBodyHasData[EARTH] = true;
   mBodyHasData[MOON]  = true;
   
   for (int body=0; body<MAX_BODIES; body++)
      mBodyRadius[body] = 0.0;

   if (mSolarSystem != NULL)
   {
      CelestialBody *bodyPtr;
      for (int body=0; body<=MOON; body++)
      {
         if (mBodyInUse[body])
         {
            bodyPtr = mSolarSystem->GetBody(BodyInfo::BODY_NAME[body]);
            if (bodyPtr != NULL)
            {
               mBodyRadius[body] = bodyPtr->GetEquatorialRadius();
               mBodyMaxZoomIn[body] = mBodyRadius[body] * RADIUS_ZOOM_RATIO;
               
               #if DEBUG_TRAJCANVAS_INIT
               MessageInterface::ShowMessage
                  ("TrajPlotCanvas() %s Radius=%f maxZoomIn=%f\n",
                   BodyInfo::BODY_NAME[body].c_str(),
                   mBodyRadius[body], mBodyMaxZoomIn[body]);
               #endif
            }
         }
      }
   }
   
   // Zoom
   mMaxZoomIn = mBodyMaxZoomIn[EARTH];
   
   // Spacecraft
   mScCount = 0;
   
   for (int i=0; i<MAX_SCS; i++)
      mScTextureIndex[i] = UNINIT_TEXTURE;
   
   // Coordinate System
   mInternalCoordSystem = theGuiInterpreter->GetInternalCoordinateSystem();
   mInternalCoordSysName = wxString(mInternalCoordSystem->GetName().c_str());

   mDesiredCoordSysName = csName;
   mDesiredCoordSystem = theGuiInterpreter->
      GetCoordinateSystem(std::string(csName.c_str()));

   // CoordinateSystem conversion
   mIsInternalCoordSystem = true;
   mNeedSpacecraftConversion = false;
   mNeedEarthConversion = false;
   mNeedOtherBodyConversion = false;
   
   if (!mDesiredCoordSysName.IsSameAs(mInternalCoordSysName))
       mNeedConversion = true;
   else
       mNeedConversion = false;

   
   #if DEBUG_TRAJCANVAS_INIT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas() internalCS=%s, desiredCS=%s\n",
       mInternalCoordSystem->GetName().c_str(), mDesiredCoordSystem->GetName().c_str());
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
   if (!LoadGLTextures())
      return false;
#endif

   //loj: 3/10/05 Actually I don't need this
   // pixel format
   //if (!SetPixelFormatDescriptor())
   //   return false;
   
   // font
   SetDefaultGLFont();
   
   mInitialized = true;
   
   return true;
}


//------------------------------------------------------------------------------
// bool IsInitialized()
//------------------------------------------------------------------------------
/**
 * Retrives initialized flag.
 */
//------------------------------------------------------------------------------
bool TrajPlotCanvas::IsInitialized()
{
   return mInitialized;
}


//------------------------------------------------------------------------------
// void SetDesiredCoordSystem(const wxString &csName)
//------------------------------------------------------------------------------
void TrajPlotCanvas::SetDesiredCoordSystem(const wxString &csName)
{
   #if DEBUG_TRAJCANVAS_INIT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas()::SetDesiredCoordSysName() internalCS=%s, desiredCS=%s\n",
       mInternalCoordSystem->GetName().c_str(), csName.c_str());
   #endif
   
   mDesiredCoordSysName = csName;

   mDesiredCoordSystem =
      theGuiInterpreter->GetCoordinateSystem(std::string(csName.c_str()));
   
   if (!mDesiredCoordSysName.IsSameAs(mInternalCoordSysName))
      mNeedConversion = true;
   else
      mNeedConversion = false;
   
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

   if (perspMode)
   {
      mfCamTransX = -mViewPointLocVector[0];
      mfCamTransY = -mViewPointLocVector[1];
      mfCamTransZ = -mViewPointLocVector[2];
   }
   else
   {
      mfCamTransX = 0;
      mfCamTransY = 0;
      mfCamTransZ = 0;
   }
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
// void UpdatePlot(bool viewAnimation)
//------------------------------------------------------------------------------
/**
 * Updates plot.
 *
 * @param <viewAnimation> true if animation is viewed
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::UpdatePlot(bool viewAnimation)
{
   if (mAxisLength < mMaxZoomIn)
   {
      mAxisLength = mMaxZoomIn;
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::UpdatePlot() distance < max zoom in. distance set to %f\n",
          mAxisLength);
   }

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

   mCurrRotXAngle = mDefaultRotXAngle;
   mCurrRotYAngle = mDefaultRotYAngle;
   mCurrRotZAngle = mDefaultRotZAngle;
   mCurrViewDist = mDefaultViewDist;
   mAxisLength = mCurrViewDist;
   mfCamTransX = 0;
   mfCamTransY = 0;
   mfCamTransZ = 0;

   mCenterViewBody = EARTH;
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
   double realDist = (mAxisLength - mZoomAmount) / log(mAxisLength);

//    if (mUsePerspectiveMode)
//    {
//       mAxisLength = mAxisLength - realDist;
//       ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
//    }
   if (mAxisLength > mMaxZoomIn)
   {
      mAxisLength = mAxisLength - realDist;

      if (mAxisLength < mMaxZoomIn)
         mAxisLength = mMaxZoomIn;
   
      ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
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
   double realDist = (mAxisLength + mZoomAmount) / log(mAxisLength);
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
      ("TrajPlotCanvas::DrawInNewCoordSysName() desiredCS=%s, newCS=%s\n",
       mDesiredCoordSysName.c_str(), csName.c_str());
   #endif

   // if current desired CS name is different from the new CS name
   if (!mDesiredCoordSysName.IsSameAs(csName))
   {
      mDesiredCoordSysName = csName;
      
      mDesiredCoordSystem =
         theGuiInterpreter->GetCoordinateSystem(std::string(csName.c_str()));
      
      if (mDesiredCoordSystem->GetName() == mInternalCoordSystem->GetName())
         mIsInternalCoordSystem = true;
      else
         mIsInternalCoordSystem = false;
         
      mNeedSpacecraftConversion = true;
      mNeedEarthConversion = true;
      mNeedOtherBodyConversion = true;
      Refresh(false);
   }
   else
   {
      mNeedSpacecraftConversion = false;
      mNeedEarthConversion = false;
      mNeedOtherBodyConversion = false;
   }
}

//------------------------------------------------------------------------------
// void DrawInOtherCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Draws objects in other coordinate system.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawInOtherCoordSystem(CoordinateSystem *cs)
{
   #if DEBUG_TRAJCANVAS_ACTION
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::DrawInNewCoordSystem() desiredCS=%s, newCS=%s\n",
       mDesiredCoordSystem->GetName().c_str(), cs->GetName().c_str());
   #endif

   // if current desired CS name is different from the new CS name
   if (mDesiredCoordSystem->GetName() != cs->GetName())
   {
      mDesiredCoordSystem = cs;
      
      if (mDesiredCoordSystem->GetName() == mInternalCoordSystem->GetName())
         mIsInternalCoordSystem = true;
      else
         mIsInternalCoordSystem = false;
         
      mNeedSpacecraftConversion = true;
      mNeedEarthConversion = true;
      mNeedOtherBodyConversion = true;
      Refresh(false);
   }
   else
   {
      mNeedSpacecraftConversion = false;
      mNeedEarthConversion = false;
      mNeedOtherBodyConversion = false;
   }
}


//---------------------------------------------------------------------------
// void GotoStdBody(int bodyId)
//---------------------------------------------------------------------------
void TrajPlotCanvas::GotoStdBody(int bodyId)
{
   
   #ifdef DEBUG_TRAJCANVAS_BODY
      MessageInterface::ShowMessage("TrajPlotCanvas::GotoStdBody() bodyId=%d\n",
                                    bodyId);
   #endif
   
   if (!mBodyHasData[bodyId])
   {
      mBodyHasData[bodyId] = true;
      CelestialBody *bodyPtr = mSolarSystem->GetBody(BodyInfo::BODY_NAME[bodyId]);
      mBodyRadius[bodyId] = bodyPtr->GetEquatorialRadius();
      mBodyMaxZoomIn[bodyId] = mBodyRadius[bodyId] * RADIUS_ZOOM_RATIO;

      for (int i=0; i<mNumData; i++)
      {
         Rvector6 bodyState = bodyPtr->GetState(mTime[i]);
         mBodyGciPos[bodyId][i][0] = bodyState[0];
         mBodyGciPos[bodyId][i][1] = bodyState[1];
         mBodyGciPos[bodyId][i][2] = bodyState[2];
         CopyVector3(mBodyTempPos[bodyId][i], mBodyGciPos[bodyId][i]);
      }
   }
   
   mCenterViewBody = bodyId;
   mMaxZoomIn = mBodyMaxZoomIn[bodyId];

   if (bodyId == EARTH)
   {
      mAxisLength = DEFAULT_DIST;
   }
   else
   {
      Rvector3 pos(mBodyTempPos[bodyId][mNumData-1][0],
                   mBodyTempPos[bodyId][mNumData-1][1],
                   mBodyTempPos[bodyId][mNumData-1][2]);
      
      mAxisLength = pos.GetMagnitude();
   }
   
   SetProjection();
   DrawPicture();
   Refresh(false);
}


//---------------------------------------------------------------------------
// void GotoOtherBody(const wxString &body)
//---------------------------------------------------------------------------
void TrajPlotCanvas::GotoOtherBody(const wxString &body)
{
   #ifdef DEBUG_TRAJCANVAS_BODY
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

   mViewAnimation = true;
   mUpdateInterval = interval;
   mHasUserInterrupted = false;
   DrawFrame();
}


//---------------------------------------------------------------------------
// int GetStdBodyId(const std::string &name)
//---------------------------------------------------------------------------
int TrajPlotCanvas::GetStdBodyId(const std::string &name)
{
   for (int i=0; i<=LAST_STD_BODY_ID; i++)
      if (BodyInfo::BODY_NAME[i] == name)
         return i;

   MessageInterface::PopupMessage
      (Gmat::ERROR_, "TrajPlotCanvas::GetStdBodyId() body name: %s not found "
       "in the default list\n", name.c_str());

   return -1;
}


//------------------------------------------------------------------------------
// void SetGlViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
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
void TrajPlotCanvas::SetGlViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                                     SpacePoint *vdObj, Real vsFactor,
                                     const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                                     const Rvector3 &vdVec, bool usevpRefVec,
                                     bool usevpVec, bool usevdVec)
{
   mViewPointRefObj = vpRefObj;
   mViewPointVectorObj = vpVecObj;
   mViewDirectionObj = vdObj;
   mViewScaleFactor = vsFactor;
   mViewPointRefVector = vpRefVec;
   mViewPointVector = vpVec;
   mViewDirectionVector = vdVec;
   mUseViewPointRefVector = usevpRefVec;
   mUseViewPointVector = usevpVec;
   mUseViewDirectionVector = usevdVec;
   
   Rvector3 lvpRefVec(vpRefVec);
   Rvector3 lvpVec(vpVec);
   Rvector3 lvdVec(vdVec);
   
   #if DEBUG_TRAJCANVAS_INIT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::SetGlViewOption() vpRefObj=%d, vpVecObj=%d, "
       "vdObj=%d, vsFactor=%f\nvpRefVec=%s, vpVec=%s, vdVec=%s, usevpRefVec=%d, "
       "usevpVec=%d, usevdVec=%d\n",  vpRefObj, vpVecObj, vdObj,
       vsFactor, lvpRefVec.ToString().c_str(), lvpVec.ToString().c_str(),
       lvdVec.ToString().c_str(), usevpRefVec, usevpVec, usevdVec);
   #endif

   // Set viewpoint ref. object id, if not spacecraft
   if (!mUseViewPointRefVector && mViewPointRefObj)
   {
      if (mViewPointRefObj->GetType() != Gmat::SPACECRAFT)
      {
         mVptRefBodyId = GmatPlot::GetBodyId(mViewPointRefObj->GetName().c_str());
         if (mVptRefBodyId == GmatPlot::UNKNOWN_BODY)
         {
            //@todo - Handle non celestial body later
            mUseViewPointRefVector = true;
            MessageInterface::ShowMessage
               ("TrajPlotCanvas::SetGlViewOption() ***** Cannot find "
                "mViewPointRefObj name=%s, so using vector=%s\n",
                mViewPointRefObj->GetName().c_str(),
                mViewPointRefVector.ToString().c_str());
         }
         else
         {
            mBodyInUse[mVptRefBodyId] = true;
            mBodyHasData[mVptRefBodyId] = true;
         }
      }
   }
   else
   {
      if (!mUseViewPointRefVector)
         MessageInterface::ShowMessage
            ("*** Warning *** TrajPlotCanvas::SetGlViewOption() "
             "ViewPointRefObject is NULL,"
             "so will use default Vector instead.\n");
   }
   
   // Set viewpoint vector object id, if not spacecraft
   if (!mUseViewPointVector && mViewPointVectorObj)
   {
      if (mViewPointVectorObj->GetType() != Gmat::SPACECRAFT)
      {
         mVptVecBodyId = GmatPlot::GetBodyId(mViewPointVectorObj->GetName().c_str());
         if (mVptVecBodyId == GmatPlot::UNKNOWN_BODY)
         {
            //@todo - Handle non celestial body later
            mUseViewPointVector = true;
            MessageInterface::ShowMessage
               ("TrajPlotCanvas::SetGlViewOption() ***** Cannot find "
                "mViewPointVectorObj name=%s, so using vector=%s\n",
                mViewPointVectorObj->GetName().c_str(),
                mViewPointVector.ToString().c_str());
         }
         else
         {
            mBodyInUse[mVptVecBodyId] = true;
            mBodyHasData[mVptVecBodyId] = true;
         }
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
   
   // Set view direction object id, if not spacecraft
   if (!mUseViewDirectionVector && mViewDirectionObj)
   {
      if (mViewDirectionObj->GetType() != Gmat::SPACECRAFT)
      {
         mVdirBodyId = GmatPlot::GetBodyId(mViewDirectionObj->GetName().c_str());
         if (mVdirBodyId == GmatPlot::UNKNOWN_BODY)
         {
            //@todo - Handle non celestial body later
            mUseViewDirectionVector = true;
            MessageInterface::ShowMessage
               ("TrajPlotCanvas::SetGlViewOption() ***** Cannot find "
                "mViewDirectionObj name=%s, so using vector=%s\n",
                mViewDirectionObj->GetName().c_str(),
                mViewDirectionVector.ToString().c_str());
         }
         else
         {
            mBodyInUse[mVdirBodyId] = true;
            mBodyHasData[mVdirBodyId] = true;
         }
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
}


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

      int sc = 0;
      for(int i=0; i<numDataPoints && i < MAX_DATA; i++)
      {
         mScTrajColor[sc][mNumData] = GmatColor::RED32;
         mTime[mNumData] = mTrajectoryData[i].time;
         mScTempPos[sc][mNumData][0] = mTrajectoryData[i].x;
         mScTempPos[sc][mNumData][1] = mTrajectoryData[i].y;
         mScTempPos[sc][mNumData][2] = mTrajectoryData[i].z;
         mEarthTempPos[mNumData][0] = 0.0;
         mEarthTempPos[mNumData][1] = 0.0;
         mEarthTempPos[mNumData][2] = 0.0;
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

   mInitialized = true;
    
   return numDataPoints;
    
} //end ReadTextTrajectory()


//------------------------------------------------------------------------------
// void UpdateSpacecraft(const StringArray &scNameArray,
//                       const Real &time, const RealArray &posX,
//                       const RealArray &posY, const RealArray &posZ,
//                       const UnsignedIntArray &color)
//------------------------------------------------------------------------------
/**
 * Updates spacecraft trajectory.
 *
 * @param <time> time
 * @param <posX> position x array
 * @param <posY> position y array
 * @param <posZ> position z array
 * @param <orbitColor> orbit color array
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::UpdateSpacecraft(const StringArray &scNameArray,
                                      const Real &time, const RealArray &posX,
                                      const RealArray &posY, const RealArray &posZ,
                                      const UnsignedIntArray &color)
{
   mScCount = posX.size();
   if (mScCount > MAX_SCS)
      mScCount = MAX_SCS;

   mScNameArray = scNameArray;
   
   //-------------------------------------------------------
   // update spacecraft position
   //-------------------------------------------------------
   if (mNumData < MAX_DATA)
   {
      for (int sc=0; sc<mScCount; sc++)
      {
         mTime[mNumData] = time;
         mScTrajColor[sc][mNumData]  = color[sc];
         mScGciPos[sc][mNumData][0] = posX[sc];
         mScGciPos[sc][mNumData][1] = posY[sc];
         mScGciPos[sc][mNumData][2] = posZ[sc];

         mEarthGciPos[mNumData][0] = 0.0;
         mEarthGciPos[mNumData][1] = 0.0;
         mEarthGciPos[mNumData][2] = 0.0;
         
         if (mNeedConversion)
         {
            Rvector6 inState, outState;
            inState.Set(posX[sc], posY[sc], posZ[sc], 0.0, 0.0, 0.0);
            mCoordConverter.Convert(time, inState, mInternalCoordSystem,
                                    outState, mDesiredCoordSystem);
            mScTempPos[sc][mNumData][0] = outState[0];
            mScTempPos[sc][mNumData][1] = outState[1];
            mScTempPos[sc][mNumData][2] = outState[2];
            
            // convert Earth posistion, if desired CS has different origin
            //mEarthTempPos[mNumData][0] = 0.0;
            //mEarthTempPos[mNumData][1] = 0.0;
            //mEarthTempPos[mNumData][2] = 0.0;
         }
         else
         {
            mScTempPos[sc][mNumData][0] = posX[sc];
            mScTempPos[sc][mNumData][1] = posY[sc];
            mScTempPos[sc][mNumData][2] = posZ[sc];
         }
      }
      
      
      //----------------------------------------------------
      // update planet position
      //----------------------------------------------------
      if (mNumData < MAX_DATA)
      {
         for (int body=0; body<MAX_BODIES; body++)
         {
            if (mBodyInUse[body])
            {
               CelestialBody *bodyPtr = mSolarSystem->GetBody(BodyInfo::BODY_NAME[body]);
               
               Rvector6 bodyState = bodyPtr->GetState(time);
               mBodyGciPos[body][mNumData][0] = bodyState[0];
               mBodyGciPos[body][mNumData][1] = bodyState[1];
               mBodyGciPos[body][mNumData][2] = bodyState[2];
               CopyVector3(mBodyTempPos[body][mNumData],
                           mBodyGciPos[body][mNumData]);

               #if DEBUG_TRAJCANVAS_UPDATE
               MessageInterface::ShowMessage
                  ("TrajPlotCanvas::UpdateSpacecraft() %s pos = %f, %f, %f\n",
                   BodyInfo::BODY_NAME[body].c_str(), mBodyTempPos[body][mNumData][0],
                   mBodyTempPos[body][mNumData][1], mBodyTempPos[body][mNumData][2]);
               #endif

               //do conversion later
               
            }
         }
      }
      
      mNumData++;
   }
   
   if (mUseViewPointInfo)
   {
      ComputeProjection(mNumData-1);
      ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
      SetProjection();
   }
   
   Refresh(false);
}


//---------------------------------------------------------------------------
// void AddBody(wxArrayString &bodyNames, UnsignedIntArray &bodyColors)
//---------------------------------------------------------------------------
void TrajPlotCanvas::AddBody(const wxArrayString &bodyNames,
                             const UnsignedIntArray &bodyColors)
{
   // clear bodies
   for (int i=0; i<MAX_BODIES; i++)
   {
      mBodyInUse[i] = false;
   }
   
   int bodyId;
   for (unsigned int i=0; i<bodyNames.GetCount(); i++)
   {
      bodyId = GetStdBodyId(std::string(bodyNames[i].c_str()));
      
      // body is not a standard body
      if (bodyId == UNKNOWN_BODY)
      {
         mOtherBodyCount++;
         bodyId = LAST_STD_BODY_ID + mOtherBodyCount;
         BodyInfo::BODY_NAME[bodyId] = bodyNames[i];
      }
      
      mBodyInUse[bodyId] = true;
      BodyInfo::BODY_COLOR[bodyId] = bodyColors[i];
         
      #if DEBUG_TRAJCANVAS_BODY
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::AddBody() body=%s, bodyId=%d, color=%d\n",
          BodyInfo::BODY_NAME[bodyId].c_str(), bodyId, BodyInfo::BODY_COLOR[bodyId]);
      #endif
   }
   
   LoadGLTextures();

}


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
   DrawPicture();
   glFlush();
   SwapBuffers();
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
         double x2 = pow(mouseX - mLastMouseX, 2);
         double y2 = pow(mouseY - mLastMouseY, 2);
         double length = sqrt(x2 + y2);
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
   //use FileManager to retrieve texture file name
   FileManager *fm = FileManager::Instance();
   std::string textureFile;
   ILboolean status;
   
#ifdef __WXMSW__
   //--------------------------------------------------
   // always load Earth texture
   //--------------------------------------------------
   if (mBodyTextureIndex[EARTH] == UNINIT_TEXTURE)
   {
      textureFile = fm->GetStringParameter("FULL_EARTH_TEXTURE_FILE");
      status = ilLoadImage((char*)textureFile.c_str());
      
      #if DEBUG_TRAJCANVAS_BODY
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::LoadGLTextures() status of loading %s = %d\n",
             textureFile.c_str(), status);
      #endif
      
      if (status != 1)
         return false;

      mBodyTextureIndex[EARTH] = ilutGLBindTexImage();
   }
   
   //--------------------------------------------------
   // load other standard bodies texture if used
   //--------------------------------------------------
   for (int body=0; body<=MOON; body++)
   {
      if (body == EARTH)
         continue;

      if (mBodyInUse[body] && mBodyTextureIndex[body] == UNINIT_TEXTURE)
      {
         // load texture
         switch (body)
         {
         case SUN:
            textureFile = fm->GetStringParameter("FULL_SUN_TEXTURE_FILE");
            status = ilLoadImage((char*)textureFile.c_str());
            break;
         case MERCURY:
            textureFile = fm->GetStringParameter("FULL_MERCURY_TEXTURE_FILE");
            status = ilLoadImage((char*)textureFile.c_str());
            break;
         case VENUS:
            textureFile = fm->GetStringParameter("FULL_VENUS_TEXTURE_FILE");
            status = ilLoadImage((char*)textureFile.c_str());
            break;
         case MARS:
            textureFile = fm->GetStringParameter("FULL_MARS_TEXTURE_FILE");
            status = ilLoadImage((char*)textureFile.c_str());
            break;
         case JUPITER:
            textureFile = fm->GetStringParameter("FULL_JUPITER_TEXTURE_FILE");
            status = ilLoadImage((char*)textureFile.c_str());
            break;
         case SATURN:
            textureFile = fm->GetStringParameter("FULL_SATURN_TEXTURE_FILE");
            status = ilLoadImage((char*)textureFile.c_str());
            break;
         case URANUS:
            textureFile = fm->GetStringParameter("FULL_URANUS_TEXTURE_FILE");
            status = ilLoadImage((char*)textureFile.c_str());
            break;
         case NEPTUNE:
            textureFile = fm->GetStringParameter("FULL_NEPTUNE_TEXTURE_FILE");
            status = ilLoadImage((char*)textureFile.c_str());
            break;
         case PLUTO:
            textureFile = fm->GetStringParameter("FULL_PLUTO_TEXTURE_FILE");
            status = ilLoadImage((char*)textureFile.c_str());
            break;
         case MOON:
            textureFile = fm->GetStringParameter("FULL_MOON_TEXTURE_FILE");
            status = ilLoadImage((char*)textureFile.c_str());
            break;
         default:
            MessageInterface::ShowMessage
               ("TrajPlotCanvas::LoadGLTextures() Texture file is not supported for body: %s\n",
                BodyInfo::BODY_NAME[body].c_str());
            status = 0;
            break;
         }
                 
         if (status != 1)
         {
            MessageInterface::ShowMessage
               ("TrajPlotCanvas::LoadGLTextures() Unable to load texture file for %s\n"
                "file name:%s\n", BodyInfo::BODY_NAME[body].c_str(), textureFile.c_str());
         }
         else
         {
            mBodyTextureIndex[body] = ilutGLBindTexImage();
         }
      }
   }
   
   return true;
#else
   return false;
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
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_PROJECTION); // first go to projection mode
   glPushMatrix();
   glLoadIdentity();
   SetupWorld();                // set it up
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
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
      
      Real dist = mViewPointLocVector.GetMagnitude();
      
      // compute fov angle
      Real fovRad = 2.0 * GmatMathUtil::ATan(size/2.0, dist) *
         GmatMathUtil::DEG_PER_RAD;
      
      //MessageInterface::ShowMessage
      //   ("size=%f, dist=%f, fov=%f\n", size, dist, fovRad);
      
      gluPerspective(fovRad, aspect, 1.0, mAxisLength*2);
   }
   else
   {
      // Setup how we view the world
      glOrtho(mfLeftPos, mfRightPos, mfBottomPos, mfTopPos, mfViewNear,
              mfViewFar);
   }
   
   //Translate Camera
   glTranslatef(mfCamTransX, mfCamTransY, mfCamTransZ);

   //put camera transformations here.
   if (mUseSingleRotAngle)
   {
      glRotatef(mfCamSingleRotAngle, mfCamRotXAxis, mfCamRotYAxis, mfCamRotZAxis);
   }
   else
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
   
   //camera moves opposite direction to center on object
   //this is the point of rotation
   //loj: 4/15/05 Added minus sign to x, y
   switch (mCenterViewBody)
   {
   case EARTH:
      glTranslatef( mEarthTempPos[mNumData-1][0],
                    mEarthTempPos[mNumData-1][1],
                   -mEarthTempPos[mNumData-1][2]);
      break;
   default:
      glTranslatef( mBodyTempPos[mCenterViewBody][mNumData-1][0],
                    mBodyTempPos[mCenterViewBody][mNumData-1][1],
                   -mBodyTempPos[mCenterViewBody][mNumData-1][2]);
      break;
   }
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
   MessageInterface::ShowMessage("ComputeProjection() frame=%d\n", frame);
   #endif

   //-----------------------------------------------------------------
   // get viewpoint reference vector
   //-----------------------------------------------------------------
   Rvector3 vpRefVec(0.0, 0.0, 0.0);

   if (!mUseViewPointRefVector && mViewPointRefObj != NULL)
   {
      #if DEBUG_TRAJCANVAS_PROJ
      MessageInterface::ShowMessage
         ("ComputeProjection() mViewPointRefObj=%d, getting state\n",
          mViewPointRefObj);
      #endif

      // if viewpoint ref. object is a spacecraft, find the index
      if (mViewPointRefObj->GetType() == Gmat::SPACECRAFT)
      {
         mVptRefScId = -1;
         for (int i=0; i<mScCount; i++)
         {
            if (mViewPointRefObj->GetName() == mScNameArray[i])
            {
               mVptRefScId = i;
               break;
            }
         }
         
         // Is this reasonable?
         // if spacecraft name not found, set to use a vector
         if (mVptRefScId == -1)
         {
            mUseViewPointRefVector = true;
            MessageInterface::ShowMessage
               ("TrajPlotCanvas::ComputeProjection() ***** Cannot find "
                "mViewPointRefObj name=%s, so using vector=%s\n",
                mViewPointRefObj->GetName().c_str(), vpRefVec.ToString().c_str());
         }
         else
         {
            vpRefVec.Set(mScTempPos[mVptRefScId][frame][0],
                         mScTempPos[mVptRefScId][frame][1],
                         mScTempPos[mVptRefScId][frame][2]);
         }
      }
      else
      {
         // if valid body id
         if (mVptRefBodyId != -1)
         {
            // for efficiency, body data are computed in UpdateSpacecraft() once.
            if (mBodyHasData[mVptRefBodyId])
            {
               vpRefVec.Set(mBodyTempPos[mVptRefBodyId][frame][0],
                            mBodyTempPos[mVptRefBodyId][frame][1],
                            mBodyTempPos[mVptRefBodyId][frame][2]);
            }
         }
         else
         {
            // If body doesn't have data, then get the state
            Rvector6 vpRefState = mViewPointRefObj->GetMJ2000State(mTime[frame]);
            vpRefVec.Set(vpRefState[0], vpRefState[1], vpRefState[2]);
         }
      }
   }
   
   //-----------------------------------------------------------------
   // get viewpoint vector
   //-----------------------------------------------------------------
   Rvector3 vpVec(mViewPointVector);
   
   if (!mUseViewPointVector && mViewPointVectorObj != NULL)
   {
      #if DEBUG_TRAJCANVAS_PROJ
      MessageInterface::ShowMessage
         ("ComputeProjection() mViewPointVectorObj=%d, type=%d, getting state\n",
          mViewPointVectorObj, mViewPointVectorObj->GetType());
      #endif

      // if viewpoint vector object is a spacecraft, find the index
      if (mViewPointVectorObj->GetType() == Gmat::SPACECRAFT)
      {
         mVptVecScId = -1;
         for (int i=0; i<mScCount; i++)
         {
            if (mViewPointVectorObj->GetName() == mScNameArray[i])
            {
               mVptVecScId = i;
               break;
            }
         }
         
         // Is this reasonable?
         // if spacecraft name not found, set to use a vector
         if (mVptVecScId == -1)
         {
            mUseViewPointVector = true;
            MessageInterface::ShowMessage
               ("TrajPlotCanvas::ComputeProjection() ***** Cannot find "
                "mViewPointVectorObj name=%s, so using vector=%s\n",
                mViewPointVectorObj->GetName().c_str(), vpVec.ToString().c_str());
         }
         else
         {
            vpVec.Set(mScTempPos[mVptVecScId][frame][0],
                      mScTempPos[mVptVecScId][frame][1],
                      mScTempPos[mVptVecScId][frame][2]);
         }
      }
      else
      {
         // if valid body id
         if (mVptVecBodyId != -1)
         {
            // for efficiency, body data are computed in UpdateSpacecraft() once.
            if (mBodyHasData[mVptVecBodyId])
            {
               vpVec.Set(mBodyTempPos[mVptVecBodyId][frame][0],
                         mBodyTempPos[mVptVecBodyId][frame][1],
                         mBodyTempPos[mVptVecBodyId][frame][2]);
            }
         }
         else
         {
            // If body doesn't have data, then get the state
            Rvector6 vpVecState = mViewPointRefObj->GetMJ2000State(mTime[frame]);
            vpVec.Set(vpVecState[0], vpVecState[1], vpVecState[2]);
         }
      }
   }

   //-----------------------------------------------------------------
   // get viewpoint location
   //-----------------------------------------------------------------
   mViewPointLocVector = vpRefVec + (mViewScaleFactor * vpVec);

   //-----------------------------------------------------------------
   // get view direction
   //-----------------------------------------------------------------
   Rvector3 vdVec(mViewDirectionVector);
   
   if (!mUseViewDirectionVector && mViewDirectionObj != NULL)
   {
      #if DEBUG_TRAJCANVAS_PROJ
      MessageInterface::ShowMessage
         ("ComputeProjection() mViewDirectionObj=%d, TypeName=%s, getting state\n",
          mViewDirectionObj, mViewDirectionObj->GetTypeName().c_str());
      #endif

      if (mViewDirectionObj->GetName() == "Earth")
      {
         vdVec = -mViewPointLocVector;
      }
      else if (mViewDirectionObj->GetType() == Gmat::SPACECRAFT)
      {
         // if view direction object is a spacecraft, find the index
         mVdirScId = -1;
         for (int i=0; i<mScCount; i++)
         {
            if (mViewDirectionObj->GetName() == mScNameArray[i])
            {
               mVdirScId = i;
               break;
            }
         }
         
         // Is this reasonable?
         // if spacecraft name not found, set to use a vector
         if (mVdirScId == -1)
         {
            mUseViewDirectionVector = true;
            MessageInterface::ShowMessage
               ("TrajPlotCanvas::ComputeProjection() ***** Cannot find "
                "mViewDirectionObj name=%s, so using vector=%s\n",
                mViewDirectionObj->GetName().c_str(), vdVec.ToString().c_str());
         }
         else
         {
            vdVec.Set(mScTempPos[mVdirScId][frame][0],
                      mScTempPos[mVdirScId][frame][1],
                      mScTempPos[mVdirScId][frame][2]);
         }
      }
      else
      {
         // if valid body id
         if (mVdirBodyId != -1)
         {
            // for efficiency, body data are computed in UpdateSpacecraft() once.
            if (mBodyHasData[mVdirBodyId])
            {
               vdVec.Set(mBodyTempPos[mVdirBodyId][frame][0],
                         mBodyTempPos[mVdirBodyId][frame][1],
                         mBodyTempPos[mVdirBodyId][frame][2]);
            }
         }
         else
         {
            // If body doesn't have data, then get the state
            Rvector6 vdVecState = mViewDirectionObj->GetMJ2000State(mTime[frame]);
            vdVec.Set(vdVecState[0], vdVecState[1], vdVecState[2]);
         }
      }
   }

   #if DEBUG_TRAJCANVAS_PROJ
   MessageInterface::ShowMessage
      ("ComputeProjection() vpRefVec=%s, vpVec=%s\nmViewPointLocVector=%s, "
       " vdVec=%s\n", vpRefVec.ToString().c_str(), vpVec.ToString().c_str(),
       mViewPointLocVector.ToString().c_str(), vdVec.ToString().c_str());
   #endif

   
   if (mUsePerspectiveMode)
   {
      // set camera location
      mfCamTransX = -mViewPointLocVector[0];
      mfCamTransY = -mViewPointLocVector[1];
      mfCamTransZ = -mViewPointLocVector[2];
   }
   else
   {
      // compute axis length (this tells how far zoom out is)
      mAxisLength = mViewPointLocVector.GetMagnitude();

      // if mAxisLength is too small, set to max zoom in value
      if (mAxisLength < mMaxZoomIn)
         mAxisLength = mMaxZoomIn;
      
      mfCamTransX = 0.0;
      mfCamTransY = 0.0;
      mfCamTransZ = 0.0;
   }
   
   // compute camera rotation angle
   Real vdMag = vdVec.GetMagnitude();
   
   mfCamSingleRotAngle = 
      GmatMathUtil::ACos(-(vdVec[2]/vdMag)) * GmatMathUtil::DEG_PER_RAD;
   
   // compute axis of rotation
   mfCamRotXAxis =  vdVec[1];
   mfCamRotYAxis = -vdVec[0];
   mfCamRotZAxis = 0.0;
   mUseSingleRotAngle = true;
   
   #if DEBUG_TRAJCANVAS_PROJ
      MessageInterface::ShowMessage
         ("ComputeProjection() mfCamTransXYZ=%f, %f, %f, mfCamSingleRotAngle=%f\n"
          "mfCamRotXYZ=%f, %f, %f mAxisLength=%f\n", mfCamTransX, mfCamTransY, mfCamTransZ,
          mfCamSingleRotAngle, mfCamRotXAxis, mfCamRotYAxis, mfCamRotZAxis, mAxisLength);
   #endif
}


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
      ("TrajPlotCanvas::DrawFrame() mNumData=%d\n", mNumData);
   #endif
   
   //mUseViewPointInfo = true;
   
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

      if (mUseViewPointInfo)
      {
         ComputeProjection(frame);
         ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
         SetProjection();
      }
      
      // tilt Earth rotation axis if needed
      if (mNeedEarthConversion)
      {
         glPushMatrix();
         TiltEarthZAxis();
      }
   
      // draw earth
      DrawEarthOrbit(frame);

      // draw equatorial plane
      if (mDrawEqPlane)
         DrawEquatorialPlane(mEqPlaneColor);
   
      // draw axes
      if (mDrawAxes)
         DrawAxes(true);
   
      // draw ecliptic plane
      if (mDrawEcPlane)
         DrawEclipticPlane();
   
      if (mNeedEarthConversion)
      {
         glPopMatrix();
      }

      // draw spacecraft orbit
      DrawSpacecraftOrbit(frame);

      // convert other bodies to proper coordinate system if needed
      if (mNeedOtherBodyConversion)
      {
         ConvertOtherBodyData();
         mNeedOtherBodyConversion = false;
      }
   
      // draw other bodies orbit
      for (int body=0; body<MAX_BODIES; body++)
      {
         if (body == EARTH)
            continue;
      
         if (mBodyInUse[body])
            DrawOtherBodyOrbit(body);
      }
   
      // draw Earth-Sun line
      if (mDrawEcLine)
         DrawEarthSunLine();
   
      // draw axes in other coord. system
      if (!mIsInternalCoordSystem)
      {
         if (mDrawAxes)
            DrawAxes();
      }

      SwapBuffers();
   }
} // end DrawFrame()


//------------------------------------------------------------------------------
//  void DrawPicture()
//------------------------------------------------------------------------------
/**
 * Draws whole picture.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawPicture()
{
   #if DEBUG_TRAJCANVAS_DRAW
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::DrawPicture() mNumData=%d\n", mNumData);
   #endif
   
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   mfViewLeft = -mAxisLength/2;
   mfViewRight = mAxisLength/2;
   mfViewTop = mAxisLength/2;
   mfViewBottom = -mAxisLength/2;
   mfViewNear = -mAxisLength/2;
   mfViewFar = mAxisLength/2;

   // tilt Earth rotation axis if needed
   if (mNeedEarthConversion)
   {
      glPushMatrix();
      TiltEarthZAxis();
   }
   
   // draw earth
   DrawEarthOrbit();

   // draw equatorial plane
   if (mDrawEqPlane)
      DrawEquatorialPlane(mEqPlaneColor);
   
   // draw axes
   if (mDrawAxes)
      DrawAxes(true);
   
   // draw ecliptic plane
   if (mDrawEcPlane)
      DrawEclipticPlane();
   
   if (mNeedEarthConversion)
   {
      glPopMatrix();
   }

   // draw spacecraft orbit
   DrawSpacecraftOrbit();

   // convert other bodies to proper coordinate system if needed
   if (mNeedOtherBodyConversion)
   {
      ConvertOtherBodyData();
      mNeedOtherBodyConversion = false;
   }
   
   // draw other bodies orbit
   for (int body=0; body<MAX_BODIES; body++)
   {
      if (body == EARTH)
         continue;
      
      if (mBodyInUse[body])
          DrawOtherBodyOrbit(body);
   }
   
   // draw Earth-Sun line
   if (mDrawEcLine)
      DrawEarthSunLine();
   
   // draw axes in other coord. system
   if (!mIsInternalCoordSystem)
   {
      if (mDrawAxes)
         DrawAxes();
   }
} // end DrawPicture()


//------------------------------------------------------------------------------
//  void DrawEarth()
//------------------------------------------------------------------------------
/**
 * Draws Earth shpere and maps texture image.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawEarth()
{
   if (mCenterViewBody == EARTH)
   {
           
      glColor3f(1.0, 1.0, 1.0);

      if (mUseTexture)
      {
         if (mBodyTextureIndex[EARTH] != UNINIT_TEXTURE)
         {
            glBindTexture(GL_TEXTURE_2D, mBodyTextureIndex[EARTH]);
            glEnable(GL_TEXTURE_2D);
         }
      }
      
      GLUquadricObj* qobj = gluNewQuadric();
      gluQuadricDrawStyle(qobj, GLU_FILL);
      gluQuadricNormals  (qobj, GLU_SMOOTH);
      gluQuadricTexture  (qobj, GL_TRUE);
      gluSphere(qobj, mEarthRadius, 50, 50);
      gluDeleteQuadric(qobj);
      
      glDisable(GL_TEXTURE_2D);
      glMatrixMode(GL_MODELVIEW);
   }
} // end DrawEarth()


//------------------------------------------------------------------------------
//  void DrawEarthOrbit()
//------------------------------------------------------------------------------
/**
 * Draws Earth trajectory and draws Earth at the last point.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawEarthOrbit()
{
   // set color
   *sIntColor = BodyInfo::BODY_COLOR[EARTH];
   glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);

   // Draw Earth trajectory line based on points
   glPushMatrix();
   glBegin(GL_LINES);

   // if current frame is not GCI
   if (mCurrViewFrame != GCI_FRAME)
   {
      for (int i=1; i<mNumData; i++)
      {
         if (mTime[i] > mTime[i-1])
         {
            glVertex3fv(mEarthTempPos[i-1]);
            glVertex3fv(mEarthTempPos[i]);
         }
      }
   }
   glEnd();
   glPopMatrix();
   
   //-------------------------------------------------------
   //draw earth with texture
   //-------------------------------------------------------
   if (mNumData > 0)
   {
      glPushMatrix();
       
      // put earth at final position
      //loj: 4/15/05 Added minus sign to x, y
      glTranslatef(-mEarthTempPos[mNumData-1][0],
                   -mEarthTempPos[mNumData-1][1],
                   mEarthTempPos[mNumData-1][2]);

      DrawEarth();
      glPopMatrix();
   }
   
} // end DrawEarthOrbit()


//------------------------------------------------------------------------------
//  void DrawEarthOrbit(int frame)
//------------------------------------------------------------------------------
/**
 * Draws Earth trajectory and draws Earth at the last point.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawEarthOrbit(int frame)
{
   // set color
   *sIntColor = BodyInfo::BODY_COLOR[EARTH];
   glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);

   // Draw Earth trajectory line based on points
   glPushMatrix();
   glBegin(GL_LINES);

   // if current frame is not GCI
   if (mCurrViewFrame != GCI_FRAME)
   {
      if (mTime[frame] > mTime[frame-1])
      {
         glVertex3fv(mEarthTempPos[frame-1]);
         glVertex3fv(mEarthTempPos[frame]);
      }
   }
   
   glEnd();
   glPopMatrix();
   
   //-------------------------------------------------------
   //draw earth with texture
   //-------------------------------------------------------
   if (mNumData > 0)
   {
      glPushMatrix();
       
      // put earth at current position
      glTranslatef(-mEarthTempPos[frame-1][0],
                   -mEarthTempPos[frame-1][1],
                   mEarthTempPos[frame-1][2]);
      DrawEarth();
      glPopMatrix();
   }
   
} // end DrawEarthOrbit()


//------------------------------------------------------------------------------
//  void DrawOtherBody(int bodyIndex)
//------------------------------------------------------------------------------
/**
 * Draws body shpere and maps texture image.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawOtherBody(int bodyIndex)
{
   //-------------------------------------------------------
   // draw anybody with texture on option
   //-------------------------------------------------------
   if (mNumData > 1 && mBodyTextureIndex[bodyIndex] != UNINIT_TEXTURE)
   {
      glPushMatrix();
      glColor4f(1.0, 1.0, 1.0, 1.0);
      //loj:glColor3f(1.0, 1.0, 1.0);

      // put anybody at final position
      //loj: 4/15/05 Added minus sign to x, y
      glTranslatef(-mBodyTempPos[bodyIndex][mNumData-1][0],
                   -mBodyTempPos[bodyIndex][mNumData-1][1],
                    mBodyTempPos[bodyIndex][mNumData-1][2]);

      glBindTexture(GL_TEXTURE_2D, mBodyTextureIndex[bodyIndex]);
      glEnable(GL_TEXTURE_2D);
      GLUquadricObj* qobj = gluNewQuadric();
      gluQuadricDrawStyle(qobj, GLU_FILL  );
      gluQuadricNormals  (qobj, GLU_SMOOTH);
      gluQuadricTexture  (qobj, GL_TRUE   );
      gluSphere(qobj, mBodyRadius[bodyIndex], 50, 50);
      gluDeleteQuadric(qobj);
   }
   
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);

} // end DrawOtherBody()


//------------------------------------------------------------------------------
//  void DrawOtherBodyOrbit(int bodyIndex)
//------------------------------------------------------------------------------
/**
 * Draws body trajectory and draws body at the last point.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawOtherBodyOrbit(int bodyIndex)
{
   #if DEBUG_TRAJCANVAS_DRAW
      MessageInterface::ShowMessage
         ("TrajPlotCanvas::DrawOtherBodyOrbit() drawing %s\n",
          BodyInfo::BODY_NAME[bodyIndex].c_str());
   #endif
   
   // Draw anybody trajectory line based on points
   glPushMatrix();
   glBegin(GL_LINES);

   // set color
   *sIntColor = BodyInfo::BODY_COLOR[bodyIndex];
   glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);

   //loj: 4/15/05 Added minus sign to x, y
   for (int i=1; i<mNumData; i++)
   {      
      if (mTime[i] >= mTime[i-1])
      {
         glVertex3f((-mBodyTempPos[bodyIndex][i-1][0]),
                    (-mBodyTempPos[bodyIndex][i-1][1]),
                    ( mBodyTempPos[bodyIndex][i-1][2]));
         
         glVertex3f((-mBodyTempPos[bodyIndex][i][0]),
                    (-mBodyTempPos[bodyIndex][i][1]),
                    ( mBodyTempPos[bodyIndex][i][2]));
      }
   }

   glEnd();
   glPopMatrix();

   //-------------------------------------------------------
   // draw anybody with texture on option
   //-------------------------------------------------------
   DrawOtherBody(bodyIndex);
   
} // end DrawOtherBodyOrbit()


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

   glMatrixMode(GL_MODELVIEW);
} // end DrawSpacecraft()


//------------------------------------------------------------------------------
//  void DrawSpacecraftOrbit()
//------------------------------------------------------------------------------
/**
 * Draws spacecraft trajectory and spacecraft at the last point.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawSpacecraftOrbit()
{
   glPushMatrix();
   glBegin(GL_LINES);

   // convert to proper coordinate system if needed
   if (mNeedSpacecraftConversion)
   {
      ConvertSpacecraftData();
      mNeedSpacecraftConversion = false;
      //loj: we need separate flag if we convert other bodies
   }
   
   // assume same number of data points for all spacecrafts
   for (int sc=0; sc<mScCount; sc++)
   {
      // Draw spacecraft orbit line based on points
      for (int i=1; i<mNumData; i++)
      {
         if (mTime[i] > mTime[i-1])
         {
            *sIntColor = mScTrajColor[sc][i];
            glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);

            //loj: 4/15/05 Added -sign to x,y
            glVertex3f((-mScTempPos[sc][i-1][0]),
                       (-mScTempPos[sc][i-1][1]),
                       ( mScTempPos[sc][i-1][2]));
            
            glVertex3f((-mScTempPos[sc][i][0]),
                       (-mScTempPos[sc][i][1]),
                       ( mScTempPos[sc][i][2]));
            
            //glVertex3fv(mScTempPos[sc][i-1]);
            //glVertex3fv(mScTempPos[sc][i]);
         }
      }
   }
   
   glEnd();
   glPopMatrix();


   //-------------------------------------------------------
   //draw spacecraft with texture
   //-------------------------------------------------------
   if (mDrawSpacecraft)
   {
      if (mNumData > 0)
      {
         for (int sc=0; sc<mScCount; sc++)
         {
            glPushMatrix();
            
            // put spacecraft at final position
            //loj: 4/15/05 Added minus sign to x, y
            glTranslatef(-mScTempPos[sc][mNumData-1][0],
                         -mScTempPos[sc][mNumData-1][1],
                          mScTempPos[sc][mNumData-1][2]);

            //MessageInterface::ShowMessage
            //   ("TrajPlotCanvas::DrawSpacecraftOrbit() scColor=%d\n",
            //    mScTrajColor[sc][mNumData-1]);
            
            DrawSpacecraft(mScTrajColor[sc][mNumData-1]);
            glPopMatrix();
         }
      }
   }
} // end DrawSpacecraftOrbit()


//------------------------------------------------------------------------------
//  void DrawSpacecraftOrbit(int frame)
//------------------------------------------------------------------------------
/**
 * Draws spacecraft trajectory and spacecraft at the last point.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawSpacecraftOrbit(int frame)
{
   if (mNeedSpacecraftConversion)
   {
      ConvertSpacecraftData(frame);
   }

   glPushMatrix();
   glBegin(GL_LINES);

   for (int sc=0; sc<mScCount; sc++)
   {
      for (int i=1; i<=frame; i++)
      {
         // Draw spacecraft orbit line based on points
         if (mTime[frame] > mTime[frame-1])
         {
            *sIntColor = mScTrajColor[sc][frame];
            glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);

            glVertex3f((-mScTempPos[sc][i-1][0]),
                       (-mScTempPos[sc][i-1][1]),
                       ( mScTempPos[sc][i-1][2]));
            
            glVertex3f((-mScTempPos[sc][i][0]),
                       (-mScTempPos[sc][i][1]),
                       ( mScTempPos[sc][i][2]));
         }
      }
   }
   
   glEnd();
   glPopMatrix();

   //-------------------------------------------------------
   //draw spacecraft with texture
   //-------------------------------------------------------
   //loj: 4/25/05 Why is spcacecraft rotaing itself?
   
   if (mDrawSpacecraft)
   {
      for (int sc=0; sc<mScCount; sc++)
      {
         glPushMatrix();
         
         // put spacecraft at final position
         glTranslatef(-mScTempPos[sc][frame][0],
                      -mScTempPos[sc][frame][1],
                       mScTempPos[sc][frame][2]);
         
         DrawSpacecraft(mScTrajColor[sc][frame]);
         glPopMatrix();
      }
   }
} // end DrawSpacecraftOrbit(int frame)


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
   double angle;
   
   static const Real RAD_PER_DEG =
      3.14159265358979323846264338327950288419716939937511 / 180.0;
   
   distance = (double)mAxisLength * 5.0;

   glPushMatrix();
   //glLoadIdentity();
   glBegin(GL_LINES);
   
   // set color
   *sIntColor = color;
   //*sIntColor = mEqPlaneColor;
   glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);

   //-----------------------------------
   // draw lines
   //-----------------------------------
   for (i=0; i<360; i+=15)
   {
      angle = RAD_PER_DEG * ((double)i);
      
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
   //glLoadIdentity();
   
   GLUquadricObj *qobj = gluNewQuadric();


   //=================================================================
   // Argosy code
//    double orthoDepth = 10;
//    double ort = OrthoDepth * 8;
//    //int pwr = gile(log10 (ort));
//    int pwr = GmatMathUtil::Floor(GmatMathUtil::Log(ort));
//    //double size = exp10(pwr)/100;
//    double size = GmatMathUtil::Pow(10.0, pwr)/100;
//    int imax = orthoDepth/size;

//    // Draw MAJOR circles
//    SetCelestialColor (GlOptions.EquatorialPlaneColor);
//    for (integer i=1;  i<=imax;  ++i)
//       if (i%10==0)
//          DrawOpenCircle(Vector(0,0,0),Vector(0,0,1),i*size);

//    // Draw minor circles
//    imax = minimum (imax,100);
//    ZColor color = GlOptions.EquatorialPlaneColor;
//    real factor = (size*100)/(ort);
//    color.Red *=factor;
//    color.Green *=factor;
//    color.Blue *=factor;
//    SetCelestialColor (color);
//    for (integer i=1;  i<=imax;  ++i)
//       if (i%10!=0 && (GlOptions.DrawDarkLines || factor > 0.5))
//          DrawOpenCircle(Vector(0,0,0),Vector(0,0,1),i*size);
   //=================================================================

   int maxCircle = (int)(mAxisLength/2000);
   if (maxCircle > 50)
      maxCircle = 50;
   
//    if (mUsePerspectiveMode)
//    {
//       for(i=1; i<maxCircle; i++)
//       {
//          gluQuadricDrawStyle(qobj, GLU_LINE  );
//          gluQuadricNormals  (qobj, GLU_SMOOTH);
//          gluQuadricTexture  (qobj, GL_FALSE  );
      
//          gluDisk(qobj, i*distance/maxCircle, i*distance/maxCircle, 50, 1); // equal distance
//       }
//    }
//    else
//    {
      double radius;
      double distAdd;
      
      for(i=1; i<maxCircle; i++)
      {
         gluQuadricDrawStyle(qobj, GLU_LINE  );
         gluQuadricNormals  (qobj, GLU_SMOOTH);
         gluQuadricTexture  (qobj, GL_FALSE  );
         //distAdd = i*distance/10;
         //distAdd = i*distance/50;
         distAdd = i*distance/maxCircle;
         
         radius = distAdd + (distAdd /100.0 / log10(distAdd) * exp(double(i)));
         gluDisk(qobj, radius, radius, 50, 1);
      }
//    }
   
   gluDeleteQuadric(qobj);
   
   glPopMatrix();
   
} // end DrawEquatorialPlane()


//------------------------------------------------------------------------------
//  void DrawEclipticPlane()
//------------------------------------------------------------------------------
/**
 * Draws ecliptic plane circles.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawEclipticPlane()
{
   // First rotate the grand coordinate system to obliquity of the ecliptic
   // (23.5) and draw equatorial plane
   
   glPushMatrix();
   glRotatef(23.5, -1, 0, 0);
   DrawEquatorialPlane(mEcPlaneColor);
   glPopMatrix();
} // end DrawEclipticPlane()


//------------------------------------------------------------------------------
//  void DrawEarthSunLine()
//------------------------------------------------------------------------------
/**
 * Draws ecliptic plane circles.
 */
//------------------------------------------------------------------------------
void TrajPlotCanvas::DrawEarthSunLine()
{
   int numSkip;
   double sunPos[3], endPos[3];
   double distance = (double)mAxisLength;
   double norm;
   
   glPushMatrix();
   glBegin(GL_LINES);
   
   // set color
   *sIntColor = mEcLineColor;
   glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);
   
   sunPos[0] = mBodyTempPos[SUN][mNumData-1][0];
   sunPos[1] = mBodyTempPos[SUN][mNumData-1][1];
   sunPos[2] = mBodyTempPos[SUN][mNumData-1][2];
   
   //--------------------------------
   // draw sun lines
   //--------------------------------
   numSkip = mNumData/12; // draw 24 lines (12*2)
   
   for (int i=0; i<mNumData; i+=numSkip)
   {      
      //loj: 4/15/05 Added minus sign to x, y
      sunPos[0] = -mBodyTempPos[SUN][i][0];
      sunPos[1] = -mBodyTempPos[SUN][i][1];
      sunPos[2] =  mBodyTempPos[SUN][i][2];
      
      // get sun unit vector and multiply by distance
      norm = sqrt(sunPos[0]*sunPos[0] + sunPos[1]*sunPos[1] + sunPos[2]*sunPos[2]);
      endPos[0] = sunPos[0]/norm * distance;
      endPos[1] = sunPos[1]/norm * distance;
      endPos[2] = sunPos[2]/norm * distance;

      //MessageInterface::ShowMessage("endPos=%g, %g, %g\n", endPos[0], endPos[1], endPos[2]);
      //normalize_vector(sunPos, sunUnitVec);
      //scalar_times_vector(distance, sunUnitVec, 3, endPos);

      //glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(endPos[0], endPos[1], endPos[2]);
      //glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(-endPos[0], -endPos[1], -endPos[2]);
   }
   
   glEnd();
   glPopMatrix();
   
} // end DrawEarthSunLine()


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
// void DrawStringAt(char* inMsg, GLfloat x, GLfloat y, GLfloat z)
//---------------------------------------------------------------------------
void TrajPlotCanvas::DrawStringAt(char* inMsg, GLfloat x, GLfloat y, GLfloat z)
{
   glRasterPos3f(x, y, z);
   glCallLists(strlen(inMsg), GL_BYTE, (GLubyte*)inMsg);
}


//---------------------------------------------------------------------------
// bool TiltEarthZAxis()
//---------------------------------------------------------------------------
bool TrajPlotCanvas::TiltEarthZAxis()
{
   if (mInternalCoordSystem == NULL || mDesiredCoordSystem == NULL)
      return false;
   
   // rotate earth Z axis if desired CS is MJ2000Ec
   if (mDesiredCoordSystem->GetName() == "EarthMJ2000Ec")
   {
      Rvector6 inState, outState;
   
      inState.Set(0.0, 0.0, 1.0, 0.0, 0.0, 0.0);
      mCoordConverter.Convert(mTime[0], inState, mInternalCoordSystem,
                              outState, mDesiredCoordSystem);
      
      #if DEBUG_TRAJCANVAS_CONVERT > 1
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::TiltEarthZAxis() in=%g, %g, %g, out=%g, %g, %g\n",
             inState[0], inState[1], inState[2], outState[0], outState[1], outState[2]);
         Rvector3 vecA(inState[0], inState[1], inState[2]);
         Rvector3 vecB(outState[0], outState[1], outState[2]);
         Real angDeg = AngleUtil::ComputeAngleInDeg(vecA, vecB);
         MessageInterface::ShowMessage
            ("TrajPlotCanvas::TiltEarthZAxis() angDeg=%g\n", angDeg);
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
   if (mInternalCoordSystem == NULL || mDesiredCoordSystem == NULL)
      return false;
   
   Rvector6 inState, outState;
   
   #if DEBUG_TRAJCANVAS_CONVERT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::ConvertSpacecraftData() internalCS=%s, desiredCS=%s\n",
       mInternalCoordSystem->GetName().c_str(), mDesiredCoordSystem->GetName().c_str());
   #endif
   
   // do not convert if desired CS is internal CS
   //if (mDesiredCoordSystem->GetName() == mInternalCoordSystem->GetName())
   if (mIsInternalCoordSystem)
   {
      for (int sc=0; sc<mScCount; sc++)
      {
         for (int i=0; i<mNumData; i++)
         {
            mScTempPos[sc][i][0] = mScGciPos[sc][i][0];
            mScTempPos[sc][i][1] = mScGciPos[sc][i][1];
            mScTempPos[sc][i][2] = mScGciPos[sc][i][2];
         }
      }
   }
   else
   {
      for (int sc=0; sc<mScCount; sc++)
      {
         for (int i=0; i<mNumData; i++)
         {
            inState.Set(mScGciPos[sc][i][0], mScGciPos[sc][i][1], mScGciPos[sc][i][2],
                        0.0, 0.0, 0.0);
            
            mCoordConverter.Convert(mTime[i], inState, mInternalCoordSystem,
                                    outState, mDesiredCoordSystem);
            
            mScTempPos[sc][i][0] = outState[0];
            mScTempPos[sc][i][1] = outState[1];
            mScTempPos[sc][i][2] = outState[2];
            
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
   if (mInternalCoordSystem == NULL || mDesiredCoordSystem == NULL)
      return false;
   
   Rvector6 inState, outState;
   
   #if DEBUG_TRAJCANVAS_CONVERT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::ConvertSpacecraftData() internalCS=%s, desiredCS=%s\n",
       mInternalCoordSystem->GetName().c_str(), mDesiredCoordSystem->GetName().c_str());
   #endif
   
   // do not convert if desired CS is internal CS
   //if (mDesiredCoordSystem->GetName() == mInternalCoordSystem->GetName())
   if (mIsInternalCoordSystem)
   {
      for (int sc=0; sc<mScCount; sc++)
      {
         mScTempPos[sc][frame][0] = mScGciPos[sc][frame][0];
         mScTempPos[sc][frame][1] = mScGciPos[sc][frame][1];
         mScTempPos[sc][frame][2] = mScGciPos[sc][frame][2];
      }
   }
   else
   {
      for (int sc=0; sc<mScCount; sc++)
      {
         inState.Set(mScGciPos[sc][frame][0], mScGciPos[sc][frame][1],
                     mScGciPos[sc][frame][2],
                     0.0, 0.0, 0.0);
         
         mCoordConverter.Convert(mTime[frame], inState, mInternalCoordSystem,
                                 outState, mDesiredCoordSystem);
         
         mScTempPos[sc][frame][0] = outState[0];
         mScTempPos[sc][frame][1] = outState[1];
         mScTempPos[sc][frame][2] = outState[2];
         
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
// bool ConvertOtherBodyData()
//---------------------------------------------------------------------------
bool TrajPlotCanvas::ConvertOtherBodyData()
{
   if (mInternalCoordSystem == NULL || mDesiredCoordSystem == NULL)
      return false;
   
   Rvector6 inState, outState;
   
   #if DEBUG_TRAJCANVAS_CONVERT
   MessageInterface::ShowMessage
      ("TrajPlotCanvas::ConvertOtherBodyData() internalCS=%s, desiredCS=%s\n",
       mInternalCoordSystem->GetName().c_str(), mDesiredCoordSystem->GetName().c_str());
   #endif
   
   // do not convert if desired CS is internal CS
   //if (mDesiredCoordSystem->GetName() == mInternalCoordSystem->GetName())
   if (mIsInternalCoordSystem)
   {
      for (int body=0; body<=MOON; body++)
      {
         if (mBodyHasData[body])
         {
            
            if (body == EARTH)
               continue;
            
            for (int i=0; i<mNumData; i++)
            {
               CopyVector3(mBodyTempPos[body][i], mBodyGciPos[body][i]);
            }
         }
      }
   }
   else
   {
      for (int body=0; body<=MOON; body++)
      {
         if (mBodyHasData[body])
         {
            #if DEBUG_TRAJCANVAS_CONVERT
               MessageInterface::ShowMessage
                  ("TrajPlotCanvas::ConvertOtherBodyData() body=%d\n", body);
            #endif
               
            if (body == EARTH)
               continue;
            
            for (int i=0; i<mNumData; i++)
            {
               inState.Set(mBodyGciPos[body][i][0], mBodyGciPos[body][i][1],
                           mBodyGciPos[body][i][2], 0.0, 0.0, 0.0);
            
               mCoordConverter.Convert(mTime[i], inState, mInternalCoordSystem,
                                       outState, mDesiredCoordSystem);
            
               mBodyTempPos[body][i][0] = outState[0];
               mBodyTempPos[body][i][1] = outState[1];
               mBodyTempPos[body][i][2] = outState[2];
            
               #if DEBUG_TRAJCANVAS_CONVERT > 1
                  MessageInterface::ShowMessage
                     ("TrajPlotCanvas::ConvertOtherBodyData() in=%g, %g, %g, "
                      "out=%g, %g, %g\n", inState[0], inState[1], inState[2],
                      outState[0], outState[1], outState[2]);
               #endif
            }
         }
      }
   }
   
   return true;
}


//---------------------------------------------------------------------------
//  void CopyVector3(float to[3], double from[3])
//---------------------------------------------------------------------------
void TrajPlotCanvas::CopyVector3(float to[3], double from[3])
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
//  void CopyVector3(double to[3], double from[3])
//---------------------------------------------------------------------------
void TrajPlotCanvas::CopyVector3(double to[3], double from[3])
{
   to[0] = from[0];
   to[1] = from[1];
   to[2] = from[2];
}


//---------------------------------------------------------------------------
//  void CopyVector3(double to[3], float from[3])
//---------------------------------------------------------------------------
void TrajPlotCanvas::CopyVector3(double to[3], float from[3])
{
   to[0] = from[0];
   to[1] = from[1];
   to[2] = from[2];
}

