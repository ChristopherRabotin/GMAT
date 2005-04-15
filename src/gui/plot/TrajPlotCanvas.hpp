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
   float GetDistance() {return mAxisLength;}
   bool  GetDrawWireFrame() {return mDrawWireFrame;}
   bool  GetDrawEqPlane() {return mDrawEqPlane;}
   bool  GetDrawEcPlane() {return mDrawEcPlane;}
   bool  GetDrawEcLine() {return mDrawEcLine;}
   bool  GetDrawAxes() {return mDrawAxes;}
   bool  GetRotateAboutXY() {return mRotateXy;}
   unsigned int GetEqPlaneColor() {return mEqPlaneColor;}
   unsigned int GetEcPlaneColor() {return mEcPlaneColor;}
   unsigned int GetEcLineColor() {return mEcLineColor;}
   int GetGotoBodyId() {return mCenterViewBody;}
   wxString GetDesiredCoordSysName() {return mDesiredCoordSysName;}
   CoordinateSystem* GetDesiredCoordSystem() { return mDesiredCoordSystem;}
   
   // setters
   void SetDistance(float dist) {mAxisLength = dist;}
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
   
   // actions
   void ClearPlot();
   void UpdatePlot();
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
   
   // data
   int  ReadTextTrajectory(const wxString &filename);
   void UpdateSpacecraft(const Real &time, const RealArray &posX,
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
   GLfloat m_fStartX, m_fStartY;
   
   // Actual params of the window
   GLfloat m_fLeftPos, m_fRightPos, m_fBottomPos, m_fTopPos;
   
   // Camera rotations
   GLfloat m_fCamRotationX, m_fCamRotationY, m_fCamRotationZ;
   
   // Camera translations
   GLfloat m_fCamTransX, m_fCamTransY, m_fCamTransZ;
   
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
   GLfloat m_fViewLeft;
   GLfloat m_fViewRight;
   GLfloat m_fViewTop;
   GLfloat m_fViewBottom;
   GLfloat m_fViewNear;
   GLfloat m_fViewFar;
   float mDefaultViewX;
   float mDefaultViewY;
   float mDefaultViewZ;
   float mDefaultViewDist;
   float mCurrViewX;
   float mCurrViewY;
   float mCurrViewZ;
   float mCurrViewDist;

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
   
   // drawing objects
   void DrawPicture();
   void DrawEarth();
   void DrawEarthOrbit();
   void DrawOtherBody(int bodyIndex);
   void DrawOtherBodyOrbit(int bodyIndex);
   void DrawSpacecraft(UnsignedInt scColor);
   void DrawSpacecraftOrbit();
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
   bool ConvertOtherBodyData();
   
   // for copy
   void CopyVector3(float to[3], double from[3]);
   void CopyVector3(float to[3], float from[3]);
   void CopyVector3(double to[3], double from[3]);
   void CopyVector3(double to[3], float from[3]);
   
};

#endif
