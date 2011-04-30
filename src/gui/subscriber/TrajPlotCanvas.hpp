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
 * Declares TrajPlotCanvas for opengl plot.
 */
//------------------------------------------------------------------------------
#ifndef TrajPlotCanvas_hpp
#define TrajPlotCanvas_hpp

#include "ViewCanvas.hpp"
#include "gmatwxdefs.hpp"
#include "MdiGlPlotData.hpp"
#include "GuiInterpreter.hpp"
#include "TextTrajectoryFile.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"

#include <map>

class TrajPlotCanvas: public ViewCanvas
{
public:
   TrajPlotCanvas(wxWindow *parent, const wxWindowID id = -1,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize, 
                  const wxString& name = wxT("TrajPlotCanvas"),
                  long style = 0);
   virtual ~TrajPlotCanvas();
   
   // initialization
   bool InitGL();
   
   // getters
   bool  GetUseViewPointInfo() { return mUseInitialViewPoint; }
   bool  GetUsePerspectiveMode() { return mUsePerspectiveMode; }
   bool  GetDrawWireFrame() { return mDrawWireFrame; }
   bool  GetDrawXyPlane() { return mDrawXyPlane; }
   bool  GetDrawEcPlane() { return mDrawEcPlane; }
   bool  GetDrawSunLine() { return mDrawSunLine; }
   bool  GetDrawAxes() { return mDrawAxes; }
   bool  GetDrawGrid() { return mDrawGrid; }
   bool  GetRotateAboutXY() { return mRotateXy; }
   bool  IsAnimationRunning() { return mIsAnimationRunning; }
   unsigned int GetXyPlaneColor() { return mXyPlaneColor; }
   unsigned int GetEcPlaneColor() { return mEcPlaneColor; }
   unsigned int GetSunLineColor() { return mSunLineColor; }
   float GetDistance() { return mAxisLength; }
   int GetAnimationUpdateInterval() { return mUpdateInterval; }
   int GetAnimationFrameIncrement() { return mFrameInc; }
   wxString GetViewCoordSysName() { return mViewCoordSysName; }
   CoordinateSystem* GetViewCoordSystem() { return pViewCoordSystem; }
   const wxArrayString& GetObjectNames() { return mObjectNames; }
   const wxArrayString& GetValidCSNames() { return mValidCSNames; }
   const wxStringBoolMap& GetShowObjectMap() { return mShowObjectMap; }
   const wxStringColorMap& GetObjectColorMap() { return mObjectColorMap; }
   wxString GetGotoObjectName();
   wxGLContext* GetGLContext();
   
   // setters
   void SetEndOfRun(bool flag = true);
   void SetEndOfData(bool flag = true) { mIsEndOfData = flag; }
   void SetDistance(float dist) { mAxisLength = dist; }
   void SetUseInitialViewDef(bool flag) { mUseInitialViewPoint = flag; }
   void SetAnimationUpdateInterval(int value) { mUpdateInterval = value; }
   void SetAnimationFrameIncrement(int value) { mFrameInc = value; }
   void SetDrawWireFrame(bool flag) { mDrawWireFrame = flag; }
	void SetDrawStars(bool flag) {}
	void SetDrawConstellations(bool flag) {}
	void SetStarCount(int count) {}
   void SetDrawXyPlane(bool flag) { mDrawXyPlane = flag; }
   void SetDrawEcPlane(bool flag) { mDrawEcPlane = flag; }
   void SetDrawSunLine(bool flag) { mDrawSunLine = flag; }
   void SetDrawAxes(bool flag) { mDrawAxes = flag; }
   void SetDrawGrid(bool flag) { mDrawGrid = flag; }
   void SetRotateAboutXY(bool flag) { mRotateXy = flag; }
   void SetXyPlaneColor(unsigned int color) { mXyPlaneColor = color; }
   void SetEcPlaneColor(unsigned int color) { mEcPlaneColor = color; }
   void SetSunLineColor(unsigned int color) { mSunLineColor = color; }
   void SetUsePerspectiveMode(bool perspMode);
   void SetObjectColors(const wxStringColorMap &objectColorMap);
   void SetShowObjects(const wxStringBoolMap &showObjMap);
   void SetShowOrbitNormals(const wxStringBoolMap &showOrbitNormalMap);
   void SetGLContext(wxGLContext *glContext = NULL);
   void SetUserInterrupt() { mHasUserInterrupted = true; }
   
