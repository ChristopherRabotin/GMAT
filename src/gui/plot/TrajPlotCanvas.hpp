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
   bool IsInitialized();

   // getters
   bool  GetUseViewPointInfo() {return mUseViewPointInfo;}
   bool  GetUsePerspectiveMode() {return mUsePerspectiveMode;}
   bool  GetDrawWireFrame() {return mDrawWireFrame;}
   bool  GetDrawEqPlane() {return mDrawEqPlane;}
   bool  GetDrawEcPlane() {return mDrawEcPlane;}
   bool  GetDrawEcLine() {return mDrawEcLine;}
   bool  GetDrawAxes() {return mDrawAxes;}
   bool  GetRotateAboutXY() {return mRotateXy;}
   unsigned int GetEqPlaneColor() {return mEqPlaneColor;}
   unsigned int GetEcPlaneColor() {return mEcPlaneColor;}
   unsigned int GetEcLineColor() {return mEcLineColor;}
   float GetDistance() {return mAxisLength;}
   int GetAnimationUpdateInterval() {return mUpdateInterval;}
   int GetGotoBodyId() {return mCenterViewBody;}
   wxString GetDesiredCoordSysName() {return mDesiredCoordSysName;}
   CoordinateSystem* GetDesiredCoordSystem() { return mDesiredCoordSystem;}
   
   // setters
   void SetDistance(float dist) {mAxisLength = dist;}
   void SetUseViewPointInfo(bool flag) {mUseViewPointInfo = flag;}
   void SetAnimationUpdateInterval(int interval) {mUpdateInterval = interval;}
   void SetDrawWireFrame(bool flag) {mDrawWireFrame = flag;}
   void SetDrawEqPlane(bool flag) {mDrawEqPlane = flag;}
   void SetDrawEcPlane(bool flag) {mDrawEcPlane = flag;}
   void SetDrawEcLine(bool flag) {mDrawEcLine = flag;}
   void SetDrawAxes(bool flag) {mDrawAxes = flag;}
   void SetRotateAboutXY(bool flag) {mRotateXy = flag;}
   void SetEqPlaneColor(unsigned int color) {mEqPlaneColor = color;}
   void SetEcPlaneColor(unsigned int color) {mEcPlaneColor = color;}
   void SetEcLineColor(unsigned int color) {mEcLineColor = color;}
   void SetDesiredCoordSystem(CoordinateSystem* cs) {mDesiredCoordSystem = cs;}
   void SetDesiredCoordSystem(const wxString &csName);
   void SetUsePerspectiveMode(bool perspMode);
   
   // actions
   void ClearPlot();
   void UpdatePlot(bool viewAnimation);
   void ShowDefaultView();
   void ZoomIn();
   void ZoomOut();
   void DrawWireFrame(bool flag);
   void DrawEqPlane(bool flag);
   void DrawEcPlane(bool flag);
   void OnDrawAxes(bool flag);
   void DrawInOtherCoordSystem(const wxString &csName);
   void DrawInOtherCoordSystem(CoordinateSystem *cs);
   void GotoStdBody(int bodyId);
   void GotoOtherBody(const wxString &bodyName);
   void ViewAnimation(int interval);

   // viewpoint (loj: 4/20/05 Added)
   void SetGlViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                        SpacePoint *vdObj, Real vscaleFactor,
                        const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                        const Rvector3 &vdVec, bool usevpRefVec,
                        bool usevpVec, bool usevdVec);
   
   // data
   int  ReadTextTrajectory(const wxString &filename);
   void UpdateSpacecraft(const StringArray &scNameArray,
                         const Real &time, const RealArray &posX,
                         const RealArray &posY, const RealArray &posZ,
                         const UnsignedIntArray &olor);
   
   // body
   void AddBody(const wxArrayString &bodyNames,
                const UnsignedIntArray &bodyColors);
   
protected:
   
   // events
   void OnPaint(wxPaintEvent &event);
   void OnTrajSize(wxSizeEvent &event);
   void OnMouse(wxMouseEvent &event);
   void OnKeyDown(wxKeyEvent &event);
   
   DECLARE_EVENT_TABLE();

