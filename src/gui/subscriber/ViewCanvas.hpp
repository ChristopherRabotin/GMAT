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
// ** Legal **
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
#include "TextTrajectoryFile.hpp"
#include "SolarSystem.hpp"
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
   
   // initialization
   //virtual bool InitGL() = 0;
   
   // getters
   virtual bool  GetUseViewPointInfo() = 0;
   virtual bool  GetUsePerspectiveMode() = 0;
   virtual bool  GetDrawWireFrame() = 0;
   virtual bool  GetDrawXyPlane() = 0;
   virtual bool  GetDrawEcPlane() = 0;
   virtual bool  GetDrawSunLine() = 0;
   virtual bool  GetDrawAxes() = 0;
   virtual bool  GetDrawGrid() = 0;
   virtual bool  GetRotateAboutXY() = 0;
   virtual bool  IsAnimationRunning() = 0;
   virtual unsigned int GetXyPlaneColor() = 0;
   virtual unsigned int GetEcPlaneColor() = 0;
   virtual unsigned int GetSunLineColor() = 0;
   virtual float GetDistance() = 0;
   virtual int GetAnimationUpdateInterval() = 0;
   virtual int GetAnimationFrameIncrement() = 0;
   virtual wxString GetViewCoordSysName() = 0;
   virtual CoordinateSystem* GetViewCoordSystem() = 0;
   virtual const wxArrayString& GetObjectNames() = 0;
   virtual const wxArrayString& GetValidCSNames() = 0;
   virtual const wxStringBoolMap& GetShowObjectMap() = 0;
   virtual const wxStringColorMap& GetObjectColorMap() = 0;
   virtual wxString GetGotoObjectName() = 0;
   virtual wxGLContext* GetGLContext() = 0;
   
   // setters
   virtual void SetEndOfRun(bool flag = true) = 0;
   virtual void SetEndOfData(bool flag = true) = 0;
   virtual void SetDistance(float dist) = 0;
   virtual void SetUseInitialViewDef(bool flag) = 0;
   virtual void SetAnimationUpdateInterval(int value) = 0;
   virtual void SetAnimationFrameIncrement(int value) = 0;
   virtual void SetDrawWireFrame(bool flag) = 0;
   virtual void SetDrawStars(bool flag) = 0;
   virtual void SetDrawConstellations(bool flag) = 0;
   virtual void SetStarCount(int count) = 0;
   virtual void SetDrawXyPlane(bool flag) = 0;
   virtual void SetDrawEcPlane(bool flag) = 0;
   virtual void SetDrawSunLine(bool flag) = 0;
   virtual void SetDrawAxes(bool flag) = 0;
   virtual void SetDrawGrid(bool flag) = 0;
   virtual void SetRotateAboutXY(bool flag) = 0;
   virtual void SetXyPlaneColor(unsigned int color) = 0;
   virtual void SetEcPlaneColor(unsigned int color) = 0;
   virtual void SetSunLineColor(unsigned int color) = 0;
   virtual void SetUsePerspectiveMode(bool perspMode) = 0;
   virtual void SetObjectColors(const wxStringColorMap &objectColorMap) = 0;
   virtual void SetShowObjects(const wxStringBoolMap &showObjMap) = 0;   
   virtual void SetGLContext(wxGLContext *glContext = NULL) = 0;
   virtual void SetUserInterrupt()  = 0;
   
   // actions
   virtual void ClearPlot() = 0;
   virtual void ResetPlotInfo() = 0;
   virtual void RedrawPlot(bool viewAnimation) = 0;
   virtual void ShowDefaultView() = 0;
   //virtual void RotatePlot(int width, int height, int mouseX, int mouseY) = 0;
   //virtual void ZoomIn() = 0;
   //virtual void ZoomOut() = 0;
   virtual void DrawWireFrame(bool flag) = 0;
   virtual void DrawXyPlane(bool flag) = 0;
   virtual void DrawEcPlane(bool flag) = 0;
   virtual void OnDrawAxes(bool flag) = 0;
   virtual void OnDrawGrid(bool flag) = 0;
   virtual void DrawInOtherCoordSystem(const wxString &csName) = 0;
   virtual void GotoObject(const wxString &objName) = 0;
   virtual void GotoOtherBody(const wxString &bodyName) = 0;
   virtual void ViewAnimation(int interval, int frameInc = 30) = 0;
   
   virtual void SetGlObject(const StringArray &objNames,
                    const UnsignedIntArray &objOrbitColors,
                    const std::vector<SpacePoint*> &objectArray) = 0;
   
   // SolarSystem
   virtual void SetSolarSystem(SolarSystem *ss) = 0;
   
   // CoordinateSystem
   virtual void SetGlCoordSystem(CoordinateSystem *internalCs,
                         CoordinateSystem *viewCs,
                         CoordinateSystem *viewUpCs) = 0;
   
   // viewpoint
   virtual void SetGlViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                        SpacePoint *vdObj, Real vscaleFactor,
                        const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                        const Rvector3 &vdVec, const std::string &upAxis,
                        bool usevpRefVec, bool usevpVec, bool usevdVec,
                        bool useFixedFov, Real fov) = 0;
   
   // drawing toggle switch
   virtual void SetGlDrawOrbitFlag(const std::vector<bool> &drawArray) = 0;
   virtual void SetGlShowObjectFlag(const std::vector<bool> &showArray) = 0;
   
   // performance
   virtual void SetUpdateFrequency(Integer updFreq) = 0;
   virtual void SetNumPointsToRedraw(Integer numPoints) = 0;
   
   // update
   virtual void UpdatePlot(const StringArray &scNames, const Real &time,
                   const RealArray &posX, const RealArray &posY,
                   const RealArray &posZ, const RealArray &velX,
                   const RealArray &velY, const RealArray &velZ,
                   const UnsignedIntArray &scColors, bool solving,
                   Integer solverOption, bool inFunction = false) = 0;
   
   virtual void TakeAction(const std::string &action) = 0;
   
   // object
   virtual void AddObjectList(const wxArrayString &objNames,
                      const UnsignedIntArray &objColors,
                      bool clearList = true) = 0;
   
   // file
   virtual int  ReadTextTrajectory(const wxString &filename) = 0;

