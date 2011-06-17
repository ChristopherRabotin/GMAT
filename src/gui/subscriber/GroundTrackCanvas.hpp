//$Id$
//------------------------------------------------------------------------------
//                              GroundTrackCanvas
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun
// Created: 2011/06/06
/**
 * Declares GroundTrackCanvas for drawing ground track plot using OpenGL.
 */
//------------------------------------------------------------------------------
#ifndef GroundTrackCanvas_hpp
#define GroundTrackCanvas_hpp

#include "ViewCanvas.hpp"
#include "gmatwxdefs.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "MdiGlPlotData.hpp"
#include "GuiInterpreter.hpp"
#include "TextTrajectoryFile.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
// #include "CoordinateConverter.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "GLStars.hpp"

#include <map>

class GroundTrackCanvas: public ViewCanvas
{
public:
   GroundTrackCanvas(wxWindow *parent, const wxWindowID id = -1,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize, 
                     const wxString& name = wxT("GroundTrackCanvas"),
                     long style = 0);
   virtual ~GroundTrackCanvas();
   
   // getters
   bool  GetUseViewPointInfo() { return mUseInitialViewPoint; }
   bool  IsAnimationRunning() { return mIsAnimationRunning; }
   
   float GetDistance() { return mAxisLength; }
   int GetAnimationUpdateInterval() { return mUpdateInterval; }
   int GetAnimationFrameIncrement() { return mFrameInc; }
   wxString GetViewCoordSysName() { return mViewCoordSysName; }
   CoordinateSystem* GetViewCoordSystem() { return pViewCoordSystem; }
   const wxArrayString& GetObjectNames() { return mObjectNames; }
   const wxArrayString& GetValidCSNames() { return mValidCSNames; }
   const wxStringBoolMap& GetShowObjectMap() { return mShowObjectMap; }
   const wxStringColorMap& GetObjectColorMap() { return mObjectColorMap; }
   
   // setters
   void SetEndOfRun(bool flag = true);
   void SetEndOfData(bool flag = true) { mIsEndOfData = flag; }
   void SetDistance(float dist) { mAxisLength = dist; }
   void SetUseInitialViewDef(bool flag) { mUseInitialViewPoint = flag; }
   void SetAnimationUpdateInterval(int value) { mUpdateInterval = value; }
   void SetAnimationFrameIncrement(int value) { mFrameInc = value; }
   
   void SetObjectColors(const wxStringColorMap &objectColorMap);
   void SetShowObjects(const wxStringBoolMap &showObjMap);
   void SetUserInterrupt() { mHasUserInterrupted = true; }
   
   void SetGl2dDrawingOption(const std::string &centralBodyName,
                        const std::string &textureMap,
                        Integer footPrintOption);
   
   
   // actions
   void ClearPlot();
   void RedrawPlot(bool viewAnimation);
   void ShowDefaultView();
   void ZoomIn();
   void ZoomOut();
   void DrawWireFrame(bool flag);
      
   void OnDrawGrid(bool flag);
   void DrawInOtherCoordSystem(const wxString &csName);
   void ViewAnimation(int interval, int frameInc = 30);
   
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
   
   // drawing option
   float mAxisLength;
   
   // central body
   std::string mCentralBodyName;
   std::string mCentralBodyTextureFile;
   
   // foot print option
   Integer mFootPrintOption;
   
   // light source
   bool mSunPresent;
   bool mEnableLightSource;
   
   // mouse rotating
   bool mRotateXy;
   bool mRotateEarthToEnd;
   Real lastLongitudeD;
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
   
   // computed viewpoint
   Rvector3 mVpLocVec;
   Rvector3 mVpRefVec;
   Rvector3 mVpVec;
   Rvector3 mVdVec;
   Rvector3 mVcVec;
   Rvector3 mUpVec;
      
   // object rotation
   Real mInitialLongitude;
   Real mInitialMha;
   Real mFinalLongitude;
   Real mFinalMha;
   Real mFinalLst;
   
   // spacecraft
   GLuint mGlList;
      
   // earth
   float mEarthRadius;
   
   // view
   wxSize  mCanvasSize;
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
   
   // view point
   void SetDefaultViewPoint();
   void InitializeViewPoint();
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
   void DrawObjectOrbit(int frame);
   void DrawObjectTexture(const wxString &objName, int obj, int objId, int frame);
   void DrawObject(const wxString &objName, int obj);
   void DrawOrbit(const wxString &objName, int obj, int objId);
   void DrawOrbitLines(int i, const wxString &objName, int obj, int objId);
   void DrawGroundTrackLines(Rvector3 &r1, Rvector3 &v1,
                             Rvector3 &r2, Rvector3 &v2);
   void DrawSolverData();
   
   void DrawCentralBodyTexture();
   
   // for rotation
   void RotateBodyUsingAttitude(const wxString &objName, int objId);
   void RotateBody(const wxString &objName, int frame, int objId);
      
   // for coordinate system
   bool ConvertObjectData();
   void ConvertObject(int objId, int index);
   
   // for debug
   void DrawDebugMessage(const wxString &msg, unsigned int textColor, int xpos, int ypos);
   
   // Linux specific fix
   #ifdef __WXGTK__
      bool hasBeenPainted;
   #endif
};

#endif
