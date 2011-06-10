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
// Author: Linda Jun
// Created: 2003/11/25
/**
 * Declares OrbitViewCanvas for opengl plot.
 */
//------------------------------------------------------------------------------
#ifndef OrbitViewCanvas_hpp
#define OrbitViewCanvas_hpp

#include "ViewCanvas.hpp"
#include "gmatwxdefs.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "MdiGlPlotData.hpp"
#include "GuiInterpreter.hpp"
//#include "TextTrajectoryFile.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "GLStars.hpp"

#include <map>

class OrbitViewCanvas: public ViewCanvas
{
public:
   OrbitViewCanvas(wxWindow *parent, const wxWindowID id = -1,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize, 
                  const wxString& name = wxT("OrbitViewCanvas"),
                  long style = 0);
   virtual ~OrbitViewCanvas();
   
   // getters
   int GetAnimationUpdateInterval() { return mUpdateInterval; }
   int GetAnimationFrameIncrement() { return mFrameInc; }
   bool  IsAnimationRunning() { return mIsAnimationRunning; }
   const wxStringBoolMap& GetShowObjectMap() { return mShowObjectMap; }
   const wxStringColorMap& GetObjectColorMap() { return mObjectColorMap; }
   
   bool  GetUseViewPointInfo() { return mUseInitialViewPoint; }
   bool  GetDrawWireFrame() { return mDrawWireFrame; }
   bool  GetDrawXyPlane() { return mDrawXyPlane; }
   bool  GetDrawEcPlane() { return mDrawEcPlane; }
   bool  GetDrawSunLine() { return mDrawSunLine; }
   bool  GetDrawAxes() { return mDrawAxes; }
   bool  GetDrawGrid() { return mDrawGrid; }
   unsigned int GetXyPlaneColor() { return mXyPlaneColor; }
   unsigned int GetEcPlaneColor() { return mEcPlaneColor; }
   unsigned int GetSunLineColor() { return mSunLineColor; }
   float GetDistance() { return mAxisLength; }
   wxString GetViewCoordSysName() { return mViewCoordSysName; }
   CoordinateSystem* GetViewCoordSystem() { return pViewCoordSystem; }
   const wxArrayString& GetObjectNames() { return mObjectNames; }
   const wxArrayString& GetValidCSNames() { return mValidCSNames; }
   wxString GetGotoObjectName();
   
   // setters
   void SetUserInterrupt() { mHasUserInterrupted = true; }
   void SetEndOfRun(bool flag = true);
   void SetEndOfData(bool flag = true) { mIsEndOfData = flag; }
   void SetAnimationUpdateInterval(int value) { mUpdateInterval = value; }
   void SetAnimationFrameIncrement(int value) { mFrameInc = value; }
   void SetObjectColors(const wxStringColorMap &objectColorMap);
   void SetShowObjects(const wxStringBoolMap &showObjMap);
   
   void SetDistance(float dist) { mAxisLength = dist; }
   void SetUseInitialViewDef(bool flag) { mUseInitialViewPoint = flag; }
   void SetDrawWireFrame(bool flag) { mDrawWireFrame = flag; }
   void SetDrawStars(bool flag) { mDrawStars = flag; }
   void SetDrawConstellations(bool flag) { mDrawConstellations = flag; }
   void SetStarCount(int count) { mStarCount = count; }
   void SetDrawXyPlane(bool flag) { mDrawXyPlane = flag; }
   void SetDrawEcPlane(bool flag) { mDrawEcPlane = flag; }
   void SetDrawSunLine(bool flag) { mDrawSunLine = flag; }
   void SetDrawAxes(bool flag) { mDrawAxes = flag; }
   void SetDrawGrid(bool flag) { mDrawGrid = flag; }
   void SetXyPlaneColor(unsigned int color) { mXyPlaneColor = color; }
   void SetEcPlaneColor(unsigned int color) { mEcPlaneColor = color; }
   void SetSunLineColor(unsigned int color) { mSunLineColor = color; }
   
   // actions
   void ClearPlot();
   void RedrawPlot(bool viewAnimation);
   void ShowDefaultView();
   void DrawWireFrame(bool flag);
   void DrawXyPlane(bool flag);
   void DrawEcPlane(bool flag);
   void OnDrawAxes(bool flag);
   void OnDrawGrid(bool flag);
   void DrawInOtherCoordSystem(const wxString &csName);
   void GotoObject(const wxString &objName);
   void GotoOtherBody(const wxString &bodyName);
   void ViewAnimation(int interval, int frameInc = 30);
   