protected:
   
   // events
   virtual void OnPaint(wxPaintEvent &event) = 0;
   virtual void OnTrajSize(wxSizeEvent &event) = 0;
   virtual void OnMouse(wxMouseEvent &event) = 0;
   virtual void OnKeyDown(wxKeyEvent &event) = 0;
   
   // windows specific functions
   virtual bool SetPixelFormatDescriptor() = 0;
   virtual void SetDefaultGLFont() = 0;
   
   // initialize
   virtual void InitializeViewPoint() = 0;
   
   // for data buffer indexing
   virtual void ComputeActualIndex() = 0;
   
   // texture
   virtual bool LoadGLTextures() = 0;
   virtual GLuint BindTexture(SpacePoint *obj, const wxString &objName) = 0;
   virtual void SetDefaultView() = 0;
   
   // view objects
   virtual void SetProjection() = 0;
   virtual void SetupWorld() = 0;
   virtual void ComputeView(GLfloat fEndX, GLfloat fEndY) = 0;
   virtual void ChangeView(float viewX, float viewY, float viewZ) = 0;
   virtual void ChangeProjection(int width, int height, float axisLength) = 0;
   virtual void ComputeViewVectors() = 0;
   virtual void ComputeUpAngleAxis() = 0;
   virtual void TransformView() = 0;
   
   // drawing objects
   virtual void DrawFrame() = 0;
   virtual void DrawPlot() = 0;
   //virtual void DrawSphere(GLdouble radius, GLint slices, GLint stacks, GLenum style,
   //                GLenum orientation = GLU_OUTSIDE, GLenum normals = GL_SMOOTH,
   //                GLenum textureCoords = GL_TRUE) = 0;
   virtual void DrawObject(const wxString &objName, int obj) = 0;
   //virtual void DrawObjectOrbit() = 0;
   virtual void DrawOrbit(const wxString &objName, int obj, int objId) = 0;
   virtual void DrawOrbitLines(int i, const wxString &objName, int obj, int objId) = 0;
   
   //virtual void DrawObjectTexture(const wxString &objName, int obj, int objId) = 0;
   virtual void DrawSolverData() = 0;
   //virtual void DrawSpacecraft(UnsignedInt scColor) = 0;
   virtual void DrawEquatorialPlane(UnsignedInt color) = 0;
   virtual void DrawEclipticPlane(UnsignedInt color) = 0;
   virtual void DrawSunLine() = 0;
   virtual void DrawAxes() = 0;
   virtual void DrawStatus(const wxString &label1, int frame, const wxString &label2,
                   double time, int xpos = 0, int ypos = 0,
                   const wxString &label3 = "") = 0;
   
   // for rotation
   virtual void ApplyEulerAngles() = 0;
   
   // drawing primative objects
   //virtual void DrawStringAt(const wxString &str, GLfloat x, GLfloat y, GLfloat z) = 0;
   //virtual void DrawCircle(GLUquadricObj *qobj, Real radius) = 0;
   
   // for object
   virtual int  GetObjectId(const wxString &name) = 0;
   virtual void ClearObjectArrays(bool deleteArrays = true) = 0;
   virtual bool CreateObjectArrays() = 0;
   
   // for data update
   virtual void UpdateSolverData(const RealArray &posX, const RealArray &posY,
                         const RealArray &posZ, const UnsignedIntArray &scColors,
                         bool solving) = 0;
   virtual void UpdateSpacecraftData(const Real &time,
                             const RealArray &posX, const RealArray &posY,
                             const RealArray &posZ, const RealArray &velX,
                             const RealArray &velY, const RealArray &velZ,
                             const UnsignedIntArray &scColors, Integer solverOption) = 0;
   virtual void UpdateOtherData(const Real &time) = 0;
   
   // for coordinate system
   virtual bool ConvertObjectData() = 0;
   virtual void ConvertObject(int objId, int index) = 0;
   
   // for utility
   virtual Rvector3 ComputeEulerAngles();
   virtual void ComputeLongitudeLst(Real time, Real x, Real y, Real *meanHourAngle,
                                    Real *longitude, Real *localSiderealTime) = 0;
   
   // for copy
   virtual void CopyVector3(Real to[3], Real from[3]);
   // for loading image
   virtual bool LoadImage(const std::string &fileName);
   
};

#endif