private:
   
   static const int MAX_DATA = 20000;
   static const int LAST_STD_BODY_ID = 10;
   static const int MAX_COORD_SYS = 10;
   static const std::string BODY_NAME[GmatPlot::MAX_BODIES];
   static const unsigned int UNINIT_TEXTURE = 999;
   static const float MAX_ZOOM_IN = 3700.0;
   static const float RADIUS_ZOOM_RATIO = 2.2;
   static const float DEFAULT_DIST = 30000.0;

   GuiInterpreter *theGuiInterpreter;
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
   
   // draw option
   float mAxisLength;
   bool mDrawWireFrame;
   bool mDrawEqPlane;
   bool mDrawEcPlane;
   bool mDrawEclipticPlane;
   bool mDrawEcLine;
   bool mDrawSpacecraft;
   bool mDrawAxes;
   
   // color
   unsigned int mEqPlaneColor;
   unsigned int mEcPlaneColor;
   unsigned int mEcLineColor;
   
   // texture
   GLuint mBodyTextureIndex[GmatPlot::MAX_BODIES];
   GLuint mScTextureIndex[GmatPlot::MAX_SCS];
   bool mUseTexture;
   
   // rotating
   bool mRotateXy;
   bool mRotateAboutXaxis;
   bool mRotateAboutYaxis;
   bool mRotateAboutZaxis;
   bool mRotateEarthToEnd;
   double lastLongitudeD;
   
   // zooming
   int   mLastMouseX;
   int   mLastMouseY;
   float mZoomAmount;
   float mMaxZoomIn;
   float mBodyMaxZoomIn[GmatPlot::MAX_BODIES];
   
   // initialization and limit
   bool mInitialized;
   bool mDdataCountOverLimit;
   int  mNumData;

   // projection
   bool mUsePerspectiveMode;
   
   // viewpoint
   StringArray mScNameArray;
   SpacePoint *mViewPointRefObj;
   SpacePoint *mViewPointVectorObj;
   SpacePoint *mViewDirectionObj;
   Rvector3 mViewPointRefVector;
   Rvector3 mViewPointVector;
   Rvector3 mViewDirectionVector;
   Rvector3 mViewPointLocVector;
   Real mViewScaleFactor;
   bool mUseViewPointInfo;
   bool mUseViewPointRefVector;
   bool mUseViewPointVector;
   bool mUseViewDirectionVector;
   int mVptRefScId;
   int mVptVecScId;
   int mVdirScId;
   int mVptRefBodyId;
   int mVptVecBodyId;
   int mVdirBodyId;
   
   // time
   double mTime[MAX_DATA];

   // spacecraft
   int   mScCount;
   float mScGciPos[GmatPlot::MAX_SCS][MAX_DATA][3];
   float mScTempPos[GmatPlot::MAX_SCS][MAX_DATA][3];
   unsigned int mScTrajColor[GmatPlot::MAX_SCS][MAX_DATA];
   float mScRadius;
   GLuint mGlList;
   
   // solar system
   SolarSystem *mSolarSystem;
   
   // earth
   float mEarthRadius;
   float mEarthGciPos[MAX_DATA][3];
   float mEarthTempPos[MAX_DATA][3];
   
   // bodies
   //std::string mBodyName[GmatPlot::MAX_BODIES];
   bool  mBodyInUse[GmatPlot::MAX_BODIES];
   bool  mBodyHasData[GmatPlot::MAX_BODIES];
   float mBodyRadius[GmatPlot::MAX_BODIES];
   float mBodyGciPos[GmatPlot::MAX_BODIES][MAX_DATA][3];
   float mBodyTempPos[GmatPlot::MAX_BODIES][MAX_DATA][3];
   short mPivotBodyIndex[GmatPlot::MAX_BODIES];
   int   mOtherBodyCount;
   
   // coordinate system
   wxString mDesiredCoordSysName;
   wxString mInternalCoordSysName;
   CoordinateSystem *mInternalCoordSystem;
   CoordinateSystem *mDesiredCoordSystem;
   
   // coordinate sytem conversion
   bool mIsInternalCoordSystem;
   bool mNeedSpacecraftConversion;
   bool mNeedEarthConversion;
   bool mNeedOtherBodyConversion;
   bool mNeedConversion;
   CoordinateConverter mCoordConverter;
   
   short mCurrViewFrame;
   short mCurrBody;
   int   mCenterViewBody;
   
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
   
   // windows specific functions
   bool SetPixelFormatDescriptor();
   void SetDefaultGLFont();

   // initialization
   bool LoadGLTextures();
   
   // view objects
   void SetProjection();
   void SetupWorld();
   void ComputeView(GLfloat fEndX, GLfloat fEndY);
   void ChangeView(float viewX, float viewY, float viewZ);
   void ChangeProjection(int width, int height, float axisLength);
   void ComputeProjection(int frame); //loj: 4/25/05 Added frame
   
   // drawing objects
   void DrawFrame();
   void DrawPicture();
   void DrawEarth();
   void DrawEarthOrbit();
   void DrawEarthOrbit(int frame);
   void DrawOtherBody(int bodyIndex);
   void DrawOtherBodyOrbit(int bodyIndex);
   void DrawSpacecraft(UnsignedInt scColor);
   void DrawSpacecraftOrbit();
   void DrawSpacecraftOrbit(int frame);
   void DrawEquatorialPlane(UnsignedInt color);
   void DrawEclipticPlane();
   void DrawEarthSunLine();
   void DrawAxes(bool gci = false);  //loj: 4/15/05 Added earthZaxis
   void DrawStringAt(char* inMsg, GLfloat x, GLfloat y, GLfloat z);

   // for body
   int GetStdBodyId(const std::string &name);

   // for coordinate sytem
   bool TiltEarthZAxis();
   bool ConvertSpacecraftData();
   bool ConvertSpacecraftData(int frame);
   bool ConvertOtherBodyData();
   
   // for copy
   void CopyVector3(float to[3], double from[3]);
   void CopyVector3(float to[3], float from[3]);
   void CopyVector3(double to[3], double from[3]);
   void CopyVector3(double to[3], float from[3]);
   
};

#endif
