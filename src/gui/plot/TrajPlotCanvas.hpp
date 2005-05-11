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
   bool IsInitialized();

   // getters
   bool  GetUseViewPointInfo() {return mUseInitialViewPoint;}
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
   wxString GetDesiredCoordSysName() {return mDesiredCoordSysName;}
   CoordinateSystem* GetDesiredCoordSystem() { return mDesiredCoordSystem;}
   const wxArrayString& GetObjectNames() { return mObjectNames;}
   const wxStringColorMap& GetObjectColorMap() { return mObjectColorMap;}
   wxString GetGotoObjectName();
   
   // setters
   void SetDistance(float dist) {mAxisLength = dist;}
   void SetUseViewPointInfo(bool flag) {mUseInitialViewPoint = flag;}
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
   void SetDesiredCoordSystem(const wxString &csName);
   void SetUsePerspectiveMode(bool perspMode);
   void SetObjectColors(const wxStringColorMap &objectColorMap);
   void SetShowObjects(const wxStringBoolMap &showObjMap);
   void UpdateObjectList(const wxArrayString &bodyNames,
                         const wxStringColorMap &bodyColors);

   // actions
   void ClearPlot();
   void RedrawPlot(bool viewAnimation);
   void ShowDefaultView();
   void ZoomIn();
   void ZoomOut();
   void DrawWireFrame(bool flag);
   void DrawEqPlane(bool flag);
   void DrawEcPlane(bool flag);
   void OnDrawAxes(bool flag);
   void DrawInOtherCoordSystem(const wxString &csName);
   void GotoObject(const wxString &objName);
   void GotoOtherBody(const wxString &bodyName);
   void ViewAnimation(int interval);

   void SetGlObject(const StringArray &nonScNames,
                    const UnsignedIntArray &nonScColors,
                    const std::vector<SpacePoint*> nonScArray);
   
   // CoordinateSystem (loj: 5/9/05 Added)
   void SetGlCoordSystem(CoordinateSystem *cs);
   
   // viewpoint (loj: 4/20/05 Added)
   void SetGlViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                        SpacePoint *vdObj, Real vscaleFactor,
                        const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                        const Rvector3 &vdVec, bool usevpRefVec,
                        bool usevpVec, bool usevdVec);
   
   // data
   int  ReadTextTrajectory(const wxString &filename);

   // update
   void UpdatePlot(const StringArray &scNames,
                   const Real &time, const RealArray &posX,
                   const RealArray &posY, const RealArray &posZ,
                   const UnsignedIntArray &scColors);
   
   // body
   void AddObjectList(const wxArrayString &bodyNames,
                      const UnsignedIntArray &bodyColors,
                      bool clearList = true);
   
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
   GLuint mObjectTextureIndex[GmatPlot::MAX_BODIES];
   std::map<wxString, GLuint> mObjectTextureIdMap;
   GLuint mScTextureIndex[GmatPlot::MAX_SCS];
   bool mUseTexture;
   
   // rotating
   bool mRotateXy;
   bool mRotateAboutXaxis;
   bool mRotateAboutYaxis;
   bool mRotateAboutZaxis;
   bool mRotateEarthToEnd;
   Real lastLongitudeD;
   
   // zooming
   int   mLastMouseX;
   int   mLastMouseY;
   float mZoomAmount;
   float mMaxZoomIn;
   
   // initialization and limit
   bool mInitialized;
   bool mDdataCountOverLimit;
   int  mNumData;

   // projection
   bool mUsePerspectiveMode;
   Real mFovDeg;
   
   // viewpoint
   StringArray mScNameArray;
   std::string mViewPointRefObjName;
   SpacePoint *mViewPointRefObj;
   SpacePoint *mViewPointVectorObj;
   SpacePoint *mViewDirectionObj;
   Rvector3 mViewPointRefVector;
   Rvector3 mViewPointVector;
   Rvector3 mViewDirectionVector;
   Rvector3 mViewPointLocVector;
   Real mViewScaleFactor;
   bool mUseInitialViewPoint;
   bool mUseViewPointRefVector;
   bool mUseViewPointVector;
   bool mUseViewDirectionVector;
   int mVpRefScId;
   int mVpVecScId;
   int mVdirScId;
   int mVpRefObjId;
   int mVpVecObjId;
   int mVdirBodyId;
   
   // time
   Real mTime[MAX_DATA];

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

   // objects
   wxArrayString mObjectNames;
   wxStringColorMap mObjectColorMap;
   wxStringBoolMap  mShowObjectMap;
   std::vector<SpacePoint*> mObjectArray;
   int mObjectCount;
   float mObjectDefaultRadius;
   float mObjectRadius[GmatPlot::MAX_BODIES];
   float mObjMaxZoomIn[GmatPlot::MAX_BODIES];

   // bodies
   bool  mObjectInUse[GmatPlot::MAX_BODIES];
   bool  mObjectHasData[GmatPlot::MAX_BODIES];
   float mObjectGciPos[GmatPlot::MAX_BODIES][MAX_DATA][3];
   float mObjectTempPos[GmatPlot::MAX_BODIES][MAX_DATA][3];
   short mPivotBodyIndex[GmatPlot::MAX_BODIES];
   int   mOtherBodyCount;
   StringArray mObjectNamesInUse;
   
   // coordinate system
   wxString mDesiredCoordSysName;
   wxString mInternalCoordSysName;
   wxString mOriginName;
   CoordinateSystem *mInternalCoordSystem;
   CoordinateSystem *mDesiredCoordSystem;
   int mOriginId;
   
   // coordinate sytem conversion
   bool mIsInternalCoordSystem;
   bool mNeedSpacecraftConversion;
   bool mNeedOriginConversion;
   bool mNeedObjectConversion;
   bool mNeedInitialConversion;
   CoordinateConverter mCoordConverter;
   
   short mCurrViewFrame;
   
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
   // texture
   bool LoadGLTextures();
   GLuint BindTexture(const wxString &objName);
   
   // view objects
   void SetProjection();
   void SetupWorld();
   void ComputeView(GLfloat fEndX, GLfloat fEndY);
   void ChangeView(float viewX, float viewY, float viewZ);
   void ChangeProjection(int width, int height, float axisLength);
   void ComputeProjection(int frame); //loj: 4/25/05 Added frame
   
   // drawing objects
   void DrawFrame();
   void DrawPlot();
   void DrawObject(const wxString &objName);
   void DrawObjectOrbit();
   void DrawEarthOrbit(int frame);
   void DrawSpacecraft(UnsignedInt scColor);
   void DrawSpacecraftOrbit();
   void DrawSpacecraftOrbit(int frame);
   void DrawEquatorialPlane(UnsignedInt color);
   void DrawEclipticPlane(UnsignedInt color);
   void DrawEarthSunLine();
   void DrawAxes(bool gci = false);
   
   // drawing primative objects
   void DrawStringAt(char* inMsg, GLfloat x, GLfloat y, GLfloat z);
   void DrawCircle(GLUquadricObj *qobj, Real radius);
   
//    // for body
//    int GetStdBodyId(const std::string &name);
//    void AddBody(const std::string &name);
   
   // for object
   int GetObjectId(const wxString &name);
   
   // for coordinate system
   bool TiltOriginZAxis();
   bool ConvertSpacecraftData();
   bool ConvertSpacecraftData(int frame);
   bool ConvertObjectData();
   void ConvertObject(int objId, int index);
   
   // for copy
   void CopyVector3(float to[3], Real from[3]);
   void CopyVector3(float to[3], float from[3]);
   void CopyVector3(Real to[3], Real from[3]);
   void CopyVector3(Real to[3], float from[3]);
   
};

#endif
