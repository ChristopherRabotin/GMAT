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
 * Declares ViewCanvas for 3D visualization.
 */
//------------------------------------------------------------------------------
#ifndef ViewCanvas_hpp
#define ViewCanvas_hpp

#include "gmatwxdefs.hpp"
#include "MdiGlPlotData.hpp"
#include "GuiInterpreter.hpp"
#include "SolarSystem.hpp"
#include "Spacecraft.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"

#include <map>

class ViewCanvas: public wxGLCanvas
{
public:
   ViewCanvas(wxWindow *parent, const wxWindowID id = -1,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, 
              const wxString& name = wxT("ViewCanvas"),
              long style = 0);
   virtual ~ViewCanvas();
   
   // GL context
   virtual wxGLContext* GetGLContext();
   virtual void SetGLContext(wxGLContext *glContext = NULL);
   
   // initialization
   virtual bool InitializePlot();
   virtual bool InitOpenGL();
   
   // user interrupt
   virtual void SetUserInterrupt() { mHasUserInterrupted = true; }
   
   // drawing options
   virtual bool GetUseInitialViewDef() { return mUseInitialViewPoint; }   
   virtual void SetUseInitialViewDef(bool flag) { mUseInitialViewPoint = flag; }
   virtual bool GetDrawWireFrame() { return mDrawWireFrame; }
   virtual void SetDrawWireFrame(bool flag) { mDrawWireFrame = flag; }
   virtual bool GetDrawGrid() { return mDrawGrid; }
   virtual void SetDrawGrid(bool flag) { mDrawGrid = flag; }
   
   // animation
   virtual int  GetAnimationUpdateInterval() { return mUpdateInterval; }
   virtual void SetAnimationUpdateInterval(int value) { mUpdateInterval = value; }
   virtual int  GetAnimationFrameIncrement() { return mFrameInc; }
   virtual void SetAnimationFrameIncrement(int value) { mFrameInc = value; }
   virtual bool IsAnimationRunning() { return mIsAnimationRunning; }
   
   // objects
   virtual const wxStringBoolMap& GetShowObjectMap() { return mShowObjectMap; }
   virtual const wxStringColorMap& GetObjectColorMap() { return mObjectColorMap; }
   virtual const wxArrayString& GetObjectNames() { return mObjectNames; }
   virtual const wxArrayString& GetValidCSNames() { return mValidCSNames; }
   virtual void SetObjectColors(const wxStringColorMap &objectColorMap);
   virtual void SetShowObjects(const wxStringBoolMap &showObjMap);  
   virtual wxString GetGotoObjectName();
   
   // coordinate system
   virtual wxString GetViewCoordSysName() { return mViewCoordSysName; }
   virtual CoordinateSystem* GetViewCoordSystem() { return pViewCoordSystem; }
   
   // view distance
   virtual float GetDistance() { return mAxisLength; }
   virtual void SetDistance(float dist) { mAxisLength = dist; }
   
   // mission run
   virtual void SetEndOfData(bool flag = true) { mIsEndOfData = flag; }
   virtual void SetEndOfRun(bool flag = true);
   
   
   
   // actions
   virtual void ClearPlot() = 0;
   virtual void RedrawPlot(bool viewAnimation) = 0;
   virtual void ShowDefaultView() = 0;
   virtual void DrawWireFrame(bool flag) = 0;
   
   virtual void OnDrawGrid(bool flag) = 0;
   
   virtual void DrawInOtherCoordSystem(const wxString &csName) = 0;
   virtual void GotoObject(const wxString &objName);
   virtual void ViewAnimation(int interval, int frameInc = 30) = 0;
   
   virtual void SetGlObject(const StringArray &objNames,
                        const UnsignedIntArray &objOrbitColors,
                        const std::vector<SpacePoint*> &objectArray);
   
   // SolarSystem
   virtual void SetSolarSystem(SolarSystem *ss);
   
   // CoordinateSystem
   virtual void SetGlCoordSystem(CoordinateSystem *internalCs,
                        CoordinateSystem *viewCs,
                        CoordinateSystem *viewUpCs);
   
   // drawing options
   virtual void SetGl2dDrawingOption(const std::string &centralBodyName,
                        const std::string &textureMap,
                        Integer footPrintOption);
   virtual void SetGl3dDrawingOption(bool drawEcPlane, bool drawXyPlane,
                        bool drawWireFrame, bool drawAxes,
                        bool drawGrid, bool drawSunLine,
                        bool usevpInfo, bool drawStars,
                        bool drawConstellations,
                        Integer starCount);
   