   void SetGlObject(const StringArray &objNames,
                    const UnsignedIntArray &objOrbitColors,
                    const std::vector<SpacePoint*> &objectArray);
   
   // SolarSystem
   void SetSolarSystem(SolarSystem *ss);
   
   // CoordinateSystem
   void SetGlCoordSystem(CoordinateSystem *internalCs,
                         CoordinateSystem *viewCs,
                         CoordinateSystem *viewUpCs);
   
   // drawing options
   void SetGl3dDrawingOption(bool drawEcPlane, bool drawXyPlane,
                             bool drawWireFrame, bool drawAxes,
                             bool drawGrid, bool drawSunLine,
                             bool usevpInfo, bool drawStars,
                             bool drawConstellations, Integer starCount);
   
   // viewpoint
   void SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                          SpacePoint *vdObj, Real vscaleFactor,
                          const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                          const Rvector3 &vdVec, const std::string &upAxis,
                          bool usevpRefVec, bool usevpVec, bool usevdVec);
   
   // drawing toggle switch
   void SetGlDrawOrbitFlag(const std::vector<bool> &drawArray);
   void SetGlShowObjectFlag(const std::vector<bool> &showArray);
   
   // performance
   void SetUpdateFrequency(Integer updFreq);
   void SetNumPointsToRedraw(Integer numPoints);
   
   // user actions   
   void TakeAction(const std::string &action);
      
protected:
   
   // events
   void OnPaint(wxPaintEvent &event);
   void OnSize(wxSizeEvent &event);
   void OnMouse(wxMouseEvent &event);
   void OnKeyDown(wxKeyEvent &event);
   
   DECLARE_EVENT_TABLE();