   // actions
   void ClearPlot();
   void ResetPlotInfo();
   void RedrawPlot(bool viewAnimation);
   void ShowDefaultView();
   void RotatePlot(int width, int height, int mouseX, int mouseY);
   void ZoomIn();
   void ZoomOut();
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
   
   // viewpoint
   void SetGlViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                        SpacePoint *vdObj, Real vscaleFactor,
                        const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                        const Rvector3 &vdVec, const std::string &upAxis,
                        bool usevpRefVec, bool usevpVec, bool usevdVec,
                        bool useFixedFov, Real fov);
   
   // drawing toggle switch
   void SetGlDrawOrbitFlag(const std::vector<bool> &drawArray);
   void SetGlShowObjectFlag(const std::vector<bool> &showArray);
   
   // performance
   void SetUpdateFrequency(Integer updFreq);
   void SetNumPointsToRedraw(Integer numPoints);
   
   // update
   void UpdatePlot(const StringArray &scNames, const Real &time,
                   const RealArray &posX, const RealArray &posY,
                   const RealArray &posZ, const RealArray &velX,
                   const RealArray &velY, const RealArray &velZ,
                   const UnsignedIntArray &scColors, bool solving,
                   Integer solverOption);
   
   void TakeAction(const std::string &action);
   
   // object
   void AddObjectList(const wxArrayString &objNames,
                      const UnsignedIntArray &objColors,
                      bool clearList = true);
   
   // file
   int  ReadTextTrajectory(const wxString &filename);

protected:
   
   // events
   void OnPaint(wxPaintEvent &event);
   void OnTrajSize(wxSizeEvent &event);
   void OnMouse(wxMouseEvent &event);
   void OnKeyDown(wxKeyEvent &event);
   
   DECLARE_EVENT_TABLE();

