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
 * Declares TrajPlotCanvas for opengl plot.
 */
//------------------------------------------------------------------------------
#ifndef TrajPlotCanvas_hpp
#define TrajPlotCanvas_hpp

#include "gmatwxdefs.hpp"
#include "MdiGlPlotData.hpp"
#include "GuiInterpreter.hpp"
#include "TextTrajectoryFile.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "Rvector3.hpp"

#include <map>

class TrajPlotCanvas: public wxGLCanvas
{
public:
   TrajPlotCanvas(wxWindow *parent, const wxWindowID id = -1,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize, 
                  const wxString &csName = "", SolarSystem *solarSys = NULL,
                  long style = 0, const wxString& name = wxT("TrajPlotCanvas"));
   ~TrajPlotCanvas();
   
   // initialization
   bool InitGL();
   
   // getters
   bool  GetUseViewPointInfo() {return mUseInitialViewPoint;}
   bool  GetUsePerspectiveMode() {return mUsePerspectiveMode;}
   bool  GetDrawWireFrame() {return mDrawWireFrame;}
   bool  GetDrawXyPlane() {return mDrawXyPlane;}
   bool  GetDrawEcPlane() {return mDrawEcPlane;}
   bool  GetDrawESLines() {return mDrawESLines;}
   bool  GetDrawAxes() {return mDrawAxes;}
   bool  GetDrawGrid() {return mDrawGrid;}
   bool  GetRotateAboutXY() {return mRotateXy;}
   unsigned int GetXyPlaneColor() {return mXyPlaneColor;}
   unsigned int GetEcPlaneColor() {return mEcPlaneColor;}
   unsigned int GetESLineColor() {return mESLinecolor;}
   float GetDistance() {return mAxisLength;}
   int GetAnimationUpdateInterval() {return mUpdateInterval;}
   wxString GetViewCoordSysName() {return mViewCoordSysName;}
   CoordinateSystem* GetViewCoordSystem() { return mViewCoordSystem;}
   const wxArrayString& GetObjectNames() { return mObjectNames;}
   const wxStringColorMap& GetObjectColorMap() { return mObjectColorMap;}
   wxString GetGotoObjectName();
   
   // setters
   void SetEndOfRun(bool flag = true);
   void SetEndOfData(bool flag = true) {mIsEndOfData = flag;}
   void SetDistance(float dist) {mAxisLength = dist;}
   void SetUseViewPointInfo(bool flag) {mUseInitialViewPoint = flag;}
   void SetAnimationUpdateInterval(int interval) {mUpdateInterval = interval;}
   void SetDrawWireFrame(bool flag) {mDrawWireFrame = flag;}
   void SetDrawXyPlane(bool flag) {mDrawXyPlane = flag;}
   void SetDrawEcPlane(bool flag) {mDrawEcPlane = flag;}
   void SetDrawESLines(bool flag) {mDrawESLines = flag;}
   void SetDrawAxes(bool flag) {mDrawAxes = flag;}
   void SetDrawGrid(bool flag) {mDrawGrid = flag;}
   void SetRotateAboutXY(bool flag) {mRotateXy = flag;}
   void SetXyPlaneColor(unsigned int color) {mXyPlaneColor = color;}
   void SetEcPlaneColor(unsigned int color) {mEcPlaneColor = color;}
   void SetESLineColor(unsigned int color) {mESLinecolor = color;}
   void SetViewCoordSystem(const wxString &csName);
   void SetUsePerspectiveMode(bool perspMode);
   void SetObjectColors(const wxStringColorMap &objectColorMap);
   void SetShowObjects(const wxStringBoolMap &showObjMap);
   void SetShowOrbitNormals(const wxStringBoolMap &showOrbitNormalMap);
   void UpdateObjectList(const wxArrayString &bodyNames,
                         const wxStringColorMap &bodyColors);

   // actions
   void ClearPlot();
   void RedrawPlot(bool viewAnimation);
   void ShowDefaultView();
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
   void ViewAnimation(int interval);
   
   void SetGlObject(const StringArray &objNames,
                    const UnsignedIntArray &objOrbitColors,
                    const std::vector<SpacePoint*> &objectArray);
   
   // CoordinateSystem
   void SetGlCoordSystem(CoordinateSystem *viewCs,
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
                   const UnsignedIntArray &scColors);
   
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
   bool mDrawESLines;
   bool mDrawAxes;
   bool mDrawGrid;
   bool mDrawOrbitNormal;
   
   // color
   unsigned int mXyPlaneColor;
   unsigned int mEcPlaneColor;
   unsigned int mESLinecolor;

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
   SpacePoint *mViewPointRefObj;
   SpacePoint *mViewPointVectorObj;
   SpacePoint *mViewDirectionObj;
   Rvector3 mViewPointRefVector;
   Rvector3 mViewPointVector;
   Rvector3 mViewDirectionVector;
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
   