private:
   
   static const int LAST_STD_BODY_ID;// = 10;
   static const int MAX_COORD_SYS;// = 10;
   static const std::string BODY_NAME[GmatPlot::MAX_BODIES];
   static const Real MAX_ZOOM_IN;// = 3700.0;
   static const Real RADIUS_ZOOM_RATIO;// = 2.2;
   static const Real DEFAULT_DIST;// = 30000.0;
      
   // stars and options
   GLStars *mStars;
   int mStarCount;
   bool mDrawStars;
   bool mDrawConstellations;
      
   // camera
   Camera mCamera;
   
   // light source
   Light mLight;
   
   // Data members used by the mouse
   GLfloat mfStartX, mfStartY;
   
   // Actual params of the window
   GLfloat mfLeftPos, mfRightPos, mfBottomPos, mfTopPos;
   
   // Camera rotations
   GLfloat mfCamRotXAngle, mfCamRotYAngle, mfCamRotZAngle;
   GLfloat mfCamSingleRotAngle, mfCamRotXAxis, mfCamRotYAxis, mfCamRotZAxis;
   
   // Camera translations
   GLfloat mfCamTransX, mfCamTransY, mfCamTransZ;
   
   // DJC added for "Up"   
   GLfloat mfUpAngle, mfUpXAxis, mfUpYAxis, mfUpZAxis;
   
   // view model
   bool mUseGluLookAt;
   
   // performance
   bool mRedrawLastPointsOnly;
   int  mNumPointsToRedraw;
   int  mUpdateFrequency;
   
   // draw option
   float mAxisLength;
   bool mDrawWireFrame;
   bool mDrawXyPlane;
   bool mDrawEcPlane;
   bool mDrawSunLine;
   bool mDrawAxes;
   bool mDrawGrid;
   
   // color
   unsigned int mXyPlaneColor;
   unsigned int mEcPlaneColor;
   unsigned int mSunLineColor;
   
   // light source
   bool mSunPresent;
   bool mEnableLightSource;
   
   // mouse rotating
   float mQuat[4];     // orientation of object
   
   // zooming
   int   mLastMouseX;
   int   mLastMouseY;
   float mZoomAmount;
   float mMaxZoomIn;
   
   // Control Modes
   int mControlMode;  // 0 = rotate around center, 1 = 6DOF movement, 2 = 6DOF movement NASA/Dunn style
   int mInversion;    // 1 is true movement, -1 is inverted
   
   // initial viewpoint
   std::string mViewPointRefObjName;
   std::string mViewUpAxisName;
   
   // passed view definition object pointers
   SpacePoint *pViewPointRefObj;
   SpacePoint *pViewPointVectorObj;
   SpacePoint *pViewDirectionObj;
   
   Rvector3 mViewPointRefVector;
   Rvector3 mViewPointVector;
   Rvector3 mViewDirectionVector;
   Rvector6 mUpState;
   Real mViewScaleFactor;
   
   bool mUseInitialViewPoint;
   bool mUseViewPointRefVector;
   bool mUseViewPointVector;
   bool mUseViewDirectionVector;
   bool mCanRotateAxes;
   
   int mVpRefObjId;
   int mVpVecObjId;
   int mVdirObjId;
   Real mViewObjRadius;
   wxString mViewObjName;
   int mViewObjId;
      
   // object rotation
   Real mInitialLongitude;
   Real mInitialMha;
   Real mFinalLongitude;
   Real mFinalMha;
   Real mFinalLst;
   
   // default spacecraft drawing
   GLuint mGlList;
   
   // earth
   float mEarthRadius;
   
   // view
   GLfloat mfViewLeft;
   GLfloat mfViewRight;
   GLfloat mfViewTop;
   GLfloat mfViewBottom;
   GLfloat mfViewNear;
   GLfloat mfViewFar;
   
   float mDefaultRotXAngle;
   float mDefaultRotYAngle;
   float mDefaultRotZAngle;
   float mDefaultViewDist;
   float mCurrRotXAngle;
   float mCurrRotYAngle;
   float mCurrRotZAngle;
   float mCurrViewDist;
   
   // windows specific functions
   bool SetPixelFormatDescriptor();
   void SetDefaultGLFont();
   
   // view point
   void SetDefaultViewPoint();
   void InitializeViewPoint();
   
   // texture
   bool LoadGLTextures();
   GLuint BindTexture(SpacePoint *obj, const wxString &objName);
   void SetDefaultView();
   
   // view objects
   void SetProjection();
   void SetupWorld();
   void ChangeView(float viewX, float viewY, float viewZ);
   void ChangeProjection(int width, int height, float axisLength);
   void TransformView();
   
   // drawing objects
   void DrawFrame();
   void DrawPlot();
   void DrawObject(const wxString &objName, int obj);
   void DrawObjectOrbit(int frame);
   void DrawOrbit(const wxString &objName, int obj, int objId);
   void DrawOrbitLines(int i, const wxString &objName, int obj, int objId);   
   void DrawObjectTexture(const wxString &objName, int obj, int objId, int frame);
   void DrawSolverData();
   void DrawEquatorialPlane(UnsignedInt color);
   void DrawEclipticPlane(UnsignedInt color);
   void DrawSunLine();
   void DrawAxes();

   // drawing stars
   void DrawStars();
   
   // for rotation
   void RotateBodyUsingAttitude(const wxString &objName, int objId);
   void RotateBody(const wxString &objName, int frame, int objId);
      
   // for data update
   void UpdateSolverData(const RealArray &posX, const RealArray &posY,
                         const RealArray &posZ, const UnsignedIntArray &scColors,
                         bool solving);
   void UpdateSpacecraftData(const Real &time,
                             const RealArray &posX, const RealArray &posY,
                             const RealArray &posZ, const RealArray &velX,
                             const RealArray &velY, const RealArray &velZ,
                             const UnsignedIntArray &scColors, Integer solverOption);
   void UpdateSpacecraftAttitude(Real time, Spacecraft *sat, int satId);
   
   void UpdateOtherData(const Real &time);
   void UpdateOtherObjectAttitude(Real time, SpacePoint *sp, int objId);
   
   // for coordinate system
   void UpdateRotateFlags();
   bool ConvertObjectData();
   void ConvertObject(int objId, int index);
   
   // for utility
   void ComputeLongitudeLst(Real time, Real x, Real y, Real *meanHourAngle,
                            Real *longitude, Real *localSiderealTime);
   
   // Linux specific fix
   #ifdef __WXGTK__
      bool hasBeenPainted;
   #endif
};

#endif