private:
   
   static const int MAX_DATA = 20000;
   static const int MAX_OBJECT = 50;
   static const int LAST_STD_BODY_ID;// = 10;
   static const int MAX_COORD_SYS;// = 10;
   static const std::string BODY_NAME[GmatPlot::MAX_BODIES];
   static const float MAX_ZOOM_IN;// = 3700.0;
   static const float RADIUS_ZOOM_RATIO;// = 2.2;
   static const float DEFAULT_DIST;// = 30000.0;
   static const int UNKNOWN_OBJ_ID;// = -999;
   
   // initialization
   wxWindow *mParent;
   bool mGlInitialized;
   wxString mPlotName;
   
   GuiInterpreter *theGuiInterpreter;
   wxStatusBar *theStatusBar;
   TextTrajectoryFile *mTextTrajFile;
   TrajectoryArray mTrajectoryData;
   
   // Data members used by the mouse
   GLfloat mfStartX, mfStartY;
   
   // Actual params of the window
   GLfloat mfLeftPos, mfRightPos, mfBottomPos, mfTopPos;
   
   // Camera rotations
   GLfloat mfCamRotXAngle, mfCamRotYAngle, mfCamRotZAngle;
   GLfloat mfCamSingleRotAngle, mfCamRotXAxis, mfCamRotYAxis, mfCamRotZAxis;
   bool mUseSingleRotAngle;
   
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
   bool mDrawEclipticPlane;
   bool mDrawSunLine;
   bool mDrawAxes;
   bool mDrawGrid;
   bool mDrawOrbitNormal;
   
   // color
   unsigned int mXyPlaneColor;
   unsigned int mEcPlaneColor;
   unsigned int mSunLineColor;
   
   // texture
   std::map<wxString, GLuint> mObjectTextureIdMap;

   // light source
   bool mSunPresent;
   bool mEnableLightSource;
   
   // mouse rotating
   bool mRotateXy;
   bool mRotateAboutXaxis;
   bool mRotateAboutYaxis;
   bool mRotateAboutZaxis;
   bool mRotateEarthToEnd;
   Real lastLongitudeD;
   float mQuat[4];     // orientation of object
   
   // zooming
   int   mLastMouseX;
   int   mLastMouseY;
   float mZoomAmount;
   float mMaxZoomIn;
   
   // projection
   bool mUsePerspectiveMode;
   Real mFovDeg;
   
   // initial viewpoint
   StringArray mScNameArray;
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
   Real mFixedFovAngle;
   
   bool mUseInitialViewPoint;
   bool mUseFixedFov;
   bool mUseViewPointRefVector;
   bool mUseViewPointVector;
   bool mUseViewDirectionVector;
   bool mCanRotateBody;
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
   
   // data
   int  mNumData;
   int  mTotalPoints;
   int  mCurrIndex;
   int  mBeginIndex1;
   int  mBeginIndex2;
   int  mEndIndex1;
   int  mEndIndex2;
   int  mRealBeginIndex1;
   int  mRealBeginIndex2;
   int  mRealEndIndex1;
   int  mRealEndIndex2;
   int  mLastIndex;
   bool mIsEndOfData;
   bool mIsEndOfRun;
   bool mIsFirstRun;
   bool mWriteWarning;
   
   // time
   Real mTime[MAX_DATA];
   Real mFinalTime;
   
   // object rotation
   Real mInitialLongitude;
   Real mInitialMha;
   Real mFinalLongitude;
   Real mFinalMha;
   Real mFinalLst;
   
   // spacecraft
   int   mScCount;
   float mScRadius;
   GLuint mGlList;
   
   // solar system
   SolarSystem *pSolarSystem;
   
   // earth
   float mEarthRadius;
   
   // objects
   wxArrayString mObjectNames;
   wxArrayString mShowObjectNames;
   wxArrayString mValidCSNames;
   wxStringColorMap mObjectColorMap;
   wxStringBoolMap  mShowObjectMap;
   wxStringBoolMap  mShowOrbitNormalMap;
   std::vector<SpacePoint*> mObjectArray;
   std::vector<bool> mDrawOrbitArray;
   std::vector<bool> mShowObjectArray;
   int mObjectCount;
   
   Real mObjectDefaultRadius;
   
   Real *mObjectRadius;            // [mObjectCount]
   Real *mObjMaxZoomIn;            // [mObjectCount]
   int  *mObjLastFrame;            // [mObjectCount]
   bool *mDrawOrbitFlag;           // [mObjectCount][MAX_DATA]
   UnsignedInt *mObjectOrbitColor; // [mObjectCount][MAX_DATA];
   
   // run data
   Real *mObjectGciPos;            // [mObjectCount][MAX_DATA][3];
   Real *mObjectGciVel;            // [mObjectCount][MAX_DATA][3];
   Real *mObjectViewPos;           // [mObjectCount][MAX_DATA][3];
   Real *mObjectViewVel;           // [mObjectCount][MAX_DATA][3];
   
   // solver data
   bool mDrawSolverData;
   std::vector<RealArray> mSolverAllPosX; // [numPoints][numSC]
   std::vector<RealArray> mSolverAllPosY; // [numPoints][numSC]
   std::vector<RealArray> mSolverAllPosZ; // [numPoints][numSC]
   UnsignedIntArray mSolverIterColorArray;
   
   // coordinate system
   wxString mInternalCoordSysName;
   wxString mViewCoordSysName;
   wxString mViewUpCoordSysName;
   wxString mOriginName;
   CoordinateSystem *pInternalCoordSystem;
   CoordinateSystem *pViewCoordSystem;
   CoordinateSystem *pViewUpCoordSystem;
   int mOriginId;
   
   // coordinate sytem conversion
   bool mViewCsIsInternalCs;
   CoordinateConverter mCoordConverter;
   
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
   
   // animation
   bool mIsAnimationRunning;
   bool mHasUserInterrupted;
   int  mUpdateInterval;
   int  mFrameInc;
   
   // message
   bool mShowMaxWarning;
   int  mOverCounter;
   
   // windows specific functions
   bool SetPixelFormatDescriptor();
   void SetDefaultGLFont();
   
   // initialize
   void InitializeViewPoint();
   
   // for data buffer indexing
   void ComputeActualIndex();
   
   // texture
   bool LoadGLTextures();
   GLuint BindTexture(SpacePoint *obj, const wxString &objName);
   void SetDefaultView();
   
   // view objects
   void SetProjection();
   void SetupWorld();
   void ComputeView(GLfloat fEndX, GLfloat fEndY);
   void ChangeView(float viewX, float viewY, float viewZ);
   void ChangeProjection(int width, int height, float axisLength);
   void ComputeViewVectors();
   void ComputeUpAngleAxis();
   void TransformView();
   
   // drawing objects
   void DrawFrame();
   void DrawPlot();
   void DrawSphere(GLdouble radius, GLint slices, GLint stacks, GLenum style,
                   GLenum orientation = GLU_OUTSIDE, GLenum normals = GL_SMOOTH,
                   GLenum textureCoords = GL_TRUE);
   void DrawObject(const wxString &objName);
   void DrawObjectOrbit();
   void DrawOrbit(const wxString &objName, int obj, int objId);
   void DrawOrbitLines(int i, const wxString &objName, int obj, int objId);
   void DrawOrbitNormal(const wxString &objName, int obj, int objId);
   void DrawOrbitNormalLines(int i, const wxString &objName, int obj, int objId);
   void DrawObjectTexture(const wxString &objName, int obj, int objId);
   void DrawSolverData();
   void DrawObjectOrbitNormal(int objId, int frame, UnsignedInt color);
   void DrawSpacecraft(UnsignedInt scColor);
   void DrawEquatorialPlane(UnsignedInt color);
   void DrawEclipticPlane(UnsignedInt color);
   void DrawSunLine();
   void DrawAxes();
   void DrawStatus(const wxString &label1, int frame, const wxString &label2,
                   double time, int xpos = 0, int ypos = 0,
                   const wxString &label3 = "");
   
   // for rotation
   void ApplyEulerAngles();
   
   // drawing primative objects
   void DrawStringAt(const wxString &str, GLfloat x, GLfloat y, GLfloat z);
   void DrawCircle(GLUquadricObj *qobj, Real radius);
   
   // for object
   int  GetObjectId(const wxString &name);
   void ClearObjectArrays(bool deleteArrays = true);
   bool CreateObjectArrays();
   
   // for data update
   void UpdateSolverData(const RealArray &posX, const RealArray &posY,
                         const RealArray &posZ, const UnsignedIntArray &scColors,
                         bool solving);
   void UpdateSpacecraftData(const Real &time,
                             const RealArray &posX, const RealArray &posY,
                             const RealArray &posZ, const RealArray &velX,
                             const RealArray &velY, const RealArray &velZ,
                             const UnsignedIntArray &scColors, Integer solverOption);
   void UpdateOtherData(const Real &time);
   
   // for coordinate system
   bool TiltOriginZAxis();
   void UpdateRotateFlags();
   bool ConvertObjectData();
   void ConvertObject(int objId, int index);
   
   // for utility
   Rvector3 ComputeEulerAngles();
   void ComputeLongitudeLst(Real time, Real x, Real y, Real *meanHourAngle,
                            Real *longitude, Real *localSiderealTime);
   
   // for copy
   void CopyVector3(Real to[3], Real from[3]);
   // for loading image
   bool LoadImage(const std::string &fileName);
   
   // Linux specific fix
   #ifdef __WXGTK__
      bool hasBeenPainted;
   #endif
};

#endif