   // data count
   int  mNumData;
   bool mIsEndOfData;
   bool mIsEndOfRun;
   
   // time
   Real mTime[MAX_DATA];

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
   SolarSystem *mSolarSystem;
   
   // earth
   float mEarthRadius;

   // objects
   wxArrayString mObjectNames;
   wxStringColorMap mObjectColorMap;
   wxStringBoolMap  mShowObjectMap;
   wxStringBoolMap  mShowOrbitNormalMap;
   std::vector<SpacePoint*> mObjectArray;
   std::vector<bool> mDrawOrbitArray;
   std::vector<bool> mShowObjectArray;
   int mObjectCount;
   float mObjectDefaultRadius;
   float mObjectRadius[MAX_OBJECT];
   float mObjMaxZoomIn[MAX_OBJECT];
   int   mObjLastFrame[MAX_OBJECT];
   bool  mShowObjectFlag[MAX_OBJECT];
   
   bool  mDrawOrbitFlag[MAX_OBJECT][MAX_DATA];
   UnsignedInt mObjectOrbitColor[MAX_OBJECT][MAX_DATA];
   
   float mObjectGciPos[MAX_OBJECT][MAX_DATA][3];
   float mObjectTempPos[MAX_OBJECT][MAX_DATA][3];
   float mObjectGciVel[MAX_OBJECT][MAX_DATA][3];
   float mObjectTempVel[MAX_OBJECT][MAX_DATA][3];
   
   // coordinate system
   wxString mInternalCoordSysName;
   wxString mInitialCoordSysName;
   wxString mViewCoordSysName;
   wxString mViewUpCoordSysName;
   wxString mOriginName;
   CoordinateSystem *mInternalCoordSystem;
   CoordinateSystem *mInitialCoordSystem;
   CoordinateSystem *mViewCoordSystem;
   CoordinateSystem *mViewUpCoordSystem;
   int mOriginId;
   
   // coordinate sytem conversion
   bool mIsInternalCoordSystem;
   bool mNeedSpacecraftConversion;
   bool mNeedOriginConversion;
   bool mNeedObjectConversion;
   bool mNeedInitialConversion;
   CoordinateConverter mCoordConverter;
   
   // view
   wxSize mCanvasSize;
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
   bool mViewAnimation;
   bool mHasUserInterrupted;
   int mUpdateInterval;

   // message
   bool mShowMaxWarning;
   int  mOverCounter;

   // windows specific functions
   bool SetPixelFormatDescriptor();
   void SetDefaultGLFont();
   
   // initialization
   // texture
   bool LoadGLTextures();
   bool LoadBodyTextures();
   GLuint BindTexture(const wxString &objName);
   void SetDefaultView();
   
   // view objects
   void SetProjection();
   void SetupWorld();
   void ComputeView(GLfloat fEndX, GLfloat fEndY);
   void ChangeView(float viewX, float viewY, float viewZ);
   void ChangeProjection(int width, int height, float axisLength);
   void ComputeProjection(int frame);
   void ComputeUpAngleAxis(int frame);
   void TransformView(int frame);
   
   // drawing objects
   void DrawFrame();
   void DrawPlot();
   void DrawSphere(GLdouble radius, GLint slices, GLint stacks, GLenum style,
                   GLenum orientation = GLU_OUTSIDE);
   void DrawObject(const wxString &objName, int frame);
   void DrawObjectOrbit(int frame);
   void DrawObjectOrbitNormal(int objId, int frame, UnsignedInt color);
   void DrawSpacecraft(UnsignedInt scColor);
   void DrawEquatorialPlane(UnsignedInt color);
   void DrawEclipticPlane(UnsignedInt color);
   void DrawESLines(int frame);
   void DrawAxes();
   void DrawStatus(const wxString &label, int frame, double time);
   
   // drawing primative objects
   //void DrawStringAt(char* inMsg, GLfloat x, GLfloat y, GLfloat z);
   void DrawStringAt(const wxString &str, GLfloat x, GLfloat y, GLfloat z);
   void DrawCircle(GLUquadricObj *qobj, Real radius);
   
   // for object
   int GetObjectId(const wxString &name);
   
   // for coordinate system
   bool TiltOriginZAxis();
   bool ConvertSpacecraftData();
   bool ConvertSpacecraftData(int frame);
   bool ConvertObjectData();
   void ConvertObject(int objId, int index);

   // for utility
   Rvector3 ComputeEulerAngles();
   void ComputeLongitudeLst(Real time, Real x, Real y, Real *meanHourAngle,
                            Real *longitude, Real *localSiderealTime);
   
   // for copy
   void CopyVector3(float to[3], Real from[3]);
   void CopyVector3(float to[3], float from[3]);
   void CopyVector3(Real to[3], Real from[3]);
   void CopyVector3(Real to[3], float from[3]);
   
   bool LoadImage(char *fileName);
   
};

#endif