   // viewpoint
   virtual void SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                        SpacePoint *vdObj, Real vscaleFactor,
                        const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                        const Rvector3 &vdVec, const std::string &upAxis,
                        bool usevpRefVec, bool usevpVec, bool usevdVec);
   
   // drawing toggle switch
   virtual void SetGlDrawOrbitFlag(const std::vector<bool> &drawArray);
   virtual void SetGlShowObjectFlag(const std::vector<bool> &showArray);
   
   // performance
   virtual void SetUpdateFrequency(Integer updFreq);
   virtual void SetNumPointsToRedraw(Integer numPoints);
   
   // update
   virtual void UpdatePlot(const StringArray &scNames, const Real &time,
                        const RealArray &posX, const RealArray &posY,
                        const RealArray &posZ, const RealArray &velX,
                        const RealArray &velY, const RealArray &velZ,
                        const UnsignedIntArray &scColors, bool solving,
                        Integer solverOption, bool inFunction = false);
   
   virtual void TakeAction(const std::string &action) = 0;
   
   // object
   virtual void AddObjectList(const wxArrayString &objNames,
                        const UnsignedIntArray &objColors,
                        bool clearList = true);
   
protected:

   enum
   {
      MAX_DATA = 20000,
      UNKNOWN_OBJ_ID = -999,
   };
   
   static const Real RADIUS_ZOOM_RATIO;// = 2.2;
   
   // parent frame
   wxWindow *mParent;
   
   // OpenGL Context
   wxGLContext *theContext;
   wxSize  mCanvasSize;
   
   // GuiInterpreter
   GuiInterpreter *theGuiInterpreter;
   wxStatusBar *theStatusBar;
   
   // initialization
   wxString mPlotName;
   bool mGlInitialized;
   bool mViewPointInitialized;
   bool mOpenGLInitialized;
   bool modelsAreLoaded;
   
   // performance
   bool mRedrawLastPointsOnly;
   int  mNumPointsToRedraw;
   int  mUpdateFrequency;
   
   // ring buffer index
   int  mBeginIndex1;
   int  mBeginIndex2;
   int  mEndIndex1;
   int  mEndIndex2;
   int  mRealBeginIndex1;
   int  mRealBeginIndex2;
   int  mRealEndIndex1;
   int  mRealEndIndex2;
   int  mLastIndex;
   int  mCurrIndex;
   
   // data points
   int  mNumData;
   int  mTotalPoints;
   
   // data control flags
   bool mIsEndOfData;
   bool mIsEndOfRun;
   bool mIsFirstRun;
   bool mWriteWarning;
   bool mNeedVelocity;
   bool mNeedAttitude;

   // drawing option
   bool mDrawWireFrame;
   bool mDrawGrid;   
   
   // light source
   bool mSunPresent;
   bool mEnableLightSource;
   
   // texture
   std::map<wxString, GLuint> mTextureIdMap;
   std::map<wxString, std::string> mTextureFileMap;
   
   // view control
   bool mUseInitialViewPoint;
   float mAxisLength;
   
   // animation
   bool mIsAnimationRunning;
   bool mHasUserInterrupted;
   int  mUpdateInterval;
   int  mFrameInc;
   
   // message
   bool mShowMaxWarning;
   int  mOverCounter;
   
   // error handling and function mode
   bool mFatalErrorFound;
   bool mWriteRepaintDisalbedInfo;
   bool mInFunction;
   
   // solar system
   SolarSystem *pSolarSystem;
   
   // coordinate system
   wxString mInternalCoordSysName;
   wxString mViewCoordSysName;
   wxString mViewUpCoordSysName;
   wxString mOriginName;
   wxString mViewObjName;   
   CoordinateSystem *pInternalCoordSystem;
   CoordinateSystem *pViewCoordSystem;
   CoordinateSystem *pViewUpCoordSystem;
   int mOriginId;
   int mViewObjId;
   
   // coordinate sytem conversion
   bool mViewCsIsInternalCs;
   CoordinateConverter mCoordConverter;
   
   // spacecraft data
   int   mScCount;
   float mScRadius;
   StringArray mScNameArray;
   
   // objects
   wxArrayString mObjectNames;
   wxArrayString mShowObjectNames;
   wxArrayString mValidCSNames;
   wxStringColorMap mObjectColorMap;
   wxStringBoolMap  mShowObjectMap;
   std::vector<SpacePoint*> mObjectArray;
   std::vector<bool> mDrawOrbitArray;
   std::vector<bool> mShowObjectArray;
   int mObjectCount;
   Real mObjectDefaultRadius;
   
   // data buffers
   Real *mTime;                    // [MAX_DATA]
   Real *mObjectRadius;            // [mObjectCount]
   Real *mObjMaxZoomIn;            // [mObjectCount]
   int  *mObjLastFrame;            // [mObjectCount]
   bool *mDrawOrbitFlag;           // [mObjectCount][MAX_DATA]
   UnsignedInt *mObjectOrbitColor; // [mObjectCount][MAX_DATA]
   
   // object positions
   Real *mObjectGciPos;            // [mObjectCount][MAX_DATA][3]
   Real *mObjectViewPos;           // [mObjectCount][MAX_DATA][3]
   
   // object velocities
   Real *mObjectViewVel;           // [mObjectCount][MAX_DATA][3]
   
   // object attitude
   Real *mObjectQuat;              // [mObjectCount][MAX_DATA][4]
   
   // Coordinate Transformation Matrices
   Real *mCoordData;               // [MAX_DATA][16]
   
   // solver data
   bool mDrawSolverData;
   std::vector<RealArray> mSolverAllPosX; // [numPoints][numSC]
   std::vector<RealArray> mSolverAllPosY; // [numPoints][numSC]
   std::vector<RealArray> mSolverAllPosZ; // [numPoints][numSC]
   UnsignedIntArray mSolverIterColorArray;
   
   // viewing
   // Coordinate Transformation Matrices
   Rmatrix mCoordMatrix;
   
   // events
   virtual void OnPaint(wxPaintEvent &event) = 0;
   virtual void OnSize(wxSizeEvent &event) = 0;
   virtual void OnMouse(wxMouseEvent &event) = 0;
   virtual void OnKeyDown(wxKeyEvent &event) = 0;
   
   // windows specific functions
   virtual bool SetPixelFormatDescriptor();
   virtual void SetDefaultGLFont();
   
   // initialization
   virtual void ResetPlotInfo();   
   virtual void InitializeViewPoint() = 0;
   virtual void SetDefaultView() = 0;
   
   // for data buffer indexing
   virtual void ComputeBufferIndex(Real time);
   virtual void ComputeActualIndex();
   
   // texture
   virtual bool LoadBodyTextures();
   virtual GLuint BindTexture(SpacePoint *obj, const wxString &objName);
   
   // model
   virtual bool LoadSpacecraftModels();
   
   // view objects
   virtual void SetupProjection();
   virtual void SetupWorld() = 0;
      
   // drawing mode
   virtual void SetDrawingMode();
   
   // light source
   virtual void HandleLightSource();
   
   // drawing objects
   virtual void DrawFrame() = 0;
   virtual void DrawPlot() = 0;
   virtual void DrawObjectOrbit(int frame) = 0;
   virtual void DrawObjectTexture(const wxString &objName, int obj, int objId, int frame) = 0;
   virtual void DrawObject(const wxString &objName, int obj) = 0;
   virtual void DrawOrbit(const wxString &objName, int obj, int objId);
   virtual void DrawOrbitLines(int i, const wxString &objName, int obj, int objId) = 0;
   
   virtual void DrawSolverData();
   
   virtual void DrawStatus(const wxString &label1, unsigned int textColor,
                        const wxString &label2, double time,  int xpos = 0,
                        int ypos = 0, bool showCS = true, const wxString &label3 = "");
      
   // for object
   virtual int  GetObjectId(const wxString &name);
   virtual void ClearObjectArrays(bool deleteArrays = true);
   virtual bool CreateObjectArrays();
   
   // for data update
   virtual void UpdateSolverData(const RealArray &posX, const RealArray &posY,
                        const RealArray &posZ, const UnsignedIntArray &scColors,
                        bool solving);
   virtual void UpdateSpacecraftData(const Real &time,
                        const RealArray &posX, const RealArray &posY,
                        const RealArray &posZ, const RealArray &velX,
                        const RealArray &velY, const RealArray &velZ,
                        const UnsignedIntArray &scColors, Integer solverOption);
   virtual void UpdateOtherData(const Real &time);
   virtual void UpdateSpacecraftAttitude(Real time, Spacecraft *sat, int satId);   
   virtual void UpdateOtherObjectAttitude(Real time, SpacePoint *sp, int objId);
   
   // for coordinate system
   virtual bool ConvertObjectData() = 0;
   virtual void ConvertObject(int objId, int index) = 0;
   
   // for copy
   virtual void CopyVector3(Real to[3], Real from[3]);
   
   // for loading image
   virtual bool LoadImage(const std::string &fileName);
   
   // for debug
   void DrawDebugMessage(const wxString &msg, unsigned int textColor, int xpos, int ypos);
   
};

#endif
